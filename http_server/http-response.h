//
// Author: cangyingzhijia@126.com
// Date: 2014-06-25
//
//
#ifndef HTTP_SERVER_HTTP_RESPONSE_H_
#define HTTP_SERVER_HTTP_RESPONSE_H_

#include "common/http_server/fwd.h"

namespace common {
namespace http_server {

class HttpResponse {
 public:
  virtual ~HttpResponse() {}
  // status
  virtual void SetStatus(HttpStatus status) = 0;
  virtual HttpStatus GetStatus() = 0;
  // response
  virtual void SetResponse(const std::string &response) = 0;
  //header
  virtual void AddHeader(const std::string &name, const std::string &value) = 0;
};

}  // namespace http_server

}  // namespace common

#endif  // HTTP_SERVER_HTTP_RESPONSE_H_
