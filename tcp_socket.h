#ifndef __asnet_tcp_socket_h__
#define __asnet_tcp_socket_h__

#include "context.h"
#include "buffer.h"
#include <Ethernet.h>
#include "lwip/tcp.h"
#include "freertos/stream_buffer.h"

#include <functional>

namespace asnet
{
namespace tcp
{
    class socket
    {

        
    public:
        using handler_t = std::function<void(error_code, size_t)>;
        static int max_socket_buf;
    public:
        socket(context &ctx);
        ~socket();

    public:
        void async_connect(IPAddress ipv4, uint16_t port, handler_t handler);
        void async_read_some(const buffer &b, handler_t handler);
        void async_write_some(const cbuffer &b, handler_t handler);

        void async_close(handler_t);
        void abort();

    private:

        static err_t async_connect_complete(void *arg, tcp_pcb *pcb, err_t err);
        static void  async_err(void *arg, err_t err);
        static void  async_recv_(void * arg, struct tcp_pcb * pcb, struct pbuf *pb, int8_t err);
        static void  async_sent_(void * arg, struct tcp_pcb * pcb, uint16_t len);
        static void  async_poll(void *arg, struct tcp_pcb * pcb);

        void async_recv(struct tcp_pcb * pcb, struct pbuf *pb, int8_t err);
        void async_sent(struct tcp_pcb * pcb, uint16_t len);
    private:
        handler_t op_;
        struct 
        {
            buffer b;
            handler_t h;
        } prd_;

        StreamBufferHandle_t rbuf_;

    private:
        context &ctx_;
        tcp_pcb *pcb_;
    };
}
}

#endif // __asnet_tcp_socket_h__