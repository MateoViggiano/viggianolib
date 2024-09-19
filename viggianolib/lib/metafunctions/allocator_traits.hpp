#pragma once
#include"pointer_traits.hpp"
namespace mpv{
    template<typename T>
    class allocator{
        public:
            using From_primary=allocator;
            using value_type=T;
            using pointer=T*;
            using const_pointer=const T*;
            using reference=T&;
            using const_reference=const T&;
            using size_type=size_t;
            using difference_type=ptrdiff_t;
            using propagate_on_container_move_assignment=true_type;
            using is_always_equal=true_type;
            template<typename Other> struct rebind{
                using other=allocator<Other>;
            };
            T* address(T& val) const noexcept{
                return __builtin_addressof(val);
            }
            const T* address(const T& val) const noexcept{
                return __builtin_addressof(val);
            }
            constexpr allocator()noexcept{}
            constexpr allocator(const allocator&)noexcept = default;
            template<typename Other>constexpr allocator(const allocator<Other>&)noexcept{}
            ~allocator()=default;
            allocator& operator=(const allocator&) = default;
            void deallocate(T* const ptr,const size_t count){
                ::operator delete(ptr,count*sizeof(T));
            }
            T* allocate(const size_t count){
                return static_cast<T*>(::operator new(count*sizeof(T)));
            }
            template<typename U,typename... Args>
            void construct(U* const ptr,Args&&... args){
                new(const_cast<void*>(static_cast<const volatile void*>(ptr))) U(static_cast<Args&&>(args)...);
            }
            template<typename U>
            void destroy(U* const ptr){
                ptr->~U();
            }
            size_t max_size()const noexcept{
                return static_cast<size_t>(-1)/sizeof(T);
            }
            constexpr bool operator==(const allocator&){
                return true;
            }
            constexpr bool operator!=(const allocator&){
                return false;
            }
    };
    template<typename Al,typename Size_type,typename CVpointer,typename=void> struct Has_allocate_hint:false_type{};
    template<typename Al,typename Size_type,typename CVpointer> struct Has_allocate_hint<Al,Size_type,CVpointer,void_t<decltype(declval<Al&>().allocate(declval<const Size_type&>(),declval<const CVpointer&>()))>>:true_type{};

    template<typename Al,typename=void> struct Is_default_allocator:false_type{};
    template<typename T> struct Is_default_allocator<allocator<T>,void_t<typename allocator<T>::From_primary>>:is_same<typename allocator<T>::From_primary,allocator<T>>{};
    template<typename Void,typename... Args> struct Has_no_alloc_construct:true_type{};
    template<typename Al,typename Pointer,typename... Args> struct Has_no_alloc_construct<void_t<decltype(declval<Al&>().construct(declval<Pointer>(),declval<Args>()...))>,Al,Pointer,Args...>:false_type{};
    template<typename Al,typename Pointer,typename... Args> using Uses_default_construct=If_t<Is_default_allocator<Al>::value or Has_no_alloc_construct<void,Al,Pointer,Args...>::value,true_type,false_type>;
    template<typename Al,typename Pointer,typename=void> struct Has_no_alloc_destroy:true_type{};
    template<typename Al,typename Pointer> struct Has_no_alloc_destroy<Al,Pointer,void_t<decltype(declval<Al&>().destroy(declval<Pointer>()))>>:false_type{};
    template<typename Al,typename Pointer> using Uses_default_destroy=If_t<Is_default_allocator<Al>::value or Has_no_alloc_destroy<Al,Pointer>::value,true_type,false_type>;
    template<typename Al,typename=void> struct Has_select_on_container_copy_construction:false_type{};
    template<typename Al> struct Has_select_on_container_copy_construction<Al,void_t<decltype(declval<const Al&>().select_on_container_copy_construction())>>:true_type{};

    template<typename Alloc>
    struct allocator_traits{
        private:
            template<typename Al,typename U,typename=void> struct rebind__: replace_first_arg<Al,U>{};
            template<typename Al,typename U> struct rebind__<Al,U,void_t<typename Al::template rebind<U>::other>>{
                using type=typename Al::template rebind<U>::other;
            };
            template<typename Al,typename=void> struct pointer__{
                using type=typename Al::value_type*;
            };
            template<typename Al> struct pointer__<Al,void_t<typename Al::pointer>>{
                using type=typename Al::pointer;
            };
        public:
            using allocator_type=Alloc;
            using value_type=typename Alloc::value_type;
            using pointer=typename pointer__<Alloc>::type;
        private:
            template<typename Al,typename PtrT,typename=void>
            struct Diff{
                using type=typename pointer_traits<PtrT>::difference_type;
            };
            template<typename Al,typename PtrT>
            struct Diff<Al,PtrT,void_t<typename Al::difference_type>>{
                using type=typename Al::difference_type;
            };
            template<typename Al,typename DiffT,typename=void>
            struct Size{
                using type=make_unsigned_t<DiffT>;
            };
            template<typename Al,typename DiffT>
            struct Size<Al,DiffT,void_t<typename Al::size_type>>{
                using type=typename Al::size_type;
            };
            template<template<typename> class Func,typename T,typename=void>
            struct Ptr{
                using type=typename pointer_traits<pointer>::template rebind<T>;
            };
            template<template<typename> class Func,typename T>
            struct Ptr<Func,T,void_t<Func<Alloc>>>{
                using type=Func<Alloc>;
            };
            template<typename Al,typename=void>
            struct CPtr{
                using type=typename pointer_traits<pointer>::template rebind<const value_type>;
            };
            template<typename Al>
            struct CPtr<Al,void_t<typename Al::const_pointer>>{
                using type=typename Al::const_pointer;
            };
            template<typename Al,typename=void>
            struct VPtr{
                using type=typename pointer_traits<pointer>::template rebind<void>;
            };
            template<typename Al>
            struct VPtr<Al,void_t<typename Al::void_pointer>>{
                using type=typename Al::void_pointer;
            };
            template<typename Al,typename=void>
            struct CVPtr{
                using type=typename pointer_traits<pointer>::template rebind<const void>;
            };
            template<typename Al>
            struct CVPtr<Al,void_t<typename Al::const_void_pointer>>{
                using type=typename Al::const_void_pointer;
            };
            template<typename Al,typename=void>
            struct POCCA{
                using type=false_type;
            };
            template<typename Al>
            struct POCCA<Al,void_t<typename Al::propagate_on_container_copy_assignment>>{
                using type=typename Al::propagate_on_container_copy_assignment;
            };
            template<typename Al,typename=void>
            struct POCMA{
                using type=false_type;
            };
            template<typename Al>
            struct POCMA<Al,void_t<typename Al::propagate_on_container_move_assignment>>{
                using type=typename Al::propagate_on_container_move_assignment;
            };
            template<typename Al,typename=void>
            struct POCS{
                using type=false_type;
            };
            template<typename Al>
            struct POCS<Al,void_t<typename Al::propagate_on_container_swap>>{
                using type=typename Al::propagate_on_container_swap;
            };
            template<typename Al,typename=void>
            struct always_eq{
                using type=If_t<is_empty_v<Al>,true_type,false_type>;
            };
            template<typename Al>
            struct always_eq<Al,void_t<typename Al::is_always_equal>>{
                using type=typename Al::is_always_equal;
            };
        public:
            using difference_type=typename Diff<Alloc,pointer>::type;
            using size_type=typename Size<Alloc,difference_type>::type;
            using const_pointer=typename CPtr<Alloc>::type;
            using void_pointer=typename VPtr<Alloc>::type;
            using const_void_pointer=typename CVPtr<Alloc>::type;
            using propagate_on_container_copy_assignment=typename POCCA<Alloc>::type;
            using propagate_on_container_move_assignment=typename POCMA<Alloc>::type;
            using propagate_on_container_swap=typename POCS<Alloc>::type;
            using is_always_equal=typename always_eq<Alloc>::type;
            template<typename T> using rebind_alloc=typename rebind__<Alloc,T>::type;
            template<typename T> using rebind_traits=allocator_traits<rebind_alloc<T>>;


            static pointer allocate(Alloc& al,const size_type n){
                return al.allocate(n);
            }
            static pointer allocate(Alloc& al,const size_type n,const const_void_pointer hint){
                if constexpr(Has_allocate_hint<Alloc,size_type,const_void_pointer>::value)
                    return al.allocate(n,hint);
                else
                    return al.allocate(n);
            }
            static void deallocate(Alloc& al,pointer p,size_type n){
                al.deallocate(p,n);
            }
            template<typename T,typename... Args>
            static auto construct(Alloc& al,T* p,Args&&... args){
                if constexpr(Uses_default_construct<Alloc,T*,Args...>::value)
                    new(p) T(static_cast<Args&&>(args)...);
                else
                    al.construct(p,static_cast<Args&&>(args)...);
            }
            template<typename T>
            static void destroy(Alloc& al,T* p){
                if constexpr(Uses_default_construct<Alloc,T*>::value)
                    p->~T();
#ifdef __clang__
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
                else al.destroy(p);    
#elif defined(_MSVC_LANG)
    #pragma warning(suppress : 4996)
                else al.destroy(p);
#else
                else al.destroy(p);
#endif  
            }
            static Alloc select_on_container_copy_construction(const Alloc& al){
                if constexpr(Has_select_on_container_copy_construction<Alloc>::value)
                    return al.select_on_container_copy_construction();
                else
                    return al;
            }
    };
    template<typename Alloc,typename U> using rebind_alloc=typename allocator_traits<Alloc>::template rebind_alloc<U>;
    template<typename Alloc,typename U> using rebind_traits=typename allocator_traits<Alloc>::template rebind_traits<U>;
}
//#undef _SILENCE_CXX17_POLYMORPHIC_ALLOCATOR_DESTROY_DEPRECATION_WARNING