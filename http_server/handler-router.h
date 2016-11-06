//
// Author: cangyingzhijia@126.com
// Date: 2014-06-25
//
//
#ifndef HTTP_SERVER_HANDLER_ROUTER_
#define HTTP_SERVER_HANDLER_ROUTER_

#include "common/http_server/fwd.h"

namespace common {
namespace http_server {

class HandlerRouter {
public:
    virtual ~HandlerRouter() {}
    virtual bool Route(HttpRequest &request, std::string *handler_name) = 0;
};

// 通过uri路径进行route
class PathHandlerRouter : public HandlerRouter {
public:
    virtual bool Route(HttpRequest &request, std::string *handler_name);
    static PathHandlerRouter * New();
};

}  // namespace http_server
}  // namespace common

#endif  // HTTP_SERVER_HANDLER_ROUTER_
