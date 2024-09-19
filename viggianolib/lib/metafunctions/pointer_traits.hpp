#pragma once
namespace mpv{
    class undefined;
    template<typename T>
    struct get_first_arg{
        using type=undefined;
    };
    template<template<typename,typename...> class Container,typename T,typename... Args>
    struct get_first_arg<Container<T,Args...>>{
        using type=T;
    };
    template<typename T> using get_first_arg_t=typename get_first_arg<T>::type;

    template<typename,typename>
    struct replace_first_arg{};
    template<template<typename,typename...> class Container,typename T,typename... Args,typename U>
    struct replace_first_arg<Container<T,Args...>,U>{
        using type=Container<U,Args...>;
    };
	template<typename T,typename U> using replace_first_arg_t=typename replace_first_arg<T,U>::type;

    template<typename T> using make_not_void=If_t<is_void_v<T>,undefined,T>;

    template<typename Pointer>
    struct pointer_traits{
        private:
            template<typename T,typename=void> struct __element_type{
                using type=get_first_arg_t<T>;
            };
            template<typename T> struct __element_type<T,void_t<typename T::element_type>>{
                using type=typename T::element_type;
            };
            template<typename T,typename=void> struct __difference_type{
                using type=ptrdiff_t;
            };
            template<typename T> struct __difference_type<T,void_t<typename T::difference_type>>{
                using type=typename T::difference_type;
            };
            template<typename T,typename U,typename=void> struct __rebind{
                using type=replace_first_arg_t<T,U>;
            };
            template<typename T,typename U> struct __rebind<T,U,void_t<typename T::template rebind<U>>>{
                using type=typename T::template rebind<U>;
            };
        public:
            using pointer=Pointer;
            using element_type=typename __element_type<Pointer>::type;
            using difference_type=typename __difference_type<Pointer>::type;
            template<typename U> using rebind=typename __rebind<Pointer,U>::type;
            static pointer pointer_to(make_not_void<element_type>& x){
                return Pointer::pointer_to(x);
            }
            static_assert(!is_same_v<element_type,undefined>,"element_type cannot be undefined");
    };
    template<typename T>
    struct pointer_traits<T*>{
        using element_type=T;
        using pointer=T*;
        using difference_type=ptrdiff_t;
        template<typename U> using rebind=U*;
        static constexpr pointer pointer_to(make_not_void<element_type>& x)noexcept{
            return __builtin_addressof(x);
        }
    };
    template<typename Pointer,typename T>
    using rebind_pointer=typename pointer_traits<Pointer>::template rebind<T>;
    
}