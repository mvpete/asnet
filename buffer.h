#ifndef __asnet_buffer_h__
#define __asnet_buffer_h__

#include <string.h>

namespace asnet
{
    using byte = unsigned char;

    struct buffer
    {
        byte *data;
        size_t size;
    };

    struct cbuffer
    {
        const byte *data;
        size_t size;
    };

    inline buffer make_buffer(byte *b, size_t s)
    {
        return buffer{b,s};
    }

    inline cbuffer make_buffer(const byte*b, size_t s)
    {
        return cbuffer{b,s};
    }

    inline cbuffer make_buffer(const char *s)
    {
        return cbuffer{(const byte*)s,strlen(s)};
    }

    inline buffer make_buffer(char *s, size_t len)
    {
        return buffer{(byte*)s,len};
    }

}


#endif // __asnet_buffer_h__