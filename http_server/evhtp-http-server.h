//
// Author: cangyingzhijia@126.com
// Date: 2014-06-25
//
#ifndef HTTP_SERVER_EVHTP_HTTP_SERVER_H_
#define HTTP_SERVER_EVHTP_HTTP_SERVER_H_ 

#include <vector>
#include <map>

#include "common/http_server/http-server.h"
#include "common/http_server/evhtp-executor-service.h"
#include "common/http_server/evhtp/evhtp.h"

namespace common {
namespace http_server {

class EvhtpHttpServer : public HttpServer {
    friend HttpServer * NewEvhtpHttpServer(const std::string &listen_spec, uint32_t thread_num);
public:
    virtual ~EvhtpHttpServer();
    virtual int Run();
    virtual void Shutdown();
    virtual common::ExecutorService * GetThreadPool();
private:
    //ipv4:xxxxxxx:port
    //ipv6:xxxxxxx:port
    //unix:xxxxx
    EvhtpHttpServer(const std::string &listen_spec, uint32_t thread_num);
    static void Trampoline(evhtp_request_t * req, void * arg);
    virtual void PostService(HttpRequest *request, HttpResponse * response);
    void DoPostService(HttpRequest *request, HttpResponse * response);
    void ProcessHttpRequest(evhtp_request_t * req);
    bool SanityCheck();
private:
    evbase_t * evbase_;
    evhtp_t  * htp_;
    std::string listen_spec_;
    uint32_t thread_num_;
    EvhtpExecutorService * worker_thread_pool_;

};

}  // namespace http_server
}  // namespace common

#endif  // HTTP_SERVER_EVHTP_HTTP_SERVER_H_
