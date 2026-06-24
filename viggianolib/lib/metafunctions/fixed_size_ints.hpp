#pragma once
namespace mpv{
    namespace fixedsizeint{
        using uint8_t=unsigned char;
        using int8_t=signed char;
        using uint16_t=unsigned short;
        using int16_t=signed short;
        using uint32_t=If_t<sizeof(unsigned int)==4,unsigned int,unsigned long>;
        using int32_t=If_t<sizeof(signed int)==4,signed int,signed long>;
        using uint64_t=If_t<sizeof(unsigned long)==8,unsigned long,unsigned long long>;
        using int64_t=If_t<sizeof(signed long)==8,signed long,signed long long>;
    #ifndef IGNORE_FIXEDINT_ASSERTS
        static_assert(sizeof(uint8_t)==1 && sizeof(int8_t)==1);
        static_assert(sizeof(uint16_t)==2 && sizeof(int16_t)==2);
        static_assert(sizeof(uint32_t)==4 && sizeof(int32_t)==4);
        static_assert(sizeof(uint64_t)==8 && sizeof(int64_t)==8);
    #endif        
    }
}