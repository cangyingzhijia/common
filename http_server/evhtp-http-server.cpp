#include "common/http_server/evhtp-http-server.h"
#include <boost/lexical_cast.hpp>
#include "common/base/executor-service.h"
#include "thirdparty/glog/logging.h"
#include "common/system/system-info.h"
#include "common/http_server/evhtp-http-request.h"
#include "common/http_server/evhtp-http-response.h"
#include "common/http_server/handler-router.h"
#include "common/http_server/evhtp/evthr.h"

namespace common {
namespace http_server {

using common::SystemInfo;

const int kBackLog = 1024;

EvhtpHttpServer::EvhtpHttpServer(const std::string &listen_spec, uint32_t thread_num):
    listen_spec_(listen_spec)
{
    evbase_ = event_base_new();
    htp_    = evhtp_new(evbase_, NULL);
    thread_num_ = thread_num > 0 ? thread_num : SystemInfo::GetHardwareConcurrency();
    int rc = evhtp_use_threads(htp_, NULL, thread_num_, NULL);
    LOG_ASSERT(rc == 0);
    LOG_ASSERT(htp_->thr_pool != NULL);
    worker_thread_pool_ = new EvhtpExecutorService(htp_->thr_pool, false);
}

EvhtpHttpServer::~EvhtpHttpServer() {
}

void EvhtpHttpServer::Trampoline(evhtp_request_t * req, void * arg) {
    EvhtpHttpServer * server = static_cast<EvhtpHttpServer *>(arg);
    evhtp_request_pause(req);
    server->ProcessHttpRequest(req);
}

void EvhtpHttpServer::ProcessHttpRequest(evhtp_request_t * req) {
    EvhtpHttpRequest * request = NewEvhtpHttpRequest();
    request->Reset(req, this);
    EvhtpHttpResponse *response = NewEvhtpHttpResponse();
    response->Reset(req);
    Service(request, response);
}

common::ExecutorService * EvhtpHttpServer::GetThreadPool() {
    return worker_thread_pool_;
}

static bool SplitListenSpec(const std::string listen_spec, std::string *baddr, uint16_t *port) {
    std::string::size_type idx = listen_spec.find_last_of(":");
    if (idx == std::string::npos) {
        LOG(ERROR) << "listen_spec format is error, format is xxx.xxx.xxx.xxx:xx:" << listen_spec;
        return false;
    }
    *baddr = listen_spec.substr(0, idx);
    std::string p = listen_spec.substr(idx + 1);
    *port = boost::lexical_cast<uint16_t>(p);
    LOG(INFO) << "bind addr:" << *baddr << ", port:" << *port;
    return true;
}

bool EvhtpHttpServer::SanityCheck() {
    LOG_IF(FATAL, handler_map_.empty()) << "handlers is null.before running must be regiester some handler in.";
    if (handler_router_vec_.empty()) {
        AddHandlerRouter(PathHandlerRouter::New());
        LOG(INFO) << "have not any HandlerRouter is exists,so use PathHandlerRouter is default.";
    }
    return true;
}

int EvhtpHttpServer::Run() {
    if (!SanityCheck()) {
        return 1;
    }
    evhtp_set_gencb(htp_, EvhtpHttpServer::Trampoline, this);
    std::string baddr ;
    uint16_t port = 0;
    if (!SplitListenSpec(listen_spec_, &baddr, &port)) {
      return 2;
    }
    if (evhtp_bind_socket(htp_, baddr.c_str(), port, kBackLog)) {
        LOG(ERROR) << "bind socket error:" << baddr.c_str() << ":" << port;
        return 3;
    }
    LOG(INFO) << "bind to socket ok:address=" << baddr << ", port=" << port;
    LOG(INFO) << "server is running ....";
    event_base_loop(evbase_, 0);
    LOG(INFO) << "server is exiting....";
    evhtp_unbind_socket(htp_);
    evhtp_free(htp_);
    event_base_free(evbase_);
    return 0;
}

void EvhtpHttpServer::Shutdown() {
    event_base_loopbreak(evbase_);
}

//必须在同一个线程里面执行
void EvhtpHttpServer::DoPostService(HttpRequest *request, HttpResponse * response) {
    EvhtpHttpRequest *evhtp_request = static_cast<EvhtpHttpRequest *>(request);
    EvhtpHttpResponse * evhtp_response = static_cast<EvhtpHttpResponse *>(response);
    evhtp_send_reply(evhtp_request->raw_request(), response->GetStatus());
    evhtp_request_resume(evhtp_request->raw_request());
    FreeEvhtpHttpRequest(evhtp_request);
    FreeEvhtpHttpResponse(evhtp_response);
}

inline static evthr_t * get_request_thr(evhtp_request_t * request) {
    evhtp_connection_t * htpconn = evhtp_request_get_connection(request);
    return htpconn->thread;
}

void EvhtpHttpServer::PostService(HttpRequest *request, HttpResponse * response) {
    EvhtpHttpRequest *evhtp_request = static_cast<EvhtpHttpRequest *>(request);
    evthr_t * thread = get_request_thr(evhtp_request->raw_request());
    assert(thread != NULL);
    worker_thread_pool_->Submit(thread, NewClosure(this, &EvhtpHttpServer::DoPostService, request, response));
}

HttpServer * NewEvhtpHttpServer(const std::string &listen_spec, uint32_t thread_num) {
    return new EvhtpHttpServer(listen_spec, thread_num);
}

}  // namespace http_server
}  // namespace common

