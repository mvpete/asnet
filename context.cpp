#include "context.h"

using namespace asnet;

context::context()
:h_(xQueueCreate(context::QUEUE_SIZE, sizeof(context::task_t*)))
{
}

context::~context()
{
    stop();
}

size_t context::poll_one(int ticks)
{
    if(h_==nullptr)
        return 0;

    int processed=0;
    task_t *tsk=nullptr;
    while(xQueueReceive(h_, &tsk, (TickType_t)ticks) == pdPASS)
    {
        tsk->fn(tsk->ctx);
        delete tsk;
        ++processed;
    }

    return processed;
}

void context::stop()
{
    xQueueReset(h_);
    h_=nullptr;
}

error_code context::post(fn_t fn, void *ctx)
{
    if(h_==nullptr)
        return error_code(-1);
    
    task_t *tsk = new task_t(fn, ctx);
    xQueueSend(h_, (void*)&tsk, (TickType_t)0);

    return error_code{};
}

error_code context::post(completion_fn_t fn)
{
    if(h_==nullptr)
        return error_code(-1);
    
    task_t *tsk = new task_t([fn](void*)
    {
        fn();
    }, nullptr);
    xQueueSend(h_, (void*)&tsk, (TickType_t)0);

    return error_code{};
}