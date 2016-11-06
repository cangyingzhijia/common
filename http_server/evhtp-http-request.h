//
// Author: cangyingzhijia@126.com
// Date: 2014-06-25
//
//
#ifndef HTTP_SERVER_EVHTP_HTTP_REQUEST_H_
#define HTTP_SERVER_EVHTP_HTTP_REQUEST_H_

#include "common/http_server/evhtp/evhtp.h"
#include "common/http_server/http-request.h"

namespace common {
namespace http_server {

class EvhtpHttpRequest : public HttpRequest{
public:
    EvhtpHttpRequest():
        raw_request_(NULL) { }

    ~EvhtpHttpRequest() {raw_request_ = NULL;}
    // prameters
    virtual const std::string & GetParameter(const std::string &name);
    virtual const std::map<std::string, std::string>& GetParameterMap();
    // cookies
    virtual const std::string & GetCookie(const std::string &name);
    virtual const std::map<std::string, std::string>& GetCookieMap();
    // headers
    virtual const std::string & GetHeader(const std::string &name);
    virtual const std::map<std::string, std::string>& GetHeaderMap();
    //other
    virtual const std::string & GetRequestURI();
    virtual const std::string & GetQueryString();
    virtual const std::string & GetRemoteHost();
    virtual const std::string & GetRawPostData();

    // http server
    virtual HttpServer * GetHttpServer();

    void Reset(evhtp_request_t *evhtp_request, HttpServer * http_server); 
    
    evhtp_request_t * raw_request() {
        return raw_request_;
    }
private:
    inline void ParseParameters();
    inline bool ParsePostParameter();
    inline bool ParseNormalPostParameter(const char *post_data, int len);
    inline bool ParseMultipartPostParameter(const char *post_data, const char *boundary);

    inline void ParseCookies();
    inline void ParseHeaders();
    inline void ParseRequestURI();
    inline void ParseQueryString();
    inline void ParseRemoteHost();
    inline void ParseRawPostData();
private:
    std::map<std::string, std::string> parameter_map_;
    std::map<std::string, std::string> cookie_map_;
    std::map<std::string, std::string> header_map_;
    std::string request_uri_;
    std::string query_string_;
    std::string remote_host_;
    std::string raw_post_data_;
    // parse flag
    bool is_parameter_parsed_;
    bool is_cookie_parsed_;
    bool is_header_parsed_;
    bool is_request_uri_parsed_;
    bool is_query_string_parsed_;
    bool is_remote_host_parsed_;
    bool is_raw_post_data_parsed_;

    evhtp_request_t *raw_request_;
    HttpServer * http_server_;
};

inline EvhtpHttpRequest * NewEvhtpHttpRequest() {
    return new EvhtpHttpRequest();
}

inline void FreeEvhtpHttpRequest(EvhtpHttpRequest * request) {
    delete request;
}

}  // namespace http_server
}  // namespace common
#endif //HTTP_SERVER_EVHTP_HTTP_REQUEST_H_
