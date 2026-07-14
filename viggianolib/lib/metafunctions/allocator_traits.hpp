#pragma once
#include"pointer_traits.hpp"
namespace mpv{
    template<typename T>
    class dummy_statefull_allocator{
        public:
            const char* msg="default msg";
            using value_type=T;
            using pointer=T*;
            using const_pointer=const T*;
            using reference=T&;
            using const_reference=const T&;
            using size_type=size_t;
            using difference_type=ptrdiff_t;
            using propagate_on_container_move_assignment=true_type;
            using propagate_on_container_copy_assignment=true_type;
            using propagate_on_container_swap=true_type;
            using is_always_equal=false_type;
            template<typename Other> struct rebind{
                using other=dummy_statefull_allocator<Other>;
            };
            T* address(T& val) const noexcept{
                //return __builtin_addressof(val);
                return &val;
            }
            const T* address(const T& val) const noexcept{
                //return __builtin_addressof(val);
                return &val;
            }
            constexpr dummy_statefull_allocator()noexcept{}
            constexpr explicit dummy_statefull_allocator(const char* msg):msg(msg){}
            constexpr dummy_statefull_allocator(const dummy_statefull_allocator&)noexcept = default;
            template<typename Other>constexpr dummy_statefull_allocator(const dummy_statefull_allocator<Other>& other):msg(other.msg){}
            ~dummy_statefull_allocator()=default;
            dummy_statefull_allocator& operator=(const dummy_statefull_allocator&) = default;
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
            constexpr bool operator==(const dummy_statefull_allocator& other)const noexcept{
                return this->msg==other.msg;
            }
            constexpr bool operator!=(const dummy_statefull_allocator& other)const noexcept{
                return this->msg!=other.msg;
            }
    };
    template<typename T>
    class always_different_test_allocator{
        public:
            using value_type=T;
            using pointer=T*;
            using const_pointer=const T*;
            using reference=T&;
            using const_reference=const T&;
            using size_type=size_t;
            using difference_type=ptrdiff_t;
            using propagate_on_container_copy_assignment=true_type;
            using is_always_equal=false_type;
            template<typename Other> struct rebind{
                using other=always_different_test_allocator<Other>;
            };
            T* address(T& val) const noexcept{
                //return __builtin_addressof(val);
                return &val;
            }
            const T* address(const T& val) const noexcept{
                //return __builtin_addressof(val);
                return &val;
            }
            constexpr always_different_test_allocator()noexcept{}
            constexpr always_different_test_allocator(const always_different_test_allocator&)noexcept = default;
            template<typename Other>constexpr always_different_test_allocator(const always_different_test_allocator<Other>&){}
            ~always_different_test_allocator()=default;
            always_different_test_allocator& operator=(const always_different_test_allocator&) = default;
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
            constexpr bool operator==(const always_different_test_allocator&)const noexcept{
                return false;
            }
            constexpr bool operator!=(const always_different_test_allocator&)const noexcept{
                return true;
            }
    };
    template<typename T>
    class throwing_test_allocator{
        public:
            using value_type=T;
            using pointer=T*;
            using const_pointer=const T*;
            using reference=T&;
            using const_reference=const T&;
            using size_type=size_t;
            using difference_type=ptrdiff_t;
            using propagate_on_container_move_assignment=true_type;
            using is_always_equal=true_type;
            size_t counter=0,limit=0;
            template<typename Other> struct rebind{
                using other=throwing_test_allocator<Other>;
            };
            T* address(T& val) const noexcept{
                //return __builtin_addressof(val);
                return &val;
            }
            const T* address(const T& val) const noexcept{
                //return __builtin_addressof(val);
                return &val;
            }
            constexpr throwing_test_allocator()noexcept{}
            constexpr throwing_test_allocator(size_t limit)noexcept:limit(limit){}
            constexpr throwing_test_allocator(const throwing_test_allocator& other)noexcept:counter(other.counter),limit(other.limit){};
            template<typename Other>constexpr throwing_test_allocator(const throwing_test_allocator<Other>& other)noexcept:counter(other.counter),limit(other.limit){}
            ~throwing_test_allocator()=default;
            throwing_test_allocator& operator=(const throwing_test_allocator& other){counter=other.counter;limit=other.limit;return *this;}
            void deallocate(T* const ptr,const size_t count){
                ::operator delete(ptr,count*sizeof(T));
            }
            T* allocate(const size_t count){
                if(++counter==limit) throw "Limit reached";
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
            constexpr bool operator==(const throwing_test_allocator&)const noexcept{
                return true;
            }
            constexpr bool operator!=(const throwing_test_allocator&)const noexcept{
                return false;
            }
    };
    template<typename T>
    class debug_allocator{
        public:
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
                using other=debug_allocator<Other>;
            };
            T* address(T& val) const noexcept{
                //return __builtin_addressof(val);
                return &val;
            }
            const T* address(const T& val) const noexcept{
                //return __builtin_addressof(val);
                return &val;
            }
            constexpr debug_allocator()noexcept{}
            constexpr debug_allocator(const debug_allocator&)noexcept = default;
            template<typename Other>constexpr debug_allocator(const debug_allocator<Other>&)noexcept{}
            ~debug_allocator()=default;
            debug_allocator& operator=(const debug_allocator&) = default;
            void deallocate(T* const ptr,const size_t count){
                DEBUG_PRINT("deallocating %llu bytes\n",count*sizeof(T));
                ::operator delete(ptr,count*sizeof(T));
            }
            T* allocate(const size_t count){
                DEBUG_PRINT("allocating %llu bytes\n",count*sizeof(T));
                return static_cast<T*>(::operator new(count*sizeof(T)));
            }
            template<typename U,typename... Args>
            void construct(U* const ptr,Args&&... args){
                DEBUG_PRINT("calling construct\n");
                new(const_cast<void*>(static_cast<const volatile void*>(ptr))) U(static_cast<Args&&>(args)...);
            }
            template<typename U>
            void destroy(U* const ptr){
                DEBUG_PRINT("calling destroy\n");
                ptr->~U();
            }
            size_t max_size()const noexcept{
                return static_cast<size_t>(-1)/sizeof(T);
            }
            constexpr bool operator==(const debug_allocator&)const noexcept{
                return true;
            }
            constexpr bool operator!=(const debug_allocator&)const noexcept{
                return false;
            }
    };
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
                //return __builtin_addressof(val);
                return &val;
            }
            const T* address(const T& val) const noexcept{
                //return __builtin_addressof(val);
                return &val;
            }
            constexpr allocator()noexcept{}
            constexpr allocator(const allocator&)noexcept = default;
            template<typename Other>constexpr allocator(const allocator<Other>&)noexcept{}
            ~allocator()=default;
            constexpr allocator& operator=(const allocator&) = default;
            constexpr void deallocate(T* const ptr,const size_t count){
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
            constexpr bool operator==(const allocator&)const noexcept{
                return true;
            }
            constexpr bool operator!=(const allocator&)const noexcept{
                return false;
            }
    };
    template<typename Al,typename Size_type,typename CVpointer,typename=void> struct Has_allocate_hint:false_type{};
    template<typename Al,typename Size_type,typename CVpointer> struct Has_allocate_hint<Al,Size_type,CVpointer,void_t<decltype(declval<Al&>().allocate(declval<const Size_type&>(),declval<const CVpointer&>()))>>:true_type{};

    template<typename Al,typename=void> struct Is_default_allocator:false_type{};
    template<typename T> struct Is_default_allocator<allocator<T>,void_t<typename allocator<T>::From_primary>>:is_same<typename allocator<T>::From_primary,allocator<T>>::type{};// this library
//    template<typename T> struct Is_default_allocator<std::allocator<T>,void_t<typename std::allocator<T>::_From_primary>>:is_same<typename std::allocator<T>::_From_primary,std::allocator<T>>::type{};// clang
//    template<template<typename> typename A,typename T> struct Is_default_allocator<A<T>>:is_same<A<T>,std::allocator<T>>::type{};                                       // other librarys
    template<typename Void,typename... Args> struct Has_no_alloc_construct:true_type{};
    template<typename Al,typename Pointer,typename... Args> struct Has_no_alloc_construct<void_t<decltype(declval<Al&>().construct(declval<Pointer>(),declval<Args>()...))>,Al,Pointer,Args...>:false_type{};

    template<typename Al,typename Pointer,typename... Args> using Uses_default_construct=If_t<Is_default_allocator<Al>::value || Has_no_alloc_construct<void,Al,Pointer,Args...>::value,true_type,false_type>;
    template<typename Al,typename Pointer,typename... Args> constexpr bool Uses_default_construct_v=Uses_default_construct<Al,Pointer,Args...>::value;

    template<typename Al,typename Pointer,typename=void> struct Has_no_alloc_destroy:true_type{};
    template<typename Al,typename Pointer> struct Has_no_alloc_destroy<Al,Pointer,void_t<decltype(declval<Al&>().destroy(declval<Pointer>()))>>:false_type{};
    
    template<typename Al,typename Pointer> using Uses_default_destroy=If_t<Is_default_allocator<Al>::value || Has_no_alloc_destroy<Al,Pointer>::value,true_type,false_type>;
    template<typename Al,typename Pointer> constexpr bool Uses_default_destroy_v=Uses_default_destroy<Al,Pointer>::value;
    template<typename Al,typename=void> struct Has_select_on_container_copy_construction:false_type{};
    template<typename Al> struct Has_select_on_container_copy_construction<Al,void_t<decltype(declval<const Al&>().select_on_container_copy_construction())>>:true_type{};

    template<typename Alloc>
    struct allocator_traits{
        private:
            template<typename Al,typename U,typename=void> struct rebind__: replace_first_arg<Al,U>{};
            template<typename Al,typename U> struct rebind__<Al,U,void_t<typename Al::template rebind<U>::other>>{
#ifdef __clang__
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
                using type=typename Al::template rebind<U>::other;
#elif defined(_MSVC_LANG)
    #pragma warning(suppress : 4996)
                using type=typename Al::template rebind<U>::other;
#else
				using type=typename Al::template rebind<U>::other;
#endif
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
            //static constexpr bool uses_default_construct=<Alloc,T*>

            static constexpr pointer allocate(Alloc& al,const size_type n){
                return al.allocate(n);
            }
            static constexpr pointer allocate(Alloc& al,const size_type n,const const_void_pointer hint){
                if constexpr(Has_allocate_hint<Alloc,size_type,const_void_pointer>::value)
                    return al.allocate(n,hint);
                else
                    return al.allocate(n);
            }
            static constexpr void deallocate(Alloc& al,pointer p,size_type n){
                al.deallocate(p,n);
            }
            template<typename T,typename... Args>
            static constexpr auto construct(Alloc& al,T* p,Args&&... args){
                if constexpr(Uses_default_construct<Alloc,T*,Args...>::value)
                    new(p) T(static_cast<Args&&>(args)...);
#ifdef __clang__
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
                else al.construct(p,static_cast<Args&&>(args)...);
#elif defined(_MSVC_LANG)
    #pragma warning(suppress : 4996)
                else al.construct(p,static_cast<Args&&>(args)...);
#else
				else al.construct(p,static_cast<Args&&>(args)...);
#endif
            }
            template<typename T>
            static constexpr void destroy(Alloc& al,T* p){
                if constexpr(Uses_default_destroy<Alloc,T*>::value)
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
            static constexpr Alloc select_on_container_copy_construction(const Alloc& al){
                if constexpr(Has_select_on_container_copy_construction<Alloc>::value)
                    return al.select_on_container_copy_construction();
                else
                    return al;
            }
    };
    template<typename Alloc,typename U> using rebind_alloc=typename allocator_traits<Alloc>::template rebind_alloc<U>;
    template<typename Alloc,typename U> using rebind_traits=typename allocator_traits<Alloc>::template rebind_traits<U>;
    
    template<typename Alloc>
    constexpr void pocca(Alloc& left,const Alloc& right)noexcept{
        if constexpr(allocator_traits<Alloc>::propagate_on_container_copy_assignment::value){
            left=right;
        }
    }
    template<typename Alloc>
    constexpr void pocma(Alloc& left,Alloc& right)noexcept{
        if constexpr(allocator_traits<Alloc>::propagate_on_container_move_assignment::value){
            left=static_cast<Alloc&&>(right);
        }
    }
    template<typename Alloc>
    constexpr void pocs(Alloc& left,Alloc& right)noexcept{
        if constexpr(allocator_traits<Alloc>::propagate_on_container_swap::value){
            swap(left,right);
        }
    }
}
