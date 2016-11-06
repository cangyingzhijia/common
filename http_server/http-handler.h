//
// Author: cangyingzhijia@126.com
// Date: 2014-06-25
//
//
#ifndef HTTP_SERVER_HTTP_HANDLER_H_
#define HTTP_SERVER_HTTP_HANDLER_H_

#include "common/http_server/fwd.h"

namespace common {
namespace http_server {

class HttpHandler {
public:
    virtual ~HttpHandler() {}
    virtual  void Run(HttpRequest *request, HttpResponse *response, Closure *done) = 0;
};

class DefaultHttpHandler: public HttpHandler {
public:
    virtual  void Run(HttpRequest *request, HttpResponse *response, Closure *done);
};

inline DefaultHttpHandler * NewDefaultHttpHandler() {
    return new DefaultHttpHandler();
}

}  // namespace http_server
}  // namespace common

#endif  //HTTP_SERVER_HTTP_HANDLER_H_ 
