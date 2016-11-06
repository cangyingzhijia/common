#include "common/http_server/http-handler.h"
#include "common/http_server/http-request.h"
#include "common/http_server/http-response.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"

DEFINE_string(health_check_uri, "/health_check", "health check uri");
DEFINE_string(health_check_file_path, "/tmp/health_check", "health check file path");

namespace common {
namespace http_server {

const static char * kResponseMessage = "have no handler to process this request";

void DefaultHttpHandler::Run(HttpRequest *request, HttpResponse *response, Closure *done) {
    const std::string &uri = request->GetRequestURI();
    const std::string &query_string = request->GetQueryString();
    if (uri == "/favicon.ico") {
        done->Run();
        return;
    }
    if (uri == FLAGS_health_check_uri) {
       // health check
       response->SetStatus(HTTP_STATUS_OK); 
       if (access(FLAGS_health_check_file_path.c_str(), F_OK) == -1) {
          response->SetStatus(HTTP_STATUS_SERVUNAVAIL); 
       }
       done->Run();
       return;
    }
    response->SetStatus(HTTP_STATUS_NOTIMPL);
    if (query_string.empty()) {
        LOG(INFO) << kResponseMessage << "#query:" << request->GetRequestURI();
    } else {
        LOG(INFO) << kResponseMessage << "#query:" << request->GetRequestURI() << "?" << request->GetQueryString();
    }
    done->Run();
}

}  // namespace http_server
}  // namespace common
