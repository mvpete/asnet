#ifndef __asnet_timer_h__
#define __asnet_timer_h__

#include "context.h"
#include "freertos/timers.h"

#include "error_code.h"

#include <functional>

namespace asnet
{
    class timer
    {
    public:
        using handler_t = std::function<void(error_code)>;
    public:
        timer(context &ctx, int ms_delay);
        ~timer();


    public:
        void stop();
        void async_wait(handler_t on_expired);
    
    private:
        static void on_expired(TimerHandle_t timer);

    private:
        context &ctx_;
        TimerHandle_t timer_;
        handler_t on_expired_;
    };
}

#endif //__asnet_timer_h__