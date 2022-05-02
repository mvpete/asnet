#ifndef __asnet_error_code_h__
#define __asnet_error_code_h__

namespace asnet
{
    class error_code
    {
    public:
        error_code()
        :value_(0)
        {
        }

        error_code(int value)
        :value_(value)
        {
        }

        int value() 
        {
            return value_;
        }

        operator bool()
        {
            return value_!=0;
        }

    private:
        int value_;
    };
}


#endif //__asnet_error_code_h_