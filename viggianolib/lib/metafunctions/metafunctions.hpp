#pragma once
#include"struct_operators.hpp"
#include"type_traits.hpp"
#include"fixed_size_ints.hpp"
#include"special_member_function_control.hpp"
#include"allocator_traits.hpp"
#include"iterator_traits.hpp"

namespace mpv{
    template<typename T,typename=void> struct is_trivially_serializable_container:false_type{};
    template<typename Cont> struct is_trivially_serializable_container<Cont,void_t<typename Cont::iterator>>:bool_constant<is_trivially_copyable_v<remove_reference_t<iter_ref_t<typename Cont::iterator>>>>{};
    template<typename T> static constexpr bool is_trivially_serializable_container_v = is_trivially_serializable_container<T>::value;

    template<typename T,typename=void> struct has_reserve:false_type{};
    template<typename T> struct has_reserve<T,void_t<decltype(mpv::declval<T>().reserve(mpv::declval<typename T::size_type>()))>>:true_type{};
    template<typename T> constexpr bool has_reserve_v=has_reserve<T>::value;

    template<typename T,typename=void> struct has_push_back:false_type{};
    template<typename T> struct has_push_back<T,void_t<decltype(mpv::declval<T>().push_back(mpv::declval<typename T::value_type>()))>>:true_type{};
    template<typename T> constexpr bool has_push_back_v=has_push_back<T>::value;

    template<typename T,typename=void> struct has_get_root:false_type{};
    template<typename T> struct has_get_root<T,void_t<decltype(mpv::declval<T>().get_root())>>:true_type{};
    template<typename T> constexpr bool has_get_root_v=has_get_root<T>::value;

    template<typename I,bool=false,typename=void> struct elements_are_movable_helper:false_type{};
    template<typename I> struct elements_are_movable_helper<I,true,void_t<decltype(static_cast<typename remove_reference_t<I>::value_type&&>(*(declval<I>().begin())))>>:true_type{};
    template<typename I> struct elements_are_movable:elements_are_movable_helper<I,is_rvalue_ref_v<I>>::type{};
    template<typename I> static constexpr bool elements_are_movable_v=elements_are_movable<I>::value;//checks if elements of an iterable class are movable

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