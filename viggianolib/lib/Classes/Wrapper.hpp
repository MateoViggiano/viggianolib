#pragma once
namespace mpv{
    template<typename T>
    struct Wrapped{
        T val;
        Wrapped(T val):val(val){}
        operator T&(){
            return this->val;
        }
        Wrapped()=default;
    };
}

