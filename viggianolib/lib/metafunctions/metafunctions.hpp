#pragma once
#include"type_traits.hpp"
#include"special_member_function_control.hpp"
#include"allocator_traits.hpp"
namespace mpv{
    template<ssize_t Base,unsigned Exp> struct static_power           :integral_constant<ssize_t,static_power<Base,Exp-1>::value*Base>{};
    template<ssize_t Base>              struct static_power<Base,0>   :integral_constant<ssize_t,1>{};
    template<ssize_t Base,unsigned Exp> constexpr ssize_t static_power_v=static_power<Base,Exp>::value;

    template<size_t n> struct static_factorial   :integral_constant<size_t,static_factorial<n-1>::value*n>{};
    template<>         struct static_factorial<0>:integral_constant<size_t,1>{};
    template<size_t n> static constexpr size_t static_factorial_v=static_factorial<n>::value;


    template<size_t n> struct static_fibonacci   :integral_constant<size_t,static_fibonacci<n-1>::value+static_fibonacci<n-2>::value>{};
    template<>         struct static_fibonacci<0>:integral_constant<size_t,0>{};
    template<>         struct static_fibonacci<1>:integral_constant<size_t,1>{};
    template<size_t n> constexpr size_t static_fibonacci_v=static_fibonacci<n>::value;
}