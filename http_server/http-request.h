//
// Author: cangyingzhijia@126.com
// Date: 2014-06-25
//
//
#ifndef HTTP_SERVER_HTTP_REQUEST_H_
#define HTTP_SERVER_HTTP_REQUEST_H_

#include "common/http_server/fwd.h"

namespace common {
namespace http_server {

class HttpRequest {
public:
    virtual ~HttpRequest() {}
    // prameters
    virtual const std::string & GetParameter(const std::string &name) = 0;
    virtual const std::map<std::string, std::string>& GetParameterMap() = 0;
    // cookies
    virtual const std::string & GetCookie(const std::string &name) = 0;
    virtual const std::map<std::string, std::string>& GetCookieMap() = 0;
    // headers
    virtual const std::string & GetHeader(const std::string &name) = 0;
    virtual const std::map<std::string, std::string>& GetHeaderMap() = 0;
    //other
    virtual const std::string & GetRequestURI() = 0;
    virtual const std::string & GetQueryString() = 0;
    virtual const std::string & GetRemoteHost() = 0;
    virtual const std::string & GetRawPostData() = 0;
    // http server
    virtual HttpServer * GetHttpServer() = 0;
};

}  // end namespace http_server
}  // namespace common

#endif  // HTTP_SERVER_HTTP_REQUEST_H_
