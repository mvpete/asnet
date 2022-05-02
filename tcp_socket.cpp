#include "tcp_socket.h"

using namespace asnet::tcp;

int socket::max_socket_buf = 1024;

socket::socket(context &ctx)
:ctx_(ctx), 
pcb_(tcp_new_ip_type(IPADDR_TYPE_V4)),
rbuf_(xStreamBufferCreate(max_socket_buf,1))
{

}

socket::~socket()
{
    abort();
}

void socket::abort()
{
    tcp_abort(pcb_);
}

void socket::async_connect(IPAddress ip, uint16_t port, handler_t handler)
{
    if(pcb_==nullptr)
    {
        ctx_.post([handler]()
        {
            handler(error_code(-99),0);
        });
        return;    
    }

    ip_addr_t addr;
    addr.type = IPADDR_TYPE_V4;
    addr.u_addr.ip4.addr = ip;

    op_=handler;

    tcp_arg(pcb_, this);
    tcp_err(pcb_, (tcp_err_fn)&socket::async_err);
    tcp_sent(pcb_, (tcp_sent_fn)&socket::async_sent_);
    tcp_recv(pcb_, (tcp_recv_fn)&socket::async_recv_);
    tcp_poll(pcb_, (tcp_poll_fn)&socket::async_poll, 1);
    err_t e = tcp_connect(pcb_, &addr, port, (tcp_connected_fn)&socket::async_connect_complete);

    if(e!=ERR_OK)
    {
        ctx_.post([handler, e]()
        {
            handler(error_code((int)e),0);
        });
    }
}

void socket::async_close(handler_t h)
{
    // TODO: error handling
    err_t ec = tcp_close(pcb_);
    // .. what todo
    ctx_.post([h](void*)
    {
        h(error_code{},0);
    },nullptr);
}

void socket::async_read_some(const buffer &b, handler_t h)
{
    size_t av = xStreamBufferBytesAvailable(rbuf_);
    if(av > 0)
    {
        size_t s = min(av,b.size);
        xStreamBufferReceive(rbuf_,b.data,s,10);
        ctx_.post([h, s]()
        {
            h(error_code{}, s);
        });
    }
    else
    {
        prd_.b=b;
        prd_.h=h;
    }
}

void socket::async_write_some(const cbuffer &b, handler_t handler)
{
    // TODO: operation in progress.

    pwt_=handler;
    tcp_write(pcb_,b.data,b.size,0);
    tcp_output(pcb_);
}


err_t socket::async_connect_complete(void *arg, tcp_pcb *pcb, err_t err)
{
    socket *s = (socket*)arg;
    
    auto op = s->op_;
    s->op_=nullptr;
    s->ctx_.post([op]()
    {
        op(error_code{},0);
    });
    return err_t{};
}

void socket::async_err(void *arg, err_t err)
{
    socket *s = (socket*)arg;
    if(s->op_)
    {
        auto op = s->op_;
        s->op_=nullptr;
        s->ctx_.post([op, err]()
        {
            op(error_code((int)err),0);
        });
    }
    else if (s->pwt_)
    {
        auto op = s->pwt_;
        s->pwt_=nullptr;
        s->ctx_.post([op, err]()
        {
            op(error_code((int)err),0);
        });
    }
}

void  socket::async_recv_(void * arg, struct tcp_pcb * pcb, struct pbuf *pb, int8_t err)
{
   socket *s = (socket*)arg;
   s->async_recv(pcb,pb,err);
}

void socket::async_recv(struct tcp_pcb * pcb, struct pbuf *pb, int8_t err)
{
    const uint8_t *dp=(const uint8_t*)pb->payload;
    size_t len = pb->len;
    if(prd_.h!=nullptr)
    {
        size_t s = min(prd_.b.size,len);
        memcpy(prd_.b.data,pb->payload,s);
        dp+=s;
        len=len-s;

        auto h = prd_.h;
        ctx_.post([h, s]()
        {
            h(error_code{},s);
        });
        prd_.b.data=nullptr;
        prd_.b.size=0;
        prd_.h=nullptr;
    }
    if(len>0)
    {
        xStreamBufferSend(rbuf_,dp,len,10);
    }
    tcp_recved(pcb,pb->len);
}

void socket::async_sent_(void * arg, struct tcp_pcb * pcb, uint16_t len)
{
    ((socket *)arg)->async_sent(pcb,len);
}

void socket::async_sent(struct tcp_pcb * pcb, uint16_t len)
{
    if(pwt_ == nullptr)
        return;
    auto pwt=pwt_;
    ctx_.post([pwt, len]()
    {
        pwt(error_code{}, len);
    });
    pwt_=nullptr;
}

void  socket::async_poll(void *arg, struct tcp_pcb * pcb)
{
}

