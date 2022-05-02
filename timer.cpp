#include "timer.h"

using namespace asnet;

timer::timer(context &ctx, int delay_ms)
:ctx_(ctx)
{
    timer_=xTimerCreate("asnet::timer",
                        pdMS_TO_TICKS(delay_ms),
                        false,
                        this,
                        timer::on_expired);
}

timer::~timer()
{
    stop();
}

void timer::async_wait(handler_t on_expired)
{
    if(xTimerIsTimerActive(timer_))
    {
        ctx_.post([on_expired](void *)
        {
            on_expired(error_code(-1));
        },nullptr);
        return;
    }
    on_expired_=on_expired;
    xTimerStart(timer_, pdMS_TO_TICKS(100));    
}

void timer::stop()
{
    xTimerStop(timer_, pdMS_TO_TICKS(100));
}

void timer::on_expired(TimerHandle_t tx)
{
    timer* t = (timer*)pvTimerGetTimerID(tx);
    if(t==nullptr)
        return;
    if(t->on_expired_)
    {
        auto expired = t->on_expired_;
        t->ctx_.post([expired](void*)
        {
            expired(error_code{});
        },nullptr);
    }
    t->on_expired_=nullptr;
}