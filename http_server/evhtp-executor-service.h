//
// Author: cangyingzhijia@126.com
// Date: 2014-06-25
//
//
#ifndef HTTP_SERVER_EVHTP_EXECUTOR_SERVICE_H_
#define HTTP_SERVER_EVHTP_EXECUTOR_SERVICE_H_

#include "common/base/closure.h"
#include "common/base/executor-service.h"
#include "common/http_server/evhtp/evthr.h"

namespace common {
namespace http_server {

class EvhtpExecutorService : public common::ExecutorService{
public:
    EvhtpExecutorService(int thread_num);

    EvhtpExecutorService(evthr_pool_t * thr_pool, bool is_own_thr_pool = false);

    virtual ~EvhtpExecutorService();
    
     //提交一个 closure任务用于执行
    virtual void Submit(Closure* closure);

    void Submit(evthr_t * thread, Closure* closure);

    // 启动一次顺序关闭，执行以前提交的任务，但不接受新任务
    virtual void Shutdown();

    // 如果此执行程序已关闭，则返回 true
    virtual bool IsShutdown();
private:
    evthr_pool_t * thr_pool_; 
    bool is_own_thr_pool_;
    bool is_shutdown_;
};

}  // namespace http_server
}  // namespace common

#endif // HTTP_SERVER_EVHTP_EXECUTOR_SERVICE_H_
