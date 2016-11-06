#ifndef HTTP_SERVER_FWD_H_
#define HTTP_SERVER_FWD_H_

#include <string>
#include <vector>
#include <map>

#include "common/base/closure.h"
#include "common/http_server/http-status.h"

namespace common {
namespace http_server {

const static std::string kEmptyString;

class HttpRequest;
class HttpResponse;
class HttpServer;
class HandlerRouter;
class HttpHandler;

}  // namespace http_server
}  // namespace common

#endif  // HTTP_SERVER_FWD_H_
