#include "common/http_server/evhtp-http-request.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <vector>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

namespace common {
namespace http_server {

int EvhtpKvsIteratorSetMap(evhtp_kv_t * kv, void * arg) {
    std::map<std::string, std::string> *store_map = static_cast<std::map<std::string, std::string> * >(arg);
    std::string key(kv->key, kv->klen);
    std::string value(kv->val, kv->vlen);
    store_map->insert(std::make_pair(key, value));
    return 0;
}

inline bool EvhtpHttpRequest::ParseNormalPostParameter(const char *post_data, int len) {
    // parse arguments in post data
    evhtp_query_t  * query = evhtp_parse_query(const_cast<char *>(post_data), len);
    if (query != NULL && query != NULL) {
        evhtp_kvs_for_each(query, EvhtpKvsIteratorSetMap, &parameter_map_);
    }
    evhtp_kvs_free(query);
    return true;
}


inline bool EvhtpHttpRequest::ParseMultipartPostParameter(const char *post_data, const char *boundary) {
    char *data = strdup(post_data);
    char * p = strstr(data, boundary);
    int boundary_len = strlen(boundary);
    char *part_end = p;
    if (p == NULL) {
        goto out;
    }
    do {
        char * part_begin = part_end + boundary_len;
        part_end = strstr(part_begin, boundary);
        if (part_end == NULL) {
            break;
        }
        *(part_end - 4)= '\0'; //减去前面的\r\n
        p = strstr(part_begin, "name=\"");
        if (p == NULL) {
            continue;
        }
        char *name = p + strlen("name=\"") ;
        p = strstr(name, "\"");
        if (p == NULL) {
            continue;
        }
        *p++ = '\0';
        std::string key(name);
        p = strstr(p, "\r\n\r\n");
        if (p == NULL) {
            continue;
        }
        p = p + 4; //point value
        std::string value(p, part_end - 4);
        parameter_map_.insert(make_pair(key, value));
    } while (true);
out:
    free(data);
    return true;
}

inline bool EvhtpHttpRequest::ParsePostParameter() {
    const std::string &post_data= GetRawPostData();
    if (post_data.size() < 2) {
        return false;
    }
    const std::string &content_type = GetHeader("Content-Type");
    std::string::size_type idx = content_type.find("multipart/form-data");
    if (idx == std::string::npos) {
        return ParseNormalPostParameter(content_type.c_str(), content_type.size());
    }
    idx = content_type.find("boundary=");
    if (idx == std::string::npos) {
        return false;
    }
    std::string boundary = content_type.substr(idx + strlen("boundary="));
    return ParseMultipartPostParameter(post_data.c_str(), boundary.c_str());
}

inline void EvhtpHttpRequest::ParseParameters() {
    if (! is_parameter_parsed_) {
        parameter_map_.clear();
        //parse get parameter
        if (raw_request_ ->uri != NULL && raw_request_->uri->query != NULL) {
            evhtp_kvs_for_each(raw_request_->uri->query, EvhtpKvsIteratorSetMap, &parameter_map_);
        }
        //parse post parameter
        ParsePostParameter();
        is_parameter_parsed_ = true;
    }
}

inline void EvhtpHttpRequest::ParseCookies() {
    if (!is_cookie_parsed_) {
        cookie_map_.clear();
        is_cookie_parsed_ = true;
        const std::string &cookie_str = GetHeader("Cookie");
        if (cookie_str.empty()) {
            return;
        }
        std::vector<std::string> vec;
        std::string key;
        std::string value;
        boost::split(vec, cookie_str, boost::is_any_of(";"));
        BOOST_FOREACH(std::string &cookie, vec) {
            std::string::size_type idx = cookie.find("=");
            if (idx != std::string::npos) {
                key = cookie.substr(0, idx);
                value = cookie.substr(idx + 1);
                boost::trim(key);
                boost::trim(value);
                cookie_map_.insert(make_pair(key, value));
            }
        }
    }
}

inline void EvhtpHttpRequest::ParseHeaders() {
    if (!is_header_parsed_) {
        header_map_.clear();
        if (raw_request_->headers_in != NULL) {
            evhtp_kvs_for_each(raw_request_->headers_in, EvhtpKvsIteratorSetMap, &header_map_);
        }
        is_header_parsed_ = true;
    }
}

inline void EvhtpHttpRequest::ParseRequestURI() {
    if (! is_request_uri_parsed_) {
        request_uri_.clear();
        if (raw_request_ ->uri != NULL && raw_request_->uri->path!= NULL) {
            request_uri_ = raw_request_->uri->path->full;
        }
        is_request_uri_parsed_ = true;
    }
}

inline void EvhtpHttpRequest::ParseQueryString() {
    if (! is_query_string_parsed_) {
        query_string_.clear();
        if (raw_request_ ->uri != NULL && raw_request_ ->uri->query_raw != NULL) {
            query_string_ = reinterpret_cast<char *>(raw_request_ ->uri->query_raw);
        }
        is_query_string_parsed_ = true;
    }
}

static std::string ConvertSockaddrToString(struct sockaddr  * sa) {
    struct sockaddr_in  *sin = reinterpret_cast<struct sockaddr_in*>(sa);
    char buff[64] = "";
    switch(sin->sin_family) {
        case AF_INET:{
            inet_ntop(AF_INET, &sin->sin_addr, buff, sizeof(buff));
            break;
        }
        case AF_INET6:{
            struct sockaddr_in6 *sin6 = reinterpret_cast<struct sockaddr_in6 *>(sa);
            inet_ntop(AF_INET6, &sin6->sin6_addr, buff, sizeof(buff));
            break;
        }
        case AF_UNIX:{
            struct sockaddr_un *sun = reinterpret_cast<struct sockaddr_un *>(sa);
            strncpy(buff, sun->sun_path, sizeof(buff) > sizeof(sun->sun_path) ? sizeof(buff) : sizeof(sun->sun_path));
            break;
        }
        default:
            break;
    }
    return std::string(buff);
}

inline void EvhtpHttpRequest::ParseRemoteHost() {
    if (! is_remote_host_parsed_) {
        remote_host_.clear();
        is_remote_host_parsed_ = true;

        remote_host_ = GetHeader("X-Forwarded-For");
        if (! remote_host_.empty()) {
            return;
        }
        remote_host_ = GetHeader("HTTP_NS_CLIENT_IP");
        if (! remote_host_.empty()) {
            return;
        }
        remote_host_ = GetHeader("HTTP_X_FORWARDED_FOR");
        if (! remote_host_.empty()) {
            return;
        }
        remote_host_ = GetHeader("X-Real-IP");
        if (! remote_host_.empty()) {
            return;
        }
        if (raw_request_->conn != NULL && 
                raw_request_->conn->saddr != NULL) {
            remote_host_ = ConvertSockaddrToString(raw_request_->conn->saddr);
        }
    }
}

inline void EvhtpHttpRequest::ParseRawPostData() {
    if (! is_raw_post_data_parsed_) {
        raw_post_data_.clear();
        if (raw_request_->buffer_in != NULL) {
            size_t len = evbuffer_get_length(raw_request_->buffer_in);
            raw_post_data_.resize(len);
            evbuffer_copyout(raw_request_->buffer_in, &(raw_post_data_[0]), len);
        }
        is_raw_post_data_parsed_ = true;
    }
}

// prameters
const std::string & EvhtpHttpRequest::GetParameter(const std::string &name) {
    ParseParameters();
    std::map<std::string, std::string>::iterator mit = parameter_map_.find(name);
    if (mit != parameter_map_.end()) {
        return mit->second;
    }
    return kEmptyString;
}

const std::map<std::string, std::string>& EvhtpHttpRequest::GetParameterMap() {
    ParseParameters();
    return parameter_map_;
}

// cookies
const std::string & EvhtpHttpRequest::GetCookie(const std::string &name) {
    ParseCookies();
    std::map<std::string, std::string>::iterator mit = cookie_map_.find(name);
    if (mit != cookie_map_.end()) {
        return mit->second;
    }
    return kEmptyString;
}

const std::map<std::string, std::string>& EvhtpHttpRequest::GetCookieMap() {
    ParseCookies();
    return cookie_map_;
}

// headers
const std::string & EvhtpHttpRequest::GetHeader(const std::string &name) {
    ParseHeaders();
    std::map<std::string, std::string>::iterator mit = header_map_.find(name);
    if (mit != header_map_.end()) {
        return mit->second;
    }
    return kEmptyString;
}

const std::map<std::string, std::string>& EvhtpHttpRequest::GetHeaderMap() {
    ParseHeaders();
    return header_map_;
}

// other
const std::string & EvhtpHttpRequest::GetRequestURI() {
    ParseRequestURI();
    return request_uri_;
}

const std::string & EvhtpHttpRequest::GetQueryString() {
    ParseQueryString();
    return query_string_;
}

const std::string & EvhtpHttpRequest::GetRemoteHost() {
    ParseRemoteHost();
    return remote_host_;
}

const std::string & EvhtpHttpRequest::GetRawPostData() {
    ParseRawPostData();
    return raw_post_data_;
}

HttpServer * EvhtpHttpRequest::GetHttpServer() {
    return http_server_;
}
 
void EvhtpHttpRequest::Reset(evhtp_request_t *evhtp_request,HttpServer * http_server) {
    is_parameter_parsed_ = false;
    is_cookie_parsed_ = false;
    is_header_parsed_ = false;
    is_request_uri_parsed_ = false;
    is_query_string_parsed_ = false;
    is_remote_host_parsed_ = false;
    is_raw_post_data_parsed_ = false;
    raw_request_ = evhtp_request;
    http_server_ = http_server;
}

}  // namespace http_server
}  // namespace common 

