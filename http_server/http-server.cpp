#include "common/http_server/http-server.h"
#include <assert.h>
#include <boost/foreach.hpp>
#include "common/http_server/handler-router.h"
#include "common/http_server/http-request.h"
#include "common/http_server/http-response.h"
#include "common/http_server/http-handler.h"

namespace common {
namespace http_server {

HttpServer::HttpServer():
    default_http_handler_(NULL) {
    SetDefaultHttpHandler(NewDefaultHttpHandler());
}

HttpServer::~HttpServer() {
    for(std::map<std::string, HttpHandler*>::iterator mit = handler_map_.begin();
            mit != handler_map_.end();  ++mit) {
        delete mit->second;
    }
    handler_map_.clear();

    BOOST_FOREACH(HandlerRouter * router, handler_router_vec_) {
        delete router;
    }
    handler_router_vec_.clear();
}

bool HttpServer::AddHandlerRouter(HandlerRouter *handler_router) {
    assert(handler_router != NULL);
    handler_router_vec_.push_back(handler_router);
    return true;
}

int HttpServer::RegisterHandler(const std::string & name, HttpHandler* handler) {
    assert(handler != NULL);
    if (name.empty()) {
        return 1;
    }
    if (handler_map_.find(name) != handler_map_.end()) {
        return 2;
    }
    handler_map_[name] = handler;
    return 0;
}

HttpHandler* HttpServer::FindHttpHandler(HttpRequest *request) {
    HttpHandler* http_handler = default_http_handler_;
    std::string handler_name;
    BOOST_FOREACH(HandlerRouter *router, handler_router_vec_) {
        handler_name.clear();
        if (! router->Route(*request, &handler_name)) {
            continue;
        }
        std::map<std::string, HttpHandler*>::iterator mit =  handler_map_.find(handler_name);
        if (mit != handler_map_.end()) {
            http_handler = mit->second;
            break;
        }
    }
    return http_handler;
}

void HttpServer::SetDefaultHttpHandler(HttpHandler *new_http_handler) {
    assert(new_http_handler != NULL);
    delete default_http_handler_;
    default_http_handler_ = new_http_handler;
}

void HttpServer::Service(HttpRequest *request, HttpResponse * response) {
    assert(request != NULL);
    assert(response != NULL);
    HttpHandler* handler = FindHttpHandler(request);
    Closure* done = NewClosure(this, &HttpServer::PostService, request, response);
    handler->Run(request, response, done);
}

}  // namespace http_server
}  // namespace common
