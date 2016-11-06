#include "common/http_server/evhtp-http-response.h"

namespace common {
namespace http_server {

void EvhtpHttpResponse::Reset(evhtp_request_t * evhtp_request) {
    if (evhtp_request == NULL) { 
        return;
    }
    status_ = HTTP_STATUS_OK;
    evhtp_request_ = evhtp_request;
}

void EvhtpHttpResponse::SetStatus(HttpStatus status) {
    status_ = status;
}

void EvhtpHttpResponse::AddHeader(const std::string &name, const std::string &value) {
    evhtp_header_t *header = evhtp_header_new(name.c_str(), value.c_str(), 1, 1);
    evhtp_headers_add_header(evhtp_request_->headers_out, header);
}

void EvhtpHttpResponse::SetResponse(const std::string &response) {
    if (!response.empty()) {
        evbuffer_add(evhtp_request_->buffer_out, response.c_str(), response.size());
    }
}

}  // namespace http_server
}  // namespace common
