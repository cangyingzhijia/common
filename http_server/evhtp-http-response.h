//
// Author: cangyingzhijia@126.com
// Date: 2014-06-25
//
//
#ifndef HTTP_SERVER_EVHTP_HTTP_RESPONSE_H_
#define HTTP_SERVER_EVHTP_HTTP_RESPONSE_H_

#include "common/http_server/evhtp/evhtp.h"
#include "common/http_server/http-response.h"

namespace common {
namespace http_server {

class EvhtpHttpResponse : public HttpResponse{
public:
    EvhtpHttpResponse():
        evhtp_request_(NULL){ }

    virtual ~EvhtpHttpResponse() {
        evhtp_request_ = NULL;
    }

    virtual void SetStatus(HttpStatus status);

    virtual HttpStatus GetStatus() {
        return status_;
    }

    virtual void SetResponse(const std::string &response);

    virtual void AddHeader(const std::string &name, const std::string &value);

    void Reset(evhtp_request_t *evhtp_request);
private:
    evhtp_request_t *evhtp_request_;
    HttpStatus status_;
};

inline EvhtpHttpResponse * NewEvhtpHttpResponse() {
    return new EvhtpHttpResponse();
}

inline void FreeEvhtpHttpResponse(EvhtpHttpResponse *response) {
    delete response;
}

}  // namespace http_server
}  // namespace common

#endif //HTTP_SERVER_EVHTP_HTTP_RESPONSE_H_
