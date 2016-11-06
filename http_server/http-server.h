//
// Description:
//     一个异步http server框架的实现，纯异步操作，
//     在实现handler的时候不应该出现同步等待的情况
//
// Author: cangyingzhijia@126.com
// Date: 2014-06-25
//
//
#ifndef HTTP_SERVER_HTTP_SERVER_H_
#define HTTP_SERVER_HTTP_SERVER_H_ 

#include "common/base/executor-service.h"
#include "common/http_server/handler-router.h"
#include "common/http_server/http-handler.h"
#include "common/http_server/http-request.h"
#include "common/http_server/http-response.h"
#include "common/http_server/http-status.h"

namespace common {
namespace http_server {

class HttpServer {
public:
    HttpServer();
    virtual ~HttpServer();

    bool AddHandlerRouter(HandlerRouter *handler_router);

    // 注册handler
    // @param name handler名字，不能重复
    // @param handler
    // @return 1:name empty; 1:name handler aready exists; 0:success
    int RegisterHandler(const std::string & name, HttpHandler* handler);

    // 轮询接收请求和对请求进行处理
    virtual int Run() = 0;

    // 停止http服务
    virtual void Shutdown() = 0;

    void SetDefaultHttpHandler(HttpHandler *new_http_handler);

    // 获取工作线程池对象，可以往此线程池里面提交任务执行
    virtual common::ExecutorService * GetThreadPool() = 0;

protected:
    // 结束服务，返回处理给客户端,如调用evhtp_send_reply
    virtual void PostService(HttpRequest *request, HttpResponse * response) = 0;
    
    // 对http request进行处理,具体做法是把请求转发给Handler来处理
    // @param request
    // @response
    void Service(HttpRequest *request, HttpResponse * response);
    HttpHandler* FindHttpHandler(HttpRequest *request);
protected:
    std::map<std::string, HttpHandler*> handler_map_;
    HttpHandler *default_http_handler_;
    std::vector<HandlerRouter *> handler_router_vec_;
};

// create a evhtp http server object
// @param listen_spec format is# ipv4:xxx:port ipv6:xxxx:port unix:xxxxx
// @param thread_num worker thread number
HttpServer * NewEvhtpHttpServer(const std::string &listen_spec, uint32_t thread_num = 0);

}  // namespace http_server

// export to common namespace
using http_server::HttpServer;
using http_server::HttpRequest;
using http_server::HttpResponse;
using http_server::HttpHandler;
using http_server::HandlerRouter;
using http_server::HttpStatus;
using http_server::NewEvhtpHttpServer;

}  // namespace common

#endif  // HTTP_SERVER_HTTP_SERVER_H_
