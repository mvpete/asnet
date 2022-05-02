#ifndef __asnet_context_h__
#define __asnet_context_h__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#include "error_code.h"

#include <functional>

namespace asnet
{

    /*
        asnet::context - encapsulates a io task queue, as well as a completion queue. Allowing the caller to control
        the threading of the competions.
    */
    class context
    {

    public:
        using fn_t = std::function<void(void *ctx)>;
        using completion_fn_t = std::function<void()>;

    public:
        struct task_t
        {
            task_t() = default;

            task_t(fn_t fn, void *ctx)
            :fn(fn), ctx(ctx)
            {
            }

            fn_t fn;
            void *ctx;
        };

    public:
        static const int QUEUE_SIZE = 32;

    public:
        context();
        context(context &ctx) = delete;
        context(context &&) = delete;
        context& operator=(const context&) = delete;
        context& operator=(const context&&) = delete;

        ~context();
        
        size_t poll_one(int ticks);
        void stop();


    public:
        error_code post(fn_t fn, void *ctx);
        error_code post(completion_fn_t fn);


    public:
        QueueHandle_t h_;

    };
}

#endif //__asnet_context_h__