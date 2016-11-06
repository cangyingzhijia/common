#include "common/http_server/evhtp-executor-service.h"

namespace common {
namespace http_server {

EvhtpExecutorService::EvhtpExecutorService(int thread_num) {
    thr_pool_ = evthr_pool_new(thread_num, NULL, NULL);
    is_own_thr_pool_ = true;
    is_shutdown_ = false;
}

EvhtpExecutorService::EvhtpExecutorService(evthr_pool_t * thr_pool, bool is_own_thr_pool):
    thr_pool_(thr_pool),
    is_own_thr_pool_(is_own_thr_pool),
    is_shutdown_(false) {
}

EvhtpExecutorService::~EvhtpExecutorService() {
    Shutdown();
}

static void EvthrCallBack(evthr_t * thr, void * cmd_arg, void * shared) {
    Closure* closure = static_cast<Closure *>(cmd_arg);
    closure->Run();
}

void EvhtpExecutorService::Submit(evthr_t * thread, Closure* closure) {
    assert(thread != NULL);
    assert(thr_pool_ != NULL);
    evthr_defer(thread, EvthrCallBack, closure);
}

void EvhtpExecutorService::Submit(Closure* closure) {
    assert(thr_pool_ != NULL);
    evthr_pool_defer(thr_pool_, EvthrCallBack, closure);
}

void EvhtpExecutorService::Shutdown() {
    if (! is_shutdown_) {
        is_shutdown_ = true;
        if (is_own_thr_pool_) {
            evthr_pool_stop(thr_pool_);
            evthr_pool_free(thr_pool_);
        }
        thr_pool_ = NULL;
        is_own_thr_pool_ = false;
    }
}

bool EvhtpExecutorService::IsShutdown() {
    return is_shutdown_;
}

}  // namespace http_server
}  // namespace common
