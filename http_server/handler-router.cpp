#include "common/http_server/handler-router.h"
#include "common/http_server/http-request.h"

namespace common {
namespace http_server {

bool PathHandlerRouter::Route(HttpRequest &request, std::string *handler_name) {
    *handler_name = request.GetRequestURI();
    if (handler_name->empty()) {
        *handler_name = "/";
    }
    return true;
}

PathHandlerRouter * PathHandlerRouter::New() {
    return new PathHandlerRouter();
}

}  // namespace http_server
}  // namespace common
