#pragma once
#ifndef VIGGIANO_H
#define VIGGIANO_H
#ifdef __linux__
	typedef long unsigned int size_t;
	typedef long int ssize_t;
	typedef long int ptrdiff_t;
#elif defined(_WIN64)
	typedef long long unsigned int size_t;
	typedef long long int ssize_t;
	typedef long long signed int ptrdiff_t;
#else
    typedef unsigned int size_t;
	typedef int ssize_t;
	typedef int ptrdiff_t;
#endif

#if (!defined(_INITIALIZER_LIST) && !defined(_INITIALIZER_LIST_) && !defined(__linux__)) || defined(USE_MPV_INITIALIZER_LIST)
#define MPV_INITIALIZER_LIST
#define _INITIALIZER_LIST
#define _INITIALIZER_LIST_
namespace std{
  	template<class T>
  	class initializer_list{
		private:
			const T* beg;
			size_t len;
			constexpr initializer_list(const T* beg,size_t len)noexcept:beg(beg),len(len){}

		public:
			constexpr initializer_list(const T* beg,const T* end)noexcept:beg(beg),len(end-beg){}
			constexpr initializer_list() noexcept :beg(nullptr),len(0){}
			constexpr const T* begin()const noexcept{return beg;}
			constexpr const T* end()const noexcept{return beg+len;}
			constexpr size_t size()const noexcept{return len;}
 	};
	template <class T>
	constexpr const T* begin(initializer_list<T> Ilist) noexcept {
		return Ilist.begin();
	}
	template <class T>
	constexpr const T* end(initializer_list<T> Ilist) noexcept {
		return Ilist.end();
	}
}
#elif defined(__linux__)
#include<initializer_list>
#endif

#if (defined(_ATOMIC_) || defined(_GLIBCXX_ATOMIC)) && !defined(DISABLE_ATOMIC)
#define USE_ATOMIC
#endif
namespace mpv{
	struct ObjectCounter{
#ifdef USE_ATOMIC
			static std::atomic<long long> count;
#else 
			static long long count;
#endif
			ObjectCounter(){count++;}
			ObjectCounter(const ObjectCounter&){count++;}
			ObjectCounter(ObjectCounter&&)noexcept{count++;}
			void operator=(const ObjectCounter&){}
			~ObjectCounter(){count--;}
			static long long use(){return count;}
	};
#ifdef USE_ATOMIC
	inline std::atomic<long long> ObjectCounter::count=0;
#else 
	inline long long ObjectCounter::count=0;
#endif
#if defined(_DEBUG) || defined(COUNT_OBJECTS)
#define COUNT_IT :public ObjectCounter
#define COUNT_IT_ ,public ObjectCounter
#else
#define COUNT_IT
#define COUNT_IT_
#endif
}
// #define COUNT_IT
// #define COUNT_IT_

#ifndef __linux__
#ifdef __clang__
void operator delete(void* x,size_t n)noexcept;
void operator delete[](void*,size_t)noexcept;
#endif //__clang__
#ifndef _NEW
	#define _NEW 
	#ifndef __PLACEMENT_NEW_INLINE
		#define __PLACEMENT_NEW_INLINE
		inline void* operator new(size_t s,void* mem)noexcept{(void)s;return mem;}
	#endif
	#ifndef __PLACEMENT_VEC_NEW_INLINE
		#define __PLACEMENT_VEC_NEW_INLINE
		inline void* operator new[](size_t s,void* mem)noexcept{(void)s;return mem;}
	#endif
#endif
#endif //__linux__
#if (defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_) || defined(_INC_STDIO)) && defined(DEBUG)
#define DEBUG_PRINT(...) printf(__VA_ARGS__);
#else
#define DEBUG_PRINT(...) 
#endif

#if defined(_INC_STRING) && !defined(USE_MEMCPY)
#define USE_MEMCPY 1
#else
#define USE_MEMCPY 0
#endif

#if USE_MEMCPY == 0
#define MEMCMP(A,B,SIZE) 0;
#define MEMSET(TO,VAL,SIZE) ;
#define MEMCPY(TO,FROM,SIZE) ;	//evita error de compilacion si memcpy no existe
#define MEMMOVE(TO,FROM,SIZE) ;	//evita error de compilacion si memmove no existe
#define STRCPY(TO,FROM) ;
#define WCSCPY(TO,FROM) ;
#define RETURN_STRLEN(S) ;
#define RETURN_WCSLEN(S) ;
#else
#define MEMCMP(A,B,SIZE) ::memcmp(A,B,SIZE);
#define MEMSET(TO,VAL,SIZE) ::memset(TO,VAL,SIZE);
#define MEMCPY(TO,FROM,SIZE) ::memcpy(TO,FROM,SIZE);
#define MEMMOVE(TO,FROM,SIZE) ::memmove(TO,FROM,SIZE);
#define STRCPY(TO,FROM) ::strcpy(TO,FROM);
#define WCSCPY(TO,FROM) ::wcscpy(TO,FROM);
#define RETURN_STRLEN(S) return ::strlen(S);
#define RETURN_WCSLEN(S) return ::wcslen(S);
#endif

#if ((defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)) || defined(_INC_STDIO)) && defined(SHOWCOUNT)
#define PUTS(S) puts(S)
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PUTS(S) 
#define PRINTF(...) 
#endif

#define CONSTRUCT_VARARGS(ALLOC,ADDRESS,ARGS)       if constexpr(is_fancy_v<typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::pointer> || !Uses_default_construct_v<remove_reference_t<decltype(ALLOC)>,typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type*,decltype(ARGS)...>){allocator_traits<remove_reference_t<decltype(ALLOC)>>::construct(ALLOC,unfancy(ADDRESS),ARGS...);}\
                                                    else{new(ADDRESS) typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type(ARGS...);}//doesn't matter if is trivially constructible or not, uses placement new
//       vvvvv  used to copy/move construct
#define CONSTRUCT(ALLOC,ADDRESS,ARG)                if constexpr(is_fancy_v<typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::pointer> || !Uses_default_construct_v<remove_reference_t<decltype(ALLOC)>,typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type*,decltype(ARG)>){allocator_traits<remove_reference_t<decltype(ALLOC)>>::construct(ALLOC,unfancy(ADDRESS),ARG);}\
                                                    else{new(ADDRESS) typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type(ARG);}//doesn't matter if is trivially constructible or not, uses placement new

#define DEFAULT_CONSTRUCT(ALLOC,ADDRESS)            {using val_type=typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type;\
                                                    if constexpr(is_fancy_v<typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::pointer> || !Uses_default_construct_v<remove_reference_t<decltype(ALLOC)>,val_type*>){allocator_traits<remove_reference_t<decltype(ALLOC)>>::construct(ALLOC,unfancy(ADDRESS));}\
                                                    else if constexpr(!is_trivially_default_constructible_v<val_type>){new(ADDRESS) val_type();}\
                                                    }//else{do nothing}
#define DESTROY(ALLOC,ADDRESS)                      {using val_type=typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::value_type;\
                                                    if constexpr(is_fancy_v<typename allocator_traits<remove_reference_t<decltype(ALLOC)>>::pointer> || !Uses_default_destroy_v<remove_reference_t<decltype(ALLOC)>,val_type*>){allocator_traits<remove_reference_t<decltype(ALLOC)>>::destroy(ALLOC,unfancy(ADDRESS));}\
                                                    else if constexpr(!is_trivially_destructible_v<val_type>){(ADDRESS)->~val_type();}\
                                                    }//else{do nothing}

#define MEMDEFAULTCONSTRUCT(ALLOC,DEST,SIZE)        if constexpr(is_fancy_v<pointer> || !Uses_default_construct_v<AlTy,value_type*,const_reference>){al::memdefaultconstruct(ALLOC,DEST,SIZE);}\
                                                    else if constexpr(!is_trivially_default_constructible_v<value_type>){dflt::memdefaultconstruct(DEST,SIZE);}
                                                    //else{do nothing}
#define MEMFILLCONSTRUCT(ALLOC,DEST,SIZE,FILLWITH)  if constexpr(is_fancy_v<pointer> || !Uses_default_construct_v<AlTy,value_type*,const_reference>){al::memfillconstruct(ALLOC,DEST,SIZE,FILLWITH);}\
                                                    else{dflt::memfillconstruct(DEST,SIZE,FILLWITH);} 

#define MEMCOPYCONSTRUCT(ALLOC,DEST,SOURCE,SIZE)    if constexpr(is_fancy_v<pointer> || !Uses_default_construct_v<AlTy,value_type*,const_reference>){al::memcopyconstruct(ALLOC,DEST,SOURCE,SIZE);}\
                                                    else{dflt::memcopyconstruct(DEST,SOURCE,SIZE);}

#define MEMMOVECONSTRUCT(ALLOC,DEST,SOURCE,SIZE)    if constexpr(is_fancy_v<pointer> || !Uses_default_construct_v<AlTy,value_type*,value_type&&>){al::memmoveconstruct(ALLOC,DEST,SOURCE,SIZE);}\
                                                    else{dflt::memmoveconstruct(DEST,SOURCE,SIZE);}

#define MEMDESTRUCTOR(ALLOC,DEST,SIZE)              if constexpr(is_fancy_v<pointer> || !Uses_default_destroy_v<AlTy,value_type*>){al::memdestructor(ALLOC,DEST,SIZE);}\
                                                    else if constexpr(!is_trivially_destructible_v<value_type>){dflt::memdestructor(DEST,SIZE);}
                                                    

namespace mpv{
    template<typename T> T fake_copy_init(T)noexcept;
    template<typename T=void> struct less{
        constexpr bool operator()(const T& a,const T& b)const noexcept(noexcept(fake_copy_init<bool>(a<b))){
            return a<b;
        }
    };
    template<typename T=void> struct greater{
        constexpr bool operator()(const T& a,const T& b)const noexcept(noexcept(fake_copy_init<bool>(a>b))){
            return a>b;
        }
    };
    template<typename T=void> struct less_equal{
        constexpr bool operator()(const T& a,const T& b)const noexcept(noexcept(fake_copy_init<bool>(a<=b))){
            return a<=b;
        }
    };
    template<typename T=void> struct greater_equal{
        constexpr bool operator()(const T& a,const T& b)const noexcept(noexcept(fake_copy_init<bool>(a>=b))){
            return a>=b;
        }
    };
    template<typename T=void> struct equal_to{
        constexpr bool operator()(const T& a,const T& b)const noexcept(noexcept(fake_copy_init<bool>(a==b))){
            return a==b;
        }
    };
    template<typename T=void> struct not_equal_to{
        constexpr bool operator()(const T& a,const T& b)const noexcept(noexcept(fake_copy_init<bool>(a!=b))){
            return a!=b;
        }
    };
    template<typename T=void> struct plus{
        constexpr T operator()(const T& a,const T& b)const noexcept(noexcept(a+b)){
            return a+b;
        }
    };
    template<typename T=void> struct minus{
        constexpr T operator()(const T& a,const T& b)const noexcept(noexcept(a-b)){
            return a-b;
        }
    };
    template<typename T=void> struct multiplies{
        constexpr T operator()(const T& a,const T& b)const noexcept(noexcept(a*b)){
            return a*b;
        }
    };
    template<typename T=void> struct divides{
        constexpr T operator()(const T& a,const T& b)const noexcept(noexcept(a/b)){
            return a/b;
        }
    };
    template<typename T=void> struct modulus{
        constexpr T operator()(const T& a,const T& b)const noexcept(noexcept(a%b)){
            return a%b;
        }
    };
    template<typename T=void> struct negate{
        constexpr T operator()(const T& a)const noexcept(noexcept(-a)){
            return -a;
        }
    };
    template<typename T=void> struct logical_and{
        constexpr bool operator()(const T& a,const T& b)const noexcept(noexcept(fake_copy_init<bool>(a && b))){
            return a && b;
        }
    };
    template<typename T=void> struct logical_or{
        constexpr bool operator()(const T& a,const T& b)const noexcept(noexcept(fake_copy_init<bool>(a || b))){
            return a || b;
        }
    };
    template<typename T=void> struct logical_not{
        constexpr bool operator()(const T& a)const noexcept(noexcept(fake_copy_init<bool>(!a))){
            return !a;
        }
    };
    template<typename T=void> struct bit_and{
        constexpr T operator()(const T& a,const T& b)const noexcept(noexcept(a & b)){
            return a & b;
        }
    };
    template<typename T=void> struct bit_or{
        constexpr T operator()(const T& a,const T& b)const noexcept(noexcept(a | b)){
            return a | b;
        }
    };
    template<typename T=void> struct bit_xor{
        constexpr T operator()(const T& a,const T& b)const noexcept(noexcept(a ^ b)){
            return a ^ b;
        }
    };
    template<typename T=void> struct bit_not{
        constexpr T operator()(const T& a)const noexcept(noexcept(~a)){
            return ~a;
        }
    };
    template<> struct less<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) < static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) < static_cast<U&&>(b)){
            return static_cast<T&&>(a) < static_cast<U&&>(b);
        }
    };
    template<> struct greater<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) > static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) > static_cast<U&&>(b)){
            return static_cast<T&&>(a) > static_cast<U&&>(b);
        }
    };
    template<> struct less_equal<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) <= static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) <= static_cast<U&&>(b)){
            return static_cast<T&&>(a) <= static_cast<U&&>(b);
        }
    };
    template<> struct greater_equal<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) >= static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) >= static_cast<U&&>(b)){
            return static_cast<T&&>(a) >= static_cast<U&&>(b);
        }
    };
    template<> struct equal_to<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) == static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) == static_cast<U&&>(b)){
            return static_cast<T&&>(a) == static_cast<U&&>(b);
        }
    };
    template<> struct not_equal_to<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) != static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) != static_cast<U&&>(b)){
            return static_cast<T&&>(a) != static_cast<U&&>(b);
        }
    };
    template<> struct plus<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) + static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) + static_cast<U&&>(b)){
            return static_cast<T&&>(a) + static_cast<U&&>(b);
        }
    };
    template<> struct minus<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) - static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) - static_cast<U&&>(b)){
            return static_cast<T&&>(a) - static_cast<U&&>(b);
        }
    };
    template<> struct multiplies<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) * static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) * static_cast<U&&>(b)){
            return static_cast<T&&>(a) * static_cast<U&&>(b);
        }
    };
    template<> struct divides<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) / static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) / static_cast<U&&>(b)){
            return static_cast<T&&>(a) / static_cast<U&&>(b);
        }
    };
    template<> struct modulus<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) % static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) % static_cast<U&&>(b)){
            return static_cast<T&&>(a) % static_cast<U&&>(b);
        }
    };
    template<> struct negate<void>{
        template<typename T>
        constexpr auto operator()(T&& a)const noexcept(noexcept(-static_cast<T&&>(a))) -> decltype(-static_cast<T&&>(a)){
            return -static_cast<T&&>(a);
        }
    };
    template<> struct logical_and<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) && static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) && static_cast<U&&>(b)){
            return static_cast<T&&>(a) && static_cast<U&&>(b);
        }
    };
    template<> struct logical_or<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) || static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) || static_cast<U&&>(b)){
            return static_cast<T&&>(a) || static_cast<U&&>(b);
        }
    };
    template<> struct logical_not<void>{
        template<typename T>
        constexpr auto operator()(T&& a)const noexcept(noexcept(!static_cast<T&&>(a))) -> decltype(!static_cast<T&&>(a)){
            return !static_cast<T&&>(a);
        }
    };
    template<> struct bit_and<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) & static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) & static_cast<U&&>(b)){
            return static_cast<T&&>(a) & static_cast<U&&>(b);
        }
    };
    template<> struct bit_or<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) | static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) | static_cast<U&&>(b)){
            return static_cast<T&&>(a) | static_cast<U&&>(b);
        }
    };
    template<> struct bit_xor<void>{
        template<typename T,typename U>
        constexpr auto operator()(T&& a,U&& b)const noexcept(noexcept(static_cast<T&&>(a) ^ static_cast<U&&>(b))) -> decltype(static_cast<T&&>(a) ^ static_cast<U&&>(b)){
            return static_cast<T&&>(a) ^ static_cast<U&&>(b);
        }
    };
    template<> struct bit_not<void>{
        template<typename T>
        constexpr auto operator()(T&& a)const noexcept(noexcept(~static_cast<T&&>(a))) -> decltype(~static_cast<T&&>(a)){
            return ~static_cast<T&&>(a);
        }
    };
}

namespace mpv{
	template<typename T,T Val>
    struct integral_constant{
        static constexpr T value=Val;
        using value_type=T;
        using type=integral_constant;
        constexpr operator T()const noexcept{
            return value;
        }
        constexpr T operator()()const noexcept{
            return value;
        }
    };
    template<bool Val> using bool_constant=integral_constant<bool,Val>;
    using true_type=bool_constant<true>;
    using false_type=bool_constant<false>;

    template<typename...> using void_t=void;

    template<typename T,typename U> struct is_same     :false_type{};
    template<typename T>            struct is_same<T,T>:true_type{};
    template<typename T,typename U> constexpr bool is_same_v=is_same<T,U>::value;

    template<typename T,typename=void> struct add_reference{
        using lvalue=T;
        using rvalue=T;
    };
    template<typename T> struct add_reference<T,void_t<T&>>{
        using lvalue=T&;
        using rvalue=T&&;
    };
    template<typename T> struct add_lvalue_reference{
        using type=typename add_reference<T>::lvalue;
    };
    template<typename T> using add_lvalue_reference_t=typename add_lvalue_reference<T>::type;

    template<typename T> struct add_rvalue_reference{
        using type=typename add_reference<T>::rvalue;
    };
    template<typename T> using add_rvalue_reference_t=typename add_rvalue_reference<T>::type;

    template<typename T> constexpr bool is_enum_v=__is_enum(T);
    template<typename T> struct is_enum:bool_constant<is_enum_v<T>>{};
    template<typename T> constexpr bool is_union_v=__is_union(T);
    template<typename T> struct is_union:bool_constant<is_union_v<T>>{};
    template<typename T> constexpr bool is_class_v=__is_class(T);
    template<typename T> struct is_class:bool_constant<is_class_v<T>>{};
    template<typename T> constexpr bool is_empty_v=__is_empty(T);
    template<typename T> struct is_empty:bool_constant<is_empty_v<T>>{};
    template<typename T> constexpr bool is_final_v=__is_final(T);
    template<typename T> struct is_final:bool_constant<is_final_v<T>>{};

    template<typename T> struct remove_reference{
        using type=T;
    };
    template<typename T> struct remove_reference<T&>{
        using type=T;
    };
    template<typename T> struct remove_reference<T&&>{
        using type=T;
    };
    template<typename T>
    using remove_reference_t=typename remove_reference<T>::type;

    template<typename T> struct remove_const{
        using type=T;
    };
    template<typename T> struct remove_const<const T>{
        using type=T;
    };
    template<typename T>
    using remove_const_t=typename remove_const<T>::type;

    template<typename T> struct remove_volatile{
        using type=T;
    };
    template<typename T> struct remove_volatile<volatile T>{
        using type=T;
    };
    template<typename T>
    using remove_volatile_t=typename remove_volatile<T>::type;

    
    template<typename T> struct remove_cv{
        using type=T;
    };
    template<typename T> struct remove_cv<const T>{
        using type=T;
    };
    template<typename T> struct remove_cv<volatile T>{
        using type=T;
    };
    template<typename T> struct remove_cv<const volatile T>{
        using type=T;
    };
    template<typename T>
    using remove_cv_t=typename remove_cv<T>::type;
    template<typename T>
    using remove_cvref_t=remove_cv_t<remove_reference_t<T>>;

    template<typename T> struct remove_pointer{
        using type=T;
    };
    template<typename T> struct remove_pointer<T*>{
        using type=T;
    };
    template<typename T> struct remove_pointer<T*const>{
        using type=T;
    };
    template<typename T> struct remove_pointer<T*volatile>{
        using type=T;
    };
    template<typename T> struct remove_pointer<T*const volatile>{
        using type=T;
    };
    template<typename T>
    using remove_pointer_t=typename remove_pointer<T>::type;

	template<typename T>
	struct remove_extent{
		using type=T;
	};
	template<typename T>
	struct remove_extent<T[]>{
		using type=T;
	};
	template<typename T, size_t N>
	struct remove_extent<T[N]>{
		using type=T;
	};
    template<typename T>
    using remove_extent_t=typename remove_extent<T>::type;

    template<typename T>
    struct remove_all_extents{
        using type=T;
    };
    template<typename T>
    struct remove_all_extents<T[]>{
        using type=typename remove_all_extents<T>::type;
    };
    template<typename T, size_t N>
    struct remove_all_extents<T[N]>{
        using type=typename remove_all_extents<T>::type;
    };
    template<typename T>
    using remove_all_extents_t=typename remove_all_extents<T>::type;

    template<typename T>
    struct remove_array_size{
        using type=T;
    };
    template<typename T,size_t N>
    struct remove_array_size<T[N]>{
        using type=T[];
    };
    template<typename T>
    using remove_array_size_t=typename remove_array_size<T>::type;

    template<bool Condition,typename Then,typename Else>
    struct If{
        using type=Then;
    };
    template<typename Then,typename Else>
    struct If<false,Then,Else>{
        using type=Else;
    };
    template<bool Condition,typename Then,typename Else>
    using If_t=typename If<Condition,Then,Else>::type;

	template<typename... Args> struct conjunction:true_type{};
	template<typename Arg1,typename... Args> struct conjunction<Arg1,Args...>:If_t<Arg1::value==false,false_type,conjunction<Args...>>{};
	template<typename Arg1> struct conjunction<Arg1>:Arg1{};
	template<typename... Args> constexpr bool conjunction_v=conjunction<Args...>::value;
	
	template<typename... Args> struct disjunction:false_type{};
	template<typename Arg1,typename... Args> struct disjunction<Arg1,Args...>:If_t<Arg1::value==true,true_type,disjunction<Args...>>{};
	template<typename Arg1> struct disjunction<Arg1>:Arg1{};
	template<typename... Args> constexpr bool disjunction_v=disjunction<Args...>::value;
	
	template<typename Arg> struct negation:bool_constant<!bool(Arg::value)>{};
	template<typename Arg> constexpr bool negation_v=negation<Arg>::value;

    using nullptr_t=decltype(nullptr);
    template<typename T> constexpr bool is_null_pointer_v=is_same_v<remove_cv_t<T>,nullptr_t>;
    template<typename T> struct is_null_pointer:bool_constant<is_null_pointer_v<T>>{};

    template<typename T> constexpr bool is_void_v=is_same_v<remove_cv_t<T>,void>;
    template<typename T> struct is_void:bool_constant<is_void_v<T>>{};

    template<typename T> constexpr bool is_bool_v=is_same_v<remove_cv_t<T>,bool>;
    template<typename T> struct is_bool:bool_constant<is_bool_v<T>>{};

    template<typename T> struct identity{ using type=T; };
    template<typename T> using identity_t=typename identity<T>::type;

    template<typename T> struct underlying_type:If<is_enum_v<T>,__underlying_type(T),T>{};
    template<typename T> using underlying_type_t=typename underlying_type<T>::type;

    template<typename T,bool=is_enum_v<T>> struct unwrap_enum_impl{using type=T;};
    template<typename T> struct unwrap_enum_impl<T,true>:underlying_type<T>{};
    template<typename T> struct unwrap_enum:unwrap_enum_impl<T>{};
    template<typename T> using unwrap_enum_t=typename unwrap_enum<T>::type;

    template<typename> struct is_integral__:false_type{};
    template<> struct is_integral__<bool>:true_type{};
    template<> struct is_integral__<unsigned char>:true_type{};
    template<> struct is_integral__<char>:true_type{};
    template<> struct is_integral__<signed char>:true_type{};
    template<> struct is_integral__<wchar_t>:true_type{};
    template<> struct is_integral__<char16_t>:true_type{};
    template<> struct is_integral__<char32_t>:true_type{};
    template<> struct is_integral__<unsigned short>:true_type{};
    template<> struct is_integral__<short>:true_type{};
    template<> struct is_integral__<unsigned int>:true_type{};
    template<> struct is_integral__<int>:true_type{};
    template<> struct is_integral__<unsigned long>:true_type{};
    template<> struct is_integral__<long>:true_type{};
    template<> struct is_integral__<unsigned long long>:true_type{};
    template<> struct is_integral__<long long>:true_type{};
    #ifdef __cpp_char8_t
    template<> struct is_integral__<char8_t>:true_type{};
    #endif
    template<typename T> struct is_integral:is_integral__<remove_cv_t<T>>::type{};
    template<typename T> constexpr bool is_integral_v=is_integral<T>::value;

    template<typename> struct is_floating_point__:false_type{};
    template<> struct is_floating_point__<float>:true_type{};
    template<> struct is_floating_point__<double>:true_type{};
    template<> struct is_floating_point__<long double>:true_type{};
    template<typename T> struct is_floating_point:is_floating_point__<remove_cv_t<T>>::type{};
    template<typename T> constexpr bool is_floating_point_v=is_floating_point<T>::value;

    template<typename T> constexpr bool is_arithmetic_v=is_integral_v<T> || is_floating_point_v<T>;
    template<typename T> struct is_arithmetic:bool_constant<is_arithmetic_v<T>>{};

    template<typename T,typename... Args> struct are_same:true_type{};
	template<typename T,typename U,typename... Args> struct are_same<T,U,Args...>   :bool_constant<is_same_v<T,U> && are_same<U,Args...>::value>{};
    template<typename T,typename... Args> constexpr bool are_same_v=are_same<T,Args...>::value;

    template<typename  > struct is_const            :false_type{};
    template<typename T> struct is_const<const T>   :true_type{};
    template<typename T> constexpr bool is_const_v=is_const<T>::value;

    template<typename  > struct is_volatile            :false_type{};
    template<typename T> struct is_volatile<volatile T>:true_type{};
    template<typename T> constexpr bool is_volatile_v=is_volatile<T>::value;

    template<typename  > struct is_lvalue_ref       :false_type{};
    template<typename T> struct is_lvalue_ref<T&>   :true_type{};
    template<typename T> constexpr bool is_lvalue_ref_v=is_lvalue_ref<T>::value;

    template<typename  > struct is_rvalue_ref       :false_type{};
    template<typename T> struct is_rvalue_ref<T&&>  :true_type{};
    template<typename T> constexpr bool is_rvalue_ref_v=is_rvalue_ref<T>::value;

    template<typename T> struct is_reference:bool_constant<is_lvalue_ref_v<T> || is_rvalue_ref_v<T>>{};
    template<typename T> constexpr bool is_reference_v=is_reference<T>::value;

    
    template<typename T> constexpr bool is_function_v=!is_const_v<const T> && !is_reference_v<T>;
    template<typename T> struct is_function:bool_constant<is_function_v<T>>{};
// only function types and reference types can't be const qualified
    template<typename T> constexpr bool is_object_v=is_const_v<const T> && !is_void_v<T>;
    template<typename T> struct is_object:bool_constant<is_object_v<T>> {};

    template<typename T> struct is_referenceable:disjunction<is_object<T>,is_reference<T>>{};
    template<typename Res, typename... Args>struct is_referenceable<Res(Args...)>:true_type {};
    template<typename Res, typename... Args>struct is_referenceable<Res(Args..., ...)>:true_type {};
    template<typename Res, typename... Args>struct is_referenceable<Res(Args...)noexcept>:true_type {};
    template<typename Res, typename... Args>struct is_referenceable<Res(Args..., ...)noexcept>:true_type {};
    template<typename T> constexpr bool is_referenceable_v=is_referenceable<T>::value;

    template<typename  > struct is_pointer                   :false_type{};
    template<typename T> struct is_pointer<T*>               :true_type{};
    template<typename T> struct is_pointer<T* const>         :true_type{};
    template<typename T> struct is_pointer<T* volatile>      :true_type{};
    template<typename T> struct is_pointer<T* const volatile>:true_type{};
    template<typename T> constexpr bool is_pointer_v=is_pointer<T>::value;

    template<typename T> struct is_no_size_array     :false_type{};
    template<typename T> struct is_no_size_array<T[]>:true_type{};
    template<typename T> constexpr bool is_no_size_array_v=is_no_size_array<T>::value;

    template<typename  >          struct is_size_array      :false_type{};
    template<typename T,size_t N> struct is_size_array<T[N]>:true_type{};
    template<typename T> constexpr bool is_size_array_v=is_size_array<T>::value;

    template<typename  >          struct is_array      :false_type{};
    template<typename T>          struct is_array<T[]> :true_type{};
    template<typename T,size_t N> struct is_array<T[N]>:true_type{};
    template<typename T> constexpr bool is_array_v=is_array<T>::value;

    template<typename T,typename U>          struct have_same_extent           :bool_constant<!(is_array_v<T> || is_array_v<U>)>{};
    template<typename T,typename U>          struct have_same_extent<T[],U[]>  :true_type{};
    template<typename T,typename U,size_t N> struct have_same_extent<T[N],U[N]>:true_type{};
    template<typename T,typename U> constexpr bool have_same_extent_v=have_same_extent<T,U>::value;

    template<typename T>          struct get_array_size{/*compilation error*/};
    template<typename T,size_t N> struct get_array_size<T[N]>:integral_constant<size_t,N>{};
    template<typename T> constexpr bool get_array_size_v=get_array_size<T>::value;

    template<bool condition,typename T=void> struct enable_if{/*compilation error*/};
    template<typename T> struct enable_if<true,T>{ using type=T; };
    template<bool condition,typename T=void> using enable_if_t=typename enable_if<condition,T>::type;

    // template<typename B,typename D,typename=void> struct is_base_of:false_type{};
    // template<typename B,typename D> struct is_base_of<B,D,enable_if_t<(B*)(D*)nullptr==(D*)nullptr>>:true_type{};///static_cast<D*>(static_cast<B*>(nullptr))==static_cast<D*>(nullptr)
    // template<typename B,typename D> constexpr bool is_base_of_v=is_base_of<B,D>::value;
    template<typename B,typename D> constexpr bool is_base_of_v=__is_base_of(B,D);
    template<typename B,typename D> struct is_base_of:bool_constant<is_base_of_v<B,D>>{};

    template<typename T,typename=void> struct add_pointer{
        using type=T;
    };
    template<typename T> struct add_pointer<T,void_t<T*>>{
        using type=typename remove_reference<T>::type*; 
    };
    template<typename T> using add_pointer_t=typename add_pointer<T>::type;

    template<typename> constexpr bool always_false=false;
    template<typename> constexpr bool always_true=true;

    template<typename T> add_rvalue_reference_t<T> declval()noexcept{
        static_assert(always_false<T>,"Calling declval is ill-formed");
    }

    template<typename...> struct TypeList{};

    template<class Container> struct empty{};
    template<template<typename...> class Container,typename... Args> struct empty<Container<Args...>>:false_type{};
    template<template<typename...> class Container> struct empty<Container<>>:true_type{};
    template<class Container> constexpr bool empty_v=empty<Container>::value;

    template<class Container> struct pop_back{};
    template<template<typename...> class Container,typename arg,typename... Args> struct pop_back<Container<arg,Args...>>{
        using type=Container<Args...>;
    };
    template<class Container> using pop_back_t=typename pop_back<Container>::type;

    template<typename T,class Container> struct contains_type{};
    template<typename T,template<typename...> class Container,typename arg,typename... Args> struct contains_type<T,Container<arg,Args...>>:
    If_t<is_same_v<arg,T>,
        true_type
        ,
        contains_type<T,Container<Args...>>
    >{};
    template<typename T,template<typename...> class Container> struct contains_type<T,Container<>>:false_type{};
    template<typename T,class Container> constexpr bool contains_type_v=contains_type<T,Container>::value;

    template<typename T,typename... rebindArgs> struct rebind{};
    template<template<typename...> class Container,typename... Args,typename... rebindArgs> struct rebind<Container<Args...>,rebindArgs...>{
        using type=Container<rebindArgs...>;
    };
    template<typename T,typename... rebindArgs> using rebind_t=typename rebind<T,rebindArgs...>::type;
    template<typename T,typename U> struct rebind_cv{
        using type=U;
    };
    template<typename T,typename U> struct rebind_cv<const T,U>{
        using type=const remove_const_t<U>;
    };
    template<typename T,typename U> struct rebind_cv<volatile T,U>{
        using type= volatile remove_volatile_t<U>;
    };
    template<typename T,typename U> struct rebind_cv<const volatile T,U>{
        using type= const volatile remove_cv_t<U>;
    };
    template<typename T,typename U> using rebind_cv_t=typename rebind_cv<T,U>::type;

    template<size_t> struct Make_Unsigned_Helper;
    template<> struct Make_Unsigned_Helper<1>{
        template<typename> using Apply=unsigned char;
    };
    template<> struct Make_Unsigned_Helper<2>{
        template<typename T> using Apply=If_t<is_same_v<T,int> || is_same_v<T,unsigned int>,unsigned int,unsigned short>;
    };
    template<> struct Make_Unsigned_Helper<4>{
        template<typename T> using Apply=If_t<is_same_v<T,long> || is_same_v<T,unsigned long>,unsigned long,unsigned int>;
    };
    template<> struct Make_Unsigned_Helper<8>{
        template<typename T> using Apply=unsigned long long;
    };
    template<typename T> using Make_Unsigned_Helper_t=typename Make_Unsigned_Helper<sizeof(T)>::template Apply<T>;
    template<typename T> struct make_unsigned{
        static_assert(is_enum_v<T> || (is_integral_v<T> && !is_bool_v<T>),"template parameter must be a non bool integer or an enum");
        using type=rebind_cv_t<T,Make_Unsigned_Helper_t<remove_cv_t<T>>>;
    };
    template<typename T> using make_unsigned_t=typename make_unsigned<T>::type;

    template<size_t> struct Make_Signed_Helper;
    template<> struct Make_Signed_Helper<1>{
        template<typename> using Apply=signed char;
    };
    template<> struct Make_Signed_Helper<2>{
        template<typename T> using Apply=If_t<is_same_v<T,int> || is_same_v<T,unsigned int>,int,short>;
    };
    template<> struct Make_Signed_Helper<4>{
        template<typename T> using Apply=If_t<is_same_v<T,long> || is_same_v<T,unsigned long>,long,int>;
    };
    template<> struct Make_Signed_Helper<8>{
        template<typename T> using Apply=long long;
    };
    template<typename T> using Make_Signed_Helper_t=typename Make_Signed_Helper<sizeof(T)>::template Apply<T>;
    template<typename T> struct make_signed{
        static_assert(is_enum_v<T> || (is_integral_v<T> && !is_bool_v<T>),"template parameter must be a non bool integer or an enum");
        using type=rebind_cv_t<T,Make_Signed_Helper_t<remove_cv_t<T>>>;
    };
    template<typename T> using make_signed_t=typename make_signed<T>::type;

    template<typename,typename=void> struct Sign{
        static constexpr bool Signed=false;
        static constexpr bool Unsigned=false;
    };
    template<typename T> struct Sign<T,enable_if_t<is_integral_v<T> || is_floating_point_v<T>>>{
        static constexpr bool Signed=T(-1)<T(0);
        static constexpr bool Unsigned=!Signed;
    };
    template<typename T> struct is_unsigned:bool_constant<Sign<T>::Unsigned>{};
    template<typename T> constexpr bool is_unsigned_v=is_unsigned<T>::value;
    template<typename T> struct is_signed:bool_constant<Sign<T>::Signed>{};
    template<typename T> constexpr bool is_signed_v=is_signed<T>::value;

    template<bool,size_t sz> struct double_size_helper;
    template<bool isSigned> struct double_size_helper<isSigned,1>{using type=If_t<isSigned,signed short,unsigned short>;};
    template<bool isSigned> struct double_size_helper<isSigned,2>{using type=If_t<isSigned,signed long,unsigned long>;};
    template<bool isSigned> struct double_size_helper<isSigned,4>{using type=If_t<isSigned,signed long long,unsigned long long>;};
    template<bool isSigned> struct double_size_helper<isSigned,8>{using type=If_t<isSigned,signed long long,unsigned long long>;};
    template<typename T> struct double_size{
        using type= If_t<is_floating_point_v<T>,
                        If_t<sizeof(T)==4,double,long double>,
                        typename double_size_helper<is_signed_v<T>,sizeof(T)>::type
                    >;
    };
    template<typename T> using double_size_t=typename double_size<T>::type;

    template<typename T,typename U> struct rebind_array{
        using type=U;
    };
    template<typename T,typename U> struct rebind_array<T[],U>{
        using type=U[];
    };
    template<typename T,typename U,size_t N> struct rebind_array<T[N],U>{
        using type=U[N];
    };
    template<typename T,typename U> using rebind_array_t=typename rebind_array<T,U>::type;

    template<typename>              struct is_member_object_pointer__        :false_type{};
    template<typename T,typename C> struct is_member_object_pointer__<T C::*>:bool_constant<!is_function_v<T>>{
        using class_type=C;
    };
    template<typename T> struct is_member_object_pointer:is_member_object_pointer__<remove_cv_t<T>>::type{};
    template<typename T> constexpr bool is_member_object_pointer_v=is_member_object_pointer<T>::value;

    template<typename>              struct is_member_function_pointer__        :false_type{};
    template<typename T,typename C> struct is_member_function_pointer__<T C::*>:bool_constant<is_function_v<T>>{
        using class_type=C;
    };
    template<typename T> struct is_member_function_pointer:is_member_function_pointer__<remove_cv_t<T>>::type{};
    template<typename T> constexpr bool is_member_function_pointer_v=is_member_function_pointer<T>::value;

    template<typename T> constexpr bool is_member_pointer_v=is_member_object_pointer_v<T> || is_member_function_pointer_v<T>;
    template<typename T> struct is_member_pointer:bool_constant<is_member_pointer_v<T>>{};

    template<typename T> constexpr bool is_scalar_v=is_arithmetic_v<T> || is_enum_v<T> || is_pointer_v<T> || is_member_pointer_v<T> || is_null_pointer_v<T>;
    template<typename T> struct is_scalar:bool_constant<is_scalar_v<T>>{};

    template<typename T,template<typename...>class Template> constexpr bool is_specialization_v=false;
    template<template<typename...>class Template,typename... Args> constexpr bool is_specialization_v<Template<Args...>,Template> = true;
    template<typename T,template<typename...>class Template> struct is_specialization:bool_constant<is_specialization_v<T,Template>>{};

    template<typename T> class reference_wrapper;

    enum class Invoker_strategy{ Functor,Pmf_object,Pmf_refwrap,Pmf_pointer,Pmd_object,Pmd_refwrap,Pmd_pointer };
    struct Invoker_functor{
        static constexpr Invoker_strategy strat=Invoker_strategy::Functor;
        template<typename Callable,typename... Args> static constexpr auto call(Callable&& obj,Args&&... args)noexcept(noexcept(static_cast<Callable&&>(obj)(static_cast<Args>(args)...)))->decltype(static_cast<Callable&&>(obj)(static_cast<Args>(args)...)){return static_cast<Callable&&>(obj)(static_cast<Args>(args)...);}
    };
    struct Invoker_pmf_object{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmf_object;
        template<typename Decayed,typename T1,typename... Args> static constexpr auto call(Decayed pmf,T1&& arg1,Args&&... args)noexcept(noexcept((static_cast<T1&&>(arg1).*pmf)(static_cast<Args&&>(args)...)))->decltype((static_cast<T1&&>(arg1).*pmf)(static_cast<Args&&>(args)...)){return (static_cast<T1&&>(arg1).*pmf)(static_cast<Args&&>(args)...);}
    };
    struct Invoker_pmf_refwrap{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmf_refwrap;
        template<typename Decayed,typename Refwrap,typename... Args> static constexpr auto call(Decayed pmf,Refwrap rw,Args&&... args)noexcept(noexcept((rw.get().*pmf)(static_cast<Args&&>(args)...)))->decltype((rw.get().*pmf)(static_cast<Args&&>(args)...)){return (rw.get().*pmf)(static_cast<Args&&>(args)...);}
    };
    struct Invoker_pmf_pointer{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmf_pointer;
        template<typename Decayed,typename T1,typename... Args> static constexpr auto call(Decayed pmf,T1&& arg,Args&&... args)noexcept(noexcept((static_cast<T1&&>(arg)->*pmf)(static_cast<Args&&>(args)...)))->decltype((static_cast<T1&&>(arg)->*pmf)(static_cast<Args&&>(args)...)){return (static_cast<T1&&>(arg)->*pmf)(static_cast<Args&&>(args)...);}
    };
    struct Invoker_pmd_object{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmd_object;
        template<typename Decayed,typename T1> static constexpr auto call(Decayed pmd,T1&& arg)noexcept->decltype(static_cast<T1&&>(arg).*pmd){return static_cast<T1&&>(arg).*pmd;}
    };
    struct Invoker_pmd_refwrap{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmd_refwrap;
        template<typename Decayed,typename Refwrap> static constexpr auto call(Decayed pmd,Refwrap rw)noexcept->decltype(rw.get().*pmd){return rw.get().*pmd;}
    };
    struct Invoker_pmd_pointer{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmd_pointer;
        template<typename Decayed,typename T1> static constexpr auto call(Decayed pmd,T1&& arg)noexcept(noexcept(static_cast<T1&&>(arg)->*pmd))->decltype(static_cast<T1&&>(arg)->*pmd){return static_cast<T1&&>(arg)->*pmd;}
    };
    template<typename Callable,typename T1,
                                        bool Is_pmf=is_member_function_pointer_v<remove_cvref_t<Callable>>,
                                        bool Is_pmd=is_member_object_pointer_v  <remove_cvref_t<Callable>>>
    struct Invoker;
    
    template<typename Callable,typename T1>
    struct Invoker<Callable,T1,true,false>:
    If_t<is_same_v<typename is_member_function_pointer__<remove_cvref_t<Callable>>::class_type,remove_cvref_t<T1>>
    || is_base_of_v<typename is_member_function_pointer__<remove_cvref_t<Callable>>::class_type,remove_cvref_t<T1>>,
        Invoker_pmf_object
        ,//else
        If_t<is_specialization_v<remove_cvref_t<T1>,reference_wrapper>,
            Invoker_pmf_refwrap
            ,//else
            Invoker_pmf_pointer>>{};

    template<typename Callable,typename T1>
    struct Invoker<Callable,T1,false,true>:
    If_t<is_same_v<typename is_member_object_pointer__<remove_cvref_t<Callable>>::class_type,remove_cvref_t<T1>>
    || is_base_of_v<typename is_member_object_pointer__<remove_cvref_t<Callable>>::class_type,remove_cvref_t<T1>>,
        Invoker_pmd_object
        ,//else
        If_t<is_specialization_v<remove_cvref_t<T1>,reference_wrapper>,
            Invoker_pmd_refwrap
            ,//else
            Invoker_pmd_pointer>>{};

    template<typename Callable,typename T1>
    struct Invoker<Callable,T1,false,false>:Invoker_functor{};

    template<typename Callable>
    constexpr auto invoke(Callable&& obj)noexcept(noexcept(static_cast<Callable&&>(obj)()))->decltype(static_cast<Callable&&>(obj)()){
        return static_cast<Callable&&>(obj)();
    }
    template<typename Callable,typename T1,typename... Args>
    constexpr auto invoke(Callable&& obj,T1&& arg1,Args&&... args)noexcept(noexcept(Invoker<Callable,T1>::call(static_cast<Callable&&>(obj),static_cast<T1&&>(arg1),static_cast<Args&&>(args)...)))->decltype(Invoker<Callable,T1>::call(static_cast<Callable&&>(obj),static_cast<T1&&>(arg1),static_cast<Args&&>(args)...)){
        using Inv=Invoker<Callable,T1>;
        if constexpr(Inv::strat==Invoker_strategy::Functor){
            return static_cast<Callable&&>(obj)(static_cast<T1&&>(arg1),static_cast<Args&&>(args)...);
        }
        else if constexpr(Inv::strat==Invoker_strategy::Pmf_object){
            return (static_cast<T1&&>(arg1).*obj)(static_cast<Args&&>(args)...);
        }
        else if constexpr(Inv::strat==Invoker_strategy::Pmf_pointer){
            return (static_cast<T1&&>(arg1)->*obj)(static_cast<Args&&>(args)...);
        }
        else if constexpr(Inv::strat==Invoker_strategy::Pmf_refwrap){
            return (arg1.get().*obj)(static_cast<Args&&>(args)...);
        }
        else if constexpr(Inv::strat==Invoker_strategy::Pmd_object){
            return static_cast<T1&&>(arg1).*obj;
        } 
        else if constexpr(Inv::strat==Invoker_strategy::Pmd_pointer){
            return static_cast<T1&&>(arg1)->*obj;
        }
        else if constexpr(Inv::strat==Invoker_strategy::Pmd_refwrap){
            return (arg1.get()).*obj;
        }
    }
    template<typename T> void refwrap_ctor_fun(identity_t<T&>)noexcept;
    template<typename T> void refwrap_ctor_fun(identity_t<T&&>)=delete;
    template<typename T> class reference_wrapper{static_assert(is_object_v<T> || is_function_v<T>,"T must be an object or function type");
        
        private:
            T* data{};
        public:
            using type=T;
            template<typename U,typename=enable_if_t<!is_same_v<reference_wrapper,remove_cvref_t<U>> && !is_rvalue_ref_v<U&&>>>
            constexpr reference_wrapper(U&& ref)noexcept(noexcept(refwrap_ctor_fun<T>(declval<U>()))):data(&static_cast<U&&>(ref)){}
            constexpr reference_wrapper(const reference_wrapper&)=default;
            constexpr reference_wrapper& operator=(const reference_wrapper&)=default;
            constexpr operator T&()const noexcept{return *data;}
            constexpr T& get()const noexcept{return *data;}
            template<typename... Args>
            constexpr auto operator()(Args&&... args)const{
                return mpv::invoke(*data,static_cast<Args&&>(args)...);
            }
    };
    template<typename T> reference_wrapper(T&) -> reference_wrapper<T>;
    template<typename T> constexpr reference_wrapper<T> ref(T& val)noexcept{
        return reference_wrapper<T>(val);
    }
    template<typename T> void ref(const T&&)=delete;
    template<typename T> constexpr reference_wrapper<T> ref(reference_wrapper<T> val)noexcept{
        return val;
    }
    template<typename T> constexpr reference_wrapper<const T> cref(const T& val){
        return reference_wrapper<const T>(val);
    }
    template<typename T> void cref(const T&&)=delete;
    template<typename T> constexpr reference_wrapper<const T> cref(reference_wrapper<T> val)noexcept{
        return val;
    }
    template<typename T> struct decay{
        using U=remove_reference_t<T>;
        using type= If_t<is_array_v<U>,
                        add_pointer_t<remove_extent_t<U>>,
                        If_t<is_function_v<U>,
                            add_pointer_t<U>,
                            remove_cv_t<U>
                        >
                    >;
    };
    template<typename T> using decay_t=typename decay<T>::type;

    template<typename T,typename... Args> constexpr bool is_trivially_constructible_v=__is_trivially_constructible(T,Args...);
    template<typename T,typename... Args> struct is_trivially_constructible:bool_constant<is_trivially_constructible_v<T,Args...>>{};
    template<typename T,typename U> constexpr bool is_trivially_assignable_v=__is_trivially_assignable(T,U);
    template<typename T,typename U> struct is_trivially_assignable:bool_constant<is_trivially_assignable_v<T,U>>{};
    template<typename T> constexpr bool is_trivially_copy_constructible_v=is_trivially_constructible_v<T,add_lvalue_reference_t<const T>>;
    template<typename T> struct is_trivially_copy_constructible:bool_constant<is_trivially_copy_constructible_v<T>>{};
    template<typename T> constexpr bool is_trivially_copy_assignable_v=is_trivially_assignable_v<add_lvalue_reference_t<T>,add_lvalue_reference_t<const T>>;
    template<typename T> struct is_trivially_copy_assignable:bool_constant<is_trivially_copy_assignable_v<T>>{};
    template<typename T> constexpr bool is_trivially_move_constructible_v=is_trivially_constructible_v<T,T>;
    template<typename T> struct is_trivially_move_constructible:bool_constant<is_trivially_move_constructible_v<T>>{};
    template<typename T> constexpr bool is_trivially_move_assignable_v=is_trivially_assignable_v<add_lvalue_reference_t<T>,T>;
    template<typename T> struct is_trivially_move_assignable:bool_constant<is_trivially_move_assignable_v<T>>{};
    template<typename T> constexpr bool is_trivially_default_constructible_v=is_trivially_constructible_v<T>;
    template<typename T> struct is_trivially_default_constructible:bool_constant<is_trivially_default_constructible_v<T>>{};

    template<typename T> constexpr bool is_trivially_copyable_v=__is_trivially_copyable(T);
    template<typename T> struct is_trivially_copyable:bool_constant<is_trivially_copyable_v<T>>{};
    template<typename T> constexpr bool is_trivial_v=is_trivially_copyable_v<T> && is_trivially_default_constructible_v<T>;
    template<typename T> struct is_trivial:bool_constant<is_trivial_v<T>>{};

    template<typename T> constexpr bool is_pod_v=__is_pod(T);
    template<typename T> struct is_pod:bool_constant<is_pod_v<T>>{};

    template<typename T,typename... Args> struct is_trivially_constructible_and_assignable:false_type{};
    template<typename T> struct is_trivially_constructible_and_assignable<T>:is_trivially_default_constructible<T>{};
    template<typename T,typename U> struct is_trivially_constructible_and_assignable<T,U>:bool_constant<is_trivially_constructible_v<T,U> && is_trivially_assignable_v<add_lvalue_reference_t<T>,U>>{};
    template<typename T,typename... Args> constexpr bool is_trivially_constructible_and_assignable_v=is_trivially_constructible_and_assignable<T,Args...>::value;
    
    template<typename T,typename=void> struct check_has_dtor:false_type{};
    template<typename T> struct check_has_dtor<T,void_t<decltype(declval<T&>().~T())>>:true_type{};
    template<typename T> struct is_destructible:
        If_t<is_void_v<T> || is_no_size_array_v<T> || is_function_v<T>,
            false_type
            ,
            If_t<is_reference_v<T> || is_scalar_v<T>,
                true_type
                ,
                check_has_dtor<remove_all_extents_t<T>>
            >
        >{};
    template<typename T> constexpr bool is_destructible_v=is_destructible<T>::value;

#ifdef __clang__
    template<typename T> constexpr bool is_trivially_destructible_v=__is_trivially_destructible(T);
#else
    template<typename T> constexpr bool is_trivially_destructible_v=is_destructible_v<T> && __has_trivial_destructor(T);
#endif
    template<typename T> struct is_trivially_destructible:bool_constant<is_trivially_destructible_v<T>>{};

    template<typename From,typename To,bool=is_void_v<From> || is_function_v<To> || is_array_v<To>> struct is_convertible_helper{
        using type=typename is_void<To>::type;
    };
    template<typename From,typename To> struct is_convertible_helper<From,To,false>{
        private:
            template<typename To1> static void test(To1)noexcept;
            template<typename From1,typename To1,typename=void> struct test_conversion:false_type{};
            template<typename From1,typename To1> struct test_conversion<From1,To1,void_t<decltype(test<To1>(declval<From1>()))>>:true_type{};
        public:
            using type=typename test_conversion<From,To>::type;
    };
    template<typename From,typename To> struct is_convertible:is_convertible_helper<From,To>::type{};
    template<typename From,typename To> constexpr bool is_convertible_v=is_convertible<From,To>::value;
    template<typename D,typename B> constexpr bool is_derived_from_v=is_base_of_v<B,D> && is_convertible_v<const volatile D*,const volatile B*>;
    template<typename D,typename B> struct is_derived_from:bool_constant<is_derived_from_v<D,B>>{};
    struct is_constructible_impl{
        template<typename T,typename... Args,typename=decltype(T(declval<Args>()...))> static true_type test(int);
        template<typename T,typename... Args> static false_type test(...);
        template<typename T,typename Arg,typename=decltype(::new T(declval<Arg>()))> static true_type test_single_arg(int);
        template<typename T,typename Arg> static false_type test_single_arg(...);
        template<typename T> static true_type ref_test(T);
        template<typename T> static false_type ref_test(...);

    };
    template<typename T,typename... Args> struct is_constructible:decltype(is_constructible_impl::test<T,Args...>(0)){};
    template<typename T,typename Arg> struct is_constructible<T,Arg>:conjunction<decltype(is_constructible_impl::test_single_arg<T,Arg>(0)),is_destructible<T>>{};
    template<typename T,typename Arg> struct is_constructible<T&,Arg>:decltype(is_constructible_impl::ref_test<T&>(declval<Arg>())){};
    template<typename T,typename Arg> struct is_constructible<T&&,Arg>:decltype(is_constructible_impl::ref_test<T&&>(declval<Arg>())){};
    template<> struct is_constructible<void>:false_type{};
    template<> struct is_constructible<const void>:false_type{};
    template<> struct is_constructible<volatile void>:false_type{};
    template<> struct is_constructible<const volatile void>:false_type{};
    template<typename T,typename... Args> constexpr bool is_constructible_v=is_constructible<T,Args...>::value;
    template<typename T> using is_default_constructible=is_constructible<T>;
    template<typename T> constexpr bool is_default_constructible_v=is_default_constructible<T>::value;
    
    struct is_assignable_impl{
        template<typename T,typename U,typename=void> struct test_assignment:false_type{};
        template<typename T,typename U> struct test_assignment<T,U,void_t<decltype(declval<T>()=declval<U>())>>:true_type{};
    };
    template<typename T,typename U> struct is_assignable:is_assignable_impl::template test_assignment<T,U>{};
    template<typename T,typename U> constexpr bool is_assignable_v=is_assignable<T,U>::value;

    template<typename T> constexpr bool is_copy_constructible_v=is_constructible_v<T,add_lvalue_reference_t<const T>>;
    template<typename T> struct is_copy_constructible:bool_constant<is_copy_constructible_v<T>>{};
    template<typename T> constexpr bool is_copy_assignable_v=is_assignable_v<add_lvalue_reference_t<T>,add_lvalue_reference_t<const T>>;
    template<typename T> struct is_copy_assignable:bool_constant<is_copy_assignable_v<T>>{};
    template<typename T> constexpr bool is_move_constructible_v=is_constructible_v<T,T>;
    template<typename T> struct is_move_constructible:bool_constant<is_move_constructible_v<T>>{};
    template<typename T> constexpr bool is_move_assignable_v=is_assignable_v<add_lvalue_reference_t<T>,T>;
    template<typename T> struct is_move_assignable:bool_constant<is_move_assignable_v<T>>{};

    template<typename T> struct is_nt_default_constructible_atom:bool_constant<noexcept(T())>{};
    template<typename T,bool=is_array_v<T>> struct is_nt_default_constructible_impl;
    template<typename T> struct is_nt_default_constructible_impl<T,true>:conjunction<is_size_array<T>,is_nt_default_constructible_atom<remove_all_extents_t<T>>>{};
    template<typename T> struct is_nt_default_constructible_impl<T,false>:is_nt_default_constructible_atom<T>{};
    template<typename T> struct is_nothrow_default_constructible:conjunction<is_default_constructible<T>,is_nt_default_constructible_impl<T>>{};
    template<typename T> constexpr bool is_nothrow_default_constructible_v=is_nothrow_default_constructible<T>::value;

    template<typename T,typename... Args> struct is_nt_constructible_impl:bool_constant<noexcept(T(declval<Args>()...))>{};
    template<typename T,typename Arg> struct is_nt_constructible_impl<T,Arg>:bool_constant<noexcept(static_cast<T>(declval<Arg>()))>{};
    template<typename T> struct is_nt_constructible_impl<T>:is_nothrow_default_constructible<T>{};
    template<typename T,typename... Args> struct is_nothrow_constructible:conjunction<is_constructible<T,Args...>,is_nt_constructible_impl<T,Args...>>{};
    template<typename T,typename... Args> constexpr bool is_nothrow_constructible_v=is_nothrow_constructible<T,Args...>::value;
    
    template<typename T,bool=is_referenceable_v<T>> struct is_nothrow_copy_constructible_impl{};
    template<typename T> struct is_nothrow_copy_constructible_impl<T,false>:false_type{};
    template<typename T> struct is_nothrow_copy_constructible_impl<T,true>:is_nothrow_constructible<T,const T&>{};
    template<typename T> struct is_nothrow_copy_constructible:is_nothrow_copy_constructible_impl<T>{};
    template<typename T> constexpr bool is_nothrow_copy_constructible_v=is_nothrow_copy_constructible<T>::value;

    template<typename T,bool=is_referenceable_v<T>> struct is_nothrow_move_constructible_impl{};
    template<typename T> struct is_nothrow_move_constructible_impl<T,false>:false_type{};
    template<typename T> struct is_nothrow_move_constructible_impl<T,true>:is_nothrow_constructible<T,T&&>{};
    template<typename T> struct is_nothrow_move_constructible:is_nothrow_move_constructible_impl<T>{};
    template<typename T> constexpr bool is_nothrow_move_constructible_v=is_nothrow_move_constructible<T>::value;

    template<typename T,typename U> struct is_nothrow_assignable:conjunction<is_assignable<T,U>,bool_constant<noexcept(declval<T>()=declval<U>())>>{};
    template<typename T,typename U> constexpr bool is_nothrow_assignable_v=is_nothrow_assignable<T,U>::value;

    template<typename T,bool=is_referenceable_v<T>> struct is_nt_copy_assignable_impl;
    template<typename T> struct is_nt_copy_assignable_impl<T,false>:false_type{};
    template<typename T> struct is_nt_copy_assignable_impl<T,true>:is_nothrow_assignable<T&,const T&>{};
    template<typename T> struct is_nothrow_copy_assignable:is_nt_copy_assignable_impl<T>{};
    template<typename T> constexpr bool is_nothrow_copy_assignable_v=is_nothrow_copy_assignable<T>::value;

    template<typename T,bool=is_referenceable_v<T>> struct is_nt_move_assignable_impl;
    template<typename T> struct is_nt_move_assignable_impl<T,false>:false_type{};
    template<typename T> struct is_nt_move_assignable_impl<T,true>:is_nothrow_assignable<T&,T&&>{};
    template<typename T> struct is_nothrow_move_assignable:is_nt_move_assignable_impl<T>{};
    template<typename T> constexpr bool is_nothrow_move_assignable_v=is_nothrow_move_assignable<T>::value;

    template<typename T> struct is_swappable;
    template<typename T> struct is_nothrow_swappable;
    template<typename T> constexpr enable_if_t<is_move_constructible_v<T> && is_move_assignable_v<T>> swap(T& v1,T& v2)noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>);
    template<typename T,size_t size> constexpr enable_if_t<is_swappable<T>::value> swap(T& v1,T& v2)noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>);
    template<typename T,typename U,typename=void> struct swappable_with_helper:false_type{};
    template<typename T,typename U> struct swappable_with_helper<T,U,void_t<decltype(swap(declval<T>(),declval<U>()))>>:true_type{};
    template<typename T,typename U> struct is_swappable_with:conjunction<swappable_with_helper<T,U>,swappable_with_helper<U,T>>{};
    template<typename T,typename U> constexpr bool is_swappable_with_v=is_swappable_with<T,U>::value;
    template<typename T> struct is_swappable:is_swappable_with<add_lvalue_reference_t<T>,add_lvalue_reference_t<T>>{};
    template<typename T> constexpr bool is_swappable_v=is_swappable<T>::value;

    template<typename T,typename U> struct swap_cannot_throw:bool_constant<noexcept(swap(declval<T>(),declval<U>())) && noexcept(swap(declval<U>(),declval<T>()))>{};
    template<typename T,typename U> struct is_nothrow_swappable_with:conjunction<is_swappable_with<T,U>,swap_cannot_throw<T,U>>{};
    template<typename T,typename U> constexpr bool is_nothrow_swappable_with_v=is_nothrow_swappable_with<T,U>::value;
    template<typename T> struct is_nothrow_swappable:is_nothrow_swappable_with<add_lvalue_reference_t<T>,add_lvalue_reference_t<T>>{};
    template<typename T> constexpr bool is_nothrow_swappable_v=is_nothrow_swappable<T>::value;

    template<typename T>
    constexpr remove_reference_t<T>&& move(T&& val)noexcept{
        return static_cast<remove_reference_t<T>&&>(val);
    }
    template<typename T>
    constexpr If_t<!is_nothrow_move_constructible_v<T> && is_copy_constructible_v<T>,const T&,T&&> move_if_noexcept(T& val)noexcept{
        return mpv::move(val);
    }
    
    template<typename T,typename U> using Conditional_type=decltype(false?declval<T>():declval<U>());
    template<typename T,typename U,typename=void> struct Const_lvalue_cond_oper{};
    template<typename T,typename U> struct Const_lvalue_cond_oper<T,U,void_t<Conditional_type<const T&,const U&>>>{using type=remove_cvref_t<Conditional_type<const T&,const U&>>;};
    template<typename T,typename U,typename=void> struct Decayed_cond_oper:Const_lvalue_cond_oper<T,U>{};
    template<typename T,typename U> struct Decayed_cond_oper<T,U,void_t<Conditional_type<T,U>>>{using type=decay_t<Conditional_type<T,U>>;};
    template<typename... Args> struct common_type;
    template<typename... Args> using common_type_t=typename common_type<Args...>::type;
    template<> struct common_type<>{};
    template<typename T> struct common_type<T>:common_type<T,T>{};
    template<typename T,typename U,typename DecayedT=decay_t<T>,typename DecayedU=decay_t<U>> struct Common_type2:common_type<DecayedT,DecayedU>{};
    template<typename T,typename U> struct Common_type2<T,U,T,U>:Decayed_cond_oper<T,U>{};
    template<typename T,typename U> struct common_type<T,U>:Common_type2<T,U>{};
    template<typename Void,typename T,typename U,typename... Args> struct Common_type3{};
    template<typename T,typename U,typename... Args> struct Common_type3<void_t<common_type_t<T,U>>,T,U,Args...>:common_type<common_type_t<T,U>,Args...>{};
    template<typename T,typename U,typename... Args> struct common_type<T,U,Args...>:Common_type3<void,T,U,Args...>{};

    template<typename F> struct FunctionRef{
        F& func;
        template<typename... Args> constexpr decltype(auto) operator()(Args&&... args){
            if constexpr(is_member_pointer_v<F>){
                return mpv::invoke(func,static_cast<Args&&>(args)...);
            }
            else{
                return func(static_cast<Args&&>(args)...);
            }
        }
    };
    template<typename F> constexpr auto pass_func(F& func)noexcept{
        if constexpr(sizeof(F)<=sizeof(void*) && is_trivially_copy_constructible_v<F> && is_trivially_destructible_v<F>) return func;
        else return FunctionRef<F>{func};
    }
}

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

namespace mpv{
    template<typename T> struct non_trivial_copy_ctor:T{
        using T::T;
        non_trivial_copy_ctor()=default;
        constexpr non_trivial_copy_ctor(const non_trivial_copy_ctor& other)noexcept(noexcept(this->construct(static_cast<const T&>(other)))){
            T::construct(static_cast<const T&>(other));// T debe tener un metodo llamado construct para que esto funcione
        }
        non_trivial_copy_ctor(non_trivial_copy_ctor&&)=default;
        non_trivial_copy_ctor& operator=(const non_trivial_copy_ctor&)=default;
        non_trivial_copy_ctor& operator=(non_trivial_copy_ctor&&)=default;
    };
    template<typename T> struct deleted_copy_ctor:T{
        using T::T;
        deleted_copy_ctor()=default;
        deleted_copy_ctor(const deleted_copy_ctor&)=delete;
        deleted_copy_ctor(deleted_copy_ctor&&)=default;
        deleted_copy_ctor& operator=(const deleted_copy_ctor&)=default;
        deleted_copy_ctor& operator=(deleted_copy_ctor&&)=default;
    };
    template<typename T,typename... Args> using smf_control_copy_ctor=
    If_t<conjunction_v<is_trivially_copy_constructible<Args>...>,
        T
        ,
        If_t<conjunction_v<is_copy_constructible<Args>...>,
            non_trivial_copy_ctor<T>
            ,
            deleted_copy_ctor<T>
        >
    >;
    template<typename T,typename... Args> struct non_trivial_move_ctor:smf_control_copy_ctor<T,Args...>{
        using base_type=smf_control_copy_ctor<T,Args...>;
        using base_type::base_type;
        non_trivial_move_ctor()=default;
        non_trivial_move_ctor(const non_trivial_move_ctor&)=default;
        constexpr non_trivial_move_ctor(non_trivial_move_ctor&& other)noexcept(noexcept(this->construct(static_cast<T&&>(other)))){
            base_type::construct(static_cast<T&&>(other));
        }
        non_trivial_move_ctor& operator=(const non_trivial_move_ctor&)=default;
        non_trivial_move_ctor& operator=(non_trivial_move_ctor&&)=default;
    };
    template<typename T,typename... Args> struct deleted_move_ctor:smf_control_copy_ctor<T,Args...>{
        using base_type=smf_control_copy_ctor<T,Args...>;
        using base_type::base_type;
        deleted_move_ctor()=default;
        deleted_move_ctor(const deleted_move_ctor&)=default;
        deleted_move_ctor(deleted_move_ctor&&)=delete;
        deleted_move_ctor& operator=(const deleted_move_ctor&)=default;
        deleted_move_ctor& operator=(deleted_move_ctor&&)=default;
    };
    template<typename T,typename... Args> using smf_control_move_ctor=
    If_t<conjunction_v<is_trivially_move_constructible<Args>...>,
        smf_control_copy_ctor<T,Args...>
        ,
        If_t<conjunction_v<is_move_constructible<Args>...>,
            non_trivial_move_ctor<T,Args...>
            ,
            deleted_move_ctor<T,Args...>
        >
    >;
    template<typename T,typename... Args> struct non_trivial_copy_assign:smf_control_move_ctor<T,Args...>{
        using base_type=smf_control_move_ctor<T,Args...>;
        using base_type::base_type;
        non_trivial_copy_assign()=default;
        non_trivial_copy_assign(const non_trivial_copy_assign&)=default;
        non_trivial_copy_assign(non_trivial_copy_assign&&)=default;
        constexpr non_trivial_copy_assign& operator=(const non_trivial_copy_assign& other)noexcept(noexcept(this->assign(static_cast<const T&>(other)))){
            base_type::assign(static_cast<const T&>(other));
            return *this;
        }
        non_trivial_copy_assign& operator=(non_trivial_copy_assign&&)=default;
    };
    template<typename T,typename... Args> struct deleted_copy_assign:smf_control_move_ctor<T,Args...>{
        using base_type=smf_control_move_ctor<T,Args...>;
        using base_type::base_type;
        deleted_copy_assign()=default;
        deleted_copy_assign(const deleted_copy_assign&)=default;
        deleted_copy_assign(deleted_copy_assign&&)=default;
        deleted_copy_assign& operator=(const deleted_copy_assign&)=delete;
        deleted_copy_assign& operator=(deleted_copy_assign&&)=default;
    };
    template<typename T,typename... Args> using smf_control_copy_assign=
    If_t<conjunction_v<is_trivially_destructible<Args>...,is_trivially_copy_constructible<Args>...,is_trivially_copy_assignable<Args>...>,
        smf_control_move_ctor<T,Args...>
        ,
        If_t<conjunction_v<is_copy_constructible<Args>...,is_copy_assignable<Args>...>,
            non_trivial_copy_assign<T,Args...>
            ,
            deleted_copy_assign<T,Args...>
        >
    >;
    template<typename T,typename... Args> struct non_trivial_move_assign:smf_control_copy_assign<T,Args...>{
        using base_type=smf_control_copy_assign<T,Args...>;
        using base_type::base_type;
        non_trivial_move_assign()=default;
        non_trivial_move_assign(const non_trivial_move_assign&)=default;
        non_trivial_move_assign(non_trivial_move_assign&&)=default;
        non_trivial_move_assign& operator=(const non_trivial_move_assign&)=default;
        constexpr non_trivial_move_assign& operator=(non_trivial_move_assign&& other)noexcept(noexcept(this->assign(static_cast<T&&>(other)))){
            base_type::assign(static_cast<T&&>(other));
            return *this;
        }
    };
    template<typename T,typename... Args> struct deleted_move_assign:smf_control_copy_assign<T,Args...>{
        using base_type=smf_control_copy_assign<T,Args...>;
        using base_type::base_type;
        deleted_move_assign()=default;
        deleted_move_assign(const deleted_move_assign&)=default;
        deleted_move_assign(deleted_move_assign&&)=default;
        deleted_move_assign& operator=(const deleted_move_assign&)=default;
        deleted_move_assign& operator=(deleted_move_assign&&)=delete;
    };

    template<typename T,typename... Args> using smf_control_move_assign=
    If_t<conjunction_v<is_trivially_destructible<Args>...,is_trivially_move_constructible<Args>...,is_trivially_move_assignable<Args>...>,
        smf_control_copy_assign<T,Args...>
        ,
        If_t<conjunction_v<is_move_constructible<Args>...,is_move_assignable<Args>...>,
            non_trivial_move_assign<T,Args...>
            ,
            deleted_move_assign<T,Args...>
        >
    >;
    template<typename T,typename... Args> using smf_control=smf_control_move_assign<T,Args...>;
}

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
            //return __builtin_addressof(x);
            return &x;
        }
    };
    template<typename Pointer,typename T>
    using rebind_pointer=typename pointer_traits<Pointer>::template rebind<T>;
    
    template<typename Pointer> struct is_fancy:true_type{};
    template<typename T> struct is_fancy<T*>:false_type{};
    template<typename Pointer> constexpr bool is_fancy_v=is_fancy<Pointer>::value;

    template<typename Fancy>
    constexpr auto unfancy(Fancy ptr)noexcept{
        return &(*ptr);
    }
    template<typename T>
    constexpr T* unfancy(T* ptr)noexcept{
        return ptr;
    }
    template<typename Fancy>
    constexpr auto unfancy_maybe_null(Fancy ptr)noexcept{
        return ptr ? &(*ptr) : nullptr;
    }
    template<typename T>
    constexpr T* unfancy_maybe_null(T* ptr)noexcept{
        return ptr;
    }

}
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

namespace mpv{
    template<typename> class move_iterator;
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag : input_iterator_tag{};
    struct bidirectional_iterator_tag : forward_iterator_tag{};
    struct random_access_iterator_tag : bidirectional_iterator_tag{};
    struct contiguous_iterator_tag : random_access_iterator_tag {};
    template<typename,typename=void> struct Iterator_traits_base{};
    template<typename It>
    struct Iterator_traits_base<It,void_t<typename It::iterator_category,typename It::value_type,typename It::difference_type,typename It::pointer,typename It::reference>>{
        using iterator_category=typename It::iterator_category;
        using value_type=typename It::value_type;
        using difference_type=typename It::difference_type;
        using pointer=typename It::pointer;
        using reference=typename It::reference;
    };
    template<typename T,bool=is_object_v<T>>
    struct Iterator_traits_pointer_base{
        using iterator_category=contiguous_iterator_tag;
        using value_type=remove_cv_t<T>;
        using difference_type=ptrdiff_t;
        using pointer=T*;
        using reference=T&;
    };
    template<typename T>struct Iterator_traits_pointer_base<T,false>{};
    template<typename T> struct iterator_traits:Iterator_traits_base<T>{};
    template<typename T> struct iterator_traits<T*>:Iterator_traits_pointer_base<T>{};

    template<typename It> constexpr bool is_forward_iterator_v=is_convertible_v<typename iterator_traits<It>::iterator_category,forward_iterator_tag>;
    template<typename It> constexpr bool is_bidirectional_iterator_v=is_convertible_v<typename iterator_traits<It>::iterator_category,bidirectional_iterator_tag>;
    template<typename It> constexpr bool is_random_access_iterator_v=is_convertible_v<typename iterator_traits<It>::iterator_category,random_access_iterator_tag>;
    template<typename It> constexpr bool is_contiguous_iterator_v=is_convertible_v<typename iterator_traits<It>::iterator_category,contiguous_iterator_tag>;
    
    template<typename It> using iter_ref_t=typename iterator_traits<It>::reference;
    template<typename It> using iter_value_t=typename iterator_traits<It>::value_type;
    template<typename It> using iter_diff_t=typename iterator_traits<It>::difference_type;
    template<typename It> using iter_cat_t=typename iterator_traits<It>::iterator_category;

    template<typename T,typename=void> struct is_iterator_impl:false_type{};
    template<typename T> struct is_iterator_impl<T,void_t<iter_cat_t<T>>>:true_type{};
    template<typename T> struct is_iterator:is_iterator_impl<T>{};
    template<typename T> constexpr bool is_iterator_v=is_iterator<T>::value;
    template<typename It1,typename It2> constexpr bool are_contiguous=is_contiguous_iterator_v<It1> && is_contiguous_iterator_v<It2>;
    template<typename It> constexpr bool iterator_is_volatile_v=is_volatile_v<remove_reference_t<iter_ref_t<It>>>;
    template<typename Src,typename Dst> constexpr bool is_pointer_address_convertible=is_void_v<Src> || is_void_v<Dst> || is_same_v<remove_cv_t<Src>,remove_cv_t<Dst>>;
    template<typename It1,typename It2> constexpr bool are_contiguous_and_not_volatile=are_contiguous<It1,It2> && !iterator_is_volatile_v<It1> && !iterator_is_volatile_v<It2>;
    
    template<typename T,typename U,typename=void> constexpr bool can_compare_with_operator_equal=false; 
    template<typename T,typename U> constexpr bool can_compare_with_operator_equal<T,U,void_t<decltype(declval<T&>()==declval<U&>())>> = true; 
    template<typename T,typename U> constexpr bool is_pointer_address_comparable=can_compare_with_operator_equal<T*,U*> && (is_pointer_address_convertible<T,U> || is_pointer_address_convertible<U,T>);
    template<typename Src,typename Dst,typename SrcRef,typename DstRef> struct Trivial_cat{
        using USrc=unwrap_enum_t<Src>;
        using UDst=unwrap_enum_t<Dst>;
        static constexpr bool same_size_and_compatible=sizeof(Src)==sizeof(Dst) && is_same_v<bool,USrc> >= is_same_v<bool,UDst> &&
            (is_same_v<UDst,USrc> || (is_integral_v<USrc> && is_integral_v<UDst>) || (is_floating_point_v<USrc> && is_floating_point_v<UDst>));

        static constexpr bool bitcopy_constructible=same_size_and_compatible && is_trivially_constructible_v<Dst,SrcRef>;
        static constexpr bool bitcopy_assignable=same_size_and_compatible && is_trivially_assignable_v<DstRef,SrcRef>;
    };
    template<typename Src,typename Dst,typename SrcRef,typename DstRef> struct Trivial_cat<Src*,Dst*,SrcRef,DstRef>{
        static constexpr bool bitcopy_constructible=is_pointer_address_convertible<Src,Dst> && is_trivially_constructible_v<Dst*,SrcRef>;
        static constexpr bool bitcopy_assignable=   is_pointer_address_convertible<Src,Dst> && is_trivially_assignable_v<DstRef,SrcRef>;
    };
    struct false_Trivial_cat{
        static constexpr bool bitcopy_constructible=false;
        static constexpr bool bitcopy_assignable=false;
    };
    template<typename SrcIt,typename DstIt,bool=are_contiguous_and_not_volatile<SrcIt,DstIt>> struct iter_move_cat:Trivial_cat<iter_value_t<SrcIt>,iter_value_t<DstIt>,remove_reference_t<iter_ref_t<SrcIt>>&&,iter_ref_t<DstIt>>{};
    template<typename SrcIt,typename DstIt> struct iter_move_cat<SrcIt,DstIt,false>:false_Trivial_cat{};
    template<typename SrcIt,typename DstIt> struct iter_move_cat<move_iterator<SrcIt>,DstIt,false>:iter_move_cat<SrcIt,DstIt>{};
    template<typename SrcIt,typename DstIt,bool=are_contiguous_and_not_volatile<SrcIt,DstIt>> struct iter_copy_cat:Trivial_cat<iter_value_t<SrcIt>,iter_value_t<DstIt>,iter_ref_t<SrcIt>,iter_ref_t<DstIt>>{};
    template<typename SrcIt,typename DstIt> struct iter_copy_cat<SrcIt,DstIt,false>:false_Trivial_cat{};
    template<typename SrcIt,typename DstIt> struct iter_copy_cat<move_iterator<SrcIt>,DstIt,false>:iter_move_cat<SrcIt,DstIt>{};

    template<typename SrcIt,typename SentIt,typename DstIt> using sent_copy_cat=If_t<is_same_v<SrcIt,SentIt>,iter_copy_cat<SrcIt,DstIt>,false_Trivial_cat>;

    template<typename SrcIt,typename DstIt> constexpr bool is_bitcopy_constructible_iter_v     =iter_copy_cat<SrcIt,DstIt>::bitcopy_constructible;
    template<typename SrcIt,typename DstIt> constexpr bool is_bitcopy_assignable_iter_v        =iter_copy_cat<SrcIt,DstIt>::bitcopy_assignable;
    template<typename SrcIt,typename DstIt> constexpr bool is_bitcopy_move_constructible_iter_v=iter_move_cat<SrcIt,DstIt>::bitcopy_constructible;
    template<typename SrcIt,typename DstIt> constexpr bool is_bitcopy_move_assignable_iter_v   =iter_move_cat<SrcIt,DstIt>::bitcopy_assignable;
    template<typename It,typename T> constexpr bool fill_memset_safe_iter=is_contiguous_iterator_v<It> && is_scalar_v<T> && is_integral_v<unwrap_enum_t<remove_reference_t<iter_ref_t<It>>>> && sizeof(remove_reference_t<iter_ref_t<It>>)==1 && !iterator_is_volatile_v<It> && is_assignable_v<iter_ref_t<It>,const T&>;
    template<typename It,typename T> constexpr bool fill_zero_memset_safe_iter=is_contiguous_iterator_v<It> && is_scalar_v<T> && is_scalar_v<iter_value_t<It>> && !is_member_pointer_v<iter_value_t<It>> && !iterator_is_volatile_v<It> && is_assignable_v<iter_ref_t<It>,const T&>;
    template<typename It,typename T> constexpr bool fill_construct_memset_safe_iter=is_contiguous_iterator_v<It> && is_scalar_v<T> && is_integral_v<unwrap_enum_t<remove_reference_t<iter_ref_t<It>>>> && sizeof(remove_reference_t<iter_ref_t<It>>)==1 && !iterator_is_volatile_v<It> && is_constructible_v<remove_reference_t<iter_ref_t<It>>,const T&>;
    template<typename It,typename T> constexpr bool fill_zero_construct_memset_safe_iter=is_contiguous_iterator_v<It> && is_scalar_v<T> && is_scalar_v<iter_value_t<It>> && !is_member_pointer_v<iter_value_t<It>> && !iterator_is_volatile_v<It> && is_constructible_v<remove_reference_t<iter_ref_t<It>>,const T&>;
    
    template<typename T,typename U,bool=sizeof(T)==sizeof(U) && is_integral_v<T> && is_integral_v<U>> constexpr bool can_memcmp_elements=is_same_v<T,bool> || is_same_v<U,bool> || static_cast<T>(-1)==static_cast<U>(-1);
    template<typename T,typename U> constexpr bool can_memcmp_elements<T*,U*,false> = is_pointer_address_comparable<T,U>;
    template<typename T,typename U> constexpr bool can_memcmp_elements<T,U,false> = false;

    template<typename T,typename U,typename Pred> constexpr bool can_memcmp_elements_with_pred=false;
    template<typename T,typename U,typename V> constexpr bool can_memcmp_elements_with_pred<T,U,mpv::equal_to<V>> = iter_copy_cat<T*,V*>::bitcopy_constructible && iter_copy_cat<U*,V*>::bitcopy_constructible && can_memcmp_elements<remove_cv_t<V>,remove_cv_t<V>>;
    template<typename T,typename U> constexpr bool can_memcmp_elements_with_pred<T,U,mpv::equal_to<>> = can_memcmp_elements<T,U>;

    template<typename It1,typename It2,typename Pred> constexpr bool equal_memcmp_is_safe_iter=are_contiguous_and_not_volatile<remove_const_t<It1>,remove_const_t<It2>> && can_memcmp_elements_with_pred<iter_value_t<remove_const_t<It1>>,iter_value_t<remove_const_t<It2>>,Pred>;

    template<typename T,typename U> constexpr bool lex_compare_memcmp_classify_elements=is_unsigned_v<T> && is_unsigned_v<U> && is_integral_v<T> && is_integral_v<U> && sizeof(T)==1 && sizeof(U)==1 && is_scalar_v<T> && is_scalar_v<U>;

    template<typename T,typename U,typename Pred> struct lex_compare_memcmp_classify_pred{using Predicate=void;};
    template<typename T,typename U,typename V> struct lex_compare_memcmp_classify_pred<T,U,mpv::less<V>>{
        using Predicate=If_t<lex_compare_memcmp_classify_elements<V,V> && iter_copy_cat<T*,V*>::bitcopy_constructible && iter_copy_cat<U*,V*>::bitcopy_constructible,less<int>,void>;
    };
    template<typename T,typename U> struct lex_compare_memcmp_classify_pred<T,U,less<>>{
        using Predicate=If_t<lex_compare_memcmp_classify_elements<T,U>,less<int>,void>;
    };
    template<typename T,typename U,typename V> struct lex_compare_memcmp_classify_pred<T,U,mpv::greater<V>>{
        using Predicate=If_t<lex_compare_memcmp_classify_elements<V,V> && iter_copy_cat<T*,V*>::bitcopy_constructible && iter_copy_cat<U*,V*>::bitcopy_constructible,greater<int>,void>;
    };
    template<typename T,typename U> struct lex_compare_memcmp_classify_pred<T,U,greater<>>{
        using Predicate=If_t<lex_compare_memcmp_classify_elements<T,U>,greater<int>,void>;
    };

	template<typename It1,typename It2,typename Pred> using lex_compare_memcmp_classify=If_t<are_contiguous_and_not_volatile<It1,It2>,typename lex_compare_memcmp_classify_pred<iter_value_t<It1>,iter_value_t<It2>,Pred>::Predicate,void>;

    template<typename Container>
    constexpr auto begin(Container& cont)noexcept(noexcept(cont.begin()))->decltype(cont.begin()){
        return cont.begin();
    }
    template<typename Container>
    constexpr auto begin(const Container& cont)noexcept(noexcept(cont.begin()))->decltype(cont.begin()){
        return cont.begin();
    }
    template<typename T,size_t Size>
    constexpr T* begin(T(&array)[Size])noexcept{
        return array;
    }
    template<typename Container>
    constexpr auto end(Container& cont)noexcept(noexcept(cont.end()))->decltype(cont.end()){
        return cont.end();
    }
    template<typename Container>
    constexpr auto end(const Container& cont)noexcept(noexcept(cont.end()))->decltype(cont.end()){
        return cont.end();
    }
    template<typename T,size_t Size>
    constexpr T* end(T(&array)[Size])noexcept{
        return array+Size;
    }
    template<typename Container>
    constexpr auto size(const Container& cont)noexcept(noexcept(cont.size()))->decltype(cont.size()){
        return cont.size();
    }
    template<typename T,size_t Size>
    constexpr size_t size(const T(&)[Size])noexcept{
        return Size;
    }
    template<typename T,typename=void> struct is_iterable:is_size_array<remove_reference_t<T>>{};
    template<typename T> struct is_iterable<T,void_t<decltype(begin(declval<T&>())!=end(declval<T&>())),decltype(++begin(declval<T&>())),decltype(*begin(declval<T&>()))>>
        :conjunction<is_iterator<decltype(begin(declval<T&>()))>,is_iterator<decltype(end(declval<T&>()))>,is_iterator<remove_reference_t<decltype(++begin(declval<T&>()))>>>{};
    template<typename T> constexpr bool is_iterable_v=is_iterable<T>::value;
    
    template<typename T,typename=void> struct is_range:is_size_array<remove_reference_t<T>>{};
    template<typename T> struct is_range<T,void_t<decltype(begin(declval<T&>())),decltype(end(declval<T&>()))>>:true_type{};
    template<typename T> constexpr bool is_range_v=is_range<T>::value;

    template<typename T,typename=void> struct has_size:is_size_array<remove_reference_t<T>>{};
    template<typename T> struct has_size<T,void_t<decltype(size(declval<T&>()))>>:true_type{};
    template<typename T> constexpr bool has_size_v=has_size<T>::value;
    template<typename T> constexpr bool is_sized_range=is_range_v<T> && has_size_v<T>;
}

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

namespace mpv{
    struct in_place_t{};
    struct nontrivial_dummy_t{
        constexpr nontrivial_dummy_t()noexcept{}
    };
    template<typename T,typename=void> struct maybe_trivially_destructible_base{
        union{
            nontrivial_dummy_t _{};
            T val;
        };
        bool has_val;
        constexpr maybe_trivially_destructible_base()noexcept:has_val(false){}
        constexpr maybe_trivially_destructible_base(bool has_val)noexcept:has_val(has_val){}
        template<typename... Args> explicit maybe_trivially_destructible_base(in_place_t,Args&&... args)noexcept(is_nothrow_constructible_v<T,Args...>):val(static_cast<Args&&>(args)...),has_val(true){}
        ~maybe_trivially_destructible_base()noexcept{
            if(has_val)val.~T();
        }
        constexpr void reset()noexcept{
            if(has_val){
                val.~T();
                has_val=false;
            }
        }
    };
    template<typename T> struct maybe_trivially_destructible_base<T,enable_if_t<is_trivially_destructible_v<T>>>{
        union{
            nontrivial_dummy_t _{};
            T val;
        };
        bool has_val;
        constexpr maybe_trivially_destructible_base()noexcept:has_val(false){}
        constexpr maybe_trivially_destructible_base(bool has_val)noexcept:has_val(has_val){}
        template<typename... Args> explicit maybe_trivially_destructible_base(in_place_t,Args&&... args)noexcept(is_nothrow_constructible_v<T,Args...>):val(static_cast<Args&&>(args)...),has_val(true){}
        constexpr void reset()noexcept{
            has_val=false;
        }
        // maybe_trivially_destructible_base(const maybe_trivially_destructible_base&)=default;
        // maybe_trivially_destructible_base(maybe_trivially_destructible_base&&)=default;
        // maybe_trivially_destructible_base& operator=(const maybe_trivially_destructible_base&)=default;
        // maybe_trivially_destructible_base& operator=(maybe_trivially_destructible_base&&)=default;
    };
    template<typename T> struct optional_base:maybe_trivially_destructible_base<T>{
        using maybe_trivially_destructible_base<T>::maybe_trivially_destructible_base;
        template<typename Self_type>
        constexpr void construct(Self_type&& other)noexcept(noexcept(new(&this->val) T(static_cast<Self_type&&>(other).val))){
            if(other.has_val){
                new(&this->val) T(static_cast<Self_type&&>(other).val);
                this->has_val=true;
            }
        }
        template<typename Self_type>
        constexpr void assign(Self_type&& other)noexcept(noexcept(new(&this->val) T(static_cast<Self_type&&>(other).val)) && noexcept(this->val=static_cast<Self_type&&>(other).val)){
            if(other.has_val){
                if(this->has_val){
                    this->val=static_cast<Self_type&&>(other).val;
                }
                else{
                    new(&this->val) T(static_cast<Self_type&&>(other).val);
                    this->has_val=true;
                }
            }
            else if(this->has_val){
                this->val.~T();
                this->has_val=false;
            }
        }
    };
    template<typename T>
    class Optional:private smf_control<optional_base<T>,T>{
        template<typename U> friend class Optional;
        using base_type=smf_control<optional_base<T>,T>;
        template<typename U> static constexpr bool allow_unwrapping=is_same_v<remove_cvref_t<T>,bool> ||
            !(is_same_v<T,U> || is_constructible_v<T,Optional<U>&> ||
            is_constructible_v<T,const Optional<U>&> || is_constructible_v<T,const Optional<U>> ||
            is_constructible_v<T,Optional<U>> || is_convertible_v<Optional<U>&,T> ||
            is_convertible_v<const Optional<U>&,T> || is_convertible_v<const Optional<U>,T>
            || is_convertible_v<Optional<U>,T>);
        template<typename U> static constexpr bool allow_direct_conversion = 
            !is_same_v<remove_cvref_t<U>,Optional> && !is_same_v<remove_cvref_t<U>,in_place_t> &&
            !(is_same_v<remove_cv_t<T>,bool> && is_specialization_v<remove_cvref_t<U>,Optional>) &&
            is_constructible_v<T,U>;
        template<typename U> static constexpr bool allow_unwrapping_assignment =
            !(is_same_v<T,U> || is_assignable_v<T&,Optional<U>&> || is_assignable_v<T&,const Optional<U>&> ||
            is_assignable_v<T&,const Optional<U>> || is_assignable_v<T&,Optional<U>>);
        public:
            using value_type=T;
            using size_type=size_t;
            using difference_type=ptrdiff_t;
            using pointer=T*;
            using const_pointer=const pointer;
            using reference=T&; 
            using const_reference=const T&;
            using base_type::reset;
            constexpr Optional()noexcept{};
            template<typename U,enable_if_t<allow_unwrapping<U> && is_constructible_v<T,const U&> && !is_convertible_v<const U&,T>>* = nullptr>
            constexpr explicit Optional(const Optional<U>& other)noexcept(is_nothrow_constructible_v<T,const U&>):base_type(other.has_val){
                if(this->has_val)
                    new(&this->val) T(other.val);
            }
            template<typename U,enable_if_t<allow_unwrapping<U> && is_constructible_v<T,const U&> && is_convertible_v<const U&,T>>* = nullptr>
            constexpr Optional(const Optional<U>& other)noexcept(is_nothrow_constructible_v<T,const U&>):base_type(other.has_val){
                if(this->has_val)
                    new(&this->val) T(other.val);
            }
            template<typename U,enable_if_t<allow_unwrapping<U> && is_constructible_v<T,U> && !is_convertible_v<U,T>>* = nullptr>
            constexpr explicit Optional(Optional<U>&& other)noexcept(is_nothrow_constructible_v<T,U>):base_type(other.has_val){
                if(this->has_val)
                    new(&this->val) T(static_cast<U&&>(other.val));
            }
            template<typename U,enable_if_t<allow_unwrapping<U> && is_constructible_v<T,U> && is_convertible_v<U,T>>* = nullptr>
            constexpr Optional(Optional<U>&& other)noexcept(is_nothrow_constructible_v<T,U>):base_type(other.has_val){
                if(this->has_val)
                    new(&this->val) T(static_cast<U&&>(other.val));
            }
            template<typename U=T,enable_if_t<allow_direct_conversion<U> && is_convertible_v<U,T>>* = nullptr>
            constexpr Optional(U&& v)noexcept(is_nothrow_constructible_v<T,U>):base_type(in_place_t{},static_cast<U&&>(v)){}

            template<typename U=T,enable_if_t<allow_direct_conversion<U> && !is_convertible_v<U,T>>* = nullptr>
            constexpr explicit Optional(U&& v)noexcept(is_nothrow_constructible_v<T,U>):base_type(in_place_t{},static_cast<U&&>(v)){}

            template<typename... Args,enable_if_t<is_constructible_v<T,Args...>>* = nullptr>
            constexpr Optional(in_place_t,Args&&... args)noexcept(is_nothrow_constructible_v<T,Args...>):base_type(in_place_t{},static_cast<Args&&>(args)...){}

            template<typename Elem,typename... Args,enable_if_t<is_constructible_v<T,std::initializer_list<Elem>&,Args...>>* = nullptr>
            constexpr Optional(in_place_t,std::initializer_list<Elem> ilist,Args&&... args)noexcept(is_nothrow_constructible_v<T,std::initializer_list<Elem>&,Args...>):base_type(in_place_t{},ilist,static_cast<Args&&>(args)...){}

            template<typename U,enable_if_t<allow_unwrapping_assignment<U> && is_constructible_v<T,const U&> && is_assignable_v<T&,const U&>>* = nullptr>
            constexpr Optional& operator=(const Optional<U>& other)noexcept(is_nothrow_constructible_v<T,const U&> && is_nothrow_assignable_v<T&,const U&>){
                if(other.has_val){
                    if(this->has_val)
                        this->val=other.val;
                    else{
                        new(&this->val)T(other.val);
                        this->has_val=true;
                    }
                }
                else if(this->has_val)
                    this->reset();
                return *this;
            }
            template<typename U,enable_if_t<allow_unwrapping_assignment<U> && is_constructible_v<T,U> && is_assignable_v<T&,U>>* = nullptr>
            constexpr Optional& operator=(Optional<U>&& other)noexcept(is_nothrow_constructible_v<T,U> && is_nothrow_assignable_v<T&,U>){
                if(other.has_val){
                    if(this->has_val)
                        this->val=static_cast<U&&>(other.val);
                    else{
                        new(&this->val)T(static_cast<U&&>(other.val));
                        this->has_val=true;
                    }
                }
                else if(this->has_val)
                    this->reset();
                return *this;
            }
            template<typename U=T,enable_if_t<!is_same_v<Optional,remove_cvref_t<U>> && !(is_scalar_v<T> && is_same_v<T,decay_t<U>>) && is_constructible_v<T,U> && is_assignable_v<T&,U>>* = nullptr>
            constexpr Optional& operator=(U&& v)noexcept(is_nothrow_constructible_v<T,U> && is_nothrow_assignable_v<T&,U>){
                if(this->has_val){
                    this->val=static_cast<U&&>(v);
                }
                else{
                    new(&this->val)T(static_cast<U&&>(v));
                    this->has_val=true;
                }
                return *this;
            }
            template<typename... Args>
            constexpr void emplace(Args&&... args)noexcept(is_nothrow_constructible_v<T,Args...>){
                if(this->has_val){
                    this->val.~T();
                    this->has_val=false;
                }
                new(&this->val) T(static_cast<Args&&>(args)...);
                this->has_val=true;
            }
            template<typename Elem,typename... Args>
            constexpr void emplace(std::initializer_list<Elem> ilist,Args&&... args)noexcept(is_nothrow_constructible_v<T,std::initializer_list<Elem>&,Args...>){
                if(this->has_val){
                    this->val.~T();
                    this->has_val=false;
                }
                new(&this->val) T(ilist,static_cast<Args&&>(args)...);
                this->has_val=true;
            }
            constexpr bool has_value()const noexcept{
                return this->has_val;
            }
            constexpr explicit operator bool()const noexcept{
                return this->has_val;
            }
            constexpr T& value()& noexcept{
                return this->val;
            }
            constexpr const T& value()const& noexcept{
                return this->val;
            }
            constexpr T&& value()&& noexcept{
                return mpv::move(this->val);
            }
            constexpr const T&& value()const&& noexcept{
                return mpv::move(this->val);
            }
            template<typename U>
            constexpr remove_cv_t<T> value_or(U&& v)const&{
                return this->has_val ? static_cast<const T&>(this->val) : static_cast<remove_cv_t<T>>(static_cast<U&&>(v));
            }
            template<typename U>
            constexpr remove_cv_t<T> value_or(U&& v)&&{
                return this->has_val ? static_cast<T&&>(this->val) : static_cast<remove_cv_t<T>>(static_cast<U&&>(v));
            }
            constexpr T& operator*()& noexcept{
                return this->val;
            }
            constexpr const T& operator*()const& noexcept{
                return this->val;
            }
            constexpr T&& operator*()&& noexcept{
                return mpv::move(this->val);
            }
            constexpr const T&& operator*()const&& noexcept{
            	return mpv::move(this->val);
			}
            constexpr T* operator->()noexcept{
                return &this->val;
            }
            constexpr const T* operator->()const noexcept{
                return &this->val;
            }
    };
    template<typename T> Optional(T) -> Optional<T>;
    template<typename T,typename U>
    constexpr bool operator==(const Optional<T>& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(*self==*other))){
        if(self.has_value() && other.has_value()) return *self==*other;
        else return self.has_value()==other.has_value();
    }
    template<typename T,typename U>
    constexpr bool operator==(const Optional<T>& self,const U& other)noexcept(noexcept(fake_copy_init<bool>(*self==other))){
        if(self.has_value()) return *self==other;
        else return false;
    }
    template<typename T,typename U>
    constexpr bool operator==(const T& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(self==*other))){
        if(other.has_value()) return self==*other;
        else return false;
    }
    template<typename T,typename U>
    constexpr bool operator!=(const Optional<T>& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(*self!=*other))){
        if(self.has_value() && other.has_value()) return *self!=*other;
        else return self.has_value()!=other.has_value();
    }
    template<typename T,typename U>
    constexpr bool operator!=(const Optional<T>& self,const U& other)noexcept(noexcept(fake_copy_init<bool>(*self!=other))){
        if(self.has_value()) return *self!=other;
        else return true;
    }
    template<typename T,typename U>
    constexpr bool operator!=(const T& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(self!=*other))){
        if(other.has_value()) return self!=*other;
        else return true;
    }
    template<typename T,typename U>
    constexpr bool operator<(const Optional<T>& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(*self<*other))){
        if(self.has_value() && other.has_value()) return *self<*other;
        else return self.has_value()<other.has_value();
    }
    template<typename T,typename U>
    constexpr bool operator<(const Optional<T>& self,const U& other)noexcept(noexcept(fake_copy_init<bool>(*self<other))){
        if(self.has_value()) return *self<other;
        else return true;
    }
    template<typename T,typename U>
    constexpr bool operator<(const T& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(self<*other))){
        if(other.has_value()) return self<*other;
        else return false;
    }
    template<typename T,typename U>
    constexpr bool operator>(const Optional<T>& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(*self>*other))){
        if(self.has_value() && other.has_value()) return *self>*other;
        else return self.has_value()>other.has_value();
    }
    template<typename T,typename U>
    constexpr bool operator>(const Optional<T>& self,const U& other)noexcept(noexcept(fake_copy_init<bool>(*self>other))){
        if(self.has_value()) return *self>other;
        else return false;
    }
    template<typename T,typename U>
    constexpr bool operator>(const T& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(self>*other))){
        if(other.has_value()) return self>*other;
        else return true;
    }
    template<typename T,typename U>
    constexpr bool operator<=(const Optional<T>& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(*self<=*other))){
        if(self.has_value() && other.has_value()) return *self<=*other;
        else return self.has_value()<=other.has_value();
    }
    template<typename T,typename U>
    constexpr bool operator<=(const Optional<T>& self,const U& other)noexcept(noexcept(fake_copy_init<bool>(*self<=other))){
        if(self.has_value()) return *self<=other;
        else return true;
    }
    template<typename T,typename U>
    constexpr bool operator<=(const T& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(self<=*other))){
        if(other.has_value()) return self<=*other;
        else return false;
    }
    template<typename T,typename U>
    constexpr bool operator>=(const Optional<T>& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(*self>=*other))){
        if(self.has_value() && other.has_value()) return *self>=*other;
        else return self.has_value()>=other.has_value();
    }
    template<typename T,typename U>
    constexpr bool operator>=(const Optional<T>& self,const U& other)noexcept(noexcept(fake_copy_init<bool>(*self>=other))){
        if(self.has_value()) return *self>=other;
        else return false;
    }
    template<typename T,typename U>
    constexpr bool operator>=(const T& self,const Optional<U>& other)noexcept(noexcept(fake_copy_init<bool>(self>=*other))){
        if(other.has_value()) return self>=*other;
        else return true;
    }
    template<typename Out,typename T>
    Out& operator<<(Out& stream,const Optional<T>& opt){
    	if(opt.has_value()) stream<<""<<opt.value();
    	else stream<<"<empty>";
    	return stream;
    }
}
  

namespace mpv{
	template<typename T>
	struct TemporaryOwner{
		remove_extent_t<T>* ptr;
		constexpr explicit TemporaryOwner(remove_extent_t<T>* const ptr)noexcept:ptr(ptr){}
		TemporaryOwner(const TemporaryOwner&)=delete;
		TemporaryOwner& operator=(const TemporaryOwner&)=delete;
		~TemporaryOwner(){
			if constexpr(is_no_size_array_v<T>) delete[] ptr;
			else 								delete ptr;
		}
	};
	template<typename Tp,typename Del>
	struct TemporaryOwnerDel{
		Tp ptr;
		Del& dt;
		bool call_deleter=true;
		constexpr explicit TemporaryOwnerDel(const Tp ptr,Del& dt)noexcept:ptr(ptr),dt(dt){}
		TemporaryOwnerDel(const TemporaryOwnerDel&)=delete;
		TemporaryOwnerDel& operator=(const TemporaryOwnerDel&)=delete;
		~TemporaryOwnerDel(){
			if(call_deleter) dt(ptr);
		}
	};
	struct GlobalDeleteGuard{
		void* ptr;
		~GlobalDeleteGuard()noexcept{
			::operator delete(ptr);
		}
	};
	template<typename Alloc>
	struct AllocatorDeleteGuard{
		Alloc& al;
		typename allocator_traits<Alloc>::pointer ptr;
		size_t size;
		constexpr AllocatorDeleteGuard(Alloc& al,const size_t n):al(al),ptr(allocator_traits<Alloc>::allocate(al,n)),size(n){}
		~AllocatorDeleteGuard(){
			if(ptr) allocator_traits<Alloc>::deallocate(al,ptr,size);
		}
		AllocatorDeleteGuard(const AllocatorDeleteGuard&)=delete;
		AllocatorDeleteGuard& operator=(const AllocatorDeleteGuard&)=delete;
	};
    template<typename Alloc>
    struct AllocConstructPtr{
        using pointer=typename allocator_traits<Alloc>::pointer;
        Alloc& al;
        pointer ptr;
        constexpr explicit AllocConstructPtr(Alloc& al):al(al),ptr(nullptr){}
        constexpr pointer release()noexcept{
            pointer aux=ptr;
            ptr=nullptr;
            return aux;
        }
        constexpr void allocate(){
            ptr=nullptr;
            ptr=allocator_traits<Alloc>::allocate(al,1);
        }
        ~AllocConstructPtr()noexcept{
            if(ptr) allocator_traits<Alloc>::deallocate(al,ptr,1);
        }
        AllocConstructPtr(const AllocConstructPtr&)=delete;
        AllocConstructPtr& operator=(const AllocConstructPtr&)=delete;
    };
	template<typename Alloc,typename It>
	struct DestroyGuard{
		Alloc& al;
		It iter;
		bool destroy=true;
		constexpr DestroyGuard(Alloc& al,It iter)noexcept(is_nothrow_constructible_v<It>):al(al),iter(iter){}
		~DestroyGuard()noexcept{
			if constexpr(!Uses_default_destroy_v<Alloc,iter_value_t<It>*> || !is_trivially_destructible_v<iter_value_t<It>>){
				if(destroy) allocator_traits<Alloc>::destroy(al,unfancy(iter));
			}
		}
		DestroyGuard(const DestroyGuard&)=delete;
		DestroyGuard& operator=(const DestroyGuard&)=delete;
	};
	template<typename Alloc,typename It>
	struct DestroySequenceGuard{// User should increment count every time an element is constructed and set it back to zero when all the elements are successfully constructed. If count is not zero when DestroySequenceGuard reaches the end of it's scope, it will assume an exception occurred during the construction of the elements and will destroy all the constructed elements
		using size_type=typename allocator_traits<Alloc>::size_type;
		Alloc& al;
		It iter;
		size_type count=0;
		constexpr DestroySequenceGuard(Alloc& al,It iter)noexcept(is_nothrow_constructible_v<It>):al(al),iter(iter){}
		constexpr DestroySequenceGuard(Alloc& al,It iter,size_type count)noexcept(is_nothrow_constructible_v<It>):al(al),iter(iter),count(count){}
		~DestroySequenceGuard()noexcept(noexcept(iter++)){
			if constexpr(!Uses_default_destroy_v<Alloc,iter_value_t<It>*> || !is_trivially_destructible_v<iter_value_t<It>>){
				while(count-->0) allocator_traits<Alloc>::destroy(al,unfancy(iter++));
			}
		}
		DestroySequenceGuard(const DestroySequenceGuard&)=delete;
		DestroySequenceGuard& operator=(const DestroySequenceGuard&)=delete;
	};//It probably only makes sens to use DestroySequenceGuard with an array
	template<typename T>
	struct DefaultDestroyArrayGuard{
		T* array;
		size_t count=0;
		constexpr DefaultDestroyArrayGuard(T* array)noexcept:array(array){}
		constexpr DefaultDestroyArrayGuard(T* array,size_t count)noexcept:array(array),count(count){}
		~DefaultDestroyArrayGuard()noexcept{
			if(array==nullptr) return;
			while(count>0) (array+--count)->~T();
		}
		DefaultDestroyArrayGuard(const DefaultDestroyArrayGuard&)=delete;
		DefaultDestroyArrayGuard& operator=(const DefaultDestroyArrayGuard&)=delete;
	};
}

namespace mpv{
	template<typename It>
	constexpr typename iterator_traits<It>::difference_type distance(It first,It last){
		if constexpr(is_random_access_iterator_v<It>) return last-first;
		else{
			typename iterator_traits<It>::difference_type dist=0;
			while(first!=last){
				++dist;
				++first;
			}
			return dist;
		}
	}
	template<typename It,typename Diff>
	constexpr void advance(It& it,Diff offset){
		if constexpr(is_random_access_iterator_v<It>){
			it+=offset;
		}
		else{
			if constexpr(is_signed_v<Diff> && is_bidirectional_iterator_v<It>){
				while(offset<0){
					--it;
					++offset;
				}
			}
			while(offset>0){
				++it;
				--offset;
			}
		}
	}
	template<typename T,typename U>
	constexpr void copy_trivial(T* dest,const U* source,size_t size){// If it does overlap, source should be >= dest
		if constexpr(USE_MEMCPY){
			MEMMOVE(dest,source,size*sizeof(U));
		}else{
			while(size--)
				*(dest++)=*(source++);
		}
	}
	template<typename T,typename U>
	constexpr void copy_backward_trivial(T* destlast,const U* sourcelast,size_t size){// If it does overlap, source should be <= dest
		if constexpr(USE_MEMCPY){
			MEMMOVE(destlast-size,sourcelast-size,size*sizeof(U));
		}else{
			while(size--)
				*(--destlast)=*(--sourcelast);
		}
	}
	template<typename T,typename U>
	constexpr void reverse_copy_trivial(T* dest,const U* source,size_t size){// If it does overlap, source should be <= dest
		if constexpr(USE_MEMCPY){
			MEMMOVE(dest,source,size*sizeof(U));
		}else{
			dest+=size;source+=size;
			while(size--)
				*(--dest)=*(--source);
		}
	}
	template<typename T,typename U>
	constexpr void copy_trivial_no_overlap(T* dest,const U* source,size_t size){// If it does overlap, source should be >= dest
		if constexpr(USE_MEMCPY){
			MEMCPY(dest,source,size*sizeof(U));
		}else{
			while(size--)
				*(dest++)=*(source++);
		}
	}
	template<typename T,typename U>
	constexpr void copy_trivial_overlap(T* dest,const U* source,size_t size){
		if constexpr(USE_MEMCPY){
			MEMMOVE(dest,source,size*sizeof(U));
		}else{
			if(source>=dest){
				while(size--)
					*(dest++)=*(source++);				
			}
			else{
				dest+=size;source+=size;
				while(size--)
					*(--dest)=*(--source);
			}
		}
	}
	template<typename T,typename V>
	constexpr void fill_trivial(T* dest,size_t size,const V val){
		if constexpr(USE_MEMCPY){
			if constexpr(fill_memset_safe_iter<T*,V>){
				MEMSET(dest,val,size*sizeof(T));
				return;
			}
			else if constexpr(fill_zero_memset_safe_iter<T*,V>){
				if(is_all_bits_zero(val)){
					MEMSET(dest,0,size*sizeof(T));
					return;
				}
			}
		}
		while(size--)
			*(dest++)=val;
	}
	template<typename T,typename U>
	constexpr int same_size_equal_trivial(const T* a,const U* b,size_t size){
		if constexpr(USE_MEMCPY){
			return 0==MEMCMP(a,b,size*sizeof(U));
		}else{
			while(size--){
				if(*(a++)!=*(b++)) return false;
			}
			return true;
		}
	}
	template<typename T,typename U>
	constexpr int same_size_lex_compare_trivial(const T* a,const U* b,size_t size){
		if constexpr(USE_MEMCPY){
			return MEMCMP(a,b,size*sizeof(U));
		}
		else{
			while(size--){
				if(*a!=*b) return *a<*b ? -1 : 1;
				a++;
				b++;
			}
			return 0;			
		}
	}
	template<typename OutIt,typename InIt>
	constexpr void copy(OutIt dest,InIt first,InIt last){
		if constexpr(!is_bitcopy_assignable_iter_v<InIt,OutIt>){
			while(first!=last)
				*(dest++)=*(first++);
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename BidIt1,typename BidIt2>
	constexpr void copy_backward(BidIt1 destlast,BidIt2 first,BidIt2 last){
		if constexpr(!is_bitcopy_assignable_iter_v<BidIt2,BidIt1>){
			while(last!=first)
				*(--destlast)=*(--last);
		}
		else{
			copy_backward_trivial(unfancy(destlast),unfancy(last),last-first);
		}
	}
	template<typename BidIt1,typename BidIt2>
	constexpr void copy_reverse(BidIt1 dest,BidIt2 first,BidIt2 last){
		if constexpr(!is_bitcopy_assignable_iter_v<BidIt2,BidIt1>){
			advance(dest,distance(first,last));
			while(last!=first)
				*(--dest)=*(--last);
		}
		else{
			reverse_copy_trivial(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename RandIt1,typename RandIt2>
	constexpr void copy_overlap(RandIt1 dest,RandIt2 first,RandIt2 last){
		if constexpr(!is_bitcopy_assignable_iter_v<RandIt2,RandIt1>){
			if(first>=dest){
				while(first!=last)
					*(dest++)=*(first++);
			}
			else{
				advance(dest,distance(first,last));
				while(last!=first)
					*(--dest)=*(--last);
			}
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename OutIt,typename InIt>
	constexpr void move(OutIt dest,InIt first,InIt last){
		if constexpr(!is_bitcopy_move_assignable_iter_v<InIt,OutIt>){
			while(first!=last)
				*(dest++)=mpv::move(*(first++));
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename BidIt1,typename BidIt2>
	constexpr void move_backward(BidIt1 destlast,BidIt2 first,BidIt2 last){
		if constexpr(!is_bitcopy_move_assignable_iter_v<BidIt2,BidIt1>){
			while(last!=first)
				*(--destlast)=mpv::move(*(--last));
		}
		else{
			copy_backward_trivial(unfancy(destlast),unfancy(last),last-first);
		}
	}
	template<typename BidIt1,typename BidIt2>
	constexpr void move_reverse(BidIt1 dest,BidIt2 first,BidIt2 last){
		if constexpr(!is_bitcopy_move_assignable_iter_v<BidIt2,BidIt1>){
			advance(dest,distance(first,last));
			while(last!=first)
				*(--dest)=mpv::move(*(--last));
		}
		else{
			reverse_copy_trivial(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename FwdIt,typename T>
	constexpr void fill(FwdIt first,FwdIt last,const T& val){
		if constexpr(!fill_memset_safe_iter<FwdIt,T> && !fill_zero_memset_safe_iter<FwdIt,T>){
			while(first!=last)
				*(first++)=val;
		}
		else{
			fill_trivial(unfancy(first),last-first,val);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt>
	constexpr void copy_construct(Alloc& alloc,OutIt dest,InIt first,InIt last){
		if constexpr(!is_bitcopy_constructible_iter_v<InIt,OutIt>){
			DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
			while(first!=last){
				allocator_traits<Alloc>::construct(alloc,unfancy(dest++),*(first++));
				++guard.count;
			}
			guard.count=0;
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt>
	constexpr void move_construct(Alloc& alloc,OutIt dest,InIt first,InIt last){
		if constexpr(!is_bitcopy_move_constructible_iter_v<InIt,OutIt>){
			DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
			while(first!=last){
				allocator_traits<Alloc>::construct(alloc,unfancy(dest++),mpv::move(*(first++)));
				++guard.count;
			}
			guard.count=0;
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt>
	constexpr void move_construct_if_nt(Alloc& alloc,OutIt dest,InIt first,InIt last){
		if constexpr(!is_bitcopy_move_constructible_iter_v<InIt,OutIt>){
			DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
			while(first!=last){
				allocator_traits<Alloc>::construct(alloc,unfancy(dest++),mpv::move_if_noexcept(*(first++)));
				++guard.count;
			}
			guard.count=0;
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename Alloc,typename FwdIt,typename T>
	constexpr void fill_construct(Alloc& alloc,FwdIt first,FwdIt last,const T& val){
		if constexpr(!fill_construct_memset_safe_iter<FwdIt,T> && !fill_zero_construct_memset_safe_iter<FwdIt,T>){
			DestroySequenceGuard<Alloc,FwdIt> guard(alloc,first);
			while(first!=last){
				allocator_traits<Alloc>::construct(alloc,unfancy(first++),val);
				++guard.count;
			}
			guard.count=0;
		}
		else{
			fill_trivial(unfancy(first),last-first,val);
		}
	}
	template<typename Alloc,typename FwdIt>
	constexpr void default_construct(Alloc& alloc,FwdIt first,FwdIt last){
		if constexpr(!is_trivially_default_constructible_v<iter_value_t<FwdIt>> || !Uses_default_construct_v<Alloc,decltype(unfancy(first++))>){
			DestroySequenceGuard<Alloc,FwdIt> guard(alloc,first);
			while(first!=last){
				allocator_traits<Alloc>::construct(alloc,unfancy(first++));
				++guard.count;
			}
			guard.count=0;
		}
	}
	template<typename Alloc,typename FwdIt>
	constexpr void destroy(Alloc& alloc,FwdIt first,FwdIt last)noexcept(noexcept(first++)){
		if constexpr(!is_trivially_destructible_v<iter_value_t<FwdIt>> || !Uses_default_destroy_v<Alloc,decltype(unfancy(first++))>){
			while(first!=last)
				allocator_traits<Alloc>::destroy(alloc,unfancy(first++));
		}
	}
	template<typename OutIt,typename InIt,typename Size>
	constexpr void copy_n(OutIt dest,InIt first,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<InIt,OutIt>){
			if(size>0){
				while(true){// Avoids extra increment whith single pass imput iterators. This is not an issue in copy_backward_n and copy_reverse_n because they only accept bidirectional iterators
					*(dest++)=*(first);
					if(--size==0) break;
					++first;
				}
			}
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename BidIt1,typename BidIt2,typename Size>
	constexpr void copy_backward_n(BidIt1 destlast,BidIt2 last,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<BidIt2,BidIt1>){
			while(size--)
				*(--destlast)=*(--last);
		}
		else{
			copy_backward_trivial(unfancy(destlast),unfancy(last),size);
		}
	}
	template<typename BidIt1,typename BidIt2,typename Size>
	constexpr void copy_reverse_n(BidIt1 dest,BidIt2 first,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<BidIt2,BidIt1>){
			advance(dest,size);
			advance(first,size);
			while(size--)
				*(--dest)=*(--first);
		}
		else{
			reverse_copy_trivial(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename RandIt1,typename RandIt2,typename Size>
	constexpr void copy_overlap_n(RandIt1 dest,RandIt2 first,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<RandIt2,RandIt1>){
			if(first>=dest){
				while(size--)
					*(dest++)=*(first++);
			}
			else{
				advance(dest,size);
				advance(first,size);
				while(size--)
					*(--dest)=*(--first);
			}
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename OutIt,typename InIt,typename Size>
	constexpr void move_n(OutIt dest,InIt first,Size size){
		if constexpr(!is_bitcopy_move_assignable_iter_v<InIt,OutIt>){
			if(size>0){
				while(true){// Avoids extra increment whith single pass imput iterators. This is not an issue in move_backward_n and move_reverse_n because they only accept bidirectional iterators
					*(dest++)=mpv::move(*(first));
					if(--size==0) break;
					++first;
				}
			}
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename BidIt1,typename BidIt2,typename Size>
	constexpr void move_backward_n(BidIt1 destlast,BidIt2 last,Size size){
		if constexpr(!is_bitcopy_move_assignable_iter_v<BidIt2,BidIt1>){
			while(size--)
				*(--destlast)=mpv::move(*(--last));
		}
		else{
			copy_backward_trivial(unfancy(destlast),unfancy(last),size);
		}
	}
	template<typename BidIt1,typename BidIt2,typename Size>
	constexpr void move_reverse_n(BidIt1 dest,BidIt2 first,Size size){
		if constexpr(!is_bitcopy_move_assignable_iter_v<BidIt2,BidIt1>){
			advance(dest,size);
			advance(first,size);
			while(size--)
				*(--dest)=mpv::move(*(--first));
		}
		else{
			reverse_copy_trivial(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename FwdIt,typename Size,typename T>
	constexpr void fill_n(FwdIt first,Size size,const T& val){
		if constexpr(!fill_memset_safe_iter<FwdIt,T> && !fill_zero_memset_safe_iter<FwdIt,T>){
			while(size--)
				*(first++)=val;
		}
		else{
			fill_trivial(unfancy(first),size,val);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt,typename Size>
	constexpr void copy_construct_n(Alloc& alloc,OutIt dest,InIt first,Size size){
		if constexpr(!is_bitcopy_constructible_iter_v<InIt,OutIt>){
			if(size>0){
				DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
				while(true){
					allocator_traits<Alloc>::construct(alloc,unfancy(dest++),*(first));
					++guard.count;
					if(--size==0) break;
					++first;
				}
				guard.count=0;				
			}
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt,typename Size>
	constexpr void move_construct_n(Alloc& alloc,OutIt dest,InIt first,Size size){
		if constexpr(!is_bitcopy_move_constructible_iter_v<InIt,OutIt>){
			if(size>0){
				DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
				while(true){
					allocator_traits<Alloc>::construct(alloc,unfancy(dest++),mpv::move(*(first)));
					++guard.count;
					if(--size==0) break;
					++first;
				}
				guard.count=0;				
			}
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt,typename Size>
	constexpr void move_construct_if_nt_n(Alloc& alloc,OutIt dest,InIt first,Size size){
		if constexpr(!is_bitcopy_move_constructible_iter_v<InIt,OutIt>){
			if(size>0){
				DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
				while(true){
					allocator_traits<Alloc>::construct(alloc,unfancy(dest++),mpv::move_if_noexcept(*(first)));
					++guard.count;
					if(--size==0) break;
					++first;
				}
				guard.count=0;				
			}
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename Alloc,typename FwdIt,typename T,typename Size>
	constexpr void fill_construct_n(Alloc& alloc,FwdIt first,Size size,const T& val){
		if constexpr(!fill_construct_memset_safe_iter<FwdIt,T> && !fill_zero_construct_memset_safe_iter<FwdIt,T>){
			DestroySequenceGuard<Alloc,FwdIt> guard(alloc,first);
			while(size--){
				allocator_traits<Alloc>::construct(alloc,unfancy(first++),val);
				++guard.count;
			}
			guard.count=0;
		}
		else{
			fill_trivial(unfancy(first),size,val);
		}
	}
	template<typename Alloc,typename FwdIt,typename Size>
	constexpr void default_construct_n(Alloc& alloc,FwdIt first,Size size){
		if constexpr(!is_trivially_default_constructible_v<iter_value_t<FwdIt>> || !Uses_default_construct_v<Alloc,decltype(unfancy(first++))>){
			DestroySequenceGuard<Alloc,FwdIt> guard(alloc,first);
			while(size--){
				allocator_traits<Alloc>::construct(alloc,unfancy(first++));
				++guard.count;
			}
			guard.count=0;
		}
	}
	template<typename Alloc,typename FwdIt,typename Size>
	constexpr void destroy_n(Alloc& alloc,FwdIt first,Size size)noexcept(noexcept(first++)){
		if constexpr(!is_trivially_destructible_v<iter_value_t<FwdIt>> || !Uses_default_destroy_v<Alloc,decltype(unfancy(first++))>){
			while(size--)
				allocator_traits<Alloc>::destroy(alloc,unfancy(first++));
		}
	}
	template<typename InIt1,typename InIt2,typename Pred=equal_to<>>
	constexpr bool equal(InIt1 first,InIt1 last,InIt2 first2,Pred pred=Pred{}){
		if constexpr(!equal_memcmp_is_safe_iter<InIt1,InIt2,Pred>){
			while(first!=last){
				if(!pred(*first++,*first2++)) return false;
			}
			return true;
		}
		else{
			return same_size_equal_trivial(unfancy(first),unfancy(first2),last-first);
		}
	}
	template<typename InIt1,typename InIt2,typename Pred=equal_to<>>
	constexpr bool equal(InIt1 first1,InIt1 last1,InIt2 first2,InIt2 last2,Pred pred=Pred{}){
		if constexpr(is_random_access_iterator_v<InIt1> && is_random_access_iterator_v<InIt2>){
			if(last1-first1!=last2-first2) return false;
			else return equal(first1,last1,first2,pass_func(pred));
		}
		else{
			while(true){
				if(first1==last1) return first2==last2;
				else if(first2==last2) return false;
				else if(!pred(*first1,*first2)) return false;
				++first1;++first2;
			}
		}
	}
	template<typename InIt1,typename InIt2,typename Size,typename Pred=equal_to<>>
	constexpr bool equal_n(InIt1 first,InIt2 first2,Size size,Pred pred=Pred{}){
		if constexpr(!equal_memcmp_is_safe_iter<InIt1,InIt2,Pred>){
			while(size--){
				if(!pred(*first++,*first2++)) return false;
			}
			return true;
		}
		else{
			return same_size_equal_trivial(unfancy(first),unfancy(first2),size);
		}
	}
	template<typename InIt1,typename InIt2,typename Pred=less<>>
	constexpr bool lexicographical_compare(InIt1 first1,InIt1 last1,InIt2 first2,InIt2 last2,Pred pred=Pred{}){
		using MemcmpPred=lex_compare_memcmp_classify<InIt1,InIt2,Pred>;
		if constexpr(!is_void_v<MemcmpPred>){
			const size_t size1=last1-first1,size2=last2-first2;
			const int res=same_size_lex_compare_trivial(unfancy(first1),unfancy(first2),size1<size2 ? size1:size2);
			return MemcmpPred{}(res,0) || (res==0 && size1<size2);
		}
		else{
			while(true){
				if(first1==last1) return first2!=last2;
				else if(first2==last2) return false;
				else if(pred(*first1,*first2)) return true;
				else if(pred(*first2,*first1)) return false;
				++first1;++first2;
			}
		}
	}
	template<typename BidIt>
	constexpr void reverse(BidIt first,BidIt last){
		while(first!=last && first!=--last){
			swap(*(first++),*last);
		}
	}
	template<typename FwdIt,typename T>
	constexpr FwdIt find(FwdIt first,FwdIt last,const T& val){
		while(first!=last){
			if(*first==val) break;
			first++;
		}
		return first;
	}
	template<typename FwdIt,typename T,typename Size=size_t>
	constexpr Size count(FwdIt first,FwdIt last,const T& val){
		Size found=0;
		while(first!=last){
			if(*first==val) ++found;
			first++;
		}
		return found;
	}
	template<typename FwdIt,typename T>
	constexpr bool contains(FwdIt first,FwdIt last,const T& val){
		while(first!=last){
			if(*first==val) return true;
			first++;
		}
		return false;
	}
	template<typename RanIt,typename T,typename Cmp=less<T>>
	constexpr RanIt binary_search(RanIt first,RanIt last,const T& val,Cmp&& cmp=Cmp{}){
		RanIt end=last-1;
		while(end>=first){
			RanIt index=first+(end-first)/2;
			if(cmp(*index,val))
				first=index+1;
			else if(cmp(val,*index))
				end=index-1;
			else
				return index;
		}
		return last;
	}
	template<typename FwdIt,typename T,typename Size=size_t>
	constexpr Size index_of(FwdIt first,FwdIt last,const T& val){ 
		Size index=0;
		while(first!=last){
			if(*first==val) return index;
			first++;
			index++;
		}
		return index;//One past the last element if val is not found
	}
	template<typename FwdIt>
	constexpr void rotate(FwdIt first,FwdIt mid,FwdIt last){
		if(mid==last) return;
		FwdIt next=mid;
		while(first!=next){
			swap(*first++,*next++);
			if(next==last) next=mid;
			else if(first==mid) mid=next;
		}
	}

	template<typename OutIt,typename InIt,typename Size>
	constexpr void advance_copy_n(OutIt dest,InIt& first,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<InIt,OutIt>){
			while(size--)
				*(dest++)=*(first++);
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),size);
			first+=size;
		}
	}
	template<typename OutIt,typename InIt,typename Size>
	constexpr void advance_move_n(OutIt dest,InIt& first,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<InIt,OutIt>){
			while(size--)
				*(dest++)=mpv::move(*(first++));
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),size);
			first+=size;
		}
	}
	template<typename FwdIt,typename Pred=less<>>
	bool is_sorted(FwdIt first,FwdIt last,Pred pred=Pred{}){
		if(first==last) return true;
		FwdIt next=first;
		++next;
		while(next!=last){
			if(pred(*next++,*first++)) return false;
		}
		return true;
	}
	template<typename BidIt,typename Pred=less<>>
	void insertion_sort(const BidIt first,const BidIt last,Pred pred=Pred{}){
		if(first==last) return;
		BidIt current_i=first,current_j=first;
		for(++current_i;current_i!=last;++current_i){
			iter_value_t<BidIt> aux=mpv::move(*current_i);
			for(current_j=current_i;current_j!=first && pred(aux,*(--BidIt(current_j)));--current_j){
				*current_j=mpv::move(*(--BidIt(current_j)));
			}
			*current_j=mpv::move(aux);
		}
	}
}

namespace mpv{
	template<typename T>
	constexpr enable_if_t<is_scalar_v<T>,bool> is_all_bits_zero(const T& val){
		constexpr T zero{};
		if constexpr(USE_MEMCPY){
			return memcmp(&val,&zero,sizeof(T))==0;
		}
		else{
			for(size_t i=0;i<sizeof(T);i++){
				if(((char*)&val)[i]!=((char*)&val)[i]) return false;
			}
			return true;
		}
	}
	template<typename T,size_t N>
	constexpr enable_if_t<is_swappable_v<T>> swap(T(&a)[N],T(&b)[N])noexcept(is_nothrow_swappable_v<T>){
		if(&a!=&b){
			for(T* afirst=a,* bfirst=b,* alast=a+N;afirst!=alast;afirst++,bfirst++)
				swap(*afirst,*bfirst);
		}
		
	}
	template<typename T>
	constexpr enable_if_t<is_move_constructible_v<T> && is_move_assignable_v<T>> swap(T& v1,T& v2)noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>){
		T aux=static_cast<T&&>(v1);
		v1=static_cast<T&&>(v2);
		v2=static_cast<T&&>(aux);
	}
#ifndef abs
	template<typename T>
	constexpr T abs(T n){
		if constexpr(is_unsigned_v<T>) return n;
		else return n<0? -n : n;
	}
#endif
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,T> mod(T a, T b){
        if((a>=0 && b>=0) || (a<0 && b<0)){
            return a%b;
        }
        else 
            return a%b==0 ? 0 : b+a%b;
    }
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,bool> divides_to(T a, T b){
        return b==0 ? true : b%a==0;
    }
	template<typename T>
	constexpr enable_if_t<is_integral_v<T>,T> gcd(T a,T b){
		if constexpr(is_signed_v<T>){
			if(a<0) a=-a;
			if(b<0) b=-b;
		}
		while(a!=0 && b!=0){
			if(a>b) a=a%b;
			else b=b%a;
		}
		return a==0 ? b : a; 
	}
	template<typename T>
	constexpr enable_if_t<is_integral_v<T>,T> lcm(T a,T b){
		return abs(a*b)/gcd(a,b);
	}
	inline constexpr double PI=3.1415926535897932384626433832795028841971693993751058209749445923078164062862;
	inline constexpr double sin(double a){// POLINOMIO DE TAYLOR DE SENO CENTRADO EN 0
		a-=2*PI*((long long)(a/(2*PI)));
		if(a>PI) a-=2*PI;
		else if(a<-PI) a+=2*PI;
		bool neg=false;
		if(a>PI/2){
			neg=true;
			a-=PI;
		}
		else if(a<-PI/2){
			neg=true;
			a+=PI;
		}
		double fact=1;
		double pow=1,ret=0;
		for(size_t i=1;i<20;i++){
			fact*=i;
			pow*=a;
			if(i%4==1){
				ret+=pow/fact;
			}		
			else if(i%4==3){
				ret-=pow/fact;
			}
		}
		return neg? -ret : ret;
	}
	inline constexpr double cos(double a){// POLINOMIO DE TAYLOR DE COSENO CENTRADO EN 0
		a-=2*PI*((long long)(a/(2*PI)));
		if(a>PI) a-=2*PI;
		else if(a<-PI) a+=2*PI;
		bool neg=false;
		if(a>PI/2){
			neg=true;
			a-=PI;
		}
		else if(a<-PI/2){
			neg=true;
			a+=PI;
		}
		double fact=1;
		double pow=1,ret=1;
		for(size_t i=1;i<23;i++){
			fact*=i;
			pow*=a;
			if(i%4==0){
				ret+=pow/fact;
			}		
			else if(i%4==2){
				ret-=pow/fact;
			}
		}
		return neg? -ret : ret;
	}
	inline constexpr double tan(double a){
		return sin(a)/cos(a);
	}
	template<unsigned long long Precision=1000000000000>
	constexpr double sqrt(double radicand){
		bool inverse=false;
		if(radicand<1){
			radicand=1/radicand;
			inverse=true;
		}
		double max=radicand,min=0,n=max/2;
		while(true){
			double attempt=n*n;
			if(abs(attempt-radicand)<1/static_cast<double>(Precision)){
				return inverse? 1/n : n;
			}
			else if(attempt>radicand){
				max=n;
				n=min+(max-min)/2;
			}
			else/*(attempt<radicand)*/{
				min=n;
				n=min+(max-min)/2;
			}
		}
	}
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,T> factorial(T n){
        T res=1;
        while(n) res*=n--;
        return res;
    }
	template<typename T>
	constexpr enable_if_t<is_integral_v<T>,T> variations(T a,T b){
		T res=1;
		T c=a-b;
		while(c++<a){
			res*=c;
		}
		return res;
	}
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,T> combinations(T a,T b){
		return variations(a,b)/factorial(b);
    }
	template<typename T,unsigned long long Precision=100000000000000>
	constexpr bool eq(T x,T y){
		if constexpr(is_floating_point_v<T>) return x-y>=0? (x-y)<1./Precision : (x-y)>-1./Precision;
		else return x==y;
	}
    template<typename T,unsigned long long Precision=100000000000000>
    constexpr bool is_zero(T v){
		//if constexpr(is_floating_point_v<T>) return v>=0? v<1e-15 : v>-1e-15;
		if constexpr(is_floating_point_v<T>) return v>=0? v<1./Precision : v>-1./Precision;
		else return v==0;
    }
	template<typename T>
	constexpr T minus1pow(T e){
		return e%2==0? 1 : -1;
	}
	template<typename T>
	constexpr bool get_bit(unsigned n,T v){//from left
		v<<=n;
		v>>=sizeof(T)*8-1;// +n-n
		v<<=sizeof(T)*8-1-n;
		return v;
	}
	template<typename T>
	constexpr bool get_bit_from_right(unsigned n,T v){
		v>>=n;
		v<<=sizeof(T)*8-1;
		v>>=sizeof(T)*8-1-n;
		return v;
	}
	template<typename Lambda,typename Iterable>
	constexpr enable_if_t<is_iterable_v<Iterable>,bool> any(const Iterable&& iterable,Lambda&& func=Lambda{}){
		for(const auto& x:iterable){
			if(func(x)) return true;
		}
		return false;
	}
	template<typename Lambda,typename Iterable>
	constexpr enable_if_t<is_iterable_v<Iterable>> foreach(Iterable&& iterable,Lambda&& func=Lambda{}){
		for(auto& x:iterable)
			func(x);
	}
	template<typename Iterable,typename D>
	constexpr enable_if_t<is_iterable_v<Iterable>,bool> contains(const Iterable&& iterable,const D& val){
		for(const auto& x:iterable){
			if(x==val) return true;
		}
		return false;
	}
	template<typename Iterable,typename D>
	constexpr enable_if_t<is_iterable_v<Iterable>,typename remove_cvref_t<Iterable>::size_type> count(Iterable&& iterable,const D& val){
		typename remove_cvref_t<Iterable>::size_type counter=0;
		for(const auto& x:iterable){
			if(x==val) counter++;
		}
		return counter;
	}
	template<typename Out,typename T,size_t size>
	enable_if_t<!is_same_v<remove_cv_t<T>,char> && !is_same_v<remove_cv_t<T>,wchar_t>,Out&> operator<<(Out& stream,T (&a)[size]){
        stream<<"[";
        for(size_t i=0;i<size;i++){
			if constexpr(is_same_v<remove_cv_t<T>,unsigned char> || is_same_v<remove_cv_t<T>,signed char>){
				stream<<""<<static_cast<short>(a[i]);
			}
            else stream<<""<<a[i];
			if(i<size-1) stream<<", ";
        }
        stream<<"]";
		return stream;
	}
}

#pragma warning(disable : 4996)
namespace mpv{
	template<typename> class Optional;
	namespace dflt{//default types
		template<typename T>
		size_t strsize(const T* str){
			if constexpr(!(is_same_v<T,char> || is_same_v<T,wchar_t>) || !USE_MEMCPY){
				size_t i=0;
				while(str[i]) ++i;
				return i;				
			}
			else if constexpr(is_same_v<T,char>){
				RETURN_STRLEN(str);
			}
			else{RETURN_WCSLEN(str);}
		}// se asume que los elementos que pertenecen a los strings son pod
		template<typename T>
		void strcopy(T* dest,const T* source){
			if constexpr(!(is_same_v<T,char> || is_same_v<T,wchar_t>) || !USE_MEMCPY){
				size_t i=0;
				while(source[i]){
					dest[i]=source[i];
					i++;
				}
				dest[i]=T();				
			}
			else if constexpr(is_same_v<T,char>){
				STRCPY(dest,source);
			}
			else{
				WCSCPY(dest,source);
			}
		}
		template<typename T>
		signed char strcomp(const T* str1,const T* str2){
			size_t sz1=strsize(str1);
			size_t sz2=strsize(str2);
			if(sz1>sz2)
				return 2;
			if(sz1<sz2)
				return -2;
			else{
				for(size_t i=0;i<sz1;i++){
					if(str1[i]>str2[i])
						return 1;
					if(str1[i]<str2[i])
						return -1;
				}
				return 0;
			}
		}
		template <typename T>
		void insertion_sort(T a[],size_t n){
			for(size_t i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && aux<a[j-1]; j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T>
		void insertion_sort_reverse(T a[],size_t n){
			for(size_t i=1,j; i<n; i++) {
				T aux=(T&&)a[i];
				for(j=i; j>0 && aux>a[j-1]; j--) {
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T,typename D>
		void insertion_sort(T a[],size_t n,D(*func)(const T&)){
			for(size_t i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && func(aux)<func(a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T,typename Cmp>
		void insertion_sort(T a[],size_t n,Cmp&& cmp=Cmp{}){
			for(size_t i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && cmp(aux,a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}

	}
	namespace al{
		template<typename Alloc>
		typename allocator_traits<Alloc>::size_type strsize(typename allocator_traits<Alloc>::const_pointer str){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !(is_same_v<typename allocator_traits<Alloc>::value_type,char> || is_same_v<typename allocator_traits<Alloc>::value_type,wchar_t>) || !USE_MEMCPY){
				typename allocator_traits<Alloc>::size_type i=0;
				while(str[i]) ++i;
				return i;				
			}
			else if constexpr(is_same_v<typename allocator_traits<Alloc>::value_type,char>){
				RETURN_STRLEN(str);
			}
			else{RETURN_WCSLEN(str);}
		}// se asume que los elementos que pertenecen a los strings son pod
		template<typename Alloc>
		void strcopy(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::const_pointer source){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !(is_same_v<typename allocator_traits<Alloc>::value_type,char> || is_same_v<typename allocator_traits<Alloc>::value_type,wchar_t>) || !USE_MEMCPY){
				typename allocator_traits<Alloc>::size_type i=0;
				while(source[i]){
					dest[i]=source[i];
					i++;
				}
				dest[i]=typename allocator_traits<Alloc>::value_type();				
			}
			else if constexpr(is_same_v<typename allocator_traits<Alloc>::value_type,char>){
				STRCPY(dest,source);
			}
			else{
				WCSCPY(dest,source);
			}
		}
		template<typename Alloc>
		signed char strcomp(typename allocator_traits<Alloc>::const_pointer str1,typename allocator_traits<Alloc>::const_pointer str2){
			typename allocator_traits<Alloc>::size_type sz1=strsize<Alloc>(str1);
			typename allocator_traits<Alloc>::size_type sz2=strsize<Alloc>(str2);
			if(sz1>sz2)
				return 2;
			if(sz1<sz2)
				return -2;
			else{
				for(typename allocator_traits<Alloc>::size_type i=0;i<sz1;i++){
					if(str1[i]>str2[i])
						return 1;
					if(str1[i]<str2[i])
						return -1;
				}
				return 0;
			}
		}
		template <typename Alloc>
		void insertion_sort(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n){
			using T=typename allocator_traits<Alloc>::value_type;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && aux<a[j-1]; j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename Alloc>
		void insertion_sort_reverse(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n){
			using T=typename allocator_traits<Alloc>::value_type;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++) {
				T aux=(T&&)a[i];
				for(j=i; j>0 && aux>a[j-1]; j--) {
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename Alloc,typename D>
		void insertion_sort(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n,D(*func)(const typename allocator_traits<Alloc>::value_type&)){
			using T=typename allocator_traits<Alloc>::value_type;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && func(aux)<func(a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename Alloc,typename Cmp>
		void insertion_sort(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n,Cmp&& cmp=Cmp{}){
			using T=typename allocator_traits<Alloc>::value_type;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && cmp(aux,a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
	}

#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
	// Input/Output de arrays
	template<typename T>
	void str__Print(T a,int b,T* dir,const char *str){
		int sz=0,i,j;
		while(str[sz]){
			++sz;
		}
		++sz;
		char* c=new char[sz]{};
		for(i=0,j=0; i<sz; i++,j++) {
			c[j]=str[i];
			if(str[i]=='{') {
				c[j]='\0';
				printf("%s",c);
				if(str[i+1]=='a') {
					std::cout<<a;
				}
				else if(str[i+1]=='i') {
					printf("%d",b);
				}
				else if(str[i+1]=='d') {
					std::cout<<dir;
				}
				i+=2;
				j=-1;
			}
		}
		printf("%s",c);
		delete[] c;
	}
	//Array Input
	template<typename T>
	void getarray(T a[],long long n,const char str[]="[{i}]=",bool reverse=false){
		int i;
		if(reverse==false) {
			for(i=0; i<n; i++) {
				str__Print(a[i],i,a+i,str);
				std::cin>>a[i];
			}
		}
		else {
			for(i=n-1; i>=0; i--) {
				str__Print(a[i],i,a+i,str);
				std::cin>>a[i];
			}
		}
	}
	//Array Output
	
	template<typename T>
	void printarray(T a[],long long n, const char *str="{a}",const char* end=", ",const char* first="[",const char *last="]\n",bool reverse=false){
		int i;
		std::cout<<first;
		if(reverse==false) {
			for(i=0; i<n; i++) {
				str__Print(a[i],i,a+i,str);
				if(i!=n-1)
					std::cout<<end;
			}
		}
		else {
			for(i=n-1; i>=0; i--) {
				str__Print(a[i],i,a+i,str);
				if(i!=0)
					std::cout<<end;
			}
		}
		std::cout<<last;
	}
#endif
}

namespace mpv{
	template<typename Char>
	constexpr bool is_alpha(Char c)noexcept{
		return (c>=65 && c<=90) || (c>=97 && c<=122);
	}
	template<typename Char>
	constexpr bool is_alnum(Char c)noexcept{
		return (c>=65 && c<=90) || (c>=97 && c<=122) || (c>=48 && c<=57);
	}
	template<typename Char>
	constexpr bool is_special(Char c)noexcept{
		return !((c>=65 && c<=90) || (c>=97 && c<=122) || (c>=48 && c<=57));
	}
	template<typename Char>
	constexpr bool is_numeric(Char c)noexcept{
		return c>=48 && c<=57;
	}
	template<typename Char>
	constexpr bool is_alnum_us(Char c)noexcept{// true if c is alphanumeric or an underscore
		return (c>=65 && c<=90) || (c>=97 && c<=122) || (c>=48 && c<=57) || c=='_';
	}
	template<typename Char>
	constexpr Char upper(const Char c)noexcept{
		return (c>=97 && c<=122)? c-32 : c;
	}
	template<typename Char>
	constexpr Char lower(const Char c)noexcept{
		return (c>=65 && c<=90)? c+32 : c;
	}
	template<typename Char>
	constexpr size_t parseUint(const Char* s){
		size_t num=0;
		size_t i=0;
		while(s[i]==' ' || s[i]=='\n' || s[i]=='\r' || s[i]=='\t')i++;
		for(;s[i]>='0' && s[i]<='9';i++){
			num*=10;
			num+=s[i]-48;// 'n'- 48 == n
		}
		return num;
	}
	template<typename Char>
	constexpr ssize_t parseInt(const Char* s){
		ssize_t num=0;
		size_t i=0;
		bool neg=false;
		while(s[i]==' ' || s[i]=='\n' || s[i]=='\r' || s[i]=='\t')i++;
		if(s[i]=='-'){
			neg=true;
			i++;
		}else if(s[i]=='+')i++;
		for(;s[i]>='0' && s[i]<='9';i++){
			num*=10;
			num+=s[i]-48;
		}
		return neg ? -num : num;
	}
	template<typename Char>
	constexpr bool is_float_convertible(const Char* s){
		if(*s=='\0') return false;
		if(*s=='+' || *s=='-') s++;
		if(*s=='e' || *s=='E') return false;
		const Char* e_pos=nullptr;
		bool dot_found=false;
		while(*s!='\0'){
			if(!is_numeric(*s)){
				if(*s=='.'){
					if(dot_found || e_pos!=nullptr) return false;
					else dot_found=true;
				}
				else if(*s=='e' || *s=='E'){
					if(e_pos!=nullptr) return false;
					e_pos=s++;
					if(*s=='\0') return false;
					if(*s=='+' || *s=='-'){
						e_pos=s;
					}
				}
				else return false;
			}
			s++;
		}
		return s-1!=e_pos;	//Checks if the dot is not the last character
	}
	template<typename Char>
	constexpr long double parseFloat(const Char* s){
		long double num=0;
		size_t i=0;
		bool neg=false;
		while(s[i]==' ' || s[i]=='\n' || s[i]=='\r' || s[i]=='\t')i++;
		if(s[i]=='-'){
			neg=true;
			i++;
		}else if(s[i]=='+')i++;
		for(;s[i]>='0' && s[i]<='9';i++){
			num*=10;
			num+=s[i]-48;
		}
		if(s[i]=='.'){i++;
			for(long double div=10;s[i]>='0' && s[i]<='9';i++,div*=10){
				num+=((s[i]-48)/div);
			}		
		}
		if(s[i]=='e' || s[i]=='E'){i++;
			long double e=10;
			unsigned int e_num=0;
			if(s[i]=='-'){
				e=0.1;
				i++;
			}else if(s[i]=='+')i++;
			for(;s[i]>='0' && s[i]<='9';i++){
				e_num*=10;
				e_num+=s[i]-48;
			}
			for(unsigned short j=0;j<e_num;j++){
				num*=e;
			}
		}
		return neg ? -num : num;
	}
	inline constexpr char to_char(int digit){
		return digit<10? digit+48:digit+55;
	}
}

#if (defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_) || defined(_INC_STDIO)) && defined(SHOWREMAININGOBJECTS)
#define SETCOUNTERS		int remainingControlBlocks=0;\
						int remainingListNodes=0;
#define INCCTRLBLOCKS				remainingControlBlocks++;
#define DECCTRLBLOCKS				remainingControlBlocks--;
#define REMAININGCTRLBLOCKS 		printf("\nREMAINING CONTROL BLOCKS: %d\n",remainingControlBlocks);
#define INCLNODES					remainingListNodes++;
#define DECLNODES					remainingListNodes--;
#define REMAININGLNODES				printf("\nREMAINING LIST NODES: %d\n",remainingListNodes);
#else
#define SETCOUNTERS
#define INCCTRLBLOCKS
#define DECCTRLBLOCKS
#define REMAININGCTRLBLOCKS
#define INCLNODES
#define DECLNODES
#define REMAININGLNODES
#endif
SETCOUNTERS

namespace mpv{
	struct byte{
		union{
			unsigned char uval;
			signed char val;
		};
		byte()=default;
		constexpr byte(unsigned char b)noexcept:uval(b){}
		constexpr void operator=(unsigned char new_val)noexcept{
			uval=new_val;
		}
		constexpr bool get(unsigned char index)const noexcept{//from left
			unsigned char v=uval<<index;
			v>>=7;
			v<<=7-index;
			return v;
		}
		template<unsigned char index>
		constexpr bool get()const noexcept{
			static_assert(index<8,"index should be <8");
			if constexpr	 (index==0) return val & 0b10000000;
			else if constexpr(index==1) return val & 0b01000000;
			else if constexpr(index==2) return val & 0b00100000;
			else if constexpr(index==3) return val & 0b00010000;
			else if constexpr(index==4) return val & 0b00001000;
			else if constexpr(index==5) return val & 0b00000100;
			else if constexpr(index==6) return val & 0b00000010;
			else if constexpr(index==7) return val & 0b00000001;
		}
		template<unsigned char index,unsigned char size>
		constexpr unsigned char get_num()const noexcept{
			static_assert(index<8,"index should be <8");
			unsigned char v=uval<<index;
			return v>>(8-size);
		}
		constexpr unsigned char get_num(unsigned char index,unsigned char size)const noexcept{
			unsigned char v=uval<<index;
			return v>>(8-size);
		}
		constexpr void write(bool value,unsigned char index)noexcept{
			unsigned char v=0b10000000;
			v>>=index;
			if(value) val|=v;
			else val&=~v;
		}
		template<unsigned char index>
		constexpr void write(bool value)noexcept{
			unsigned char v=0b10000000;
			v>>=index;
			if(value) val|=v;
			else val&=~v;
		}
		template<unsigned char index,unsigned char size>
		constexpr void write_num(unsigned char num)noexcept{
			static_assert(index<8,"index should be <8");
			uval&=~byte(0b11111111).get_num<index,size>();
			num<<=8-size;
			uval|=num>>index;
		}
		constexpr void write_num(unsigned char num,unsigned char index,unsigned char size)noexcept{
			uval&=~byte(0b11111111).get_num(index,size);
			num<<=8-size;
			uval|=num>>index;
		}
		constexpr operator unsigned char()const noexcept{
			return uval;
		}
		template<unsigned char size>
		constexpr unsigned char get_right_bits()const noexcept{
			static_assert(size<=8,"index should be <8");
			if constexpr(size==0) return 0;
			else if constexpr(size==1) return uval & 0b00000001;
			else if constexpr(size==2) return uval & 0b00000011;
			else if constexpr(size==3) return uval & 0b00000111;
			else if constexpr(size==4) return uval & 0b00001111;
			else if constexpr(size==5) return uval & 0b00011111;
			else if constexpr(size==6) return uval & 0b00111111;
			else if constexpr(size==7) return uval & 0b01111111;
			else if constexpr(size==8) return uval & 0b11111111;
		}
		template<unsigned char size>
		constexpr unsigned char get_left_bits()const noexcept{
			static_assert(size<=8,"size should be <=8");
			return uval>>(8-size);
		}
		template<unsigned char size>
		constexpr void write_right_bits(unsigned char new_val)noexcept{
			static_assert(size<=8,"size should be <=8");
			uval>>=size;
			uval<<=size;
			new_val<<=8-size;
			new_val>>=8-size;
			uval|=new_val;
		}
		template<unsigned char size>
		constexpr void write_left_bits(unsigned char new_val)noexcept{
			static_assert(size<=8,"size should be <=8");
			uval<<=size;
			uval>>=size;
			new_val<<=8-size;
			uval|=new_val;
		}
	};
    template<typename Out>
    Out& operator<<(Out& stream,const byte b){
		for(unsigned char i=0;i<8;i++){
			stream<<""<<b.get(i);
		}
        return stream;
    }
    template<typename T,T P1,T P2> struct params{
        static constexpr T p1=P1;
        static constexpr T p2=P2;
    };
	struct reserve_tag{};
	struct setSize{};
	//	^^^^^^ tags
	template<typename T> struct Wrap{T val;};
	template<typename T1,typename T2>
	struct Pair COUNT_IT{
		T1 x1;
		T2 x2;
		Pair()=default;
		template<typename T,typename D>
		constexpr Pair(T&& data1,D&& data2):x1((T&&)data1),x2((D&&)data2){}
		constexpr bool operator==(const Pair& other)const{
			return this->x1==other.x1 && this->x2==other.x2;
		}
		constexpr bool operator!=(const Pair& other)const{
			return this->x1!=other.x1 || this->x2!=other.x2;
		}
	};
	template<typename Out,typename T1,typename T2>
	Out& operator<<(Out& stream,const Pair<T1,T2>& pair){
		stream<<"("<<pair.x1<<","<<pair.x2<<")";
		return stream;
	}
	template<typename T>
	struct default_types{
		using value_type=T;
		using size_type=size_t;
		using difference_type=ptrdiff_t;
		using pointer=T*;
		using const_pointer=const T*;
		using reference=T&;
		using const_reference=const T&;
	};
}

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

namespace mpv{
	template<typename T,bool=is_empty_v<T> && !is_final_v<T>> 
	struct EBCO:T{	// Empty Base Class Optimization
		private:
			using base_type=T;
		public:
			constexpr explicit EBCO()noexcept(is_nothrow_default_constructible_v<T>):T(){}
			template<typename U,enable_if_t<!is_same_v<remove_cvref_t<U>,EBCO>,int> = 0> constexpr explicit EBCO(U&& val)noexcept(is_nothrow_constructible_v<T,U>):T(static_cast<U&&>(val)){}
			constexpr T& get_val()noexcept{ return *this; }
			constexpr const T& get_val()const noexcept{ return *this; }
	};
	template<typename T>
	struct EBCO<T,false>{	// EBCO not possible
		private:
			T val;
		public:
			constexpr explicit EBCO()noexcept(is_nothrow_default_constructible_v<T>):val(){}
			template<typename U,enable_if_t<!is_same_v<remove_cvref_t<U>,EBCO>,int> = 0> constexpr explicit EBCO(U&& val)noexcept(is_nothrow_constructible_v<T,U>):val(static_cast<U&&>(val)){}
			constexpr T& get_val()noexcept{ return val;}
			constexpr const T& get_val()const noexcept{ return val; }
	};

	struct arg1_tag{};// indica que el primer argumento no es parte de los argumentos variadicos, sino que va al primer elemento
	template<typename T1,typename T2,bool=is_empty_v<T1> && !is_final_v<T1>>
	struct CompressedPair final:private T1{
		T2 V2;
		template<typename A,typename... B>
		constexpr CompressedPair(arg1_tag,A&& v1,B&&... v2)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_constructible<T2,B...>>):T1(static_cast<A&&>(v1)),V2(static_cast<B&&>(v2)...){}
		template<typename... B>
		constexpr explicit CompressedPair(B&&... v2)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_constructible<T2,B...>>):T1(),V2(static_cast<B&&>(v2)...){}
		constexpr T1& getV1()noexcept{ return *this; }
		constexpr const T1& getV1()const noexcept{ return const_cast<CompressedPair&>(*this); }
	};
	template<typename T1,typename T2>
	struct CompressedPair<T1,T2,false>final{
		T1 V1;
		T2 V2;
		template<typename A,typename... B>
		constexpr CompressedPair(arg1_tag,A&& v1,B&&... v2)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_constructible<T2,B...>>):V1(static_cast<A&&>(v1)),V2(static_cast<B&&>(v2)...){}
		template<typename... B>
		constexpr CompressedPair(B&&... v2)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_constructible<T2,B...>>):V1(),V2(static_cast<B&&>(v2)...){}
		constexpr T1& getV1()noexcept{ return V1; }
		constexpr const T1& getV1()const noexcept{ return V1; }
	};
	struct arg1and2_tag{};// indica que el primer y segundo argumento no son parte de los argumentos variadicos, sino que van al primer y segundo elemento
	struct arg2_tag{};// indica que el primer argumento no es parte de los argumentos variadicos, sino que va al segundo elemento
	template<typename T1,typename T2,typename T3,bool=is_empty_v<T1> && !is_final_v<T1>>
	struct CompressedTriple final:private T1{
		CompressedPair<T2,T3> pair;
		template<typename A,typename B,typename... C>
		constexpr CompressedTriple(arg1and2_tag,A&& v1,B&& v2,C&&... v3)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_constructible<T2,B>,is_nothrow_constructible<T3,C...>>):T1(static_cast<A&&>(v1)),pair(arg1_tag{},static_cast<B&&>(v2),static_cast<C&&>(v3)...){}
		template<typename A,typename... C>
		constexpr CompressedTriple(arg1_tag,A&& v1,C&&... v3)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_default_constructible<T2>,is_nothrow_constructible<T3,C...>>):T1(static_cast<A&&>(v1)),pair(static_cast<C&&>(v3)...){}
		template<typename B,typename... C>
		constexpr CompressedTriple(arg2_tag,B&& v2,C&&... v3)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_constructible<T2,B>,is_nothrow_constructible<T3,C...>>):T1(),pair(arg1_tag{},static_cast<B&&>(v2),static_cast<C&&>(v3)...){}
		template<typename... C>
		constexpr CompressedTriple(C&&... v3)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_default_constructible<T2>,is_nothrow_constructible<T3,C...>>):T1(),pair(static_cast<C&&>(v3)...){}
		constexpr T1& getV1()noexcept{return *this;}
		constexpr T2& getV2()noexcept{return pair.getV1();}
		constexpr T3& getV3()noexcept{return pair.V2;}
		constexpr const T1& getV1()const noexcept{return const_cast<CompressedTriple&>(*this);}
		constexpr const T2& getV2()const noexcept{return pair.getV1();}
		constexpr const T3& getV3()const noexcept{return pair.V2;}
	};
	template<typename T1,typename T2,typename T3>
	struct CompressedTriple<T1,T2,T3,false> final{
		T1 V1;
		CompressedPair<T2,T3> pair;
		template<typename A,typename B,typename... C>
		constexpr CompressedTriple(arg1and2_tag,A&& v1,B&& v2,C&&... v3)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_constructible<T2,B>,is_nothrow_constructible<T3,C...>>):V1(static_cast<A&&>(v1)),pair(arg1_tag{},static_cast<B&&>(v2),static_cast<C&&>(v3)...){}
		template<typename A,typename... C>
		constexpr CompressedTriple(arg1_tag,A&& v1,C&&... v3)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_default_constructible<T2>,is_nothrow_constructible<T3,C...>>):V1(static_cast<A&&>(v1)),pair(static_cast<C&&>(v3)...){}
		template<typename B,typename... C>
		constexpr CompressedTriple(arg2_tag,B&& v2,C&&... v3)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_constructible<T2,B>,is_nothrow_constructible<T3,C...>>):V1(),pair(arg1_tag{},static_cast<B&&>(v2),static_cast<C&&>(v3)...){}
		template<typename... C>
		constexpr CompressedTriple(C&&... v3)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_default_constructible<T2>,is_nothrow_constructible<T3,C...>>):T1(),pair(static_cast<C&&>(v3)...){}
		constexpr T1& getV1()noexcept{return V1;}
		constexpr T2& getV2()noexcept{return pair.getV1();}
		constexpr T3& getV3()noexcept{return pair.V2;}
		constexpr const T1& getV1()const noexcept{return V1;}
		constexpr const T2& getV2()const noexcept{return pair.getV1();}
		constexpr const T3& getV3()const noexcept{return pair.V2;}
	};
}

namespace mpv{
	template<typename Types> class const_random_access_iterator;
	template<typename Types>
	class random_access_iterator{
		friend const_random_access_iterator<Types>;
		public:
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=typename Types::reference;
			using iterator_category=typename iterator_traits<pointer>::iterator_category;
		private:
			pointer ptr;
		public:
			constexpr random_access_iterator(pointer ptr)noexcept:ptr(ptr){}
			constexpr reference operator*()const noexcept{
				return *ptr;
			}
			constexpr reference operator[](difference_type n)const noexcept{
				return *(ptr+n);
			}
			constexpr pointer operator->()const noexcept{
				return ptr;
			}
			constexpr random_access_iterator& operator++()noexcept{
				++ptr;
				return *this;
			}
			constexpr random_access_iterator operator++(int)noexcept{
				random_access_iterator aux=*this;
				++ptr;
				return aux;
			}
			constexpr random_access_iterator& operator--()noexcept{
				--ptr;
				return *this;
			}
			constexpr random_access_iterator operator--(int)noexcept{
				random_access_iterator aux=*this;
				--ptr;
				return aux;
			}
			constexpr random_access_iterator operator+(difference_type n)const noexcept{
				return random_access_iterator(this->ptr+n);
			}
			constexpr random_access_iterator operator-(difference_type n)const noexcept{
				return random_access_iterator(this->ptr-n);
			}
			constexpr random_access_iterator& operator+=(difference_type n)noexcept{
				ptr+=n;
				return *this;
			}
			constexpr random_access_iterator& operator-=(difference_type n)noexcept{
				ptr-=n;
				return *this;
			}
			constexpr difference_type operator-(const random_access_iterator& other)const noexcept{
				return this->ptr-other.ptr;
			}
			constexpr difference_type operator-(const const_random_access_iterator<Types>& other)const noexcept{
				return this->ptr-other.ptr;
			}
			constexpr bool operator==(const random_access_iterator& other)const noexcept{
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const random_access_iterator& other)const noexcept{
				return this->ptr!=other.ptr;
			}
			constexpr bool operator<=(const random_access_iterator& other)const noexcept{
				return this->ptr<=other.ptr;
			}
			constexpr bool operator>=(const random_access_iterator& other)const noexcept{
				return this->ptr>=other.ptr;
			}
			constexpr bool operator<(const random_access_iterator& other)const noexcept{
				return this->ptr<other.ptr;
			}
			constexpr bool operator>(const random_access_iterator& other)const noexcept{
				return this->ptr>other.ptr;
			}
			template<typename> friend class const_random_access_iterator;
	};
	template<typename Types>
	class const_random_access_iterator{
		friend random_access_iterator<Types>;
		public:
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
			using iterator_category=typename iterator_traits<pointer>::iterator_category;
		private:
			pointer ptr;
		public:
			constexpr const_random_access_iterator(pointer ptr)noexcept:ptr(ptr){}
			constexpr const_random_access_iterator(random_access_iterator<Types> nonconst_it)noexcept:ptr(nonconst_it.ptr){}
			constexpr reference operator*()const noexcept{
				return *ptr;
			}
			constexpr reference operator[](difference_type n)const noexcept{
				return *(ptr+n);
			}
			constexpr pointer operator->()const noexcept{
				return ptr;
			}
			constexpr const_random_access_iterator& operator++()noexcept{
				++ptr;
				return *this;
			}
			constexpr const_random_access_iterator operator++(int)noexcept{
				const_random_access_iterator aux=*this;
				++ptr;
				return aux;
			}
			constexpr const_random_access_iterator& operator--()noexcept{
				--ptr;
				return *this;
			}
			constexpr const_random_access_iterator operator--(int)noexcept{
				const_random_access_iterator aux=*this;
				--ptr;
				return aux;
			}
			constexpr const_random_access_iterator operator+(difference_type n)const noexcept{
				return const_random_access_iterator(this->ptr+n);
			}
			constexpr const_random_access_iterator operator-(difference_type n)const noexcept{
				return const_random_access_iterator(this->ptr-n);
			}
			constexpr const_random_access_iterator& operator+=(difference_type n)noexcept{
				ptr+=n;
				return *this;
			}
			constexpr const_random_access_iterator& operator-=(difference_type n)noexcept{
				ptr-=n;
				return *this;
			}
			constexpr difference_type operator-(const const_random_access_iterator& other)const noexcept{
				return this->ptr-other.ptr;
			}
			constexpr difference_type operator-(const random_access_iterator<Types>& other)const noexcept{
				return this->ptr-other.ptr;
			}
			constexpr bool operator==(const const_random_access_iterator& other)const noexcept{
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const const_random_access_iterator& other)const noexcept{
				return this->ptr!=other.ptr;
			}
			constexpr bool operator<=(const const_random_access_iterator& other)const noexcept{
				return this->ptr<=other.ptr;
			}
			constexpr bool operator>=(const const_random_access_iterator& other)const noexcept{
				return this->ptr>=other.ptr;
			}
			constexpr bool operator<(const const_random_access_iterator& other)const noexcept{
				return this->ptr<other.ptr;
			}
			constexpr bool operator>(const const_random_access_iterator& other)const noexcept{
				return this->ptr>other.ptr;
			}
	};
    template<typename It>
	class move_iterator{
		public:
			using iterator_category=iter_cat_t<It>;
			using iterator_type=It;
			using value_type=iter_value_t<It>;
			using difference_type=iter_diff_t<It>;
			using pointer=It;
			using reference=If_t<is_reference_v<iter_ref_t<It>>,remove_reference_t<iter_ref_t<It>>&&,iter_ref_t<It>>;
			iterator_type it;
			constexpr move_iterator(It it)noexcept(is_nothrow_move_constructible_v<It>):it(mpv::move(it)){}
			template<typename It2>
			constexpr move_iterator(const move_iterator<It2>& other)noexcept(is_nothrow_constructible_v<It,const It2&>):it(other.base()){}
			template<typename It2>
			constexpr move_iterator& operator=(const move_iterator<It2>& other)noexcept(is_nothrow_assignable_v<It&,const It2&>){
				it=other.base();
				return *this;
			}
			constexpr iterator_type base()const noexcept(is_nothrow_copy_constructible_v<It>){
				return it;
			}
			constexpr reference operator*()const noexcept(noexcept(static_cast<reference>(*it))){
				return static_cast<reference>(*it);
			}
			constexpr reference operator[](const difference_type n)const noexcept(noexcept(mpv::move(it[n]))){
				return mpv::move(it[n]);
			}
			constexpr pointer operator->()const noexcept(is_nothrow_copy_constructible_v<It>){
				return it;
			}
			constexpr move_iterator& operator++()noexcept(noexcept(++it)){
				++it;
				return *this;
			}
			constexpr move_iterator operator++(int)noexcept(noexcept(is_nothrow_copy_constructible_v<It> && noexcept(++it))){
				move_iterator aux=*this;
				++it;
				return aux;
			}
			constexpr move_iterator& operator--()noexcept(noexcept(--it)){
				--it;
				return *this;
			}
			constexpr move_iterator operator--(int)noexcept(noexcept(is_nothrow_copy_constructible_v<It> && noexcept(--it))){
				move_iterator aux=*this;
				--it;
				return aux;
			}
			constexpr move_iterator& operator+=(const difference_type n)noexcept(noexcept(it+=n)){
				it+=n;
				return *this;
			}
			constexpr move_iterator& operator-=(const difference_type n)noexcept(noexcept(it-=n)){
				it-=n;
				return *this;
			}
			constexpr move_iterator operator+(const difference_type n)const noexcept(noexcept(move_iterator(it+n))){
				return move_iterator(it+n);
			}
			constexpr move_iterator operator-(const difference_type n)const noexcept(noexcept(move_iterator(it-n))){
				return move_iterator(it-n);
			}
			constexpr bool operator==(const move_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->it==other.it))){
				return this->it==other.it;
			}
			constexpr bool operator!=(const move_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->it!=other.it))){
				return this->it!=other.it;
			}
			constexpr bool operator<=(const move_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->it<=other.it))){
				return this->it<=other.it;
			}
			constexpr bool operator>=(const move_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->it>=other.it))){
				return this->it>=other.it;
			}
			constexpr bool operator<(const move_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->it<other.it))){
				return this->it<other.it;
			}
			constexpr bool operator>(const move_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->it>other.it))){
				return this->it>other.it;
			}
	};
}

namespace mpv{
	struct Fibonacci{
		size_t count,max;
		constexpr Fibonacci(size_t max)noexcept:count(0),max{max}{}
		constexpr Fibonacci(size_t start,size_t max)noexcept:count(start),max{max}{}
		struct iterator{
			using iterator_category=forward_iterator_tag;
			using value_type=size_t;
			using pointer=size_t*;
			using reference=size_t&;
			size_t pos,a=1,b=0,c=0;
			constexpr iterator(size_t pos)noexcept:pos(pos){
				for(size_t i=0;i<pos;i++){
					c=a+b;
					a=b;
					b=c;
				}
			}	
			constexpr size_t operator*()noexcept{
				return c;
			}
			constexpr iterator& operator++()noexcept{
				pos++;
				c=a+b;
				a=b;
				b=c;
				return *this;
			}
			constexpr iterator operator++(int)noexcept{
				iterator aux=*this;
				pos++;
				c=a+b;
				a=b;
				b=c;
				return aux;
			}
			constexpr bool operator!=(size_t n)noexcept{
				return this->pos<n;
			}
		};
		constexpr iterator begin()const noexcept{
			return iterator(count);
		}
		constexpr size_t end()const noexcept{
			return max;
		}
	};
	template<typename T,typename U=T>
	struct Range{
		T start,stop;
		U step;
		constexpr Range(T start,T stop,U step)noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>):start(start),stop(stop),step(step){}
		constexpr Range(T start,T stop)noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>):start(start),stop(stop),step(1){}
		constexpr Range(T stop)noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>):start(0),stop(stop),step(1){}
		constexpr T operator[](size_t index)const noexcept(noexcept(start+index*step)){
			return start+index*step;			
		}
		constexpr size_t size()const noexcept(is_scalar_v<T> && is_scalar_v<U>){
			if constexpr(is_signed_v<U>){
				if((step>=0 && start>=stop) || (step<0 && start<stop)) return 0;
			}else{
				if(start>=stop) return 0;
			}
			return U(stop-start)/step + size_t((stop-start)%step!=0);
		}
		struct iterator{
			using iterator_category=random_access_iterator_tag;
			using value_type=remove_cv_t<T>;
			using difference_type=ptrdiff_t;
			using pointer=const T*;
			using reference=value_type;
			T pos;U step;
			constexpr iterator(T pos,U step)noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>):pos(pos),step(step){}
			constexpr reference operator*()noexcept{
				return pos;
			}
			constexpr reference operator[](difference_type n)noexcept(noexcept(pos+n*step)){
				return pos+n*step;
			}
			constexpr iterator& operator++()noexcept(noexcept(pos+=step)){
				pos+=step;
				return *this;
			}
			constexpr iterator operator++(int)noexcept(noexcept(pos+=step) && noexcept(iterator(pos-step,step))){
				pos+=step;
				return iterator(pos-step,step);
			}
			constexpr iterator& operator--()noexcept(noexcept(pos-=step)){
				pos-=step;
				return *this;
			}
			constexpr iterator operator--(int)noexcept(noexcept(pos-=step) && noexcept(iterator(pos+step,step))){
				pos-=step;
				return iterator(pos+step,step);
			}
			constexpr iterator& operator+=(difference_type n)noexcept(noexcept(pos+=step*n)){
				pos+=step*n;
				return *this;
			}
			constexpr iterator& operator-=(difference_type n)noexcept(noexcept(pos-=step*n)){
				pos-=step*n;
				return *this;
			}
			constexpr iterator operator+(difference_type n)const noexcept(noexcept(iterator(pos+step*n,step))){
				return iterator(pos+step*n,step);
			}
			constexpr iterator operator-(difference_type n)const noexcept(noexcept(iterator(pos-step*n,step))){
				return iterator(pos-step*n,step);
			}
			constexpr difference_type operator-(const iterator& other)const noexcept{
				return this->pos/this->step-other.pos/other.step;
			}
			constexpr bool operator!=(iterator other)const noexcept{
				if constexpr(U(-1)>U()){
					return this->pos<other.pos;
				}else{
					return step>=U() ? pos<other.pos : pos>other.pos;			
				}
			}
		};
		constexpr iterator begin()const noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>){
			return iterator(start,step);
		}
		constexpr iterator end()const noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>){
			return iterator(stop,step);
		}
	};
	template<typename T,typename U> Range(T,U)->Range<T,U>;
    template<typename Out,typename T,typename U>
    Out& operator<<(Out& stream,const Range<T,U>& range){
        stream<<"[";
        auto it(range.begin());
        auto it_end(range.end());
        while(it!=it_end){
            stream<<""<<*it;
            if(++it!=it_end)
                stream<<", ";
        }
		stream<<"]";
        return stream;
    }
}

namespace mpv{
    template<typename T,typename Tp>
    struct Ptr__ COUNT_IT{
		using element_type=T;
        protected:
            Tp data;
            constexpr Ptr__(Tp data):data(data){}
        public:
			constexpr add_lvalue_reference_t<element_type> operator*()const{
				return *data;
			}
			constexpr Tp operator->()const{
				return data;
			}
			constexpr Tp get()const{
				return data;
			}
			template<typename D,typename Dp> constexpr bool operator==(const Ptr__<D,Dp>& other)const{
				return this->data==other.get();
			}
			template<typename D,typename Dp> constexpr bool operator!=(const Ptr__<D,Dp>& other)const{
				return this->data!=other.get();
			}
			template<typename D,typename Dp> constexpr bool operator<=(const Ptr__<D,Dp>& other)const{
				return this->data<=other.get();
			}
			template<typename D,typename Dp> constexpr bool operator>=(const Ptr__<D,Dp>& other)const{
				return this->data>=other.get();
			}
			template<typename D,typename Dp> constexpr bool operator< (const Ptr__<D,Dp>& other)const{
				return this->data<other.get();
			}
			template<typename D,typename Dp> constexpr bool operator> (const Ptr__<D,Dp>& other)const{
				return this->data>other.get();
			}
			constexpr bool operator==(decltype(nullptr))const{
				return this->data==nullptr;
			}
			constexpr bool operator!=(decltype(nullptr))const{
				return this->data!=nullptr;
			}
			constexpr bool operator<=(decltype(nullptr))const{
				return this->data<=nullptr;
			}
			constexpr bool operator>=(decltype(nullptr))const{
				return this->data>=nullptr;
			}
			constexpr bool operator< (decltype(nullptr))const{
				return this->data<nullptr;
			}
			constexpr bool operator> (decltype(nullptr))const{
				return this->data>nullptr;
			}
			constexpr operator bool()const{
				return data!=nullptr;
			}
    };
	template<typename T,typename Tp=remove_extent_t<T>*>
	class PtrBase:public Ptr__<T,Tp>{
		protected:
			constexpr PtrBase():Ptr__<T,Tp>(nullptr){}
			constexpr PtrBase(Tp data):Ptr__<T,Tp>(data){}
	};
	template<typename T,typename Tp>
	class PtrBase<T[],Tp>:public Ptr__<T,Tp>{
		protected:
			constexpr PtrBase():Ptr__<T,Tp>(nullptr){}
			constexpr PtrBase(Tp data):Ptr__<T,Tp>(data){}
		public:
			constexpr add_lvalue_reference_t<T> operator[](size_t index)const{
				return Ptr__<T,Tp>::data[index];
		}
	};
	template<typename T,size_t N,typename Tp>
	class PtrBase<T[N],Tp>:public Ptr__<T,Tp>{
		struct Val_types{
			using value_type=typename pointer_traits<Tp>::element_type;
			using difference_type=typename pointer_traits<Tp>::difference_type;
			using size_type=make_unsigned_t<difference_type>;
			using pointer=Tp;
			using const_pointer=typename pointer_traits<Tp>::template rebind<const value_type>;
			using reference=T&;
			using const_reference=const T&;
		};
		protected:
			constexpr PtrBase():Ptr__<T,Tp>(nullptr){}
			constexpr PtrBase(Tp data):Ptr__<T,Tp>(data){}
		public:
			constexpr add_lvalue_reference_t<T> operator[](size_t index)const{
				return Ptr__<T,Tp>::data[index];
			}
			static constexpr size_t len=N;
			static constexpr size_t size=sizeof(T[N]);

			using iterator=random_access_iterator<Val_types>;
			constexpr iterator begin()const{
				return this->data;
			}
			constexpr iterator end()const{
				return this->data+len;
			}
			constexpr iterator rbegin()const{
				return this->data+len-1;
			}
			constexpr iterator rend()const{
				return this->data-1;
			}
	};
}

namespace mpv{
	template<typename T>
	struct default_delete{
		constexpr default_delete()noexcept=default;
		template<typename U>constexpr default_delete(const default_delete<U>&)noexcept{}
		template<typename U>constexpr void operator()(U* p)noexcept{delete p;}
	};
	template<typename T>struct default_delete<T[]>{
		constexpr default_delete()noexcept=default;
		template<typename U>constexpr default_delete(const default_delete<U[]>&)noexcept{}
		template<typename U,size_t N>constexpr default_delete(const default_delete<U[N]>&)noexcept{}
		template<typename U>constexpr void operator()(U* p)noexcept{delete[] p;}
	};
	template<typename T,size_t N>struct default_delete<T[N]>{
		constexpr default_delete()noexcept=default;
		template<typename U>constexpr default_delete(const default_delete<U[N]>&)noexcept{}
		template<typename U>constexpr void operator()(U* p)noexcept{delete[] p;}
	};
	template<typename Alloc>
	struct alloc_wrapper_delete:EBCO<Alloc>{
		using pointer=typename allocator_traits<Alloc>::pointer;
		typename allocator_traits<Alloc>::size_type size;
		constexpr alloc_wrapper_delete(size_t size)noexcept:size(size){}
		constexpr alloc_wrapper_delete(const Alloc& al,size_t size):EBCO<Alloc>(al),size(size){}
		template<typename pU>constexpr void operator()(pU p)noexcept{
			for(typename allocator_traits<Alloc>::size_type i=0;i<size;i++)
				allocator_traits<Alloc>::destroy(this->get_val(),unfancy(p+i));
			allocator_traits<Alloc>::deallocate(this->get_val(),p,size);
		}
	};
	template<typename T,typename deleter,typename=void> struct p_ty{using pointer=remove_extent_t<T>*;};
	template<typename T,typename deleter> 				struct p_ty<T,deleter,void_t<typename remove_reference_t<deleter>::pointer>>{using pointer=typename remove_reference_t<deleter>::pointer;};

	template<typename T,typename Del=default_delete<T>>
	class uPtr:public PtrBase<T,typename p_ty<T,Del>::pointer>,protected EBCO<Del>{static_assert(!is_rvalue_ref<Del>::value,"deleter cannot be an rvalue ref");
		public:
			using pointer=typename p_ty<T,Del>::pointer;
			using element_type=remove_extent_t<T>;
			using deleter_type=Del;
			static_assert(!(is_array_v<T> && sizeof(element_type)!=sizeof(typename pointer_traits<pointer>::element_type)));
			uPtr(const uPtr&)=delete;
			uPtr& operator=(const uPtr&)=delete;
			constexpr uPtr()noexcept:PtrBase<T,pointer>(nullptr),EBCO<Del>(Del()){}

			constexpr uPtr(decltype(nullptr))noexcept:PtrBase<T,pointer>(nullptr),EBCO<Del>(Del()){}

			constexpr explicit uPtr(pointer dat)noexcept:PtrBase<T,pointer>(dat),EBCO<Del>(Del()){static_assert(!is_pointer<Del>::value,"Shouldn't default construct pointer to deleter");}

			constexpr uPtr(pointer dat,const Del& new_del)noexcept:PtrBase<T,pointer>(dat),EBCO<Del>(new_del){}

			template<typename Del2=Del>//Esto es porque si no uso un template el compilador va a evaluar la funcion y va a generar un error.
			constexpr uPtr(pointer dat,enable_if_t<!is_lvalue_ref_v<Del2>,Del&&> new_del)noexcept:PtrBase<T,pointer>(dat),EBCO<Del>(static_cast<Del&&>(new_del)){}
							//	  ^^^^^  Uso el enable_if porq en caso de que Del sea una referencia, por el colapso de referencias esta funcion quedaria igual a la de arriba y habria un error de funcion ambigua.

			constexpr uPtr(uPtr&& other)noexcept:PtrBase<T,pointer>(other.data),EBCO<Del>(static_cast<Del&&>(other.get_val())){
				other.data=nullptr;
			}
			template<typename U,typename Del2,enable_if_t<is_convertible_v<typename uPtr<U,Del2>::pointer,pointer> && (have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>)),int> = 0>
			constexpr uPtr(uPtr<U,Del2>&& other)noexcept:PtrBase<T,pointer>(other.data),EBCO<Del>(static_cast<Del2&&>(other.get_val())){
				static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
				other.data=nullptr;
			}
			constexpr uPtr& operator=(decltype(nullptr))noexcept{
				if(this->data!=nullptr) this->get_val()(this->data);
				this->data=nullptr;
				return *this;
			}
			constexpr uPtr& operator=(pointer dat)noexcept{
				if(this->data!=nullptr) this->get_val()(this->data);
				this->data=dat;
				return *this;
			}
			constexpr uPtr& operator=(uPtr<T,Del>&& other)noexcept{
				if(this->data!=nullptr) this->get_val()(this->data);
				this->get_val()=static_cast<Del&&>(other.get_val());
				this->data=other.data;
				other.data=nullptr;
				return *this;
			}
			template<typename U,typename Del2,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr uPtr& operator=(uPtr<U,Del2>&& other)noexcept{
				static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
				if(this->data!=nullptr) this->get_val()(this->data);
				this->get_val()=static_cast<Del2&&>(other.get_val());
				this->data=other.data;
				other.data=nullptr;
				return *this;
			}
			constexpr pointer drop()noexcept{
				pointer p=this->data;
				this->data=nullptr;
				return p;
			}
			constexpr Del& get_deleter()noexcept{
				return this->get_val();
			}
			~uPtr()noexcept{
				if(this->data!=nullptr) this->get_val()(this->data);
			}
		template<typename,typename>friend class uPtr;
		template<typename t,typename del1,typename u,typename del2> friend constexpr enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> static_ptr_cast(uPtr<u,del2>&&)noexcept;
		template<typename t,typename del1,typename u,typename del2> friend constexpr enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> reinterpret_ptr_cast(uPtr<u,del2>&&)noexcept;
		template<typename t,typename del1,typename u,typename del2> friend constexpr enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> const_ptr_cast(uPtr<u,del2>&&)noexcept;
		template<typename t,typename del1,typename u,typename del2> friend constexpr enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> dynamic_ptr_cast(uPtr<u,del2>&&);
	};
	template<typename T,typename Del1=default_delete<T>,typename U,typename Del2=default_delete<U>>
	constexpr enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> static_ptr_cast(uPtr<U,Del2>&& other)noexcept{
		static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=static_cast<typename uPtr<T,Del1>::pointer>(other.data);
		other.data=nullptr;
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
	template<typename T,typename Del1=default_delete<T>,typename U,typename Del2=default_delete<U>>
	constexpr enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> reinterpret_ptr_cast(uPtr<U,Del2>&& other)noexcept{
		static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=reinterpret_cast<typename uPtr<T,Del1>::pointer>(other.data);
		other.data=nullptr;
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
	template<typename T,typename Del1=default_delete<T>,typename U,typename Del2=default_delete<U>>
	constexpr enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> const_ptr_cast(uPtr<U,Del2>&& other)noexcept{
		static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=const_cast<typename uPtr<T,Del1>::pointer>(other.data);
		other.data=nullptr;
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
	template<typename T,typename Del1=default_delete<T>,typename U,typename Del2=default_delete<U>>
	constexpr enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> dynamic_ptr_cast(uPtr<U,Del2>&& other){
		static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=dynamic_cast<typename uPtr<T,Del1>::pointer>(other.data);
		if(p==nullptr) other.get_val()(other.data);
		other.data=nullptr;
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
}

namespace mpv{
	struct CtrlBlock{
#ifdef USE_ATOMIC
		std::atomic<size_t> sharedCount;
		std::atomic<size_t> weakCount;
		// size_t sharedCount;
		// size_t weakCount;		
#else
		size_t sharedCount;
		size_t weakCount;
#endif
		constexpr CtrlBlock():sharedCount(1),weakCount(1){
			//INCCTRLBLOCKS
		}
		virtual void destroy_object()noexcept=0;
		virtual void delete_this()noexcept=0;
		~CtrlBlock()noexcept{
			//DECCTRLBLOCKS
			//REMAININGCTRLBLOCKS
		}
		CtrlBlock(const CtrlBlock&)=delete;
		CtrlBlock& operator=(const CtrlBlock&)=delete;
	};
	template<typename T>
	struct Block_pointer final:CtrlBlock{
		remove_extent_t<T>* ptr;
		constexpr Block_pointer(remove_extent_t<T>* p):ptr(p){}
		void destroy_object()noexcept override{	if constexpr(is_no_size_array_v<T>) delete[] ptr;
										else 								delete ptr;  }
		void delete_this()noexcept override{ delete this; }
	};

	template<typename Tp,typename Del>
	struct Block_pointer_and_deleter final:CtrlBlock{
		CompressedPair<Del,Tp> compressed_pair;
		constexpr Block_pointer_and_deleter(Tp p,const Del& deleter):compressed_pair(arg1_tag{},deleter,p){}
		constexpr Block_pointer_and_deleter(Tp p,Del&& deleter):compressed_pair(arg1_tag{},static_cast<Del&&>(deleter),p){}
		void destroy_object()noexcept override{compressed_pair.getV1()(compressed_pair.V2);}
		void delete_this()noexcept override{ delete this; }
	};
	template<typename Tp,typename Del,typename Alloc>
	struct Block_pointer_deleter_and_allocator final:CtrlBlock{
		CompressedTriple<Del,Alloc,Tp> compressed_triple;
		constexpr Block_pointer_deleter_and_allocator(Tp p,const Del& deleter,const Alloc& alloc):compressed_triple(arg1and2_tag{},deleter,alloc,p){}
		void destroy_object()noexcept override{ compressed_triple.getV1()(compressed_triple.getV3()); }
		void delete_this()noexcept override{
			using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_pointer_deleter_and_allocator>;
			BlockAlloc ctrlblock_alloc(compressed_triple.getV2());
			this->~Block_pointer_deleter_and_allocator();
			allocator_traits<BlockAlloc>::deallocate(ctrlblock_alloc,this,1);
		}
	};
	template<typename T>
	struct Block_object final:CtrlBlock{
		union{T obj;};
		template<typename... Args>
		constexpr Block_object(Args&&... args):obj(static_cast<Args&&>(args)...){}
		void destroy_object()noexcept override{if constexpr(is_class_v<T>) obj.~T();}
		void delete_this()noexcept override{ delete this; }
		~Block_object()noexcept{}
	};
	template<typename T,size_t N>
	struct Block_object<T[N]> final:CtrlBlock{
		union{T obj[N];};
		constexpr Block_object():obj{}{}
		constexpr Block_object(const T& init){
			for(size_t i=0;i<N;i++)
				new(obj+i) T(init);
		}
		void destroy_object()noexcept override{
			if constexpr(is_class_v<T>){
				for(size_t i=0;i<N;i++)
					obj[i].~T();
			}
        }
		void delete_this()noexcept override{ delete this; }
		~Block_object()noexcept{}
	};

	template<typename T>
	struct Block_object<T[]> final:CtrlBlock{
		const size_t count;
		//T obj[];//Arreglo flexible
		constexpr Block_object(const size_t count):count(count){
			new(this+1) T[count]();
		}
		constexpr Block_object(const size_t count,const T& init):count(count){
			for(size_t i=0;i<count;i++)
				new(reinterpret_cast<T*>(this+1)+i) T(init);
		}
		void destroy_object()noexcept override{
			if constexpr(is_class_v<T>){
				for(size_t i=0;i<count;i++)
					(reinterpret_cast<T*>(this+1)+i)->~T();
			}
		}
		void delete_this()noexcept override{
			this->~Block_object();
			::operator delete(this);
		}
	};
	template<typename T,typename Alloc>
	struct Block_object_and_allocator final:CtrlBlock,EBCO<typename allocator_traits<Alloc>::template rebind_alloc<T>>{
		using TAlloc=typename allocator_traits<Alloc>::template rebind_alloc<T>;
		union{ T obj; };
		template<typename... Args>
		constexpr Block_object_and_allocator(const Alloc& alloc,Args&&... args):EBCO<TAlloc>(alloc){
			allocator_traits<TAlloc>::construct(this->get_val(),&obj,static_cast<Args&&>(args)...);
		}
		void destroy_object()noexcept override{
			allocator_traits<TAlloc>::destroy(this->get_val(),&obj);
		}
		void delete_this()noexcept override{
			using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator>;
			BlockAlloc ctrlblock_alloc(this->get_val());
			this->~Block_object_and_allocator();
			allocator_traits<BlockAlloc>::deallocate(ctrlblock_alloc,this,1);
		}
		~Block_object_and_allocator()noexcept{}
	};
	template<typename T,size_t N,typename Alloc>
	struct Block_object_and_allocator<T[N],Alloc> final:CtrlBlock,EBCO<typename allocator_traits<Alloc>::template rebind_alloc<T>>{
		using TAlloc=typename allocator_traits<Alloc>::template rebind_alloc<T>;
		union{ T obj[N]; };
		constexpr Block_object_and_allocator(const Alloc& alloc):EBCO<TAlloc>(alloc){
			for(size_t i=0;i<N;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),obj+i);
		}
		constexpr Block_object_and_allocator(const Alloc& alloc,const T& init):EBCO<TAlloc>(alloc){
			for(size_t i=0;i<N;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),obj+i,init);
		}
		void destroy_object()noexcept override{
			for(size_t i=0;i<N;i++)
				allocator_traits<TAlloc>::destroy(this->get_val(),obj+i);
		}
		void delete_this()noexcept override{
			using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator>;
			BlockAlloc ctrlblock_alloc(this->get_val());
			this->~Block_object_and_allocator();
			allocator_traits<BlockAlloc>::deallocate(ctrlblock_alloc,this,1);
		}
		~Block_object_and_allocator()noexcept{}
	};
	template<typename T,typename Alloc>
	struct Block_object_and_allocator<T[],Alloc> final:CtrlBlock,EBCO<typename allocator_traits<Alloc>::template rebind_alloc<T>>{
		const size_t count;
		using TAlloc=typename allocator_traits<Alloc>::template rebind_alloc<T>;
		//T obj[];//Arreglo flexible
		constexpr Block_object_and_allocator(const size_t count,const Alloc& alloc):EBCO<TAlloc>(alloc),count(count){
			for(size_t i=0;i<count;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),reinterpret_cast<T*>(this+1)+i);
		}
		constexpr Block_object_and_allocator(const size_t count,const Alloc& alloc,const T& init):EBCO<TAlloc>(alloc),count(count){
			for(size_t i=0;i<count;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),reinterpret_cast<T*>(this+1)+i,init);
		}
		void destroy_object()noexcept override{
			for(size_t i=0;i<count;i++)
				allocator_traits<TAlloc>::destroy(this->get_val(),reinterpret_cast<T*>(this+1)+i);
		}
		void delete_this()noexcept override{
			using ByteAlloc=typename allocator_traits<Alloc>::template rebind_alloc<char>;
			ByteAlloc byte_allocator(this->get_val());
			this->~Block_object_and_allocator();
			allocator_traits<ByteAlloc>::deallocate(byte_allocator,reinterpret_cast<char*>(this),sizeof(Block_object_and_allocator)+count*sizeof(T));
		}
	};
	template<typename T> class wPtr;
	template<typename T>
	class sPtr:public PtrBase<T>{
		private:
			CtrlBlock* ctrl_block=nullptr;
			template<typename U>
			constexpr void setp(U* const ptr){
				TemporaryOwner<remove_array_size_t<rebind_array_t<T,U>>> temp_owner(ptr);
				ctrl_block=new Block_pointer<remove_array_size_t<rebind_array_t<T,U>>>(ptr);
				this->data=ptr;
				temp_owner.ptr=nullptr;
			}
			template<typename Up,typename Del>
			constexpr void setpd(const Up ptr,Del dt){
				TemporaryOwnerDel<Up,Del> temp_owner(ptr,dt);
				ctrl_block=new Block_pointer_and_deleter<Up,Del>(ptr,static_cast<Del&&>(dt));
				this->data=unfancy(ptr);
				temp_owner.call_deleter=false;
			}
			template<typename Up,typename Del,typename Alloc>
			constexpr void setpda(const Up ptr,Del dt,Alloc alloc){
				using CtrlBlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_pointer_deleter_and_allocator<Up,Del,Alloc>>;
				TemporaryOwnerDel<Up,Del> temp_owner(ptr,dt);
				CtrlBlockAlloc ctrlblock_alloc(alloc);
				AllocConstructPtr allocation_temp_owner(ctrlblock_alloc);
				allocation_temp_owner.allocate();
				new(unfancy(allocation_temp_owner.ptr)) Block_pointer_deleter_and_allocator<Up,Del,Alloc>(temp_owner.ptr,static_cast<Del&&>(dt),alloc);
				this->data=unfancy(temp_owner.ptr);
				this->ctrl_block=unfancy(allocation_temp_owner.ptr);
				temp_owner.call_deleter=false;
				allocation_temp_owner.ptr=nullptr;
			} 
			constexpr sPtr(CtrlBlock* ctrl_block,remove_extent_t<T>* ptr)noexcept:PtrBase<T>(ptr),ctrl_block(ctrl_block){// Solo las funciones de casteo pueden usar este constructor
				if(ctrl_block!=nullptr) ctrl_block->sharedCount++;
			}
			constexpr void release()noexcept{
				if(this->ctrl_block!=nullptr){//	El puntero actual deja de apuntar a la memoria de la que era dueño, si esta no tiene mas dueños se libera
					if((--this->ctrl_block->sharedCount)==0){	//Si sharedCount llega a 0 borra la memoria a la que apunta
						this->ctrl_block->destroy_object();
						if(--this->ctrl_block->weakCount==0)	//Solo borra el contador de referencias si weakCount tambien es 0. En caso de que 
							this->ctrl_block->delete_this();		//weakCount no sea 0, ctrl_block no se borrara y el destructor del wPtr que lo mantiene vivo se ocupara de esto
					}
				}
				this->data=nullptr;
				this->ctrl_block=nullptr;
			}
		public:
			using element_type=typename PtrBase<T>::element_type;
			template<typename U>
			constexpr sPtr(const wPtr<U>& weakPtr)noexcept:PtrBase<T>(weakPtr.data),ctrl_block(weakPtr.ctrl_block){	//construye un puntero fuerte en base a uno debil
				if(weakPtr.ctrl_block==nullptr || weakPtr.ctrl_block->sharedCount==0){
					this->ctrl_block=nullptr;
					this->data=nullptr;
				}
				else
					this->ctrl_block->sharedCount++;
			}
			constexpr sPtr()noexcept{}
			constexpr sPtr(decltype(nullptr))noexcept{}
			template<typename U>
			constexpr explicit sPtr(U* dat){
				if(dat) setp(dat);
			}
			template<typename U,typename Del>
			constexpr sPtr(U* dat,Del deleter){
				if(dat) setpd(dat,static_cast<Del&&>(deleter));
			}
			template<typename U,typename Del,typename Alloc>
			constexpr sPtr(U* dat,Del deleter,Alloc alloc):PtrBase<T>(dat),ctrl_block(nullptr){
				if(dat) setpda(dat,static_cast<Del&&>(deleter),alloc);
			}
			template<typename U,typename Del>
			constexpr sPtr(uPtr<U,Del>&& unique){
				if(unique.get()){
					setpd<typename uPtr<U,Del>::pointer,
						If_t<is_reference_v<Del>,
							reference_wrapper<remove_reference_t<Del>>//lo mismo que decltype(ref(unique.get_deleter()))
							,Del>>(unique.drop(),static_cast<Del&&>(unique.get_deleter()));
				}
			}
			constexpr sPtr(const sPtr& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){//Si no declaro este constructor el compilador lo declara de forma implicita y prefiere llamarlo antes instanciar un template, lo que genera comportamiento indefinido.
				if(this->ctrl_block!=nullptr) this->ctrl_block->sharedCount++;
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr sPtr(const sPtr<U>& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
				if(this->ctrl_block!=nullptr) this->ctrl_block->sharedCount++;
			}
			constexpr sPtr(sPtr<T>&& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
				other.data=nullptr;
				other.ctrl_block=nullptr;
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr sPtr(sPtr<U>&& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
				other.data=nullptr;
				other.ctrl_block=nullptr;
			}
			template<typename U>
			constexpr sPtr(const sPtr<U>& other,remove_extent_t<T>* alias_ptr)noexcept:PtrBase<T>(nullptr),ctrl_block(other.ctrl_block){
				if(this->ctrl_block!=nullptr){
					this->ctrl_block->sharedCount++;
					this->data=alias_ptr;
				}
			}
			template<typename U>
			constexpr sPtr(sPtr<U>&& other,remove_extent_t<T>* alias_ptr)noexcept:PtrBase<T>(other.ctrl_block!=nullptr ? alias_ptr : nullptr),ctrl_block(other.ctrl_block){
				other.ctrl_block=nullptr;
				other.data=nullptr;
			}
			constexpr sPtr& operator=(decltype(nullptr))noexcept{
				this->release();
				return *this;
			}
			template<typename U>
			constexpr sPtr& operator=(U* dat){
				this->release();
				if(dat) setp(dat);
				return *this;
			}
			template<typename U>
			constexpr sPtr& reasign(U* dat){
				this->release();
				if(dat) setp(dat);
				return *this;
			}
			template<typename U,typename Del>
			constexpr sPtr& reasign(U* dat,Del&& deleter){
				this->release();
				if(dat) setpd(dat,static_cast<Del&&>(deleter));
				return *this;
			}
			template<typename U,typename Del,typename Alloc>
			constexpr sPtr& reasign(U* dat,Del&& deleter,Alloc alloc){
				this->release();
				if(dat) setpda(dat,static_cast<Del&&>(deleter),alloc);
				return *this;
			}
			template<typename U,typename Del,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr sPtr& operator=(uPtr<U,Del>&& unique){
				this->release();
				if(unique.get()){
					setpd<typename uPtr<U,Del>::pointer,
						If_t<is_reference_v<Del>,
							reference_wrapper<remove_reference_t<Del>>//lo mismo que decltype(ref(unique.get_deleter()))
							,Del>>(unique.drop(),static_cast<Del&&>(unique.get_deleter()));
				}
				return *this;
			}
			constexpr sPtr& operator=(const sPtr& other)noexcept{	//Con este pasa lo mismo que con el constructor de copia, asi que tambien lo tengo que declarar
				if(other.ctrl_block!=nullptr) other.ctrl_block->sharedCount++;				
				this->release();
				this->data=other.data;
				this->ctrl_block=other.ctrl_block;
				return *this;
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr sPtr& operator=(const sPtr<U>& other)noexcept{
				if(other.ctrl_block!=nullptr) other.ctrl_block->sharedCount++;
				this->release();
				this->data=other.data;
				this->ctrl_block=other.ctrl_block;
				return *this;
			}
			constexpr sPtr& operator=(sPtr&& other)noexcept{
				if(this==&other) return *this;
				this->release();// No es necesario comprobar si es el mismo ctrl_block que el de other porq si se esta moviendo un sPtr a otro
				this->data=other.data;// y ninguno apunta a null, eso significa que sharedCount es mas de 2.
				this->ctrl_block=other.ctrl_block;
				other.data=nullptr;
				other.ctrl_block=nullptr;
				return *this;
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr sPtr& operator=(sPtr<U>&& other)noexcept{
				this->release();
				this->data=other.data;
				this->ctrl_block=other.ctrl_block;
				other.data=nullptr;
				other.ctrl_block=nullptr;
				return *this;
			}
			constexpr size_t currentOwners()const noexcept{
				return this->ctrl_block==nullptr? 0 : (size_t)this->ctrl_block->sharedCount;
			}
			~sPtr()noexcept{
				release();
			}
			template<typename u> friend class sPtr;
			template<typename u> friend class wPtr;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> static_ptr_cast(const sPtr<u>&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> reinterpret_ptr_cast(const sPtr<u>&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> const_ptr_cast(const sPtr<u>&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> dynamic_ptr_cast(const sPtr<u>&);
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> static_ptr_cast(sPtr<u>&&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> reinterpret_ptr_cast(sPtr<u>&&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> const_ptr_cast(sPtr<u>&&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> dynamic_ptr_cast(sPtr<u>&&);
			template<typename t,typename ...Args> constexpr friend enable_if_t<!is_array_v<t>,sPtr<t>> make_sPtr(Args&&...);
			template<typename t> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> make_sPtr();
			template<typename t> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> make_sPtr(const remove_extent_t<t>&);
			template<typename t> friend enable_if_t<is_no_size_array_v<t>,sPtr<t>> make_sPtr(size_t);
			template<typename t> friend enable_if_t<is_no_size_array_v<t>,sPtr<t>> make_sPtr(size_t,const remove_extent_t<t>&);
			template<typename t,typename Alloc,typename... Args> friend constexpr enable_if_t<!is_array_v<t>,sPtr<t>> alloc_sPtr(const Alloc&,Args&&...);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> alloc_sPtr(const Alloc&);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> alloc_sPtr(const Alloc&,const remove_extent_t<t>&);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_no_size_array_v<t>,sPtr<t>> alloc_sPtr(size_t,const Alloc&);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_no_size_array_v<t>,sPtr<t>> alloc_sPtr(size_t,const Alloc&,const remove_extent_t<t>&);
	};
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> static_ptr_cast(const sPtr<U>& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		return sPtr<T>(other.ctrl_block,static_cast<remove_extent_t<T>*>(&*other));
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> reinterpret_ptr_cast(const sPtr<U>& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		return sPtr<T>(other.ctrl_block,reinterpret_cast<remove_extent_t<T>*>(&*other));
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> const_ptr_cast(const sPtr<U>& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		return sPtr<T>(other.ctrl_block,const_cast<remove_extent_t<T>*>(&*other));
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> dynamic_ptr_cast(const sPtr<U>& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		remove_extent_t<T>* ptr=dynamic_cast<remove_extent_t<T>*>(&*other);
		return ptr!=nullptr? sPtr<T>(other.ctrl_block,ptr) : sPtr<T>();
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> static_ptr_cast(sPtr<U>&& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		sPtr<T> new_ptr;
		new_ptr.data=static_cast<remove_extent_t<T>*>(other.data);
		new_ptr.ctrl_block=other.ctrl_block;
		other.ctrl_block=nullptr;
		other.data=nullptr;
		return new_ptr;
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> reinterpret_ptr_cast(sPtr<U>&& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		sPtr<T> new_ptr;
		new_ptr.data=reinterpret_cast<remove_extent_t<T>*>(other.data);
		new_ptr.ctrl_block=other.ctrl_block;
		other.ctrl_block=nullptr;
		other.data=nullptr;
		return new_ptr;
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> const_ptr_cast(sPtr<U>&& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		sPtr<T> new_ptr;
		new_ptr.data=const_cast<remove_extent_t<T>*>(other.data);
		new_ptr.ctrl_block=other.ctrl_block;
		other.ctrl_block=nullptr;
		other.data=nullptr;
		return new_ptr;
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> dynamic_ptr_cast(sPtr<U>&& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		sPtr<T> new_ptr;
		new_ptr.data=dynamic_cast<remove_extent_t<T>*>(other.data);
		if(new_ptr.data==nullptr)
			other.release();
		else
			new_ptr.ctrl_block=other.ctrl_block;
		other.ctrl_block=nullptr;
		other.data=nullptr;
		return new_ptr;
	}

	template<typename T,typename... Args>
	constexpr enable_if_t<!is_array_v<T>,sPtr<T>> make_sPtr(Args&& ...args){
		sPtr<T> ptr;
		ptr.ctrl_block=new Block_object<remove_cv_t<remove_cv_t<T>>>((Args&&)args...);
		ptr.data = &static_cast<Block_object<remove_cv_t<T>>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> make_sPtr(){
		sPtr<T> ptr;
		ptr.ctrl_block=new Block_object<remove_cv_t<T>>();
		ptr.data=static_cast<Block_object<remove_cv_t<T>>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> make_sPtr(const remove_extent_t<T>& init){
		sPtr<T> ptr;
		ptr.ctrl_block=new Block_object<remove_cv_t<T>>(init);
		ptr.data=static_cast<Block_object<remove_cv_t<T>>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T>
	enable_if_t<is_no_size_array_v<T>,sPtr<T>> make_sPtr(size_t n){
		sPtr<T> ptr;
		CtrlBlock* bytes=(CtrlBlock*)::operator new(sizeof(Block_object<T>)+sizeof(remove_extent_t<T>)*n);
		GlobalDeleteGuard guard{bytes};
		new(bytes) Block_object<remove_cv_t<T>>(n);
		ptr.ctrl_block=bytes;
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object<remove_cv_t<T>>));
		guard.ptr=nullptr;
		return ptr;
	}
	template<typename T>
	enable_if_t<is_no_size_array_v<T>,sPtr<T>> make_sPtr(size_t n,const remove_extent_t<T>& init){
		sPtr<T> ptr;
		CtrlBlock* bytes=(CtrlBlock*)::operator new(sizeof(Block_object<T>)+sizeof(remove_extent_t<T>)*n);
		GlobalDeleteGuard guard{bytes};
		new(bytes) Block_object<remove_cv_t<T>>(n,init);
		ptr.ctrl_block=bytes;
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object<remove_cv_t<T>>));
		guard.ptr=nullptr;
		return ptr;
	}
	template<typename T,typename Alloc,typename... Args>
	constexpr enable_if_t<!is_array_v<T>,sPtr<T>> alloc_sPtr(const Alloc& alloc,Args&&... args){
		sPtr<T> ptr;
		using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator<T,Alloc>>;
		BlockAlloc block_allocator(alloc);
		AllocConstructPtr<BlockAlloc> allocation_temp_owner(block_allocator);
		allocation_temp_owner.allocate();
		new(unfancy(allocation_temp_owner.ptr)) Block_object_and_allocator<remove_cv_t<T>,Alloc>(alloc,static_cast<Args&&>(args)...);
		ptr.ctrl_block=unfancy(allocation_temp_owner.ptr);
		ptr.data=&allocation_temp_owner.ptr->obj;
		allocation_temp_owner.ptr=nullptr;
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> alloc_sPtr(const Alloc& alloc){
		sPtr<T> ptr;
		using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator<T,Alloc>>;
		BlockAlloc block_allocator(alloc);
		AllocConstructPtr allocation_temp_owner(block_allocator);
		allocation_temp_owner.allocate();
		new(unfancy(allocation_temp_owner.ptr)) Block_object_and_allocator<remove_cv_t<T>,Alloc>(alloc);
		ptr.ctrl_block=unfancy(allocation_temp_owner.ptr);
		ptr.data=allocation_temp_owner.ptr->obj;
		allocation_temp_owner.ptr=nullptr;
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> alloc_sPtr(const Alloc& alloc,const remove_extent_t<T>& init){
		sPtr<T> ptr;
		using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator<T,Alloc>>;
		BlockAlloc block_allocator(alloc);
		AllocConstructPtr allocation_temp_owner(block_allocator);
		allocation_temp_owner.allocate();
		new(unfancy(allocation_temp_owner.ptr)) Block_object_and_allocator<remove_cv_t<T>,Alloc>(alloc,init);
		ptr.ctrl_block=unfancy(allocation_temp_owner.ptr);
		ptr.data=allocation_temp_owner.ptr->obj;
		allocation_temp_owner.ptr=nullptr;
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_no_size_array_v<T>,sPtr<T>> alloc_sPtr(size_t n,const Alloc& alloc){
		sPtr<T> ptr;
		using ByteAlloc=typename allocator_traits<Alloc>::template rebind_alloc<char>;
		ByteAlloc byte_alloc(alloc);
		AllocatorDeleteGuard<ByteAlloc> guard{byte_alloc,sizeof(Block_object_and_allocator<T,Alloc>)+n*sizeof(remove_extent_t<T>)};
		new(reinterpret_cast<void*>(unfancy(guard.ptr))) Block_object_and_allocator<remove_cv_t<T>,Alloc>(n,alloc);
		ptr.ctrl_block=reinterpret_cast<CtrlBlock*>(unfancy(guard.ptr));
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object_and_allocator<remove_cv_t<T>,Alloc>));
		guard.ptr=nullptr;
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_no_size_array_v<T>,sPtr<T>> alloc_sPtr(size_t n,const Alloc& alloc,const remove_extent_t<T>& init){
		sPtr<T> ptr;
		using ByteAlloc=typename allocator_traits<Alloc>::template rebind_alloc<char>;
		ByteAlloc byte_alloc(alloc);
		AllocatorDeleteGuard<ByteAlloc> guard{byte_alloc,sizeof(Block_object_and_allocator<T,Alloc>)+n*sizeof(remove_extent_t<T>)};
		new(reinterpret_cast<void*>(unfancy(guard.ptr))) Block_object_and_allocator<remove_cv_t<T>,Alloc>(n,alloc,init);
		ptr.ctrl_block=reinterpret_cast<CtrlBlock*>(unfancy(guard.ptr));
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object_and_allocator<remove_cv_t<T>,Alloc>));
		guard.ptr=nullptr;
		return ptr;
	}
}

namespace mpv{
    template<typename T>
    class wPtr:public PtrBase<T>{
        private:
            CtrlBlock* ctrl_block=nullptr;
            constexpr void release()noexcept{
                if(ctrl_block!=nullptr){
                    if(--ctrl_block->weakCount==0){
                        ctrl_block->delete_this();
                    }
                }
            }
        public:
            constexpr wPtr()noexcept=default;
            constexpr wPtr(const wPtr& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(this->ctrl_block!=nullptr) this->ctrl_block->weakCount++;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
            constexpr wPtr(const wPtr<U>& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(this->ctrl_block!=nullptr) this->ctrl_block->weakCount++;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U>,int> = 0>
            constexpr wPtr(wPtr<U>&& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                other.data=nullptr;
                other.ctrl_block=nullptr;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U>,int> = 0>
            constexpr wPtr(const sPtr<U>& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(this->ctrl_block!=nullptr) this->ctrl_block->weakCount++;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U>,int> = 0>
            constexpr wPtr(sPtr<U>&& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(other.ctrl_block!=nullptr){
                    other.ctrl_block->weakCount++;
                    if(--other.ctrl_block->sharedCount==0){
                        other.ctrl_block->destroy_object();
                        other.ctrl_block->weakCount--;
                    }
                }
                other.data=nullptr;
                other.ctrl_block=nullptr;
            }
            constexpr wPtr& operator=(const wPtr& other)noexcept{
                if(other.ctrl_block!=nullptr) other.ctrl_block->weakCount++;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                return *this;
            }
            template<typename U>
            constexpr wPtr& operator=(const wPtr<U>& other)noexcept{
                if(other.ctrl_block!=nullptr) other.ctrl_block->weakCount++;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                return *this;
            }
            constexpr wPtr& operator=(wPtr&& other)noexcept{
                if(this==&other) return other;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                other.data=nullptr;
                other.ctrl_block=nullptr;
                return *this;
            }
            template<typename U>
            constexpr wPtr& operator=(wPtr<U>&& other)noexcept{
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                other.data=nullptr;
                other.ctrl_block=nullptr;
                return *this;
            }
            template<typename U>
            constexpr wPtr& operator=(const sPtr<U>& other)noexcept{
                if(other.ctrl_block!=nullptr) other.ctrl_block->weakCount++;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                return *this;
            }
            template<typename U>
            constexpr wPtr& operator=(sPtr<U>&& other)noexcept{
                if(other.ctrl_block!=nullptr){
                    other.ctrl_block->weakCount++;
                    if(--other.ctrl_block->sharedCount==0){
                        other.ctrl_block->destroy_object();
                        other.ctrl_block->weakCount--;
                    }
                }
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                other.data=nullptr;
                other.ctrl_block=nullptr;
                return *this;
            }
            constexpr size_t currentOwners()noexcept{
                return ctrl_block==nullptr ? 0 : (size_t)ctrl_block->sharedCount;
            }
            constexpr bool expired()noexcept{
                return ctrl_block==nullptr ? true : !ctrl_block->sharedCount;
            }
            ~wPtr()noexcept{
                release();
            }
    };
}

namespace mpv{
	template<typename T=char,typename Alloc=allocator<T>,typename params=params<typename allocator_traits<Alloc>::template rebind_traits<T>::size_type,16,3>>
	class Str COUNT_IT{
        private:
            using AlTy=rebind_alloc<Alloc,T>;
            using AlTy_traits=allocator_traits<AlTy>;
			struct Val_types{
				using value_type=T;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=T&;
				using const_reference=const T&;
			};
            static constexpr typename Val_types::size_type ss_cap=params::p1;
            static constexpr typename Val_types::size_type M=params::p2;
            static constexpr bool POCCA=AlTy_traits::propagate_on_container_copy_assignment::value;
            static constexpr bool POCMA=AlTy_traits::propagate_on_container_move_assignment::value;
            static constexpr bool POCS=AlTy_traits::propagate_on_container_swap::value;
            static constexpr bool ALWAYS_EQ=AlTy_traits::is_always_equal::value;
        public:
			using allocator_type=Alloc;
			using value_type=typename Val_types::value_type;
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
            using iterator=random_access_iterator<Val_types>;
			using const_iterator=const_random_access_iterator<Val_types>;
		static_assert(ss_cap>=sizeof(pointer) && M>1,"ss_cap must be >=sizeof(pointer) and M must be >1");
		union sbo{
			value_type buffer[ss_cap];
			pointer heap;
			constexpr sbo()noexcept:heap(nullptr){};
		};
		private:
		/* maxLen es el valor maximo al que puede llegar length antes de tener que reasignar memoria. 
		Cuando se necesita reasignar, el espacio que se asigna es de maxLen+1(para el \0) ya que maxLen y length(que no tiene en cuenta el \0)
		deben(por conveniencia) poder coincidir en un mismo numero sin tener que reasignar.
		Si maxLen es 0, significa que se esta usando el buffer para almacenar el string. Si maxLen>0 el buffer se usa para almacenar un puntero al string en memoria dinamica.*/
			CompressedPair<AlTy,size_type> cp=CompressedPair<AlTy,size_type>(0);//contiene el allocator y maxLen

			size_type length;// Es el tamaanio del string, sin tener en cuenta el '\0'
		/*length no puede ser >= a ss_cap sin tener que asignar memoria, una vez que se esta usando memoria dinamica
		length debe se siempre <= a maxLen (puede llegar a ser igual a maxLen porque maxLen no cuenta el \0
											como un lugar, en cambio ss_cap si) */
			sbo text;
#define maxLen cp.V2
#define alloc cp.getV1()
			template<typename It>
			constexpr iterator insert_counted_range(const_iterator pos,It first,size_type count){
				size_type new_len=this->length+count,index=pos-begin();
				pointer ret;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						value_type* insert_pos=this->text.buffer+index;
						copy_reverse_n(insert_pos+count,insert_pos,this->length-index);
						copy_n(insert_pos,first,count);
						this->text.buffer[new_len]=T();
						ret=pointer(this->text.buffer+index);
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),this->text.buffer,index);
						copy_n(insert_pos,first,count);
						copy_n(insert_pos+count,this->text.buffer+index,this->length-index);
						new_array[new_len]=T();
						this->text.heap=new_array;
						ret=new_array+index;
					}
				}
				else{
					if(new_len<=this->maxLen){
						value_type* insert_pos=unfancy(this->text.heap)+index;
						copy_reverse_n(insert_pos+count,insert_pos,this->length-index);
						copy_n(insert_pos,first,count);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),unfancy(this->text.heap),index);
						copy_n(insert_pos,first,count);
						copy_n(insert_pos+count,unfancy(this->text.heap)+index,this->length-index);
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
					ret=this->text.heap+index;
				}
				this->length=new_len;
				return ret;
			}
            template<typename It>
            constexpr iterator insert_uncounted_range(const_iterator pos,It first,It last){
                size_type index=pos-begin(),prev_length=length;
                while(first!=last) push_back(static_cast<T>(*first++));
				value_type* array=c_str();
                rotate(array+index,array+prev_length,array+length);
                return pointer(array+index);
            }
		public:
			constexpr const T* c_str()const noexcept{//devuelve un puntero a la memoria que se esta usando actualmente en la union. NO se puede llamar si se modifico maxLen.
				return maxLen==0 ? this->text.buffer : unfancy(this->text.heap);
			}
			constexpr T* c_str()noexcept{//devuelve un puntero a la memoria que se esta usando actualmente en la union. NO se puede llamar si se modifico maxLen.
				return maxLen==0 ? this->text.buffer : unfancy(this->text.heap);
			}
			constexpr Str()noexcept:cp(0),length(0),text{}{}
			constexpr Str(const Alloc& al)noexcept:cp(arg1_tag{},al,0),length(0){}
			template<typename It,enable_if_t<is_iterator_v<It>,int> = 0>
			constexpr Str(It first,It last,const Alloc& al=Alloc{}):cp(arg1_tag{},al,0),length(0){
                insert(begin(),first,last);
			}
			constexpr explicit Str(size_type sz):cp(0),length(sz){
				if(sz>=ss_cap){
					maxLen=length*M;
					text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					text.heap[length]=T();
				}
				else text.buffer[length]=T();
			}
			constexpr explicit Str(size_type sz,const Alloc& al):cp(arg1_tag{},al,0),length(sz){
				if(sz>=ss_cap){
					maxLen=length*M;
					text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					text.heap[length]=T();
				}
				else text.buffer[length]=T();
			}
			constexpr Str(const Str& other):cp(arg1_tag{},AlTy_traits::select_on_container_copy_construction(other.alloc),0),length(other.length){
				if(other.length<ss_cap){
					copy_n(this->text.buffer,other.c_str(),other.length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),other.c_str(),other.length+1);
				}
			}
			constexpr Str(const Str& other,const Alloc& al):cp(arg1_tag{},al,0),length(other.length){
				if(other.length<ss_cap){
					copy_n(this->text.buffer,other.c_str(),other.length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),other.c_str(),other.length+1);
				}
			}
			constexpr Str(const value_type* str):cp(0),length(al::strsize<AlTy>(str)){
				if(this->length<ss_cap){
					copy_n(this->text.buffer,str,length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),str,length+1);
				}
			}
			constexpr Str(const value_type* str,const Alloc& al):cp(arg1_tag{},al,0),length(al::strsize<AlTy>(str)){
				if(this->length<ss_cap){
					copy_n(this->text.buffer,str,length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),str,length+1);
				}
			}
			constexpr Str(const value_type* str,size_type n):cp(0),length(n){
				if(this->length<ss_cap){
					copy_n(this->text.buffer,str,n);
					this->text.buffer[length]=T();
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),str,n);
					this->text.heap[length]=T();
				}
			}
			constexpr Str(const value_type* str,size_type n,const Alloc& al):cp(arg1_tag{},al,0),length(n){
				if(this->length<ss_cap){
					copy_n(this->text.buffer,str,n);
					this->text.buffer[length]=T();
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),str,n);
					this->text.heap[length]=T();
				}
			}
			constexpr explicit Str(T chr)noexcept:cp(0),length(1){
				this->text.buffer[0]=chr;
				this->text.buffer[1]=T();
			}
			constexpr Str(T chr,const Alloc& al)noexcept:cp(arg1_tag{},al,0),length(1){
				this->text.buffer[0]=chr;
				this->text.buffer[1]=T();
			}
			constexpr Str(size_type n,T chr):cp(0),length(n){
				if(this->length<ss_cap){
					fill_n(this->text.buffer,n,chr);
					this->text.buffer[length]=T();
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					fill_n(unfancy(this->text.heap),n,chr);
					this->text.heap[length]=T();
				}
			}
			constexpr Str(size_type n,T chr,const Alloc& al):cp(arg1_tag{},al,0),length(n){
				if(this->length<ss_cap){
					fill_n(this->text.buffer,n,chr);
					this->text.buffer[length]=T();
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					fill_n(unfancy(this->text.heap),n,chr);
					this->text.heap[length]=T();
				}
			}
			constexpr Str(Str&& other)noexcept:cp(arg1_tag{},static_cast<AlTy&&>(other.alloc),other.maxLen),length(other.length){
				if(other.maxLen==0)
					copy_n(this->text.buffer,other.text.buffer,other.length+1);
				else
					this->text.heap=other.text.heap;
				other.text.heap=nullptr;
				other.length=0;
				other.maxLen=0;
			}
			constexpr Str(Str&& other,const Alloc& al)noexcept(ALWAYS_EQ):cp(arg1_tag{},al,other.maxLen),length(other.length){
				if(other.maxLen==0)
					copy_n(this->text.buffer,other.text.buffer,other.length+1);
				else{
					if(this->alloc==other.alloc)
						this->text.heap=other.text.heap;
					else{
						this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
						copy_n(unfancy(this->text.heap),unfancy(other.text.heap),other.length+1);
						AlTy_traits::deallocate(this->alloc,other.text.heap,other.maxLen+1);
					}
				}
				other.text.heap=nullptr;
				other.length=0;
				other.maxLen=0;
			}
			constexpr Str& operator=(const Str& other){
				if(this==&other) return *this;
				if(this->maxLen==0){
					pocca(this->alloc,other.alloc);
					if(other.length<ss_cap){
						copy_n(this->text.buffer,other.c_str(),other.length+1);// +1 para copiar el \0 tambien
					}
					else{
						this->text.heap=AlTy_traits::allocate(this->alloc,other.length*M+1);
						this->maxLen=other.length*M;
						copy_n(unfancy(this->text.heap),other.c_str(),other.length+1);
					}
				}
				else{
					if(other.length<=this->maxLen){
						if constexpr(POCCA && !ALWAYS_EQ){
							if(this->alloc!=other.alloc){
								pointer new_array=AlTy_traits::allocate(this->alloc,other.maxLen+1);
								AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
								this->text.heap=new_array;
								pocca(this->alloc,other.alloc);
								this->maxLen=other.maxLen;
							}
						}
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,other.length*M+1);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->text.heap=new_array;
						pocca(this->alloc,other.alloc);
						this->maxLen=other.length*M;
					}
					copy_n(unfancy(this->text.heap),other.c_str(),other.length+1);
				}
				this->length=other.length;
				return *this;
			}
			constexpr Str& operator=(const value_type* str){
				size_type new_length=al::strsize<AlTy>(str);
				if(this->maxLen==0){
					if(new_length<ss_cap)
						copy_n(this->text.buffer,str,new_length+1);
					else{
						this->text.heap=AlTy_traits::allocate(this->alloc,new_length*M+1);
						this->maxLen=new_length*M;
						copy_n(unfancy(this->text.heap),str,new_length+1);
					}
				}
				else{
					if(new_length>this->maxLen){
						pointer new_array=AlTy_traits::allocate(this->alloc,new_length*M+1);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_length*M;
						this->text.heap=new_array;
						
					}
					copy_n(unfancy(this->text.heap),str,new_length+1);
				}
				this->length=new_length;
				return *this;
			}
			constexpr Str& operator=(T chr)noexcept{
				this->length=1;
				if(this->maxLen==0){
					this->text.buffer[0]=chr;
					this->text.buffer[1]=T();
				}
				else{
					this->text.heap[0]=chr;
					this->text.heap[1]=T();
				}
				return *this;
			}
			constexpr Str& operator=(Str&& other)noexcept(ALWAYS_EQ || POCMA){
				if(this==&other) return *this;
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->alloc!=other.alloc){
						if(this->maxLen==0){
							if(other.maxLen==0)
								copy_n(this->text.buffer,other.text.buffer,other.length+1);
							else{
								this->text.heap=AlTy_traits::allocate(this->alloc,other.maxLen+1);
								this->maxLen=other.maxLen;
								copy_n(unfancy(this->text.heap),unfancy(other.text.heap),other.length+1);
							}
						}
						else{
							if(other.maxLen==0)
								copy_n(unfancy(this->text.heap),other.text.buffer,other.length+1);
							else{
								pointer new_array=AlTy_traits::allocate(this->alloc,other.maxLen+1);
								AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
								this->text.heap=new_array;
								this->maxLen=other.maxLen;
								copy_n(unfancy(this->text.heap),unfancy(other.text.heap),other.length+1);
							}
						}
						this->length=other.length;
						return *this;						
					}
				}
				if(this->maxLen==0){
					if(other.maxLen==0)
						copy_n(this->text.buffer,other.text.buffer,other.length+1);
					else{
						this->maxLen=other.maxLen;
						this->text.heap=other.text.heap;
						other.maxLen=0;
					}
				}
				else{
					if(other.maxLen==0)
						copy_n(unfancy(this->text.heap),other.text.buffer,other.length+1);
					else{
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=other.maxLen;
						this->text.heap=other.text.heap;
						other.maxLen=0;
					}
				}
				pocma(this->alloc,other.alloc);
				this->length=other.length;
				other.length=0;
				other.text.heap=nullptr;
				return *this;
			}
			constexpr Str operator+(const Str& other)const&{
				Str new_string(this->length+other.length,AlTy_traits::select_on_container_copy_construction(alloc));//Constructor que asigna un length(ya tiene en cuenta el espacio extra para el \0, al cual asigna \0) y reserva la memoria necesaria
				copy_n(new_string.c_str(),this->c_str(),this->length);
				copy_n(new_string.c_str()+this->length,other.c_str(),other.length);
				return new_string;
			}

			constexpr Str operator+(const value_type* str)const&{
				Str new_string(this->length+al::strsize<AlTy>(str),AlTy_traits::select_on_container_copy_construction(alloc));
				copy_n(new_string.c_str(),this->c_str(),this->length);
				copy_n(new_string.c_str()+this->length,str,new_string.length-this->length+1);//strcopy<AlTy>(new_string.c_str()+this->length,str);
				return new_string;
			}
			constexpr Str operator+(T chr)const&{
				Str new_string(this->length+1,AlTy_traits::select_on_container_copy_construction(alloc));
				copy_n(new_string.c_str(),this->c_str(),this->length);
				new_string.c_str()[this->length]=chr;
				return new_string;
			}
			constexpr Str operator+(Str&& other)const&{
				Str new_string(static_cast<Str&&>(other));
				size_type new_len=new_string.length+this->length;
				if(new_string.maxLen==0){
					if(new_len<ss_cap){
						copy_reverse_n(new_string.text.buffer+this->length,new_string.text.buffer,new_string.length+1);
						copy_n(new_string.text.buffer,this->c_str(),this->length);
					}
					else{
						pointer new_array=AlTy_traits::allocate(new_string.alloc,new_len*M+1);
						new_string.maxLen=new_len*M;
						copy_n(unfancy(new_array)+this->length,new_string.text.buffer,new_string.length+1);
						copy_n(unfancy(new_array),this->c_str(),this->length);
						new_string.text.heap=new_array;
					}
				}
				else{
					if(new_len<=new_string.maxLen){
						copy_reverse_n(unfancy(new_string.text.heap)+this->length,unfancy(new_string.text.heap),new_string.length+1);
						copy_n(unfancy(new_string.text.heap),this->c_str(),this->length);
					}
					else{
						pointer new_array=AlTy_traits::allocate(new_string.alloc,new_len*M+1);
						copy_n(unfancy(new_array)+this->length,unfancy(new_string.text.heap),new_string.length+1);
						copy_n(unfancy(new_array),this->c_str(),this->length);
						AlTy_traits::deallocate(new_string.alloc,new_string.text.heap,new_string.maxLen+1);
						new_string.maxLen=new_len*M;
						new_string.text.heap=new_array;
					}
				}
				new_string.length=new_len;
				return new_string;
			}
			constexpr Str operator+(const Str& other)&&{
				Str new_string(static_cast<Str&&>(*this));
				new_string+=other;
				return new_string;
			}
			constexpr Str operator+(const value_type* str)&&{
				Str new_string(static_cast<Str&&>(*this));
				new_string+=str;
				return new_string;
			}
			constexpr Str operator+(T chr)&&{
				Str new_string(static_cast<Str&&>(*this));
				new_string+=chr;
				return new_string;
			}
			constexpr Str operator+(Str&& other)&&{
				Str new_string(static_cast<Str&&>(*this));
				new_string+=static_cast<Str&&>(other);
				return new_string;
			}
			constexpr Str& operator+=(const Str& other){
				size_type new_len=this->length+other.length;
				if(this->maxLen==0){
					if(new_len<ss_cap)
						copy_reverse_n(this->text.buffer+this->length,other.c_str(),other.length+1);//copy_reverse porque this y other podrian ser el mismo objeto
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						copy_n(unfancy(new_array),this->text.buffer,this->length);
						copy_n(unfancy(new_array)+this->length,other.c_str(),other.length+1);
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen)
						copy_reverse_n(unfancy(this->text.heap)+this->length,other.c_str(),other.length+1);
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						copy_n(unfancy(new_array),unfancy(this->text.heap),this->length);
						copy_n(unfancy(new_array)+this->length,other.c_str(),other.length+1);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr Str& operator+=(Str&& other){//si this y other son el mismo objeto es comportamiento indeinido
				size_type new_len=this->length+other.length;
				if(this->maxLen==0){
					if(new_len<ss_cap)
						copy_n(this->text.buffer+this->length,other.c_str(),other.length+1);
					else if((POCMA || this->alloc==other.alloc) && new_len<=other.maxLen){
						pocma(this->alloc,other.alloc);
						copy_reverse_n(unfancy(other.text.heap)+this->length,unfancy(other.text.heap),other.length+1);
						copy_n(unfancy(other.text.heap),this->text.buffer,this->length);
						this->text.heap=other.text.heap;
						this->maxLen=other.maxLen;
						other.length=0;
						other.maxLen=0;
						other.text.heap=nullptr;
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						copy_n(unfancy(new_array),this->text.buffer,this->length);
						copy_n(unfancy(new_array)+this->length,other.c_str(),other.length+1);
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen)
						copy_n(unfancy(this->text.heap)+this->length,other.c_str(),other.length+1);
					else if((POCMA || this->alloc==other.alloc) && new_len<=other.maxLen){
						pocma(this->alloc,other.alloc);
						copy_reverse_n(unfancy(other.text.heap)+this->length,unfancy(other.text.heap),other.length+1);
						copy_n(unfancy(other.text.heap),unfancy(this->text.heap),this->length);
						this->text.heap=other.text.heap;
						this->maxLen=other.maxLen;
						other.length=0;
						other.maxLen=0;
						other.text.heap=nullptr;
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						copy_n(unfancy(new_array),unfancy(this->text.heap),this->length);
						copy_n(unfancy(new_array)+this->length,other.c_str(),other.length+1);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr Str& operator+=(const value_type* str){
				size_type str_size=al::strsize<AlTy>(str);
				size_type new_len=this->length+str_size;
				if(this->maxLen==0){
					if(new_len<ss_cap)
						copy_reverse_n(this->text.buffer+this->length,str,str_size+1);//reversecopy porq this->c_str() puede ser str
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						copy_n(unfancy(new_array),this->text.buffer,this->length);
						copy_n(unfancy(new_array)+this->length,str,str_size+1);//strcopy<AlTy>(new_array+this->length,str);
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen)
						copy_reverse_n(unfancy(this->text.heap)+this->length,str,str_size+1);
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						copy_n(unfancy(new_array),unfancy(this->text.heap),this->length);
						copy_n(unfancy(new_array)+this->length,str,str_size+1);//strcopy<AlTy>(new_array+this->length,str);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr Str& operator+=(T chr){
				push_back(chr);
				return *this;
			}
			constexpr Str operator*(unsigned int num)const{
				Str new_string(this->length*num,AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_array=new_string.c_str();
				const value_type* this_array=this->c_str();
				for(size_type index=0;index<new_string.length;index+=this->length)
					copy_n(new_array+index,this_array,this->length);
				return new_string;
			}
			constexpr Str& operator*=(unsigned int num){
				size_type new_len=this->length*num;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						for(size_type index=this->length;index<new_len;index+=this->length)
							copy_n(this->text.buffer+index,this->text.buffer,this->length);
						this->text.buffer[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						for(size_type index=0;index<new_len;index+=this->length)
							copy_n(unfancy(new_array)+index,this->text.buffer,this->length);
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen){
						for(size_type index=this->length;index<new_len;index+=this->length)
							copy_n(unfancy(this->text.heap)+index,unfancy(this->text.heap),this->length);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						for(size_type index=0;index<new_len;index+=this->length)
							copy_n(unfancy(new_array)+index,unfancy(this->text.heap),this->length);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr bool operator==(const Str& other)const{
				if(this->length!=other.length) return false;
				else return mpv::equal_n(this->c_str(),other.c_str(),length);
			}
			constexpr bool operator!=(const Str& other)const{
				if(this->length!=other.length) return true;
				else return !mpv::equal_n(this->c_str(),other.c_str(),length);
			}
			constexpr bool operator<(const Str& other)const{
				return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
			}
			constexpr bool operator>(const Str& other)const{
				return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
			}
			constexpr bool operator<=(const Str& other)const{
				return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
			}
			constexpr bool operator>=(const Str& other)const{
				return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
			}
			constexpr const T& operator[](size_type index)const noexcept{
				return this->c_str()[index];
			}
			constexpr T& operator[](size_type index)noexcept{
				return this->c_str()[index];
			}
			constexpr void push_back(T chr){
				size_type new_len=this->length+1;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						this->text.buffer[length]=chr;
						this->text.buffer[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						copy_n(unfancy(new_array),this->text.buffer,length);
						new_array[length]=chr;
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen){
						this->text.heap[length]=chr;
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						copy_n(unfancy(new_array),unfancy(this->text.heap),length);
						new_array[length]=chr;
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
			}
			constexpr Str& insert(size_type index,T chr){
				size_type new_len=this->length+1;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						value_type* insert_pos=this->text.buffer+index;
						copy_reverse_n(insert_pos+1,insert_pos,this->length-index);
						*insert_pos=chr;
						this->text.buffer[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),this->text.buffer,index);
						*insert_pos=chr;
						copy_n(insert_pos+1,this->text.buffer+index,this->length-index);
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen){
						value_type* insert_pos=unfancy(this->text.heap)+index;
						copy_reverse_n(insert_pos+1,insert_pos,this->length-index);
						*insert_pos=chr;
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),unfancy(this->text.heap),index);
						*insert_pos=chr;
						copy_n(insert_pos+1,unfancy(this->text.heap)+index,this->length-index);
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr iterator insert(const_iterator pos,T chr){
				size_type index=pos-begin();
				insert(index,chr);
				return pointer(c_str()+index);
			}
            template<typename It>
            constexpr enable_if_t<is_iterator_v<It>,iterator> insert(const_iterator pos,It first,It last){
                if constexpr(is_forward_iterator_v<It>){
                    return insert_counted_range(pos,first,distance(first,last));
                }
                else{
                    return insert_uncounted_range(pos,first,last);
                }
            }
			constexpr iterator insert(const_iterator pos,const size_type count,T val){
				size_type new_len=this->length+count,index=pos-begin();
				pointer ret;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						value_type* insert_pos=this->text.buffer+index;
						copy_reverse_n(insert_pos+count,insert_pos,this->length-index);
						fill_n(insert_pos,count,val);
						this->text.buffer[new_len]=T();
						ret=pointer(this->text.buffer+index);
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),this->text.buffer,index);
						fill_n(insert_pos,count,val);
						copy_n(insert_pos+count,this->text.buffer+index,this->length-index);
						new_array[new_len]=T();
						this->text.heap=new_array;
						ret=new_array+index;
					}
				}
				else{
					if(new_len<=this->maxLen){
						value_type* insert_pos=unfancy(this->text.heap)+index;
						copy_reverse_n(insert_pos+count,insert_pos,this->length-index);
						fill_n(insert_pos,count,val);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),unfancy(this->text.heap),index);
						fill_n(insert_pos,count,val);
						copy_n(insert_pos+count,unfancy(this->text.heap)+index,this->length-index);
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
					ret=this->text.heap+index;
				}
				this->length=new_len;
				return ret;
			}
			constexpr Str& insert(size_type index,const Str& other){
				size_type new_len=this->length+other.length;
				if(this->maxLen==0){
					if(new_len<ss_cap && this!=&other){
						value_type* insert_pos=this->text.buffer+index;
						copy_reverse_n(insert_pos+other.length,insert_pos,this->length-index);
						copy_n(insert_pos,other.c_str(),other.length);
						this->text.buffer[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),this->text.buffer,index);
						copy_n(insert_pos,other.c_str(),other.length);
						copy_n(insert_pos+other.length,this->text.buffer+index,this->length-index);
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen && this!=&other){
						value_type* insert_pos=unfancy(this->text.heap)+index;
						copy_reverse_n(insert_pos+other.length,insert_pos,this->length-index);
						copy_n(insert_pos,other.c_str(),other.length);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),unfancy(this->text.heap),index);
						copy_n(insert_pos,other.c_str(),other.length);
						copy_n(insert_pos+other.length,unfancy(this->text.heap)+index,this->length-index);
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr Str& insert(size_type index,const value_type* str){
				size_type str_size=al::strsize<AlTy>(str);
				size_type new_len=this->length+str_size;
				if(this->maxLen==0){
					if(new_len<ss_cap && this->text.buffer!=str){
						value_type* insert_pos=this->text.buffer+index;
						copy_reverse_n(insert_pos+str_size,insert_pos,this->length-index);
						copy_n(insert_pos,str,str_size);
						this->text.buffer[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),this->text.buffer,index);
						copy_n(insert_pos,str,str_size);
						copy_n(insert_pos+str_size,this->text.buffer+index,this->length-index);
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen && unfancy(this->text.heap)!=str){
						value_type* insert_pos=unfancy(this->text.heap)+index;
						copy_reverse_n(insert_pos+str_size,insert_pos,this->length-index);
						copy_n(insert_pos,str,str_size);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),unfancy(this->text.heap),index);
						copy_n(insert_pos,str,str_size);
						copy_n(insert_pos+str_size,unfancy(this->text.heap)+index,this->length-index);
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr void del_back()noexcept{
				this->c_str()[--length]=T();
			}
			constexpr T pop_back()noexcept{
				T chr=this->c_str()[length-1];
				this->c_str()[--length]=T();
				return chr;
			}
			constexpr Str& erase(size_type start,size_type end)noexcept{
				size_type new_len=this->length-(end-start);
				value_type* this_array=this->c_str();
				copy_n(this_array+start,this_array+end,this->length-end);
				this_array[new_len]=T();
				this->length=new_len;
				return *this;
			}
			constexpr Str& erase(size_type start)noexcept{
				this->c_str()[start]=T();
				this->length=start;
				return *this;
			}
			constexpr Str& remove(const Str& substring,size_type start,size_type end)noexcept{
				size_type index=this->find(substring,start,end);
				if(index!=length){
					this->erase(index,index+substring.length);	
				}
				return *this;
			}
			constexpr Str& remove(const value_type* substring,size_type start,size_type end)noexcept{
				size_type index=this->find(substring,start,end);
				if(index!=length){
					this->erase(index,index+al::strsize<AlTy>(substring));	
				}
				return *this;
			}
			constexpr Str& rremove(const Str& substring,size_type start,size_type end)noexcept{
				size_type index=this->rfind(substring,start,end);
				if(index!=length){
					this->erase(index,index+substring.length);	
				}
				return *this;
			}
			constexpr Str& rremove(const value_type* substring,size_type start,size_type end)noexcept{
				size_type index=this->rfind(substring,start,end);
				if(index!=length){
					this->erase(index,index+al::strsize<AlTy>(substring));	
				}
				return *this;
			}
			constexpr Str& remove(const Str& substring,size_type start=0)noexcept{
				size_type index=this->find(substring,start);
				if(index!=length){
					this->erase(index,index+substring.length);	
				}
				return *this;
			}
			constexpr Str& remove(const value_type* substring,size_type start=0)noexcept{
				size_type index=this->find(substring,start);
				if(index!=length){
					this->erase(index,index+al::strsize<AlTy>(substring));	
				}
				return *this;
			}
			constexpr Str& rremove(const Str& substring,size_type start=0)noexcept{
				size_type index=this->rfind(substring,start);
				if(index!=length){
					this->erase(index,index+substring.length);	
				}
				return *this;
			}
			constexpr Str& rremove(const value_type* substring,size_type start=0)noexcept{
				size_type index=this->rfind(substring,start);
				if(index!=length){
					this->erase(index,index+al::strsize<AlTy>(substring));	
				}
				return *this;
			}
			constexpr Str& outplace(const Str& substring,size_type start,size_type end){
				size_type new_len=this->length-(end-start)+substring.length;
				const value_type* substring_text=substring.c_str();
				value_type* this_array=this->c_str();
				if((new_len<ss_cap && this->maxLen==0) || new_len<=this->maxLen){			//		  vv for \0
					mpv::copy_overlap_n(this_array+start+substring.length,this_array+end,this->length-end+1);// copyes memory correctly even if source and dest overlap
					copy_n(this_array+start,substring_text,substring.length);
				}
				else{
					pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
					copy_n(unfancy(new_array),this_array,start);
					copy_n(unfancy(new_array)+start,substring_text,substring.length);
					copy_n(unfancy(new_array)+start+substring.length,this_array+end,this->length-end+1);
					if(this->maxLen>0) AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
					this->text.heap=new_array;
					this->maxLen=new_len*M;
				}
				this->length=new_len;
				return *this;
			}
			template<bool whole_words_only=false>
			Str& replace(const Str& substring1,const Str& substring2,Optional<size_type> max_count={}){
				size_type count=this->count<whole_words_only>(substring1);
				if(max_count.has_value() && count>max_count.value())
					count=max_count.value();
				if(count==0) return *this;
				using AlInt=rebind_alloc<AlTy,size_type>;
				AlInt uint_alloc(this->alloc);
				//size_type* array=allocator_traits<AlInt>::allocate(uint_alloc,count);
				mpv::uPtr<size_type[],alloc_wrapper_delete<AlInt>> array(allocator_traits<AlInt>::allocate(uint_alloc,count),alloc_wrapper_delete<AlInt>(uint_alloc,count));
				for(size_type i=0,last=0;i<count;last=array[i++]+substring1.length)
					allocator_traits<AlInt>::construct(uint_alloc,&array[i],this->find<whole_words_only>(substring1,last));
				for(size_type i=0;i<count;i++)
					array[i]=array[i]+(substring2.length-substring1.length)*i;
				if(substring1.length==0){
					for(size_type i=0;i<count;i++)
						array[i]+=i;
				}
				size_type new_len=this->length+(count*substring2.length-count*substring1.length);
				const value_type* substring2_text=substring2.c_str();
				if(this->maxLen==0){
					if(new_len<=this->length){
						for(size_type i=array[0],j=array[0],k=0;i<new_len;){
							if(k<count && i==array[k]){
								copy_n(this->text.buffer+array[k++],substring2_text,substring2.length);
								i+=substring2.length;
								j+=substring1.length;
							}
							else{
								this->text.buffer[i]=this->text.buffer[j];
								i++;
								j++;
							}
						}
						this->text.buffer[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						for(size_type i=0,j=0,k=0;i<new_len;){
							if(k<count && i==array[k]){
								copy_n(unfancy(new_array)+array[k++],substring2_text,substring2.length);
								i+=substring2.length;
								j+=substring1.length;
							}
							else{
								new_array[i]=this->text.buffer[j];
								i++;
								j++;
							}
						}
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->length){//aunque new_len<=maxLen si new_len<=length no se cumple se necesita reasignar memoria
						for(size_type i=array[0],j=array[0],k=0;i<new_len;){
							if(k<count && i==array[k]){
								copy_n(unfancy(this->text.heap)+array[k++],substring2_text,substring2.length);
								i+=substring2.length;
								j+=substring1.length;
							}
							else{
								this->text.heap[i]=this->text.heap[j];
								i++;
								j++;
							}
						}
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						for(size_type i=0,j=0,k=0;i<new_len;){
							if(k<count && i==array[k]){
								copy_n(unfancy(new_array)+array[k++],substring2_text,substring2.length);
								i+=substring2.length;
								j+=substring1.length;
							}
							else{
								new_array[i]=this->text.heap[j];
								i++;
								j++;
							}
						}
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;						
					}
				}
				this->length=new_len;
				return *this;
				//allocator_traits<AlInt>::deallocate(uint_alloc,array,count);
			}
			constexpr Str substr(size_type start,size_type end)const{
				Str new_string(end-start,AlTy_traits::select_on_container_copy_construction(alloc));//el tamaÃƒÂ±o del nuevo string lo genera el constructor
				copy_n(new_string.c_str(),this->c_str()+start,new_string.length);
				return new_string;
			}
			constexpr Str substr(size_type start)const{
				Str new_string(length-start,AlTy_traits::select_on_container_copy_construction(alloc));
				copy_n(new_string.c_str(),this->c_str()+start,new_string.length);
				return new_string;
			}
			constexpr Str extract(size_type index,size_type pos)const{
				Str new_string(pos,AlTy_traits::select_on_container_copy_construction(alloc));
				copy_n(new_string.c_str(),this->c_str()+index,pos);
				return new_string;
			}
			constexpr Str extract(size_type index)const{
				Str new_string(length-index,AlTy_traits::select_on_container_copy_construction(alloc));
				copy_n(new_string.c_str(),this->c_str()+index,length-index);
				return new_string;
			}
			template<T SPACE=' '>
			constexpr void noExtraSpaces()noexcept{
				value_type* this_array=this->c_str();
				size_type i=0,j=0;
				while(j<this->length){
					if(this_array[j]==SPACE && this_array[j+1]==SPACE){
						while(j<this->length && this_array[j]==this_array[j+1]){
							j++;
						}						
					}
					else{
						this_array[i++]=this_array[j++];
					}
				}
				this->length=i;
				this_array[i]=T();
			}
			template<T SPACE=' '>
			constexpr void lstrip()noexcept{
				value_type* this_array=this->c_str();
				if(this_array[0]!=SPACE) return;// Para no tener que copiar el string completo si es que no hay espacios que borrar
				size_type count=0;
				while(count<this->length && this_array[count]==SPACE)
					count++;
				this->length-=count;
				copy_n(this_array,this_array+count,this->length+1);
			}
			template<T SPACE=' '>
			constexpr void rstrip()noexcept{
				value_type* this_array=this->c_str();
				while(length>0 && this_array[length-1]==SPACE){
					length--;
				}
				this_array[length]=T();
			}
			template<T SPACE=' '>
			constexpr void strip()noexcept{
				rstrip<SPACE>();
				lstrip<SPACE>();
			}
			constexpr void upper()noexcept{
				value_type* this_array=this->c_str();
				for(size_type i=0;i<length;i++)
					if(this_array[i]>=97 && this_array[i]<=122)
						this_array[i]-=32;
			}
			constexpr void lower()noexcept{
				value_type* this_array=this->c_str();
				for(size_type i=0;i<length;i++)
					if(this_array[i]>=65 && this_array[i]<=90)
						this_array[i]+=32;
			}
			template<T SPACE=' '>
			constexpr Str noExtraSpaces_cpy()const{
				const value_type* this_array=this->c_str();
				size_type count=0;
				for(size_type i=0;i<this->length;i++)
					if(this_array[i]==SPACE && this_array[i+1]==SPACE)
						count++;
				Str new_string(this->length-count,AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_array=new_string.c_str();
				for(size_type i=0,j=0;i<this->length;i++){
					if(!(this_array[i]==SPACE && this_array[i+1]==SPACE)){
						new_array[j]=this_array[i];
						j++;
					}
				}
				return new_string;
			}
			template<T SPACE=' '>
 			constexpr Str lstrip_cpy()const{
				if(this->length==0) return Str();
				const value_type* this_array=this->c_str();
				size_type count=0;
				while(count<this->length && this_array[count]==SPACE)
					count++;
				Str new_string(this->length-count,AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_array=new_string.c_str();
				for(size_type i=0;count<this->length;i++,count++)
					new_array[i]=this_array[count];
				return new_string;
			}
			template<T SPACE=' '>
 			constexpr Str rstrip_cpy()const{
				const value_type* this_array=this->c_str();
				size_type count=this->length;
				while(count>0 && this_array[count-1]==SPACE)
					count--;
				Str new_string(count,AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_array=new_string.c_str();
				for(count=0;count<new_string.length;count++)
					new_array[count]=this_array[count];
				return new_string;
			}
			template<T SPACE=' '>
			constexpr Str strip_cpy()const{
				if(this->length==0) return Str();
				const value_type* this_array=this->c_str();
				size_type lcount=0,rcount=this->length;
				while(lcount<this->length && this_array[lcount]==SPACE)
					lcount++;
				if(lcount<this->length)//Por si el string solamente contiene espacios
					while(/* rcount>0 &&  */this_array[rcount-1]==SPACE)
						rcount--;					
				Str new_string(rcount-lcount,AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_array=new_string.c_str();
				for(rcount=0;rcount<new_string.length;lcount++,rcount++)
					new_array[rcount]=this_array[lcount];
				return new_string;
			}
			constexpr Str upper_cpy()const{
				Str new_string(this->length,AlTy_traits::select_on_container_copy_construction(alloc));
				if(this->length>=ss_cap)
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.heap[i]>=97 && this->text.heap[i]<=122 ? this->text.heap[i]-32:this->text.heap[i];
				else
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.buffer[i]>=97 && this->text.buffer[i]<=122 ? this->text.buffer[i]-32:this->text.buffer[i];
				return new_string;
			}
			constexpr Str lower_cpy()const{
				Str new_string(this->length,AlTy_traits::select_on_container_copy_construction(alloc));
				if(this->length>=ss_cap)
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.heap[i]>=65 && this->text.heap[i]<=90 ? this->text.heap[i]+32:this->text.heap[i];
				else
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.buffer[i]>=65 && this->text.buffer[i]<=90 ? this->text.buffer[i]+32:this->text.buffer[i];
				return new_string;
			}
			constexpr bool is_alpha()const noexcept{
				const value_type* s=this->c_str();
				for(size_type i=0;i<this->length;i++)
					if(!mpv::is_alpha(s[i])) return false;
				return true;
			}
			constexpr bool is_numeric()const noexcept{
				const value_type* s=this->c_str();
				for(size_type i=0;i<this->length;i++)
					if(!mpv::is_numeric(s[i])) return false;
				return true;
			}			
			constexpr bool is_alnum()const noexcept{
				const value_type* s=this->c_str();
				for(size_type i=0;i<this->length;i++)
					if(!mpv::is_alnum(s[i])) return false;
				return true;
			}
			constexpr bool is_alnum_us()const noexcept{		// Returns true if the string contains only letters, digits or underscores
				const value_type* s=this->c_str();
				for(size_type i=0;i<this->length;i++)
					if(!mpv::is_alnum_us(s[i])) return false;
				return true;
			}
			constexpr bool is_identifier()const noexcept{	// Returns true if the string is a valid name for a variable
				return this->is_alnum_us() && this->length>0 && !mpv::is_numeric(this->c_str()[0]);
			}
			constexpr bool is_int()const noexcept{
				const value_type* s=this->c_str();
				if(!mpv::is_numeric(s[0]) && s[0]!='-') return false;
				for(size_type i=1;i<this->length;i++)
					if(!mpv::is_numeric(s[i])) return false;
				return true;
			}	
			constexpr bool is_float_convertible()const noexcept{
				const value_type* s=c_str();
				if(*s==T()) return false;
				if(*s=='+' || *s=='-') s++;
				if(*s=='e' || *s=='E') return false;
				const char* e_pos=nullptr;
				bool dot_found=false;
				while(*s!=T()){
					if(!mpv::is_numeric(*s)){
						if(*s=='.'){
							if(dot_found || e_pos!=nullptr) return false;
							else dot_found=true;
						}
						else if(*s=='e' || *s=='E'){
							if(e_pos!=nullptr) return false;
							e_pos=s++;
							if(*s==T()) return false;
							if(*s=='+' || *s=='-'){
								e_pos=s;
							}
						}
						else return false;
					}
					s++;
				}
				return s-1!=e_pos;
			}
			constexpr bool contains(T c)const noexcept{
				return mpv::contains(c_str(),c_str()+length,c);
			}
			template<bool whole_words_only=false>
			constexpr bool contains(const Str& substring)const noexcept{
				if(this->length<substring.length) return false;
				if(substring.length==0) return true;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				for(size_type i=0;i<=this->length-substring.length;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring.length) && (i==0 || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))) return true;
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)) return true;
					}
				}
				return false;
			}
			template<bool whole_words_only=false>
			constexpr bool contains(const value_type* c_substring)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(this->length<substring_size) return false;
				if(substring_size==0) return true;
				const value_type* this_array=this->c_str();
				for(size_type i=0;i<=this->length-substring_size;i++){
					if constexpr(whole_words_only){		// comprobar si i==end-c_substring.length no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(equal_n(c_substring,this_array+i,substring_size) && (i==0 || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))) return true;
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)) return true;
					}
				}
				return false;
			}
			template<bool whole_words_only=false>
			constexpr size_type find(const Str& substring,size_type start,size_type end)const noexcept{
				if(end<start+substring.length) return length;
				if(substring.length==0) return start;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				for(size_type i=start;i<=end-substring.length;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring.length || !mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)) return i;
					}
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type rfind(const Str& substring,size_type start,size_type end)const noexcept{
				if(end<start+substring.length) return length;
				if(substring.length==0) return end;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				for(size_type i=end-substring.length;i>=start;i--){
					if constexpr(whole_words_only){ // i==start para q no mire una posicon a la izquierda de start, porq esa posicion no esta en el rango de busqueda que se especifico en los parametros de la funcion(start y end).
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring.length || !mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))) return i;
					}else{																								// Y i==end-substring lo mismo que i==start pero a la derecha del string
						if(equal_n(c_substring,this_array+i,substring.length)) return i;
					}
					if(i==0) return length;//because i is unsigned
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type count(const Str& substring,size_type start,size_type end)const noexcept{
				if(end<start+substring.length) return 0;
				if(substring.length==0) return end-start+1;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=end-substring.length;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring.length || !mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))){
							counter++;
							i+=substring.length-1;
						}
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)){
							counter++;
							i+=substring.length-1;
						}
					}
				}
				return counter;
			}
			template<bool whole_words_only=false>
			constexpr size_type find(const value_type* c_substring,size_type start,size_type end)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(end<start+substring_size) return length;
				if(substring_size==0) return start;
				const value_type* this_array=this->c_str();
				for(size_type i=start;i<=end-substring_size;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring_size || !mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)) return i;
					}
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type rfind(const value_type* c_substring,size_type start,size_type end)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(end<start+substring_size) return length;
				if(substring_size==0) return end;
				const value_type* this_array=this->c_str();
				for(size_type i=end-substring_size;i>=start;i--){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring_size || !mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)) return i;
					}
					if(i==0) return length;
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type count(const value_type* c_substring,size_type start,size_type end)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(end<start+substring_size) return 0;
				if(substring_size==0) return end-start+1;
				const value_type* this_array=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=end-substring_size;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring_size || !mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))){
							counter++;
							i+=substring_size-1;
						}
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)){
							counter++;
							i+=substring_size-1;
						}
					}
				}
				return counter;
			}
			template<bool whole_words_only=false>
			constexpr size_type find(const Str& substring,size_type start=0)const noexcept{
				if(this->length<start+substring.length) return length;
				if(substring.length==0) return start;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				for(size_type i=start;i<=this->length-substring.length;i++){
					if constexpr(whole_words_only){		// comprobar si i==end-substring.length no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)) return i;
					}
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type rfind(const Str& substring,size_type start=0)const noexcept{
				if(this->length<start+substring.length) return length;
				if(substring.length==0) return this->length;//this->length==end
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				for(size_type i=this->length-substring.length;i>=start;i--){
					if constexpr(whole_words_only){		// comprobar si i==end-substring.length no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)) return i;
					}
					if(i==0) return length;
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type count(const Str& substring,size_type start=0)const noexcept{
				if(this->length<start+substring.length) return 0;
				if(substring.length==0) return this->length-start+1;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=this->length-substring.length;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))){
							counter++;
							i+=substring.length-1;
						}
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)){
							counter++;
							i+=substring.length-1;
						}
					}
				}
				return counter;
			}
			template<bool whole_words_only=false>
			constexpr size_type find(const value_type* c_substring,size_type start=0)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(this->length<start+substring_size) return length;
				if(substring_size==0) return start;
				const value_type* this_array=this->c_str();
				for(size_type i=start;i<=this->length-substring_size;i++){
					if constexpr(whole_words_only){		// comprobar si i==end-c_substring.length no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)) return i;
					}
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type rfind(const value_type* c_substring,size_type start=0)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(this->length<start+substring_size) return length;
				if(substring_size==0) return this->length;//this->length==end
				const value_type* this_array=this->c_str();
				for(size_type i=this->length-substring_size;i>=start;i--){
					if constexpr(whole_words_only){		// comprobar si i==end-substring_size no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)) return i;
					}
					if(i==0) return length;
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type count(const value_type* c_substring,size_type start=0)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(this->length<start+substring_size) return 0;
				if(substring_size==0) return this->length-start+1;
				const value_type* this_array=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=this->length-substring_size;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))){
							counter++;
							i+=substring_size-1;
						}
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)){
							counter++;
							i+=substring_size-1;
						}
					}
				}
				return counter;
			}
			template<template<typename...> typename Container,typename... Args>
			constexpr Container<Str,Args...> split(const value_type* substring)const{
				Container<Str> list;
				size_type substring_length=al::strsize<AlTy>(substring);
				if(substring_length==0){
					//if(this->length==0) list.emplace_back("",AlTy_traits::select_on_container_copy_construction(alloc));else //asi funcionaria como el split de java
					for(size_type i=0;i<this->length;i++){
						list.emplace_back((*this)[i],AlTy_traits::select_on_container_copy_construction(alloc));
					}
				}
				else{
					size_type start=0,end=0;
					while(start<=this->length){
						end=this->find(substring,start);// end=this->length if substring is not found
						list.push_back(this->substr(start,end));
						start=end+substring_length;
					}					
				}
				return list;
			}
			template<template<typename...> typename Container,typename... Args>
			constexpr Container<Str,Args...> split(const value_type* substring,const typename Container<Str,Args...>::allocator_type al)const{
				Container<Str> list(al);
				size_type substring_length=al::strsize<AlTy>(substring);
				if(substring_length==0){
					//if(this->length==0) list.emplace_back("",AlTy_traits::select_on_container_copy_construction(alloc));else //asi funcionaria como el split de java
					for(size_type i=0;i<this->length;i++){
						list.emplace_back((*this)[i],AlTy_traits::select_on_container_copy_construction(alloc));
					}
				}
				else{
					size_type start=0,end=0;
					while(start<=this->length){
						end=this->find(substring,start);
						list.push_back(this->substr(start,end));
						start=end+substring_length;
					}					
				}
				return list;
			}
			template<template<typename...> typename Container,typename... Args>
			constexpr Container<Str,Args...> split(const Str& substring)const{
				Container<Str> list;
				if(substring.length==0){
					//if(this->length==0) list.emplace_back("",AlTy_traits::select_on_container_copy_construction(alloc));else //asi funcionaria como el split de java
					for(size_type i=0;i<this->length;i++){
						list.emplace_back((*this)[i],AlTy_traits::select_on_container_copy_construction(alloc));
					}
				}
				else{
					size_type start=0,end=0;
					while(start<=this->length){
						end=this->find(substring,start);
						list.push_back(this->substr(start,end));
						start=end+substring.length;
					}					
				}
				return list;
			}
			template<template<typename...> typename Container,typename... Args>
			constexpr Container<Str,Args...> split(const Str& substring,const typename Container<Str,Args...>::allocator_type al)const{
				Container<Str> list(al);
				if(substring.length==0){
					//if(this->length==0) list.emplace_back("",AlTy_traits::select_on_container_copy_construction(alloc));else //asi funcionaria como el split de java
					for(size_type i=0;i<this->length;i++){
						list.emplace_back((*this)[i],AlTy_traits::select_on_container_copy_construction(alloc));
					}
				}
				else{
					size_type start=0,end=0;
					while(start<=this->length){
						end=this->find(substring,start);
						list.push_back(this->substr(start,end));
						start=end+substring.length;
					}					
				}
				return list;
			}
			constexpr size_type max_size()const noexcept{
				return this->maxLen;
			}
			constexpr size_type size()const noexcept{
				return this->length;
			}
            constexpr const_reference back()const noexcept{
                return c_str()[length-1];
            }
            constexpr reference back()noexcept{
                return c_str()[length-1];
            }
			constexpr bool empty()const noexcept{
				return !this->length;
			}
			constexpr bool endswith(const Str& substring)const noexcept{
				if(this->length>=substring.length)
					return equal_n(this->c_str()+this->length-substring.length,substring.c_str(),substring.length);
				else return 0;
			}
			constexpr bool endswith(const value_type* substring)const noexcept{
				size_type substr_size=al::strsize<AlTy>(substring);
				if(this->length>=substr_size)
					return equal_n(this->c_str()+this->length-substr_size,substring,substr_size);
				else return 0;
			}
			constexpr bool startswith(const Str& substring)const noexcept{
				if(this->length>=substring.length)
					return equal_n(this->c_str(),substring.c_str(),substring.length);
				else return 0;
			}
			constexpr bool startswith(const value_type* substring)const noexcept{
				size_type substr_size=al::strsize<AlTy>(substring);
				if(this->length>=substr_size)
					return equal_n(this->c_str(),substring,substr_size);
				else return 0;
			}
			constexpr bool continueswith(const Str& substring,size_type start)const noexcept{
				if(this->length-start>=substring.length)
					return equal_n(this->c_str()+start,substring.c_str(),substring.length);
				else return 0;
			}
			constexpr bool continueswith(const value_type* substring,size_type start)const noexcept{
				size_type substr_size=al::strsize<AlTy>(substring);
				if(this->length-start>=substr_size)
					return equal_n(this->c_str()+start,substring,substr_size);
				else return 0;
			}
			constexpr Str& reverse()noexcept{
				value_type* start = c_str();
				mpv::reverse(start,start+length);
				return *this;
			}
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const{
                for(const_reference val:*this)
                    if(func(val))return 1;
                return 0;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{}){
				for(reference val:*this)
					func(val);
			}
			template<typename Lambda>
			constexpr Str filter(Lambda&& func=Lambda{})const{
				Str new_str(AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_c_str=new_str.text.buffer;
				const value_type* this_array=this->c_str();
				if(this->length>=ss_cap){
					new_str.text.heap=AlTy_traits::allocate(new_str.alloc,this->length+1);
					new_str.maxLen=this->length;
					new_c_str=unfancy(new_str.text.heap);
				}
				for(size_type i=0;i<this->length;i++)
					if(func(this_array[i]))
						new_c_str[new_str.length++]=this_array[i];
				new_c_str[new_str.length]=T();
				return new_str;
			}
			template<typename Uint>
			constexpr enable_if_t<is_unsigned_v<Uint>,Uint> parse()const noexcept(is_scalar_v<Uint>){
				const value_type* array=c_str();
				Uint num=0;
				size_type i=0;
				while(array[i]==' ' || array[i]=='\n' || array[i]=='\r' || array[i]=='\t')i++;
				for(;array[i]>='0' && array[i]<='9';i++){
					num*=10;
					num+=array[i]-48;// 'n'- 48 == n
				}
				return num;
			}
			template<typename Int>
			constexpr enable_if_t<is_signed_v<Int> && !is_floating_point_v<Int>,Int> parse()const noexcept(is_scalar_v<Int>){
				const value_type* array=c_str();
				Int num=0;
				size_type i=0;
				bool neg=false;
				while(array[i]==' ' || array[i]=='\n' || array[i]=='\r' || array[i]=='\t')i++;
				if(array[i]=='-'){
					neg=true;
					i++;
				}else if(array[i]=='+')i++;
				for(;array[i]>='0' && array[i]<='9';i++){
					num*=10;
					num+=array[i]-48;
				}
				return neg ? -num : num;
			}
			template<typename Float>
			constexpr enable_if_t<is_floating_point_v<Float>,Float> parse()const noexcept(is_scalar_v<Float>){
				const value_type* array=c_str();
				Float num=0;
				size_type i=0;
				bool neg=false;
				while(array[i]==' ' || array[i]=='\n' || array[i]=='\r' || array[i]=='\t')i++;
				if(array[i]=='-'){
					neg=true;
					i++;
				}else if(array[i]=='+')i++;
				for(;array[i]>='0' && array[i]<='9';i++){
					num*=10;
					num+=array[i]-48;
				}
				if(array[i]=='.'){i++;
					for(Float div=10;array[i]>='0' && array[i]<='9';i++,div*=10){
						num+=((array[i]-48)/div);
					}		
				}
				if(array[i]=='e' || array[i]=='E'){i++;
					Float e=10;
					unsigned int e_num=0;
					if(array[i]=='-'){
						e=0.1;
						i++;
					}else if(array[i]=='+')i++;
					for(;array[i]>='0' && array[i]<='9';i++){
						e_num*=10;
						e_num+=array[i]-48;
					}
					for(unsigned int j=0;j<e_num;j++){
						num*=e;
					}
				}
				return neg ? -num : num;
			}
			constexpr void clear()noexcept{
				length=0;
				c_str()[0]=T();
			}
			constexpr void free()noexcept{
				if(maxLen!=0) AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
				maxLen=0;
				text.heap=nullptr;
				length=0;
			}
            constexpr allocator_type get_allocator()const noexcept{
                return this->alloc;
            }
			~Str()noexcept{
				if(maxLen!=0) AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
			}
			constexpr iterator begin()noexcept{
				return pointer(c_str());
			}
			constexpr iterator end()noexcept{
				return pointer(c_str()+length);
			}
			constexpr const_iterator begin()const noexcept{
				return const_pointer(c_str());
			}
			constexpr const_iterator end()const noexcept{
				return const_pointer(c_str()+length);
			}
			template<typename t,typename a,typename p> friend Str<t,a,p> constexpr operator+(const typename Str<t,a,p>::value_type*,const Str<t,a,p>&);
			template<typename t,typename a,typename p> friend Str<t,a,p> constexpr operator+(typename Str<t,a,p>::value_type,const Str<t,a,p>&);

			template<typename In>
			void read(In& stream,size_type length){
				this->clear();
				value_type* text;
				if(this->maxLen==0){
					if(length<ss_cap)
						text=this->text.buffer;
					else{
						this->text.heap=AlTy_traits::allocate(this->alloc,length*M+1);
						this->maxLen=length*M;
						text=unfancy(this->text.heap);
					}
				}
				else{
					if(length>this->maxLen){
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->text.heap=AlTy_traits::allocate(this->alloc,length*M+1);
						this->maxLen=length*M;
					}
					text=unfancy(this->text.heap);
				}
				this->length=length;
				stream.read(text,length);
				text[length]=T();
			}

			template<typename In>
			void read(In& stream){
				this->clear();
				size_type current_pos=stream.tellg();
				stream.seekg(0,In::ios_base::end);
				this->length=(size_type)stream.tellg()-current_pos;
				stream.seekg(current_pos);
				value_type* text;
				if(this->maxLen==0){
					if(length<ss_cap)
						text=this->text.buffer;
					else{
						this->text.heap=AlTy_traits::allocate(this->alloc,length*M+1);
						this->maxLen=length*M;
						text=unfancy(this->text.heap);
					}
				}
				else{
					if(length>this->maxLen){
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->text.heap=AlTy_traits::allocate(this->alloc,length*M+1);
						this->maxLen=length*M;
					}
					text=unfancy(this->text.heap);
				}
				stream.read(text,length);
				text[length]=T();
			}
			template<typename In>
			void readtext(In& stream){
				this->clear();
				T c;
				while(stream.get(c))
					this->push_back(c);
			}

	};
	template<typename T,typename Alloc,typename params>
	constexpr Str<T,Alloc,params> operator+(const typename Str<T,Alloc,params>::value_type* c_string,const Str<T,Alloc,params>& str){
		using AlTy=typename Str<T,Alloc,params>::AlTy;
		using AlTy_traits=typename Str<T,Alloc,params>::AlTy_traits;
		auto c_string_size=al::strsize<AlTy>(c_string);
		Str<T,Alloc,params> new_str(c_string_size+str.length,AlTy_traits::select_on_container_copy_construction(str.alloc));
		copy_n(new_str.c_str(),c_string,c_string_size);
		copy_n(new_str.c_str()+c_string_size,str.c_str(),str.length);
		return new_str;
	}
	template<typename T,typename Alloc,typename params>
	constexpr Str<T,Alloc,params> operator+(typename Str<T,Alloc,params>::value_type chr,const Str<T,Alloc,params>& str){
		using AlTy_traits=typename Str<T,Alloc,params>::AlTy_traits;
		Str<T,Alloc,params> new_str(1+str.length,AlTy_traits::select_on_container_copy_construction(str.alloc));
		new_str[0]=chr;
		copy_n(new_str.c_str()+1,str.c_str(),str.length);
		return new_str;
	}
	template<typename Out,typename T,typename Alloc,typename params>
	Out& operator<<(Out& stream,const Str<T,Alloc,params>& string){
		//const typename Str<T,Alloc,params>::value_type* c_string=string.c_str();
		//stream.write(c_string,string.size());
		stream<<string.c_str();
		return stream;
	}
	#ifndef STR_ENDLINE
	#define STR_ENDLINE '\n'
	#endif
	template<typename In,typename T,typename Alloc,typename params,T endchar=STR_ENDLINE>
	In& operator>>(In& stream,Str<T,Alloc,params>& string){
		string.clear();
		T c;
		while(stream.get(c)){
			if(c==endchar)
				break;
			string+=c;
		}
		return stream;
	}
	using String=Str<char>;
	using WString=Str<wchar_t>;
	template<typename Int,typename String_type=String,unsigned short base=10>
	constexpr String_type to_str(Int num){
		String_type new_str,aux;
		if constexpr (mpv::is_signed_v<Int>) {
			if(num<0){
				num=-num;
				new_str='-';
			}
		}
		do{
			aux+=to_char(num%base);
			num/=base;
		}while(num);
		for(auto i=aux.size();i>0;i--)
			new_str+=aux[i-1];
		return new_str;
	}
	inline String operator""_s(const char* s,size_t len){
		return String(s,len);
	}
#undef alloc
#undef maxLen
}

namespace mpv{
	template<typename,typename> class List;
	template<typename,typename> struct List_Node;
	template<typename T,typename VoidPtr>
	struct Base_List_Node{
		using BaseNodePtr=rebind_pointer<VoidPtr,Base_List_Node>;
		using const_BaseNodePtr=rebind_pointer<VoidPtr,const Base_List_Node>;
		using NodePtr=rebind_pointer<VoidPtr,List_Node<T,VoidPtr>>;
		using const_NodePtr=rebind_pointer<VoidPtr,const List_Node<T,VoidPtr>>;
		NodePtr next;
		NodePtr prev;
		constexpr void link_right(NodePtr other)noexcept{//links other to the right of this
			this->next->prev=other;
			other->next=this->next;
			this->next=other;
			other->prev=NodePtr(this);
		}
		constexpr void link_left(NodePtr other)noexcept{//links other to the left of this
			this->prev->next=other;
			other->prev=this->prev;
			this->prev=other;
			other->next=NodePtr(this);
		}
		constexpr void unlink()noexcept{
			this->prev->next=this->next;
			this->next->prev=this->prev;
		}
		constexpr NodePtr address()noexcept{
			return NodePtr(reinterpret_cast<List_Node<T,VoidPtr>*>(this));
		}
		constexpr const_NodePtr address()const noexcept{
			return const_NodePtr(reinterpret_cast<const List_Node<T,VoidPtr>*>(this));
		}
	};
	template<typename T,typename VoidPtr>
	struct List_Node:public Base_List_Node<T,VoidPtr>{
		T data;
		template<typename... Args>
		constexpr List_Node(Args&&... args)noexcept(is_nothrow_constructible_v<T,Args...>):data(static_cast<Args&&>(args)...){}
		List_Node(const List_Node&)=delete;
		List_Node& operator=(const List_Node&)=delete;
	};
	template<typename Types>
	class List_iterator{
		template<typename,typename> friend class List;
		public:
			using iterator_category=bidirectional_iterator_tag;
			using NodePtr=typename Types::NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=value_type&;
		private:
			NodePtr ptr;
		public:
			constexpr List_iterator(NodePtr ptr)noexcept:ptr(ptr){}
			constexpr reference operator*()const noexcept{
				return ptr->data;
			}
			constexpr pointer operator->()const noexcept{
				return pointer_traits<pointer>::pointer_to(ptr->data);
			}
			constexpr List_iterator& operator++()noexcept{
				ptr=ptr->next;
				return *this;
			}
			constexpr List_iterator operator++(int)noexcept{
				List_iterator aux=*this;
				ptr=ptr->next;
				return aux;
			}
			constexpr List_iterator& operator--()noexcept{
				ptr=ptr->prev;
				return *this;
			}
			constexpr List_iterator operator--(int)noexcept{
				List_iterator aux=*this;
				ptr=ptr->prev;
				return aux;
			}
			constexpr List_iterator operator+(difference_type n)const noexcept{
				List_iterator ret(this->ptr);
				advance(ret,n);
				return ret;
			}
			constexpr List_iterator operator-(difference_type n)const noexcept{
				List_iterator ret(this->ptr);
				advance(ret,-n);
				return ret;
			}
			constexpr List_iterator& operator+=(difference_type n)noexcept{
				advance(*this,n);
				return *this;
			}
			constexpr List_iterator& operator-=(difference_type n)noexcept{
				advance(*this,-n);
				return *this;
			}
			constexpr bool operator==(const List_iterator& other)const noexcept{
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const List_iterator& other)const noexcept{
				return this->ptr!=other.ptr;
			}
			template<typename> friend class const_List_iterator;
	};
	template<typename Types>
	class const_List_iterator{
		template<typename,typename> friend class List;
		public:
			using iterator_category=bidirectional_iterator_tag;
			using NodePtr=typename Types::const_NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
		private:
			NodePtr ptr;
		public:
			constexpr const_List_iterator(NodePtr ptr)noexcept:ptr(ptr){}
			constexpr const_List_iterator(List_iterator<Types> nonconst_it)noexcept:ptr(nonconst_it.ptr){}
			constexpr reference operator*()const noexcept{
				return ptr->data;
			}
			constexpr pointer operator->()const noexcept{
				return pointer_traits<pointer>::pointer_to(ptr->data);
			}
			constexpr const_List_iterator& operator++()noexcept{
				ptr=ptr->next;
				return *this;
			}
			constexpr const_List_iterator operator++(int)noexcept{
				const_List_iterator aux=*this;
				ptr=ptr->next;
				return aux;
			}
			constexpr const_List_iterator& operator--()noexcept{
				ptr=ptr->prev;
				return *this;
			}
			constexpr const_List_iterator operator--(int)noexcept{
				const_List_iterator aux=*this;
				ptr=ptr->prev;
				return aux;
			}
			constexpr const_List_iterator operator+(difference_type n)const noexcept{
				const_List_iterator ret(this->ptr);
				advance(ret,n);
				return ret;
			}
			constexpr const_List_iterator operator-(difference_type n)const noexcept{
				const_List_iterator ret(this->ptr);
				advance(ret,-n);
				return ret;
			}
			constexpr const_List_iterator& operator+=(difference_type n)noexcept{
				advance(*this,n);
				return *this;
			}
			constexpr const_List_iterator& operator-=(difference_type n)noexcept{
				advance(*this,-n);
				return *this;
			}
			constexpr bool operator==(const const_List_iterator& other)const noexcept{
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const const_List_iterator& other)const noexcept{
				return this->ptr!=other.ptr;
			}
	};
	template<typename T,typename Alloc=allocator<T>>
	class List COUNT_IT{
		private:
			using BaseNode=Base_List_Node<T,typename allocator_traits<Alloc>::void_pointer>;
			using Node=List_Node<T,typename allocator_traits<Alloc>::void_pointer>;
			using AlTy=rebind_alloc<Alloc,T>;
			using AlTy_traits=allocator_traits<AlTy>;
			using AlNode=rebind_alloc<Alloc,Node>;
			using AlNode_traits=allocator_traits<AlNode>;			
			using NodePtr=typename AlNode_traits::pointer;
			using const_NodePtr=typename AlNode_traits::const_pointer;

			struct Val_types{
				using value_type=T;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=T&;
				using const_reference=const T&;
				using NodePtr=typename AlNode_traits::pointer;
				using const_NodePtr=typename AlNode_traits::const_pointer;
			};
		public:
            static constexpr bool POCCA=AlTy_traits::propagate_on_container_copy_assignment::value;
            static constexpr bool POCMA=AlTy_traits::propagate_on_container_move_assignment::value;
            static constexpr bool POCS=AlTy_traits::propagate_on_container_swap::value;
            static constexpr bool ALWAYS_EQ=AlTy_traits::is_always_equal::value;
			using allocator_type=Alloc;
			using value_type=typename Val_types::value_type;
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
			using iterator=List_iterator<Val_types>;
			using const_iterator=const_List_iterator<Val_types>;
			using move_iterator=move_iterator<iterator>;
		private:
#define HEAD cp.V2.next
#define TAIL cp.V2.prev
#define NULLPTR cp.V2.address()
#define alloc cp.getV1()
			CompressedPair<AlNode,BaseNode> cp;
			size_type length=0;
			template<typename... Args>
			constexpr NodePtr create_node(Args&&... args){
                AllocConstructPtr guard(alloc);
                guard.allocate();
				CONSTRUCT_VARARGS(this->alloc,guard.ptr,static_cast<Args&&>(args));
				guard.ptr->next=guard.ptr->prev=NULLPTR;
				return guard.release();
			}
			constexpr void delete_node(NodePtr node)noexcept{
				/* AlNode_traits::destroy */DESTROY(this->alloc,node);
				AlNode_traits::deallocate(this->alloc,node,1);
			}
			constexpr void destroy_and_free()noexcept{
				NodePtr aux;
				TAIL=NULLPTR;
				while(HEAD!=NULLPTR){
					aux=HEAD;
					HEAD=HEAD->next;
					/* AlNode_traits::destroy */DESTROY(this->alloc,aux);
					AlNode_traits::deallocate(this->alloc,aux,1);
				}//TAIL and HEAD en up pointing to NULLPTR
			}
			constexpr void reset()noexcept{
				HEAD=TAIL=NULLPTR;
				length=0;
			}
			constexpr void set_null_sides()noexcept{
				this->HEAD->prev=this->TAIL->next=this->NULLPTR;
			}
			constexpr NodePtr get_pos(size_type index){
				NodePtr p=HEAD;
				for(size_type i=0;i<index;i++) p=p->next;
				return p;
			}
			constexpr void link_back(NodePtr node)noexcept{
				TAIL->next=node;// node is allready pointing to null when created
				TAIL->next->prev=TAIL;
				TAIL=TAIL->next;
			}
			constexpr void link_front(NodePtr node)noexcept{
				HEAD->prev=node;
				HEAD->prev->next=HEAD;
				HEAD=HEAD->prev;
			}
			constexpr void link_at(size_type index,NodePtr node)noexcept{
				NodePtr p=HEAD;
				for(size_type i=0;i<index;i++) p=p->next;
				p->link_left(node);
			}
			constexpr NodePtr unlink_back()noexcept{
				NodePtr p=TAIL;
				p->unlink();
				return p;
			}
			constexpr NodePtr unlink_at(size_type index)noexcept{
				NodePtr p=HEAD;
				for(size_type i=0;i<index;i++) p=p->next;
				p->unlink();
				return p;
			}
/*	Cada una de las funciones que siguen asume que recibe una lista valida en la que length coincide con 
	la actual cantidad de nodos  en la lista, sus nodos correctamente enlazados y apuntando a null 
	en caso de que size sea 0. 
	Modifican la lista this, dejan length coincidiendo con la cantidad de nodos y en caso de que modifiquen la
	segunda lista(porque la primera lista se apropia de los elementos de la segunda), se encargan de que esta
	siga siendo valida, dejando length en 0 y haciendo que HEAD y TAIL apunten a other.null

*/
			constexpr void copy_elements(const List& other){
				Guard guard(this->alloc,other.begin(),other.end());
				this->length=guard.transfer_at(HEAD);
			}
			constexpr void copy_elements(List&& other){
				Guard guard(this->alloc,move_iterator(other.begin()),move_iterator(other.end()));
				this->length=guard.transfer_at(HEAD);
			}
			constexpr void move_elements(List& other)noexcept{
				this->length=other.length;
				if(other.length){
					this->HEAD=other.HEAD;
					this->TAIL=other.TAIL;
					this->set_null_sides();
					other.reset();					
				}
				else{
					this->HEAD=this->NULLPTR;
					this->TAIL=this->NULLPTR;
				}
			}
			constexpr void copy_elements_keeping_this_allocation(const List& other){
				if(this->length>=other.length){
					NodePtr i=this->HEAD, j=other.HEAD;
					while(j!=other.NULLPTR){
						this->TAIL=i;
						i->data=j->data;
						i=i->next;
						j=j->next;
					}
					this->TAIL=i->prev;
					TAIL->next=this->NULLPTR;
					while(i!=this->NULLPTR){
						NodePtr aux=i;
						i=i->next;
						this->delete_node(aux);
					}
					this->length=other.length;
				}
				else{
					NodePtr i=this->HEAD, j=other.HEAD;
					while(i!=this->NULLPTR){
						i->data=j->data;
						i=i->next;
						j=j->next;
					}
					Guard guard(this->alloc,const_iterator(j),other.end());
					this->length+=guard.transfer_at(this->NULLPTR);
				}
			}
			constexpr void copy_elements_keeping_this_allocation(List&& other){
				if(this->length>=other.length){
					NodePtr i=this->HEAD, j=other.HEAD;
					while(j!=other.NULLPTR){
						this->TAIL=i;
						i->data=static_cast<value_type&&>(j->data);
						i=i->next;
						j=j->next;
					}
					this->TAIL=i->prev;
					TAIL->next=this->NULLPTR;
					while(i!=this->NULLPTR){
						NodePtr aux=i;
						i=i->next;
						this->delete_node(aux);
					}
					this->length=other.length;
					other.clear();
				}
				else{
					NodePtr i=this->HEAD, j=other.HEAD;
					while(i!=this->NULLPTR){
						i->data=static_cast<value_type&&>(j->data);
						i=i->next;
						j=j->next;
					}
					Guard guard(this->alloc,move_iterator(iterator(j)),move_iterator(other.end()));
					this->length+=guard.transfer_at(this->NULLPTR);
				}
			}
			constexpr void add_back(const List& other){
				Guard guard(this->alloc,other.begin(),other.end());
				this->length+=guard.transfer_at(this->NULLPTR);
			}
			constexpr void add_back(List&& other){
				Guard guard(this->alloc,move_iterator(other.begin()),move_iterator(other.end()));
				this->length+=guard.transfer_at(this->NULLPTR);
			}
			constexpr void move_back(List& other)noexcept{
				this->length+=other.length;
				this->TAIL->next=other.HEAD;
				other.HEAD->prev=this->TAIL;
				this->TAIL=other.TAIL;
				this->TAIL->next=this->NULLPTR;
				other.reset();
			}
			constexpr void add_front(const List& other){
				Guard guard(this->alloc,other.begin(),other.end());
				this->length+=guard.transfer_at(this->HEAD);
			}
			constexpr void add_front(List&& other){
				Guard guard(this->alloc,move_iterator(other.begin()),move_iterator(other.end()));
				this->length+=guard.transfer_at(this->HEAD);
			}
			constexpr void move_front(List& other)noexcept{
				this->length+=other.length;
				this->HEAD->prev=other.TAIL;
				other.TAIL->next=this->HEAD;
				this->HEAD=other.HEAD;
				this->HEAD->prev=this->NULLPTR;
				other.reset();
			}
			constexpr void move_at(NodePtr p,List& other)noexcept{
				this->length+=other.length;
				p->prev->next=other.HEAD;
				other.HEAD->prev=p->prev;
				p->prev=other.TAIL;
				other.TAIL->next=p;
				other.reset();
			}
			struct Guard{
				AlNode& al;
				size_type length=0;
				NodePtr head=nullptr,tail=nullptr;
				template<typename U>
				constexpr NodePtr create_node(U&& val){
					AllocConstructPtr guard(al);
					guard.allocate();
					CONSTRUCT(this->al,guard.ptr,static_cast<U&&>(val));
					guard.ptr->next=guard.ptr->prev=nullptr;
					return guard.release();
				}
				constexpr Guard(AlNode& al)noexcept:al(al){}
				template<typename It>
				constexpr Guard(AlNode& al,It first,It last):al(al){
					Guard guard(al);
					if(first!=last){
						guard.head=guard.tail=guard.create_node(*first++);
						guard.length=1;
						while(first!=last){
							guard.tail->next=guard.create_node(*first++);
							guard.tail->next->prev=guard.tail;
							guard.tail=guard.tail->next;
							++guard.length;
						}
						this->tail=guard.tail;
						this->head=guard.head;
						this->length=guard.length;
						guard.head=nullptr;
					}
				}
				constexpr Guard(AlNode& al,size_type sz,const value_type& fillwith=value_type{}):al(al){
					Guard guard(al);
					if(guard.length<sz){
						guard.head=guard.tail=guard.create_node(fillwith);
						guard.length=1;
						while(guard.length<sz){
							guard.tail->next=guard.create_node(fillwith);
							guard.tail->next->prev=guard.tail;
							guard.tail=guard.tail->next;
							++guard.length;
						}
						this->tail=guard.tail;
						this->head=guard.head;
						this->length=guard.length;
						guard.head=nullptr;
					}
				}
				constexpr size_type transfer_at(NodePtr p)noexcept{
					if(length!=0){
						p->prev->next=head;
						head->prev=p->prev;
						p->prev=tail;
						tail->next=p;
						head=nullptr;
					}
					return length;
				}
				~Guard()noexcept{
					while(head!=nullptr){
						NodePtr aux=head;
						head=head->next;
						DESTROY(al,aux);
						AlNode_traits::deallocate(al,aux,1);
					}
				}
			};
		 public:
			constexpr List()noexcept{
				HEAD=NULLPTR;
				TAIL=NULLPTR;
			}
			constexpr explicit List(const Alloc& al)noexcept:cp(arg1_tag{},al){
				HEAD=NULLPTR;
				TAIL=NULLPTR;
			}
			constexpr List(const List& other):cp(arg1_tag{},AlNode_traits::select_on_container_copy_construction(other.alloc)){
				HEAD=NULLPTR;
				TAIL=NULLPTR;
				Guard guard(this->alloc,other.begin(),other.end());
				length=guard.transfer_at(HEAD);
			}
			constexpr List(const List& other,const Alloc& al):cp(arg1_tag{},al){
				HEAD=NULLPTR;
				TAIL=NULLPTR;
				Guard guard(this->alloc,other.begin(),other.end());
				length=guard.transfer_at(HEAD);
			}
			constexpr List(List&& other)noexcept:cp(arg1_tag{},static_cast<AlNode&&>(other.alloc)),length(other.length){
				if(other.length){
					HEAD=other.HEAD;
					TAIL=other.TAIL;
					this->set_null_sides();
					other.reset();					
				}
				else{
					HEAD=NULLPTR;
					TAIL=NULLPTR;
				}
			}
			constexpr List(List&& other,const Alloc& al)noexcept(ALWAYS_EQ):cp(arg1_tag{},al),length(other.length){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				if(this->alloc==other.alloc){
					if(other.length){
						this->HEAD=other.HEAD;
						this->TAIL=other.TAIL;
						this->set_null_sides();
						other.reset();						
					}
				}
				else{
					this->copy_elements(static_cast<List&&>(other));
				}
			}
			constexpr explicit List(size_type length,const Alloc& al=Alloc()):cp(arg1_tag{},al){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				Guard guard(this->alloc,length);
				this->length=guard.transfer_at(HEAD);
			}
			constexpr List(size_type length,const value_type& fillwith,const Alloc& al=Alloc()):cp(arg1_tag{},al){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				Guard guard(this->alloc,length,fillwith);
				this->length=guard.transfer_at(HEAD);
			}
			template<typename It, enable_if_t<is_iterator_v<It>,int> = 0>
			constexpr List(It first,It last,const Alloc& al=Alloc{}):cp(arg1_tag{},al){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				Guard guard(this->alloc,first,last);
				length=guard.transfer_at(HEAD);
			}
			constexpr List(std::initializer_list<value_type> initlist){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				Guard guard(this->alloc,initlist.begin(),initlist.end());
				length=guard.transfer_at(HEAD);
			}
			constexpr List(std::initializer_list<value_type> initlist,const Alloc& al):cp(arg1_tag{},al){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				Guard guard(this->alloc,initlist.begin(),initlist.end());
				length=guard.transfer_at(HEAD);
			}
			constexpr List& operator=(const List& other){										//Operador de copia(optimizado)
				if(this!=&other){
					if constexpr(!ALWAYS_EQ && POCCA){
						if(this->alloc!=other.alloc){
							this->destroy_and_free();
							this->alloc=other.alloc;
							this->copy_elements(other);
							return *this;
						}
					}
					this->copy_elements_keeping_this_allocation(other);
				}
				return *this;
			}
			constexpr List& operator=(List&& other)noexcept(ALWAYS_EQ || POCMA){												//Operador de movimiento
				if(this==&other) return *this;
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->alloc!=other.alloc){
						this->copy_elements_keeping_this_allocation(static_cast<List&&>(other));
						return *this;
					}
				}
				this->destroy_and_free();
				if constexpr(POCMA) this->alloc=static_cast<AlNode&&>(other.alloc);
				this->move_elements(other);
				return *this;
			}
			constexpr List operator+(const List& other)const&{
				List new_list(*this);
				new_list.add_back(other);
				return new_list;
			}
			constexpr List operator+(List&& other)const&{
				List new_list(static_cast<List&&>(other));
				new_list.add_front(*this);
				return new_list;
			}
			constexpr List operator+(const List& other)&&{
				List new_list(static_cast<List&&>(*this));
				new_list.add_back(other);
				return new_list;
			}
			constexpr List operator+(List&& other)&& noexcept(ALWAYS_EQ){
				List new_list(static_cast<List&&>(*this));
				if constexpr(!ALWAYS_EQ){
					if(new_list.alloc==other.alloc) new_list.move_back(other);
					else new_list.add_back(static_cast<List&&>(other));
				}
				else new_list.move_back(other);
				return new_list;
			}
			constexpr List& operator+=(const List& other){
				this->add_back(other);
				return *this;
			}
			constexpr List& operator+=(List&& other)noexcept(ALWAYS_EQ){
				if constexpr(ALWAYS_EQ){
					this->move_back(other);
				}
				else{
					if(this->alloc==other.alloc) this->move_back(other);
					else this->add_back(static_cast<List&&>(other));
				}
				return *this;
			}
			constexpr List operator*(size_type num)const{
				List new_list(AlNode_traits::select_on_container_copy_construction(alloc));
				for(size_type i=0;i<num;i++)
					new_list+=*this;
				return new_list;
			}
			constexpr List& operator*=(size_type num){
				if(length==0 || num==1)
					return *this;
				else if(num==0){
					clear();
					return *this;
				}
				else{
					Guard guard(alloc,begin(),end());
					for(size_type i=2;i<num;i++){
						NodePtr p=HEAD;
						for(size_type j=0;j<length;j++){
							guard.tail->next=guard.create_node(p->data);
							guard.tail->next->prev=guard.tail;
							guard.tail=guard.tail->next;
							p=p->next;
							++guard.length;
						}
					}
					length+=guard.transfer_at(NULLPTR);
					return *this;					
				}
			}
			template<typename... Args>
			constexpr void emplace_back(Args&&... args){
				link_back(create_node(static_cast<Args&&>(args)...));
				++length;
			}
			constexpr void push_back(const value_type& val){
				link_back(create_node(val));
				++length;
			}
			constexpr void push_back(value_type&& val){
				link_back(create_node(static_cast<value_type&&>(val)));
				++length;
			}
			template<typename... Args>
			constexpr iterator emplace(const_iterator pos,Args&&... args){
				NodePtr p=create_node(static_cast<Args&&>(args)...);
				const_cast<NodePtr>(pos.ptr)->link_left(p);
				++length;
				return iterator(p);
			}
			constexpr iterator insert(const_iterator pos,const value_type& val){
				NodePtr p=create_node(val);
				const_cast<NodePtr>(pos.ptr)->link_left(p);
				++length;
				return iterator(p);
			}
			constexpr iterator insert(const_iterator pos,value_type&& val){
				NodePtr p=create_node(static_cast<value_type&&>(val));
				const_cast<NodePtr>(pos.ptr)->link_left(p);
				++length;
				return iterator(p);
			}
            template<typename It>
            constexpr enable_if_t<is_iterator_v<It>,iterator> insert(const_iterator pos,It first,It last){
                Guard guard(alloc,first,last);
				if(guard.head==nullptr) return iterator(const_cast<NodePtr>(pos.ptr));
				else{
					NodePtr p=guard.head;
					this->length+=guard.transfer_at(const_cast<NodePtr>(pos.ptr));
					return iterator(p);
				}
            }
            constexpr iterator insert(const_iterator pos,const size_type count,const T& val){
                Guard guard(alloc,count,val);
				if(guard.head==nullptr) return iterator(const_cast<NodePtr>(pos.ptr));
				else{
					NodePtr p=guard.head;
					this->length+=guard.transfer_at(const_cast<NodePtr>(pos.ptr));
					return iterator(p);
				}
            }
			template<typename... Args>
			constexpr iterator emplace_at(size_type index,Args&&... args){
				return emplace(begin()+index,static_cast<Args&&>(args)...);
			}
			constexpr iterator insert_at(size_type index,const value_type& val){
				return insert(begin()+index,val);
			}
			constexpr iterator insert_at(size_type index,value_type&& val){
				return insert(begin()+index,static_cast<value_type&&>(val));
			}
            template<typename It>
            constexpr enable_if_t<is_iterator_v<It>,iterator> insert_at(size_type index,It first,It last){
                return insert(begin()+index,first,last);
            }
            constexpr iterator insert_at(size_type index,const size_type count,const T& val){
                return insert(begin()+index,count,val);
            }
			constexpr value_type pop(const_iterator pos)noexcept(is_nothrow_move_constructible_v<value_type>){
				--length;
				NodePtr p=const_cast<NodePtr>(pos.ptr);
				p->unlink();
				value_type aux=static_cast<value_type&&>(p->data);
				delete_node(p);
				return aux;
			}
			constexpr void del(const_iterator pos)noexcept{
				--length;
				const_cast<NodePtr>(pos.ptr)->unlink();
				delete_node(const_cast<NodePtr>(pos.ptr));
			}
			constexpr value_type pop_at(size_type index)noexcept(is_nothrow_move_constructible_v<value_type>){
				--length;
				NodePtr p=unlink_at(index);
				value_type aux=static_cast<value_type&&>(p->data);
				delete_node(p);
				return aux;
			}
			constexpr void del_at(size_type index)noexcept{
				--length;
				delete_node(unlink_at(index));
			}
			constexpr value_type pop_back()noexcept(is_nothrow_move_constructible_v<value_type>){
				--length;
				NodePtr p=unlink_back();
				value_type aux=static_cast<value_type&&>(p->data);
				delete_node(p);
				return aux;
			}
			void del_back()noexcept{
				--length;
				delete_node(unlink_back());
			}
			constexpr List concat(const_iterator pos,const List& other)const&{
				List new_list(this->begin(),pos,AlNode_traits::select_on_container_copy_construction(alloc));
				new_list.insert(new_list.end(),other.begin(),other.end());
				new_list.insert(new_list.end(),pos,this->end());
				return new_list;
			}
			constexpr List concat(const_iterator pos,List&& other)const&{
				List new_list(static_cast<List&&>(other));
				new_list.insert(new_list.begin(),this->begin(),pos);
				new_list.insert(new_list.end(),pos,this->end());
				return new_list;
			}
			constexpr List concat(const_iterator pos,const List& other)&&{
				this->insert(pos,other.begin(),other.end());
				return List(static_cast<List&&>(*this));
			}
			constexpr List concat(const_iterator pos,List&& other)&& noexcept(ALWAYS_EQ){
				this->join(pos,static_cast<List&&>(other));
				return List(static_cast<List&&>(*this));
			}
			constexpr void join(const_iterator pos,const List& other){
				this->insert(pos,other.begin(),other.end());
			}
			constexpr void join(const_iterator pos,List&& other)noexcept(ALWAYS_EQ){
				if constexpr(!ALWAYS_EQ){
					if(this->alloc!=other.alloc){
						Guard guard(this->alloc,move_iterator(other.begin()),move_iterator(other.end()));
						length+=guard.transfer_at(const_cast<NodePtr>(pos.ptr));
						return;
					}
				}
				move_at(const_cast<NodePtr>(pos.ptr),other);
			}
			constexpr List sublist(const_iterator first,const_iterator last)const&{
				return List(first,last,AlNode_traits::select_on_container_copy_construction(alloc));
			}
			constexpr List sublist(const_iterator first,const_iterator last)&& noexcept{
				if(first==last) return {};
				wipe(last,end());
				wipe(begin(),first);
				return List(static_cast<List&&>(*this));
			}
			constexpr List& wipe(const_iterator first,const_iterator last)noexcept{
				NodePtr fst=const_cast<NodePtr>(first.ptr),lst=const_cast<NodePtr>(last.ptr);
				fst->prev->next=lst;
				lst->prev=fst->prev;
				while(fst!=lst){
					--length;
					NodePtr aux=fst;
					fst=fst->next;
					delete_node(aux);
				}
				return *this;
			}
			constexpr List cut(const_iterator first,const_iterator last)& noexcept(ALWAYS_EQ){
				List new_list(AlNode_traits::select_on_container_copy_construction(alloc));
				if(first==last) return new_list;
				NodePtr fst=const_cast<NodePtr>(first.ptr), lst=const_cast<NodePtr>(last.ptr);
				if constexpr(!ALWAYS_EQ){
					if(this->alloc!=new_list.alloc){
						fst->prev->next=lst;
						lst->prev=fst->prev;
						this->length-=distance(first,last);
						new_list.insert(new_list.begin(),move_iterator(iterator(fst)),move_iterator(iterator(lst)));
						while(fst!=lst){
							NodePtr aux=fst;
							fst=fst->next;
							delete_node(aux);
						}
						return new_list;
					}
				}
				new_list.length=distance(first,last);
				this->length-=new_list.length;
				new_list.HEAD=fst;
				new_list.TAIL=lst->prev;
				fst->prev->next=lst;
				lst->prev=fst->prev;
				new_list.set_null_sides();
				return new_list;
			}
			constexpr List cut(const_iterator first,const_iterator last)&& noexcept{
				return static_cast<List&&>(*this).sublist(first,last);
			}
			constexpr size_type size()const noexcept{
				#if defined(DEBUG) && (defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_))
				size_type node_count=0,node_count_reverse=0;
				for(NodePtr current=this->HEAD;current!=this->NULLPTR;current=current->next){
					node_count++;
				}
				for(NodePtr current=this->TAIL;current!=this->NULLPTR;current=current->prev){
					node_count_reverse++;
				}
				if(node_count!=node_count_reverse){
					std::cerr<<"\nNODE_COUNT AND NODE_COUNT_REVERSE DO NOT MATCH\a\n"
							<<"NODE_COUNT: "<<node_count<<"\nNODE_COUNT_REVERSE: "<<node_count_reverse<<std::endl;
					exit(-1);
				}
				if(node_count!=length){
					std::cerr<<"\nSIZE AND NODE_COUNT DO NOT MATCH\a\n"
							<<"LEN: "<<length<<"\nNODE_COUNT: "<<node_count<<std::endl;
					exit(-1);
				}
				#endif
				return length;
			}
			constexpr bool remove(const T& val){
				NodePtr current=this->HEAD;
				while(current!=NULLPTR){
					if(current->data==val){
						current->unlink();
						delete_node(current);
						this->length--;
						return true;
					}
					else
						current=current->next;
				}
				return false;
			}
			constexpr size_type remove_all(const T& val){	//devuelve la cantidad removida
				NodePtr aux, current=this->HEAD;
				size_type found=0;
				while(current!=NULLPTR){
					if(current->data==val){
						aux=current;
						current=current->next;
						aux->unlink();
						delete_node(aux);
						this->length--;
						found++;
					}
					else
						current=current->next;
				}
				return found;
			}
			constexpr void resize(size_type new_size){
				while(this->length<new_size){
					Guard guard(alloc,new_size-this->length);
					this->length+=guard.transfer_at(NULLPTR);
				}
				while(this->length>new_size){
					delete_node(unlink_back());
					--length;
				}
			}
            constexpr const_iterator find(const T& val)const{
                return mpv::find(begin(),end(),val);
            }
            constexpr iterator find(const T& val){
                return mpv::find(begin(),end(),val);
            }
            constexpr size_type count(const T& val)const{
                return mpv::count<const_iterator,T,size_type>(begin(),end(),val);
            }
			constexpr bool contains(const T& val)const{
				return mpv::contains<const_iterator,T>(begin(),end(),val);
			}
            constexpr size_type index_of(const T& val)const{
                return mpv::index_of<const_iterator,T,size_type>(begin(),end(),val);
            }
			template<typename Pred=less<>>
			constexpr void sort(Pred pred=Pred{}){
				mpv::insertion_sort(begin(),end(),pred);
			}
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const{
                for(const_reference x:*this)
                    if(func(x))return 1;
                return 0;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{}){
				for(reference x:*this)
					func(x);
			}
			template<typename Lambda>
			constexpr auto map(Lambda&& func=Lambda{})const{
				List<decltype(func(declval<const_reference>())),rebind_alloc<Alloc,decltype(func(declval<const_reference>()))>> new_list;
				for(const_reference x:*this)
					new_list.push_back(func(x));
				return new_list;
			}
			template<typename Lambda>
			constexpr List filter(Lambda&& func=Lambda{})const{
				List new_list(AlNode_traits::select_on_container_copy_construction(alloc));
				for(const_reference x:*this)
					if(func(x)) new_list.push_back(x);
				return new_list;
			}
			constexpr List& reverse(){
				mpv::reverse(begin(),end());
				return *this;
			}
			constexpr bool empty()const noexcept{
				return length==0;
			}
			constexpr const_reference operator[](size_type index)const noexcept{
				NodePtr current=this->HEAD;
				for(size_type i=0;i<index;i++)
					current=current->next;
				return current->data;
			}
			constexpr reference operator[](size_type index)noexcept{
				NodePtr current=this->HEAD;
				for(size_type i=0;i<index;i++)
					current=current->next;
				return current->data;
			}
			constexpr const_reference back()const noexcept{
				return TAIL->data;
			}
			constexpr reference back()noexcept{
				return TAIL->data;
			}
			constexpr bool operator==(const List& other)const{
				if(this->length!=other.length) return false;
				else return equal(this->begin(),this->end(),other.begin());
			}
			constexpr bool operator!=(const List& other)const{
				if(this->length!=other.length) return true;
				else return !equal(this->begin(),this->end(),other.begin());
			}
            constexpr bool operator<(const List& other)const{
                return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool operator>(const List& other)const{
                return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator<=(const List& other)const{
                return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator>=(const List& other)const{
                return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
			constexpr void clear()noexcept{
				NodePtr aux;
				length=0;
				TAIL=NULLPTR;
				while(HEAD!=NULLPTR){
					aux=HEAD;
					HEAD=HEAD->next;
					delete_node(aux);
				}
			}
            constexpr allocator_type get_allocator()const noexcept{
                return this->alloc;
            }
			~List()noexcept{
#if defined(DEBUG) && (defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_))
				(void)size();
#endif
				NodePtr aux;
				while(HEAD!=NULLPTR){
					aux=HEAD;
					HEAD=HEAD->next;
					delete_node(aux);
				}
			}
			constexpr iterator begin()noexcept{
				return NodePtr(HEAD);
			}
			constexpr iterator end()noexcept{
				return NodePtr(NULLPTR);
			}
			constexpr const_iterator begin()const noexcept{
				return const_NodePtr(HEAD);
			}
			constexpr const_iterator end()const noexcept{
				return const_NodePtr(NULLPTR);
			}
			template<typename t,typename Alloc_>friend void debug(const List<t,Alloc_>&);
	};
	template<typename Out,typename T,typename Alloc>
	Out& operator<<(Out& stream,const List<T,Alloc>& list){
		stream<<"[";
		typename List<T,Alloc>::const_iterator last=list.end();
		--last;
		for(typename List<T,Alloc>::const_iterator i=list.begin();i!=list.end();i++){
			stream<<""<<*i;
			if(i!=last)
				stream<<", ";		
		}
		stream<<"]";
		return stream;
	}
#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
    template<typename T,typename Alloc>
    void debug(const List<T,Alloc>& list){
        std::cout<<"length: "<<list.size()<<std::endl
                 <<"head: "<<list.HEAD<<std::endl
                 <<"tail: "<<list.TAIL<<std::endl;
    }
#endif
#undef HEAD
#undef TAIL
#undef NULLPTR
#undef alloc
}

#ifndef VECTOR_MMS 
#define VECTOR_MMS 16 //min max_size
#endif
namespace mpv{
    template<typename T,typename Alloc=allocator<T>,typename realloc_params=params<typename allocator_traits<Alloc>::template rebind_traits<T>::size_type,VECTOR_MMS,2>>
    class Vector COUNT_IT {static_assert(realloc_params::p1>0,"min_maxLen cannot be less than 1");
        private:
            using AlTy=rebind_alloc<Alloc,T>;
            using AlTy_traits=allocator_traits<AlTy>;
			struct Val_types{
				using value_type=T;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=T&;
				using const_reference=const T&;
			};
            static constexpr typename Val_types::size_type min_maxLen=realloc_params::p1;
            static constexpr typename Val_types::size_type realloc_factor=realloc_params::p2;
            static constexpr bool POCCA=AlTy_traits::propagate_on_container_copy_assignment::value;
            static constexpr bool POCMA=AlTy_traits::propagate_on_container_move_assignment::value;
            static constexpr bool POCS=AlTy_traits::propagate_on_container_swap::value;
            static constexpr bool ALWAYS_EQ=AlTy_traits::is_always_equal::value;
        public:
			using allocator_type=Alloc;
			using value_type=typename Val_types::value_type;
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
            using iterator=random_access_iterator<Val_types>;
            using const_iterator=const_random_access_iterator<Val_types>;
            // using iterator=pointer;
            // using const_iterator=const_pointer;

        private:
            CompressedPair<AlTy,size_type> cp=CompressedPair<AlTy,size_type>(0);
            size_type length=0;
            pointer array=nullptr;

#define alloc cp.getV1()
#define maxLen cp.V2

            struct Guard{
                AlTy& al;
                size_type length=0,max_length;
                pointer array=nullptr;
                constexpr Guard(AlTy& al,size_type max_length):al(al),max_length(max_length),array(max_length>0? AlTy_traits::allocate(al,max_length) : nullptr){}
                constexpr Guard(AlTy& al,const Vector& v):al(al),max_length(v.length*realloc_factor),array(max_length>0? AlTy_traits::allocate(al,max_length) : nullptr){}
                constexpr void transfer_to(Vector& v)noexcept{
                    v.array=this->array;
                    v.maxLen=this->max_length;
                    v.length=this->length;
                    this->array=nullptr;
                }
                ~Guard()noexcept{
                    if(array){
                        destroy_n(al,array,length);
                        AlTy_traits::deallocate(al,array,max_length);
                    }
                }
                Guard(const Guard&)=delete;
                Guard& operator=(const Guard&)=delete;
            };
            constexpr Vector(reserve_tag,size_type max_length,const Alloc& al):cp(arg1_tag{},al,max_length),array(max_length>0? AlTy_traits::allocate(alloc,max_length) : nullptr){}
            constexpr void destroy_and_free()noexcept{
                if(array){
                    destroy_n(this->alloc,array,length);
                    AlTy_traits::deallocate(this->alloc,array,maxLen);                    
                }
            }
            constexpr void reset()noexcept{
                this->length=0;
                this->maxLen=0;
                this->array=nullptr;
            }
            constexpr void take_content(Vector& other)noexcept{
                this->length=other.length;
                this->maxLen=other.maxLen;
                this->array=other.array;
                other.reset();
            }
            template<typename It>
            constexpr void allocate_and_assign_counted_range(It first,size_type count){// assumes *this has no storage
                Guard guard(alloc,count*realloc_factor);
                copy_construct_n(alloc,guard.array,first,count);
                guard.length=count;
                guard.transfer_to(*this);
            }
            template<typename It>
            constexpr void assign_counted_range(It first,size_type count){
                if(maxLen<count){
                    free_storage();
                    allocate_and_assign_counted_range(first,count);
                }
                else{
                    if(length>=count){
                        destroy_n(alloc,array+count,length-count);length=count;
                        copy_n(array,first,count);
                    }
                    else{
                        advance_copy_n(array,first,length);
                        copy_construct_n(alloc,array+length,first,count-length);length=count;
                    }
                }
            }
            template<typename It>
            constexpr void allocate_and_move_counted_range(It first,size_type count){// assumes *this has no storage
                Guard guard(alloc,count*realloc_factor);
                move_construct_n(alloc,guard.array,first,count);
                guard.length=count;
                guard.transfer_to(*this);
            }
            template<typename It>
            constexpr void move_counted_range(It first,size_type count){
                if(maxLen<count){
                    free_storage();
                    allocate_and_move_counted_range(first,count);
                }
                else{
                    if(length>=count){
                        destroy_n(alloc,array+count,length-count);length=count;
                        move_n(array,first,count);
                    }
                    else{
                        advance_move_n(array,first,length);
                        move_construct_n(alloc,array+length,first,count-length);length=count;
                    }
                }
            }
            template<typename It>
            constexpr iterator insert_uncounted_range(const_iterator pos,It first,It last){
                size_type index=pos-begin(),prev_length=length;
                while(first!=last) push_back(*first++);
                rotate(array+index,array+prev_length,array+length);
                return array+index;
            }
            template<typename It>
            constexpr iterator insert_counted_range(const_iterator pos,It first,size_type count){// can't insert into itself
                size_type index=pos-begin();
                if(length+count>maxLen){
                    Guard guard(alloc,(length+count)*realloc_factor);
                    copy_construct_n(alloc,guard.array+index,first,count);
                    DestroySequenceGuard tempguard(alloc,guard.array+index,count);
                    move_construct_if_nt_n(alloc,guard.array,array,index);
                    tempguard.count=0;
                    guard.length=index+count;
                    move_construct_if_nt_n(alloc,guard.array+index+count,array+index,length-index); guard.length+=length-index;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    if(index+count>=length){
                        move_construct_n(alloc,array+count+index,array+index,length-index);
                        DestroySequenceGuard guard(alloc,array+count+index,length-index);
                        advance_copy_n(array+index,first,length-index);//equivalente a copy_n(array+index,first,length-index);advance(first,length-index);
                        copy_construct_n(alloc,array+length,first,count-(length-index));
                        guard.count=0;
                        length+=count;
                    }
                    else{
                        size_type old_length=length;
                        move_construct_n(alloc,array+old_length,array+old_length-count,count);length+=count;
                        move_reverse_n(array+index+count,array+index,old_length-index-count);
                        copy_n(array+index,first,count);
                    }
                }
                return array+index;
            }
            template<typename It>
            constexpr iterator insert_counted_range_into_itself(const_iterator pos,It first,size_type count){//always reallocates
                size_type index=pos-begin();
                Guard guard(alloc,(length+count)*realloc_factor);
                copy_construct_n(alloc,guard.array+index,first,count);
                DestroySequenceGuard tempguard(alloc,guard.array+index,count);
                move_construct_if_nt_n(alloc,guard.array,array,index);
                tempguard.count=0;
                guard.length=index+count;
                move_construct_if_nt_n(alloc,guard.array+index+count,array+index,length-index); guard.length+=length-index;
                this->destroy_and_free();
                guard.transfer_to(*this);
                return array+index;
            }
            template<typename It>
            constexpr iterator move_insert_uncounted_range(const_iterator pos,It first,It last){
                size_type index=pos-begin(),prev_length=length;
                while(first!=last) push_back(mpv::move(*first++));
                rotate(array+index,array+prev_length,array+length);
                return array+index;
            }
            template<typename It>
            constexpr iterator move_insert_counted_range(const_iterator pos,It first,size_type count){
                size_type index=pos-begin();
                if(length+count>maxLen){
                    Guard guard(alloc,(length+count)*realloc_factor);
                    move_construct_n(alloc,guard.array+index,first,count);
                    DestroySequenceGuard tempguard(alloc,guard.array+index,count);
                    move_construct_if_nt_n(alloc,guard.array,array,index);
                    tempguard.count=0;
                    guard.length=index+count;
                    move_construct_if_nt_n(alloc,guard.array+index+count,array+index,length-index); guard.length+=length-index;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    if(index+count>=length){
                        move_construct_n(alloc,array+count+index,array+index,length-index);
                        DestroySequenceGuard guard(alloc,array+count+index,length-index);
                        advance_move_n(array+index,first,length-index);//equivalente a copy_n(array+index,first,length-index);advance(first,length-index);
                        move_construct_n(alloc,array+length,first,count-(length-index));
                        guard.count=0;
                        length+=count;
                    }
                    else{
                        size_type old_length=length;
                        move_construct_n(alloc,array+old_length,array+old_length-count,count);length+=count;
                        move_reverse_n(array+index+count,array+index,old_length-index-count);
                        move_n(array+index,first,count);
                    }
                }
                return array+index;
            }
            template<typename It>
            constexpr void append_uncounted_range(It first,It last){
                while(first!=last) push_back(*first++);
            }
            template<typename It>
            constexpr void append_counted_range(It first,size_type count){//can append a range into itself
                if(length+count>maxLen){
                    Guard guard(alloc,(length+count)*realloc_factor);
                    copy_construct_n(alloc,guard.array+length,first,count);
                    DestroySequenceGuard tempguard(alloc,guard.array+length,count);
                    move_construct_if_nt_n(alloc,guard.array,array,length); 
                    tempguard.count=0;
                    guard.length=length+count;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    copy_construct_n(alloc,array+length,first,count); length+=count;
                }
            }
            template<typename It>
            constexpr void move_append_uncounted_range(It first,It last){
                while(first!=last) push_back(mpv::move(*first++));
            }
            template<typename It>
            constexpr void move_append_counted_range(It first,size_type count){
                if(length+count>maxLen){
                    Guard guard(alloc,(length+count)*realloc_factor);
                    move_construct_n(alloc,guard.array+length,first,count);
                    DestroySequenceGuard tempguard(alloc,guard.array+length,count);
                    move_construct_if_nt_n(alloc,guard.array,array,length); 
                    tempguard.count=0;
                    guard.length=length+count;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    move_construct_n(alloc,array+length,first,count); length+=count;
                }
            }
        public:
            constexpr Vector()noexcept(is_nothrow_default_constructible_v<AlTy>) = default;
            constexpr explicit Vector(const Alloc& al)noexcept:cp(arg1_tag{},al){}
            constexpr Vector(const Vector& other):cp(arg1_tag{},AlTy_traits::select_on_container_copy_construction(other.alloc),0){
                this->allocate_and_assign_counted_range(other.array,other.length);
            }
            constexpr Vector(const Vector& other,const Alloc& al):cp(arg1_tag{},al,0){
                this->allocate_and_assign_counted_range(other.array,other.length);
            }
            constexpr Vector(Vector&& other)noexcept:cp(arg1_tag{},static_cast<AlTy&&>(other.alloc),other.maxLen),length(other.length),array(other.array){
                other.reset();
            }
            constexpr Vector(Vector&& other,const Alloc& al)noexcept(ALWAYS_EQ):cp(arg1_tag{},al,0){
                if(this->alloc==other.alloc){
                    this->take_content(other);
                }
                else{
                    allocate_and_move_counted_range(other.array,other.length);
                }
            }
            constexpr explicit Vector(size_type sz){
                Guard guard(alloc,sz*realloc_factor);
                default_construct_n(alloc,guard.array,sz);
                guard.length=sz;
                guard.transfer_to(*this);
            }
            constexpr Vector(size_type sz,const Alloc& al):cp(arg1_tag{},al,0){
                Guard guard(alloc,sz*realloc_factor);
                default_construct_n(alloc,guard.array,sz);
                guard.length=sz;
                guard.transfer_to(*this);
            }
            constexpr Vector(size_type sz,const T& fillwith,const Alloc& al=Alloc{}):cp(arg1_tag{},al,0){
                Guard guard(alloc,sz*realloc_factor);
                fill_construct_n(alloc,guard.array,sz,fillwith);
                guard.length=sz;
                guard.transfer_to(*this);
            }
            template<typename It, enable_if_t<is_iterator_v<It>,int> = 0>
            constexpr Vector(It first,It last,const Alloc& al=Alloc{}):cp(arg1_tag{},al,0){
                if constexpr(is_forward_iterator_v<It>){
                    this->allocate_and_assign_counted_range(first,distance(first,last));
                }
                else{
                    while(first!=last) push_back(*first++);
                }
            }
            constexpr Vector(std::initializer_list<value_type> initlist,const Alloc& al=Alloc()):cp(arg1_tag{},al,0){
                this->allocate_and_assign_counted_range(initlist.begin(),initlist.size());
            }
            constexpr Vector& operator=(const Vector& other){
                if(this==&other) return *this;
                if constexpr(POCCA && !ALWAYS_EQ){
                    if(this->alloc!=other.alloc){
                        this->free_storage();
                    }
                }
                pocca(this->alloc,other.alloc);
                this->assign_counted_range(other.array,other.length);
                return *this;
            }
            constexpr Vector& operator=(Vector&& other)noexcept(ALWAYS_EQ || POCMA){
                if(this==&other) return *this;
                if constexpr(!ALWAYS_EQ && !POCMA){
                    if(this->alloc!=other.alloc){
                        this->move_counted_range(other.array,other.length);
                        return *this;
                    }
                }
                this->destroy_and_free();
                pocma(this->alloc,other.alloc);
                this->take_content(other);
                return *this;
            }
            constexpr Vector operator+(const Vector& other)const&{
                Vector new_vec(reserve_tag{},(this->length+other.length)*realloc_factor,AlTy_traits::select_on_container_copy_construction(alloc));
                copy_construct_n(new_vec.alloc,new_vec.array,this->array,this->length);new_vec.length=this->length;
                copy_construct_n(new_vec.alloc,new_vec.array+this->length,other.array,other.length);new_vec.length+=other.length;
                return new_vec;
            }
            constexpr Vector operator+(Vector&& other)const&{
                Vector new_vec(static_cast<Vector&&>(other));
                new_vec.insert_counted_range(new_vec.begin(),this->array,this->length);
                return new_vec;
            }
            constexpr Vector operator+(const Vector& other)&&{
                Vector new_vec(static_cast<Vector&&>(*this));
                new_vec.append_counted_range(other.array,other.length);
                return new_vec;
            }
            constexpr Vector operator+(Vector&& other)&&{
                if(this->length+other.length>other.maxLen || this->length+other.length<=this->maxLen){
                    Vector new_vec(static_cast<Vector&&>(*this));
                    new_vec.move_append_counted_range(other.begin(),other.length);
                    return new_vec;
                }
                else{
                    Vector new_vec(static_cast<Vector&&>(other));
                    new_vec.move_insert_counted_range(new_vec.begin(),this->array,this->length);
                    return new_vec;
                }
            }
            constexpr Vector& operator+=(const Vector& other){
                this->append_counted_range(other.array,other.length);
                return *this;
            }
            constexpr Vector& operator+=(Vector&& other){
                this->move_append_counted_range(other.array,other.length);
                return *this;
            }
            constexpr Vector operator*(size_type num)const{
                Vector new_vec(reserve_tag{},this->length*num*realloc_factor,AlTy_traits::select_on_container_copy_construction(alloc));
                for(size_type i=0;i<num;i++){
                    copy_construct_n(new_vec.alloc,new_vec.array+new_vec.length,this->array,this->length);
                    new_vec.length+=this->length;
                }
                return new_vec;
            }
            constexpr Vector& operator*=(size_type num){
                if(num==0) clear();
                else{
                    if(length*num>this->maxLen){
                        Guard guard(alloc,length*num*realloc_factor);
                        move_construct_n(this->alloc,guard.array,array,length);
                        guard.length=length;
                        this->destroy_and_free();
                        guard.transfer_to(*this);
                    }
                    for(size_type i=1,old_length=length;i<num;i++,length+=old_length)
                        copy_construct_n(this->alloc,array+length,array,old_length);
                }
                return *this;
            }
            template<typename... Args>
            constexpr void emplace_back(Args&&... args){
                if(length==maxLen){
                    Guard guard(alloc,maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor);
                    CONSTRUCT_VARARGS(alloc,guard.array+length,static_cast<Args&&>(args));
                    DestroyGuard tempguard(alloc,guard.array+length);
                    move_construct_if_nt_n(alloc,guard.array,array,length);
                    tempguard.destroy=false;
                    guard.length=length+1;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    CONSTRUCT_VARARGS(this->alloc,array+length,static_cast<Args&&>(args));
                    length++;
                }
            }
            constexpr void push_back(const T& val){emplace_back(val);}
            constexpr void push_back(value_type&& val){emplace_back(static_cast<value_type&&>(val));}
            template<typename... Args>
            constexpr iterator emplace(const_iterator pos,Args&&... args){
                size_type index=pos-begin();
                if(length==maxLen){
                    Guard guard(alloc,maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor);
                    CONSTRUCT_VARARGS(this->alloc,guard.array+index,static_cast<Args&&>(args));
                    DestroyGuard tempguard(alloc,guard.array+index);
                    move_construct_if_nt_n(this->alloc,guard.array,array,index);
                    tempguard.destroy=false;
                    guard.length=index+1;
                    move_construct_if_nt_n(this->alloc,guard.array+index+1,array+index,length-index);guard.length=this->length+1;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    if(index==length){
                        CONSTRUCT_VARARGS(this->alloc,array+length,static_cast<Args&&>(args));length++;
                    }
                    else{
                        CONSTRUCT(this->alloc,array+length,static_cast<value_type&&>(array[length-1]));
                        move_reverse_n(array+index+1,array+index,(length++)-index-1);
                        array[index]=value_type(static_cast<Args&&>(args)...);
                    }
                }
                return array+index;
            }
            template<typename... Args>
            constexpr iterator emplace_at(size_type index,Args&&... args){
                if(length==maxLen){
                    Guard guard(alloc,maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor);
                    CONSTRUCT_VARARGS(this->alloc,guard.array+index,static_cast<Args&&>(args));
                    DestroyGuard tempguard(alloc,guard.array+index);
                    move_construct_if_nt_n(this->alloc,guard.array,array,index);
                    tempguard.destroy=false;
                    guard.length=index+1;
                    move_construct_if_nt_n(this->alloc,guard.array+index+1,array+index,length-index);guard.length=this->length+1;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    if(index==length){
                        CONSTRUCT_VARARGS(this->alloc,array+length,static_cast<Args&&>(args));length++;
                    }
                    else{
                        CONSTRUCT(this->alloc,array+length,static_cast<value_type&&>(array[length-1]));
                        move_reverse_n(array+index+1,array+index,(length++)-index-1);
                        array[index]=value_type(static_cast<Args&&>(args)...);
                    }
                }
                return array+index;
            }
            constexpr iterator insert(const_iterator pos,const T& val){return emplace(pos,val);}
            constexpr iterator insert(const_iterator pos,value_type&& val){return emplace(pos,static_cast<value_type&&>(val));}
            constexpr iterator insert_at(size_type index,const T& val){return emplace_at(index,val);}
            constexpr iterator insert_at(size_type index,value_type&& val){return emplace_at(index,static_cast<value_type&&>(val));}
            template<typename It>
            constexpr enable_if_t<is_iterator_v<It>,iterator> insert_at(size_type index,It first,It last){
                if constexpr(is_forward_iterator_v<It>){
                    return insert_counted_range(const_iterator(array+index),first,distance(first,last));
                }
                else{
                    return insert_uncounted_range(const_iterator(array+index),first,last);
                }
            }
            template<typename It>
            constexpr enable_if_t<is_iterator_v<It>,iterator> insert(const_iterator pos,It first,It last){
                if constexpr(is_forward_iterator_v<It>){
                    return insert_counted_range(pos,first,distance(first,last));
                }
                else{
                    return insert_uncounted_range(pos,first,last);
                }
            }
            constexpr iterator insert_at(size_type index,const size_type count,const T& val){
                if(length+count>maxLen){
                    Guard guard(alloc,(length+count)*realloc_factor);
                    fill_construct_n(alloc,guard.array+index,count,val);
                    DestroySequenceGuard tempguard(alloc,guard.array+index,count);
                    move_construct_if_nt_n(alloc,guard.array,array,index); 
                    tempguard.count=0;
                    guard.length=index+count;
                    move_construct_if_nt_n(alloc,guard.array+index+count,array+index,length-index); guard.length+=length-index;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    size_type old_length=length;
                    if(index+count>=length){
                        fill_construct_n(alloc,array+old_length,count-(old_length-index),val);length=count+index;
                        move_construct_n(alloc,array+count+index,array+index,old_length-index);length+=old_length-index;
                        fill_n(array+index,old_length-index,val);
                        
                    }
                    else{
                        move_construct_n(alloc,array+old_length,array+old_length-count,count);length+=count;
                        move_reverse_n(array+index+count,array+index,old_length-index-count);
                        fill_n(array+index,count,val);
                    }
                }
                return array+index;
            }
            constexpr iterator insert(const_iterator pos,const size_type count,const T& val){
                return insert_at(pos-begin(),count,val);
            }
            constexpr void del_at(size_type index)noexcept(is_nothrow_move_assignable_v<value_type>){
                mpv::move(array+index,array+index+1,array+length);
                DESTROY(alloc,array+length-1);
                --length;
            }
            constexpr void del(const_iterator pos)noexcept(is_nothrow_move_assignable_v<value_type>){
                mpv::move(array+(pos-begin()),pos+1,const_iterator(array+length));
                DESTROY(alloc,array+length-1);
                --length;
            }
            constexpr value_type pop_at(size_type index)noexcept(is_nothrow_move_constructible_v<value_type> && is_nothrow_move_assignable_v<value_type>){
                value_type aux=static_cast<value_type&&>(array[index]);
                mpv::move(array+index,array+index+1,array+length);
                DESTROY(alloc,array+length-1);
                --length;
                return aux;
            }
            constexpr value_type pop(const_iterator pos)noexcept(is_nothrow_move_constructible_v<value_type> && is_nothrow_move_assignable_v<value_type>){
                iterator it=array+(pos-begin());
                value_type aux=static_cast<value_type&&>(*it);
                mpv::move(it,pos+1,const_iterator(array+length));
                DESTROY(alloc,array+length-1);
                --length;
                return aux;
            }
            constexpr void del_back()noexcept{
                DESTROY(this->alloc,array+length-1);
                --length;
            }
            constexpr value_type pop_back()noexcept(is_nothrow_move_constructible_v<value_type>){
                value_type aux=static_cast<value_type&&>(array[length-1]);
                DESTROY(this->alloc,array+length-1);
                --length;
                return aux;
            }
            constexpr void join(const_iterator pos,const Vector& other){
                if(this==&other) insert_counted_range_into_itself(pos,array,length);
                else insert_counted_range(pos,other.array,other.length);
            }
            constexpr void join(const_iterator pos,Vector&& other){
                move_insert_counted_range(pos,other.array,other.length);
            }
            constexpr Vector concat(const_iterator pos,const Vector& other)const{
                size_type index=pos-begin();
                Vector new_vec(reserve_tag{},(this->length+other.length)*realloc_factor,AlTy_traits::select_on_container_copy_construction(alloc));
                copy_construct_n(new_vec.alloc,new_vec.array,this->array,index);new_vec.length=index;
                copy_construct_n(new_vec.alloc,new_vec.array+index,other.array,other.length);new_vec.length+=other.length;
                copy_construct_n(new_vec.alloc,new_vec.array+index+other.length,this->array+index,this->length-index);new_vec.length+=this->length-index;
                return new_vec;
            }
            constexpr Vector concat(const_iterator pos,Vector&& other)const{
                size_type index=pos-begin();
                Vector new_vec(static_cast<Vector&&>(other));
                if(this->length+new_vec.length>new_vec.maxLen){
                    Guard guard(new_vec.alloc,(this->length+other.length)*realloc_factor);
                    copy_construct_n(guard.al,guard.array,this->array,index);guard.length=index;
                    move_construct_n(guard.al,guard.array+index,new_vec.array,new_vec.length);guard.length+=new_vec.length;
                    copy_construct_n(guard.al,guard.array+index+new_vec.length,this->array+index,this->length-index);guard.length+=this->length-index;
                    new_vec.destroy_and_free();
                    guard.transfer_to(new_vec);
                }
                else{
                    size_type old_length=new_vec.length;
                    if(index<old_length){
                        move_construct_n(new_vec.alloc,new_vec.array+old_length,new_vec.array+old_length-index,index);new_vec.length+=index;
                        move_reverse_n(new_vec.array+index,new_vec.array,old_length-index);
                        copy_n(new_vec.array,this->array,index);
                    }
                    else{
                        move_construct_n(new_vec.alloc,new_vec.array+index,new_vec.array,new_vec.length);
                        DestroySequenceGuard tempguard(new_vec.alloc,new_vec.array+index,new_vec.length);
                        copy_n(new_vec.array,this->array,new_vec.length);
                        copy_construct_n(new_vec.alloc,new_vec.array+new_vec.length,this->array+new_vec.length,index-new_vec.length);
                        tempguard.count=0;
                        new_vec.length+=index;
                    }
                    copy_construct_n(new_vec.alloc,new_vec.array+index+old_length,this->array+index,this->length-index);
                    new_vec.length+=this->length-index;
                }
                return new_vec;
            }
            constexpr Vector sublist(const_iterator first,const_iterator last)const{
                return Vector(first,last,AlTy_traits::select_on_container_copy_construction(alloc));
            }
            constexpr Vector cut(const_iterator first,const_iterator last){
                Vector new_vec(AlTy_traits::select_on_container_copy_construction(alloc));
                new_vec.allocate_and_move_counted_range(first,last-first);
                move_n(this->array+(first-begin()),this->array+(last-begin()),end()-last);
                destroy_n(alloc,this->array+this->length-new_vec.length,new_vec.length);
                this->length-=new_vec.length;
                return new_vec;
            }
            constexpr Vector& wipe(const_iterator first,const_iterator last){
                size_type deleted_size=last-first;
                move_n(this->array+(first-begin()),this->array+(last-begin()),end()-last);
                destroy_n(alloc,this->array+this->length-deleted_size,deleted_size);
                this->length-=deleted_size;
                return *this;
            }
            constexpr bool remove(const T& val){
                for(size_type i=0;i<length;i++){
                    if(array[i]==val){
                        move_n(array+i,array+i+1,(length-1)-i);
                        DESTROY(alloc,array+length-1);
                        --length;
                        return true;
                    }
                }
                return false;
            }
            constexpr size_type remove_all(const T& val){
                size_type i=0,j=0;
                while(j<length){
                    if(array[j]!=val){
                        if(i!=j)
                            array[i]=static_cast<value_type&&>(array[j]);
                        i++;
                    }
                    j++;
                }
                destroy_n(alloc,array+i,length-i);
                length=i;
                return j-i;
            }
            constexpr void resize(size_type new_len){
                if(new_len>maxLen){
                    Guard guard(alloc,new_len*realloc_factor);
                    default_construct_n(alloc,guard.array+length,new_len-length);
                    DestroySequenceGuard tempguard(alloc,guard.array+length,new_len-length);
                    move_construct_if_nt_n(alloc,guard.array,array,length);
                    tempguard.count=0;
                    guard.length=new_len;
                    destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    if(length>new_len){
                        destroy_n(alloc,array+new_len,length-new_len);
                    }
                    else if(length<new_len){
                        default_construct_n(alloc,array+length,new_len-length);
                    }
                    length=new_len;
                }
            }
            constexpr const_iterator find(const T& val)const{
                return mpv::find(begin(),end(),val);
            }
            constexpr iterator find(const T& val){
                return mpv::find(begin(),end(),val);
            }
            template<typename Cmp=less<T>>
            constexpr const_iterator binary_search(const T& val,Cmp&& cmp=Cmp{})const{
                return mpv::binary_search(begin(),end(),val,static_cast<Cmp&&>(cmp));
            }
            template<typename Cmp=less<T>>
            constexpr iterator binary_search(const T& val,Cmp&& cmp=Cmp{}){
                return mpv::binary_search(begin(),end(),val,static_cast<Cmp&&>(cmp));
            }
            constexpr size_type count(const T& val)const{
                return mpv::count<const_iterator,T,size_type>(begin(),end(),val);
            }
			constexpr bool contains(const T& val)const{
				return mpv::contains<const_iterator,T>(begin(),end(),val);
			}
            constexpr size_type index_of(const T& val)const{
                return mpv::index_of<const_iterator,T,size_type>(begin(),end(),val);
            }
            constexpr bool operator==(const Vector& other)const{
                if(this->length!=other.length) return false;
                else return equal(this->begin(),this->end(),other.begin());
            }
            constexpr bool operator!=(const Vector& other)const{
                if(this->length!=other.length) return true;
                else return !equal(this->begin(),this->end(),other.begin());
            }
            constexpr bool operator<(const Vector& other)const{
                return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool operator>(const Vector& other)const{
                return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator<=(const Vector& other)const{
                return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator>=(const Vector& other)const{
                return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr const_reference operator[](size_type index)const noexcept{
                return array[index];
            }
            constexpr reference operator[](size_type index)noexcept{
                return array[index];
            }
            constexpr const_reference back()const noexcept{
                return array[length-1];
            }
            constexpr reference back()noexcept{
                return array[length-1];
            }
            constexpr size_type size()const noexcept{
                return length;
            }
            constexpr size_type max_size()const noexcept{
                return maxLen;
            }
            constexpr const_pointer get_array()const noexcept{
                return array;
            }
            constexpr pointer get_array()noexcept{
                return array;
            }
            constexpr allocator_type get_allocator()const noexcept{
                return this->alloc;
            }
			template<typename Pred=less<>>
			constexpr void sort(Pred pred=Pred{}){
				mpv::insertion_sort(begin(),end(),pred);
			}
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const{
                for(const_reference x:*this)
                    if(func(x))return true;
                return false;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{})const{
				for(const_reference x:*this)
					func(x);
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{}){
				for(reference x:*this)
					func(x);
			}
            template<typename,typename,typename> friend class Vector;
			template<typename Lambda>
			constexpr auto map(Lambda&& func=Lambda{})const{
                Vector<decltype(func(declval<const_reference>())),rebind_alloc<Alloc,decltype(func(declval<const_reference>()))>,realloc_params> new_vec(reserve_tag{},this->maxLen,AlTy_traits::select_on_container_copy_construction(alloc));
                //Vector<decltype(func(*this->array)),rebind_alloc<Alloc,decltype(func(*this->array))>,realloc_params> new_vec(reserve_tag{},this->maxLen);
                for(size_type i=0;i<this->length;i++){
                    CONSTRUCT(new_vec.alloc,new_vec.array+i,func(this->array[i]));
                    ++new_vec.length;
                    //decltype(new_vec)::AlTy_traits::construct(new_vec.alloc,new_vec.array+i,func(this->array[i]));
                }
                    
				return new_vec;
			}
			template<typename Lambda>
			constexpr Vector filter(Lambda&& func=Lambda{})const{
				Vector new_vec(reserve_tag{},this->length,AlTy_traits::select_on_container_copy_construction(alloc));
				for(size_type i=0;i<this->length;i++)
					if(func(this->array[i])){
                        CONSTRUCT(new_vec.alloc,new_vec.array+new_vec.length,this->array[i]);
                        ++new_vec.length;
                        //AlTy_traits::construct(new_vec.alloc,new_vec.array+new_vec.length++,this->array[i]);
                    }
				return new_vec;
			}
            constexpr Vector& reverse(){
                mpv::reverse(begin(),end());
                return *this;
            }
			constexpr bool empty()const noexcept{
				return length==0;
			}
            constexpr void clear()noexcept{
                destroy_n(this->alloc,array,length);
                length=0;
            }
            constexpr void shrink_to_fit(){
                if(length<maxLen){
                    Guard guard(alloc,length);
                    move_construct_if_nt_n(alloc,guard.array,this->array,length);
                    guard.length=length;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
            }
            constexpr void reserve(size_type reserve_size){
                if(reserve_size>maxLen){
                    Guard guard(alloc,reserve_size);
                    move_construct_if_nt_n(alloc,guard.array,array,length);
                    guard.length=length;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
            }
            constexpr void free_storage()noexcept{
                if(array){
                    destroy_n(this->alloc,array,length);
                    AlTy_traits::deallocate(this->alloc,array,maxLen);                    
                }
                this->length=0;
                this->maxLen=0;
                this->array=nullptr;
            }
            ~Vector()noexcept{
                destroy_and_free();
            }
            constexpr iterator begin()noexcept{
                return pointer(array);
            }
            constexpr iterator end()noexcept{
                return pointer(array+length);
            }
            constexpr const_iterator begin()const noexcept{
                return const_pointer(array);
            }
            constexpr const_iterator end()const noexcept{
                return const_pointer(array+length);
            }
#undef alloc
#undef maxLen
    };
    template<typename Out,typename T,typename Alloc,typename realloc_params>
    Out& operator<<(Out& stream,const Vector<T,Alloc,realloc_params>& list){
        stream<<"[";
        for(typename Vector<T,Alloc,realloc_params>::size_type i=0,length=list.size();i<length;i++){
            stream<<""<<list[i];
            if(i<length-1)
                stream<<", ";
        }
        stream<<"]";
        return stream;
    }
#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
    template<typename T,typename Alloc,typename realloc_params>
    void debug(const Vector<T,Alloc,realloc_params>& vec){
        std::cout<<"maxLen: "<<vec.max_size()<<std::endl
                 <<"length: "<<vec.size()<<std::endl
                 <<"array: "<<vec.get_array()<<std::endl;
    }
#endif
}

namespace mpv{
	template<typename T,typename VoidPtr>
	struct Basic_Node COUNT_IT{
		using NodePtr=rebind_pointer<VoidPtr,Basic_Node>;
		using const_NodePtr=rebind_pointer<VoidPtr,const Basic_Node>;
		NodePtr next=nullptr;
		T data;
		template<typename... Args>
		constexpr Basic_Node(Args&&... args)noexcept(is_nothrow_constructible_v<T,Args...>):data(static_cast<Args&&>(args)...){}
		Basic_Node(const Basic_Node&)=delete;
		Basic_Node& operator=(const Basic_Node&)=delete;
	};
	template<typename Types>
	class Node_iterator{
		public:
			using iterator_category=forward_iterator_tag;
			using NodePtr=typename Types::NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=typename Types::reference;
		private:
			NodePtr ptr;
		public:
			constexpr Node_iterator(NodePtr ptr)noexcept:ptr(ptr){}
			constexpr reference operator*()const noexcept{
				return ptr->data;
			}
			constexpr pointer operator->()const noexcept{
				return pointer_traits<pointer>::pointer_to(ptr->data);
			}
			constexpr Node_iterator& operator++()noexcept{
				ptr=ptr->next;
				return *this;
			}
			constexpr Node_iterator operator++(int)noexcept{
				Node_iterator aux=*this;
				ptr=ptr->next;
				return *this;
			}
			constexpr bool operator==(const Node_iterator& other)const noexcept{
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const Node_iterator& other)const noexcept{
				return this->ptr!=other.ptr;
			}
			template<typename> friend class const_Node_iterator;
	};
	template<typename Types>
	class const_Node_iterator{
		public:
			using iterator_category=forward_iterator_tag;
			using NodePtr=typename Types::const_NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
		private:
			NodePtr ptr;
		public:
			constexpr const_Node_iterator(NodePtr ptr)noexcept:ptr(ptr){}
			constexpr const_Node_iterator(Node_iterator<Types> nonconst_it)noexcept:ptr(nonconst_it.ptr){}
			constexpr reference operator*()const noexcept{
				return ptr->data;
			}
			constexpr pointer operator->()const noexcept{
				return pointer_traits<pointer>::pointer_to(ptr->data);
			}
			constexpr const_Node_iterator& operator++()noexcept{
				ptr=ptr->next;
				return *this;
			}
			constexpr const_Node_iterator operator++(int)noexcept{
				const_Node_iterator aux=*this;
				ptr=ptr->next;
				return aux;
			}
			constexpr bool operator==(const const_Node_iterator& other)const noexcept{
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const const_Node_iterator& other)const noexcept{
				return this->ptr!=other.ptr;
			}
	};
#define head cp.V2
#define alloc cp.getV1()
	template<typename T,typename Alloc=allocator<T>>
	class Queue COUNT_IT{
		private:
			using Node=Basic_Node<T,typename allocator_traits<Alloc>::void_pointer>;
			using AlTy=rebind_alloc<Alloc,T>;
			using AlTy_traits=allocator_traits<AlTy>;
			using AlNode=rebind_alloc<Alloc,Node>;
			using AlNode_traits=allocator_traits<AlNode>;
			using NodePtr=typename AlNode_traits::pointer;
			using const_NodePtr=typename AlNode_traits::const_pointer;
			struct Val_types{
				using value_type=T;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=T&;
				using const_reference=const T&;
				using NodePtr=typename AlNode_traits::pointer;
				using const_NodePtr=typename AlNode_traits::const_pointer;
			};
            static constexpr bool POCCA=AlTy_traits::propagate_on_container_copy_assignment::value;
            static constexpr bool POCMA=AlTy_traits::propagate_on_container_move_assignment::value;
            static constexpr bool POCS=AlTy_traits::propagate_on_container_swap::value;
            static constexpr bool ALWAYS_EQ=AlTy_traits::is_always_equal::value;
		public:
			using allocator_type=Alloc;
			using value_type=typename Val_types::value_type;
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
			using iterator=Node_iterator<Val_types>;
			using const_iterator=const_Node_iterator<Val_types>;
		private:
			CompressedPair<AlNode,NodePtr> cp=CompressedPair<AlNode,NodePtr>(nullptr);
			NodePtr tail=nullptr;
			constexpr void delete_node(NodePtr node)noexcept{
				DESTROY(this->alloc,node);
				AlNode_traits::deallocate(this->alloc,node,1);
			}
			template<typename... Args>
			constexpr NodePtr create_node(Args&&... args){
                AllocConstructPtr guard(alloc);
                guard.allocate();
				CONSTRUCT_VARARGS(this->alloc,guard.ptr,static_cast<Args&&>(args));
				return guard.release();
			}
			constexpr void reset()noexcept{
				head=nullptr;
				tail=nullptr;
			}
			template<typename... Args>
			constexpr void create_first_node(Args&&... args){
				this->head=this->create_node(static_cast<Args&&>(args)...);
				this->tail=this->head;
			}
			template<typename... Args>
			constexpr void link_back(Args&&... args){
				this->tail->next=this->create_node(static_cast<Args&&>(args)...);
				this->tail=this->tail->next;
			}
			constexpr void copy_elements(const Queue& other){
				NodePtr p=other.head;
				if(p!=nullptr){
					this->create_first_node(p->data);
					p=p->next;
					while(p!=nullptr){
						this->link_back(p->data);
						p=p->next;
					}
				}
			}
			constexpr void copy_elements(Queue&& other){
				if(other.head!=nullptr){
					this->create_first_node(static_cast<value_type&&>(other.head->data));
					NodePtr aux=other.head;
					other.head=other.head->next;
					other.delete_node(aux);
					while(other.head!=nullptr){
						this->link_back(other.head->data);
						aux=other.head;
						other.head=other.head->next;
						other.delete_node(aux);
					}
				}
				other.reset();
			}
			constexpr void move_elements(Queue& other){
				this->head=other.head;
				this->tail=other.tail;
				other.reset();
			}
		public:
			constexpr Queue()noexcept=default;
			constexpr explicit Queue(const Alloc& al)noexcept:cp(arg1_tag{},al){}
			constexpr Queue(const Queue& other):cp(arg1_tag{},AlNode_traits::select_on_container_copy_construction(other.alloc)){
				this->copy_elements(other);
			}
			constexpr Queue(const Queue& other,const Alloc& al):cp(arg1_tag{},al){
				this->copy_elements(other);
			}
			constexpr Queue(Queue&& other)noexcept:cp(arg1_tag{},static_cast<AlNode&&>(other.alloc),other.head),tail(other.tail){
				other.reset();
			}
			constexpr Queue(Queue&& other,const Alloc& al)noexcept(ALWAYS_EQ):cp(arg1_tag{},al){
				if(this->alloc==other.alloc){
					head=other.head;
					tail=other.tail;
					other.reset();
				}
				else{
					if(other.head!=nullptr){
						this->create_first_node(static_cast<value_type&&>(other.head->data));
						NodePtr aux=other.head;
						other.head=other.head->next;
						other.delete_node(aux);
						while(other.head!=nullptr){
							this->link_back(static_cast<value_type&&>(other.head->data));
							aux=other.head;
							other.head=other.head->next;
							other.delete_node(aux);
						}
						other.tail=nullptr;
					}
				}
			}
			constexpr Queue& operator=(const Queue& other){
				if(this==&other) return *this;
				this->clear();
				pocca(this->alloc,other.alloc);
				this->copy_elements(other);
				return *this;
			}
			constexpr Queue& operator=(Queue&& other)noexcept(ALWAYS_EQ || POCMA){
				if(this==&other) return *this;
				this->clear();
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->alloc!=other.alloc){
						this->copy_elements(static_cast<Queue&&>(other));
						return *this;
					}
				}
				pocma(this->alloc,other.alloc);
				this->move_elements(other);
				return *this;
			}
			template<typename... Args>
			constexpr void emplace(Args&&... args){
				NodePtr new_node=create_node(static_cast<Args&&>(args)...);
				if(this->head==nullptr)
					this->head=new_node;	
				else
					this->tail->next=new_node;
				this->tail=new_node;
			}
			constexpr void push(const_reference val){
				NodePtr new_node=create_node(val);
				if(this->head==nullptr)
					this->head=new_node;	
				else
					this->tail->next=new_node;
				this->tail=new_node;	
			}
			constexpr void push(value_type&& val){
				NodePtr new_node=create_node(static_cast<value_type&&>(val));
				if(this->head==nullptr)
					this->head=new_node;	
				else
					this->tail->next=new_node;
				this->tail=new_node;	
			}
			constexpr const_reference front()const noexcept{
				return this->head->data;
			}
			constexpr const_reference back()const noexcept{
				return this->tail->data;
			}
			constexpr reference front()noexcept{
				return this->head->data;
			}
			constexpr reference back()noexcept{
				return this->tail->data;
			}
			constexpr bool empty()const noexcept{
				return this->head==nullptr;
			}
			constexpr value_type pop()noexcept(is_nothrow_move_constructible_v<value_type>){
				NodePtr aux=this->head;
				value_type aux_data=static_cast<value_type&&>(this->head->data);
				this->head=this->head->next;
				if(this->head==nullptr) this->tail=nullptr;
				delete_node(aux);
				return aux_data;
			}
			constexpr void del()noexcept{
				NodePtr aux=this->head;
				this->head=this->head->next;
				if(this->head==nullptr) this->tail=nullptr;
				delete_node(aux);
			}
			constexpr void clear()noexcept{
				while(head!=nullptr){
					NodePtr aux=this->head;
					this->head=this->head->next;
					delete_node(aux);
				}
				tail=nullptr;
			}
			~Queue()noexcept{
				while(head!=nullptr){
					NodePtr aux=this->head;
					this->head=this->head->next;
					delete_node(aux);
				}
			}
			constexpr iterator begin()noexcept{
				return NodePtr(head);
			}
			constexpr iterator last()noexcept{
				return NodePtr(tail);
			}
			constexpr iterator end()noexcept{
				return NodePtr(nullptr);
			}
			constexpr const_iterator begin()const noexcept{
				return const_NodePtr(head);
			}
			constexpr const_iterator last()const noexcept{
				return const_NodePtr(tail);
			}
			constexpr const_iterator end()const noexcept{
				return const_NodePtr(nullptr);
			}
	};
	template<typename Out,typename T,typename Alloc>
	Out& operator<<(Out& stream,const Queue<T,Alloc>& queue){
		stream<<"[";
		for(typename Queue<T,Alloc>::const_iterator i=queue.begin();i!=queue.end();){
			stream<<""<<*i;
			if(++i!=queue.end())
				stream<<", ";		
		}
		stream<<"]";
		return stream;
	}
	template<typename T,typename Alloc=allocator<T>>
	class Stack COUNT_IT{
		private:
			using Node=Basic_Node<T,typename allocator_traits<Alloc>::void_pointer>;
			using AlTy=rebind_alloc<Alloc,T>;
			using AlTy_traits=allocator_traits<AlTy>;
			using AlNode=rebind_alloc<Alloc,Node>;
			using AlNode_traits=allocator_traits<AlNode>;
			using NodePtr=typename AlNode_traits::pointer;
			using const_NodePtr=typename AlNode_traits::const_pointer;
			struct Val_types{
				using value_type=T;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=T&;
				using const_reference=const T&;
				using NodePtr=typename AlNode_traits::pointer;
				using const_NodePtr=typename AlNode_traits::const_pointer;
			};
            static constexpr bool POCCA=AlTy_traits::propagate_on_container_copy_assignment::value;
            static constexpr bool POCMA=AlTy_traits::propagate_on_container_move_assignment::value;
            static constexpr bool POCS=AlTy_traits::propagate_on_container_swap::value;
            static constexpr bool ALWAYS_EQ=AlTy_traits::is_always_equal::value;
		public:
			using allocator_type=Alloc;
			using value_type=typename Val_types::value_type;
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
			using iterator=Node_iterator<Val_types>;
			using const_iterator=const_Node_iterator<Val_types>;
		private:
			CompressedPair<AlNode,NodePtr> cp=CompressedPair<AlNode,NodePtr>(nullptr);
			constexpr void delete_node(NodePtr node)noexcept{
				DESTROY(this->alloc,node);
				AlNode_traits::deallocate(this->alloc,node,1);
			}
			template<typename... Args>
			constexpr NodePtr create_node(Args&&... args){
                AllocConstructPtr guard(alloc);
                guard.allocate();
				CONSTRUCT_VARARGS(this->alloc,guard.ptr,static_cast<Args&&>(args));
				return guard.release();
			}
			constexpr void copy_elements(const Stack& other){
				NodePtr p=other.head;
				if(p!=nullptr){
					this->head=this->create_node(p->data);
					p=p->next;
					for(NodePtr last=this->head;p!=nullptr;last=last->next){
						last->next=this->create_node(p->data);
						p=p->next;
					}
				}
			}
			constexpr void copy_elements(Stack&& other){
				if(other.head!=nullptr){
					this->head=this->create_node(static_cast<value_type&&>(other.head->data));
					NodePtr aux=other.head;
					other.head=other.head->next;
					other.delete_node(aux);
					for(NodePtr last=this->head;other.head!=nullptr;last=last->next){
						last->next=this->create_node(other.head->data);
						aux=other.head;
						other.head=other.head->next;
						other.delete_node(aux);
					}
				}				
			}
		public:
			constexpr Stack()noexcept=default;
			constexpr explicit Stack(const Alloc& al)noexcept:cp(arg1_tag{},al){}
			constexpr Stack(const Stack& other):cp(arg1_tag{},AlNode_traits::select_on_container_copy_construction(other.alloc)){
				this->copy_elements(other);
			}
			constexpr Stack(const Stack& other,const Alloc& al):cp(arg1_tag{},al){
				this->copy_elements(other);
			}
			constexpr Stack(Stack&& other)noexcept:cp(arg1_tag{},static_cast<AlNode&&>(other.alloc),other.head){
				other.head=nullptr;
			}
			constexpr Stack(Stack&& other,const Alloc& al)noexcept(ALWAYS_EQ):cp(arg1_tag{},al){
				if(this->alloc==other.alloc){
					this->head=other.head;
					other.head=nullptr;
				}
				else
					this->copy_elements(static_cast<Stack&&>(other));
			}
			constexpr Stack& operator=(const Stack& other){
				if(this==&other) return *this;
				this->clear();
				pocca(this->alloc,other.alloc);
				this->copy_elements(other);
				return *this;
			}
			constexpr Stack& operator=(Stack&& other)noexcept(ALWAYS_EQ || POCMA){
				if(this==&other) return *this;
				this->clear();
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->alloc!=other.alloc){
						this->copy_elements(static_cast<Stack&&>(other));
						return *this;
					}
				}
				pocma(this->alloc,other.alloc);
				this->head=other.head;
				other.head=nullptr;
				return *this;
			}
			template<typename... Args>
			constexpr void emplace(Args&&... args){
				NodePtr new_node=create_node(static_cast<Args&&>(args)...);
				new_node->next=this->head;
				this->head=new_node;
			}
			constexpr void push(const_reference val){
				NodePtr new_node=create_node(val);
				new_node->next=this->head;
				this->head=new_node;
			}
			constexpr void push(value_type&& val){
				NodePtr new_node=create_node(static_cast<value_type&&>(val));
				new_node->next=this->head;
				this->head=new_node;
			}
			constexpr const_reference top()const noexcept{
				return this->head->data;
			}
			constexpr reference top()noexcept{
				return this->head->data;
			}
			constexpr bool empty()const noexcept{
				return this->head==nullptr;
			}
			constexpr value_type pop()noexcept(is_nothrow_move_constructible_v<value_type>){
				NodePtr aux=this->head;
				value_type aux_data=static_cast<value_type&&>(head->data);
				this->head=this->head->next;
				delete_node(aux);
				return aux_data;
			}
			constexpr void del()noexcept{
				NodePtr aux=this->head;
				this->head=this->head->next;
				delete_node(aux);
			}
			constexpr void clear()noexcept{
				while(head!=nullptr){
					NodePtr aux=head;
					head=head->next;
					delete_node(aux);
				}
			}
			~Stack()noexcept{
				clear();
			}
			constexpr iterator begin()noexcept{
				return NodePtr(head);
			}
			constexpr iterator last()noexcept{
				NodePtr p=head;
				if(head!=nullptr){
					while(p->next!=nullptr){
						p=p->next;
					}
					return p;
				}
				else return NodePtr(nullptr);
			}
			constexpr iterator end()noexcept{
				return NodePtr(nullptr);
			}
			constexpr const_iterator begin()const noexcept{
				return const_NodePtr(head);
			}
			constexpr const_iterator last()const noexcept{
				const_NodePtr p=head;
				if(head!=nullptr){
					while(p->next!=nullptr){
						p=p->next;
					}
					return const_iterator(p);
				}
				else return const_NodePtr(nullptr);
			}
			constexpr const_iterator end()const noexcept{
				return const_NodePtr(nullptr);
			}
	};
	template<typename Out,typename T,typename Alloc>
	Out& operator<<(Out& stream,const Stack<T,Alloc>& queue){
		stream<<"[";
		for(typename Stack<T,Alloc>::const_iterator i=queue.begin();i!=queue.end();){
			stream<<""<<*i;
			if(++i!=queue.end())
				stream<<", ";		
		}
		stream<<"]";
		return stream;
	}
#undef alloc
#undef head
}

namespace mpv{
    template<typename T,typename NodeAlloc>
    class HandleBase{
        public:
            using allocator_type=mpv::rebind_alloc<NodeAlloc,T>;
            using value_type=T;
        protected:
            using AlNode_traits=mpv::allocator_traits<NodeAlloc>;
            using NodePtr=typename AlNode_traits::pointer;
            NodePtr node=nullptr;
            mpv::Optional<NodeAlloc> node_alloc;
            void destroy()noexcept{
                if(node!=nullptr){
                    DESTROY(*node_alloc,node);
                    AlNode_traits::deallocate(*node_alloc,node,1);
                }
            }
            constexpr NodePtr drop()noexcept{
                NodePtr aux=node;
                node_alloc.reset();
                node=nullptr;
                return aux;
            }
        public:
            constexpr HandleBase()noexcept=default;
            HandleBase(const NodePtr node,const NodeAlloc& node_alloc)noexcept:node(node),node_alloc(node_alloc){}
            HandleBase(HandleBase&& other)noexcept:node(other.node),node_alloc(mpv::move(other.node_alloc)){
                other.node=nullptr;
                other.node_alloc.reset();
            }
            HandleBase& operator=(HandleBase&& other)noexcept{
                if(this==&other) return *this;
                destroy();
                node=other.node;
                if constexpr(is_move_assignable_v<NodeAlloc>){
                    if(AlNode_traits::propagate_on_container_move_assignment::value || !node_alloc.has_value()){
                        node_alloc=mpv::move(other.node_alloc);
                    }
                }
                other.node=nullptr;
                other.node_alloc.reset();
                return *this;
            }
            constexpr allocator_type get_allocator()const noexcept{
                return allocator_type(*node_alloc);
            }
            constexpr value_type& value()noexcept{
                return node->data;
            }
            constexpr const value_type& value()const noexcept{
                return node->data;
            }
            constexpr explicit operator bool()const noexcept{
                return node!=nullptr;
            }
            constexpr bool empty()const noexcept{
                return node==nullptr;
            }
            ~HandleBase(){
                destroy();
            }
            template<typename> friend class Tree;
    };
    template<typename K,typename Val,typename NodeAlloc>
    class NodeHandle:public HandleBase<Val,NodeAlloc>{
        public:
            using base=HandleBase<Val,NodeAlloc>;
            using typename base::NodePtr;
            using key_type=typename Val::key_type;
            using mapped_type=typename Val::mapped_type;
            constexpr NodeHandle()noexcept=default;
            NodeHandle(NodeHandle&&)noexcept=default;
            NodeHandle& operator=(NodeHandle&&)noexcept=default;
            NodeHandle(const NodePtr node,const NodeAlloc& node_alloc)noexcept:base(node,node_alloc){}

            constexpr const key_type& key()const noexcept{
                return this->node->data.key;
            }
            constexpr key_type& key()noexcept{
                return const_cast<key_type&>(this->node->data.key);
            }
            constexpr const mapped_type& mapped()const noexcept{
                return this->node->data.val;
            }
            constexpr mapped_type& mapped()noexcept{
                return this->node->data.val;
            }
    };
    template<typename K,typename NodeAlloc>
    class NodeHandle<K,K,NodeAlloc>:public HandleBase<K,NodeAlloc>{
        public:
            using base=HandleBase<K,NodeAlloc>;
            using typename base::NodePtr;
            using key_type=K;
            constexpr NodeHandle()noexcept=default;
            NodeHandle(NodeHandle&&)noexcept=default;
            NodeHandle& operator=(NodeHandle&&)noexcept=default;
            NodeHandle(const NodePtr node,const NodeAlloc& node_alloc)noexcept:base(node,node_alloc){}
    };
}
namespace mpv{
    enum RedOrBlack:bool{RED,BLACK};
    template<typename T,typename VoidPtr>struct Tree_Node;
    template<typename T,typename VoidPtr>
    struct Base_Tree_Node{
        using BaseNodePtr=rebind_pointer<VoidPtr,Base_Tree_Node>;
        using NodePtr=rebind_pointer<VoidPtr,Tree_Node<T,VoidPtr>>;
        using const_NodePtr=rebind_pointer<VoidPtr,const Tree_Node<T,VoidPtr>>;
        NodePtr prev=nullptr;
        NodePtr left=nullptr;
        NodePtr right=nullptr;
        RedOrBlack color=BLACK;
        constexpr NodePtr address()noexcept{
            return NodePtr(reinterpret_cast<Tree_Node<T,VoidPtr>*>(this));
        }
        constexpr const_NodePtr address()const noexcept{
            return const_NodePtr(reinterpret_cast<const Tree_Node<T,VoidPtr>*>(this));
        }
        constexpr void reset()noexcept{
            prev=left=right=nullptr;
            color=RED;
        }
        constexpr Base_Tree_Node()noexcept=default;
        Base_Tree_Node(const Base_Tree_Node&)=delete;
        Base_Tree_Node& operator=(const Base_Tree_Node&)=delete;
        constexpr Base_Tree_Node(RedOrBlack color)noexcept:color(color){}
    };
    template<typename T,typename VoidPtr>
    struct Temp_Tree_Node:public Base_Tree_Node<T,VoidPtr>{
        using NodePtr=typename Base_Tree_Node<T,VoidPtr>::NodePtr;
        NodePtr replace_with=nullptr;
        constexpr Temp_Tree_Node()noexcept:Base_Tree_Node<T,VoidPtr>(BLACK){}
        constexpr void link_to_right_of(NodePtr node)noexcept{
            replace_with=node->right;// node->right may be the sentinel(node would be right_most in this case) so we need to link it back 
            node->right=static_cast<NodePtr>(static_cast<typename Base_Tree_Node<T,VoidPtr>::BaseNodePtr>(this));
            this->prev=node;
        }
        constexpr void link_to_left_of(NodePtr node)noexcept{
            replace_with=node->left;// node->left may be the sentinel(node would be left_most in this case) so we need to link it back 
            node->left=static_cast<NodePtr>(static_cast<typename Base_Tree_Node<T,VoidPtr>::BaseNodePtr>(this));
            this->prev=node;
        }
        ~Temp_Tree_Node()noexcept{
            if(this->prev!=nullptr){
                if(this->prev->right==static_cast<typename Base_Tree_Node<T,VoidPtr>::NodePtr>(static_cast<typename Base_Tree_Node<T,VoidPtr>::BaseNodePtr>(this)))
                    this->prev->right=replace_with;
                else
                    this->prev->left=replace_with;
            }   
        }
    };
    template<typename T,typename VoidPtr>
    struct Tree_Node:public Base_Tree_Node<T,VoidPtr>{
        using NodePtr=typename Base_Tree_Node<T,VoidPtr>::NodePtr;
        using const_NodePtr=typename Base_Tree_Node<T,VoidPtr>::const_NodePtr;
        T data;
        template<typename... Args>
        constexpr Tree_Node(Args&&... args)noexcept(is_nothrow_constructible_v<T,Args...>):Base_Tree_Node<T,VoidPtr>(RED),data(static_cast<Args&&>(args)...){}
        Tree_Node(const Tree_Node&)=delete;
        Tree_Node& operator=(const Tree_Node&)=delete;
        constexpr NodePtr maximum()noexcept{
            NodePtr p=this;
            while(p->right)
                p=p->right;
            return p;
        }
        constexpr NodePtr minimum()noexcept{
            NodePtr p=this;
            while(p->left)
                p=p->left;
            return p;
        }
        constexpr const_NodePtr maximum()const noexcept{
            const_NodePtr p=this;
            while(p->right)
                p=p->right;
            return p;
        }
        constexpr const_NodePtr minimum()const noexcept{
            const_NodePtr p=this;
            while(p->left)
                p=p->left;
            return p;
        }
    };
    template<typename Types>
    class Tree_iterator{//in order
		public:
            using iterator_category=bidirectional_iterator_tag;
			using NodePtr=typename Types::NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=typename Types::reference;
        private:
            NodePtr ptr;
        public:
            constexpr Tree_iterator(NodePtr ptr)noexcept:ptr(ptr){}
            constexpr reference operator*()const noexcept{
                return ptr->data;
            }
            constexpr pointer operator->()const noexcept{
                return pointer_traits<pointer>::pointer_to(ptr->data);
            }
            constexpr Tree_iterator& operator++()noexcept{
                if(ptr->right!=nullptr){
                    if(ptr==ptr->right->left) ptr=ptr->right;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->right->minimum();
                }
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return *this;
            }
            constexpr Tree_iterator operator++(int)noexcept{
                Tree_iterator aux=*this;
                if(ptr->right!=nullptr){
                    if(ptr==ptr->right->left) ptr=ptr->right;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->right->minimum();
                }
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return aux;
            }
            constexpr Tree_iterator& operator--()noexcept{
                if(ptr->left!=nullptr){
                    if(ptr==ptr->left->right) ptr=ptr->left;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->left->maximum();
                }
                else if(ptr==ptr->prev->right)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return *this;
            }
            constexpr Tree_iterator operator--(int)noexcept{
                Tree_iterator aux=*this;
                if(ptr->left!=nullptr){
                    if(ptr==ptr->left->right) ptr=ptr->left;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->left->maximum();
                }
                else if(ptr==ptr->prev->right)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return aux;
            }
            constexpr bool operator==(const Tree_iterator& other)const noexcept{
                return this->ptr==other.ptr;
            }
            constexpr bool operator!=(const Tree_iterator& other)const noexcept{
                return this->ptr!=other.ptr;
            }
            template<typename> friend class const_Tree_iterator;
            template<typename> friend class Tree;
    };
    template<typename Types>
    class const_Tree_iterator{//in order
		public:
            using iterator_category=bidirectional_iterator_tag;
			using NodePtr=typename Types::const_NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
        private:
            NodePtr ptr;
        public:
            constexpr const_Tree_iterator(NodePtr ptr)noexcept:ptr(ptr){}
            constexpr const_Tree_iterator(Tree_iterator<Types> nonconst_it)noexcept:ptr(nonconst_it.ptr){}
            constexpr reference operator*()const noexcept{
                return ptr->data;
            }
            constexpr pointer operator->()const noexcept{
                return pointer_traits<pointer>::pointer_to(ptr->data);
            }
            constexpr const_Tree_iterator& operator++()noexcept{
                if(ptr->right!=nullptr){
                    if(ptr==ptr->right->left) ptr=ptr->right;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->right->minimum();
                }
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return *this;
            }
            constexpr const_Tree_iterator operator++(int)noexcept{
                const_Tree_iterator aux=*this;
                if(ptr->right!=nullptr){
                    if(ptr==ptr->right->left) ptr=ptr->right;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->right->minimum();
                }
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return aux;
            }
            constexpr const_Tree_iterator& operator--()noexcept{
                if(ptr->left!=nullptr){
                    if(ptr==ptr->left->right) ptr=ptr->left;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->left->maximum();
                }
                else if(ptr==ptr->prev->right)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return *this;
            }
            constexpr const_Tree_iterator operator--(int)noexcept{
                const_Tree_iterator aux=*this;
                if(ptr->left!=nullptr){
                    if(ptr==ptr->left->right) ptr=ptr->left;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->left->maximum();
                }
                else if(ptr==ptr->prev->right)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return aux;
            }
            constexpr bool operator==(const const_Tree_iterator& other)const noexcept{
                return this->ptr==other.ptr;
            }
            constexpr bool operator!=(const const_Tree_iterator& other)const noexcept{
                return this->ptr!=other.ptr;
            }
            template<typename> friend class const_Tree_iterator;
            template<typename> friend class Tree;
    };
    template<typename Traits>
    class Tree COUNT_IT{
        public:
            using value_type=typename Traits::value_type;
            using key_type=typename Traits::key_type;
            using allocator_type=typename Traits::allocator_type;
            using key_compare=typename Traits::key_compare;
            using value_compare=typename Traits::value_compare;
            static constexpr bool allow_repeated=Traits::allow_repeated;
		protected:
            using Alloc=allocator_type;
			using Node=Tree_Node<value_type,typename allocator_traits<Alloc>::void_pointer>;
            using TempNode=Temp_Tree_Node<value_type,typename allocator_traits<Alloc>::void_pointer>;
            using BaseNode=Base_Tree_Node<value_type,typename allocator_traits<Alloc>::void_pointer>;
			using AlTy=rebind_alloc<Alloc,value_type>;
			using AlTy_traits=allocator_traits<AlTy>;
			using AlNode=rebind_alloc<Alloc,Node>;
			using AlNode_traits=allocator_traits<AlNode>;
			using NodePtr=typename AlNode_traits::pointer;
            using const_NodePtr=typename AlNode_traits::const_pointer;
			struct Val_types{
				using value_type=typename Traits::value_type;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=value_type&;
				using const_reference=const value_type&;
				using NodePtr=typename AlNode_traits::pointer;
                using const_NodePtr=typename AlNode_traits::const_pointer;
			};
            static constexpr bool POCCA=AlTy_traits::propagate_on_container_copy_assignment::value;
            static constexpr bool POCMA=AlTy_traits::propagate_on_container_move_assignment::value;
            static constexpr bool POCS=AlTy_traits::propagate_on_container_swap::value;
            static constexpr bool ALWAYS_EQ=AlTy_traits::is_always_equal::value;
		public:
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
            using const_iterator=const_Tree_iterator<Val_types>;
            using iterator=If_t<is_same_v<key_type,value_type>,const_iterator,Tree_iterator<Val_types>>;
            using node_type=NodeHandle<key_type,value_type,AlNode>;
		protected:
            struct handle_value_compare:private EBCO<value_compare>{
                constexpr handle_value_compare(key_compare cmp):EBCO<value_compare>(mpv::move(cmp)){}
                constexpr bool operator()(const node_type& fst,const value_type& scnd)const{
                    return this->get_val()(fst.value(),scnd);
                }
                constexpr bool operator()(const value_type& fst,const node_type& scnd)const{
                    return this->get_val()(fst,scnd.value());
                }
            };
            CompressedTriple<key_compare,AlNode,BaseNode> ct;
            size_type length=0;
#define alloc       ct.getV2()
#define root        ct.getV3().prev
#define left_most   ct.getV3().right
#define right_most  ct.getV3().left
#define sent        ct.getV3().address()
            constexpr static bool is_red(NodePtr p)noexcept{
                return p!=nullptr && p->color==RED;
            }
            constexpr static bool is_black(NodePtr p)noexcept{
                return p==nullptr || p->color==BLACK;
            }
            constexpr bool is_null(NodePtr p)const noexcept{
                return p==nullptr || p==sent;
            }
            constexpr void reset()noexcept{
                length=0;
                root=left_most=right_most=sent;
            }
            constexpr bool cmp(const key_type& a,const key_type& b)const{
            	return ct.getV1()(a,b);
			}
			template<typename... Args>
			constexpr NodePtr create_node(Args&&... args){
                AllocConstructPtr guard(alloc);
                guard.allocate();
				CONSTRUCT_VARARGS(this->alloc,guard.ptr,static_cast<Args&&>(args));
				return guard.release();
			}
            constexpr NodePtr create_node(node_type&& handle)noexcept(ALWAYS_EQ){
                if constexpr(ALWAYS_EQ) return handle.drop();
                else{
                    if(alloc==handle.node_alloc) return handle.drop();
                    else return create_node(static_cast<value_type&&>(handle.value()));
                }
            }
			constexpr void delete_node(NodePtr node)noexcept{
				DESTROY(this->alloc,node);
				AlNode_traits::deallocate(this->alloc,node,1);
			}
            constexpr void clr(NodePtr node)noexcept{
                if(is_null(node)) return;
                else{
                    clr(node->left);
                    clr(node->right);
                    delete_node(node);
                }
            }
            template<typename Lambda>
            constexpr void pre_order_helper(NodePtr node,Lambda& func)const{
                if(is_null(node)) return;
                func(node->data);
                pre_order_helper(node->left,func);
                pre_order_helper(node->right,func);
            }
            template<typename Lambda>
            constexpr void in_order_helper(NodePtr node,Lambda& func)const{
                if(is_null(node)) return;
                in_order_helper(node->left,func);
                func(node->data);
                in_order_helper(node->right,func);
            }
            template<typename Lambda>
            constexpr void post_order_helper(NodePtr node,Lambda& func)const{
                if(is_null(node)) return;
                post_order_helper(node->left,func);
                post_order_helper(node->right,func);
                func(node->data);
            }
            constexpr void insert_fixup(NodePtr x)noexcept{
                while(is_red(x->prev)){
                    if(x->prev==x->prev->prev->left){															//If the father is a left node
                        if(is_red(x->prev->prev->right)){			//case 1:X.uncle==RED
                            x->prev->color=BLACK;
                            x->prev->prev->right->color=BLACK;
                            x->prev->prev->color=RED;
                            x=x->prev->prev;
                        }
                        else/*(x->prev->prev->right->color==BLACK)*/{//case 2 & 3:x.uncle==BLACK
                            if(x==x->prev->right){							//case 2:(triangle)
                                x=x->prev;
                                left_rotate(x);
                            }
                            x->prev->color=BLACK;							//case 3:(line)
                            x->prev->prev->color=RED;
                            right_rotate(x->prev->prev);
                        }
                    }
                    else{																						//If the father is a right node
                        if(is_red(x->prev->prev->left)){					//case 1:X.uncle==RED
                            x->prev->color=BLACK;
                            x->prev->prev->left->color=BLACK;
                            x->prev->prev->color=RED;
                            x=x->prev->prev;
                        }
                        else/*(x->prev->prev->left->color==BLACK)*/{			//case 2 & 3:x.uncle==BLACK
                            if(x==x->prev->left){							//case 2:(triangle)
                                x=x->prev;
                                right_rotate(x);    // Case 2 always leads to case 3
                            }
                            x->prev->color=BLACK;							//case 3:(line)
                            x->prev->prev->color=RED;
                            left_rotate(x->prev->prev);
                        }
                    }
                }
                this->root->color=BLACK;									//case 0:root==RED
            }
            constexpr void delete_fixup(NodePtr x)noexcept{
                while(x!=root && x->color==BLACK){
                    if(x==x->prev->left){
                        NodePtr w=x->prev->right;
                        if(is_red(w)){
                            w->color=BLACK;
                            x->prev->color=RED;
                            left_rotate(x->prev);
                            w=x->prev->right;
                        }
                        if(is_black(w->left) && is_black(w->right)){
                            w->color=RED;
                            x=x->prev;
                        }
                        else{
                            if(is_black(w->right)){
                                w->left->color=BLACK;
                                w->color=RED;
                                right_rotate(w);
                                w=x->prev->right;
                            }
                            w->color=x->prev->color;
                            x->prev->color=BLACK;
                            w->right->color=BLACK;
                            left_rotate(x->prev);
                            x=root;
                        }
                    }
                    else{
                        NodePtr w=x->prev->left;
                        if(is_red(w)){
                            w->color=BLACK;
                            x->prev->color=RED;
                            right_rotate(x->prev);
                            w=x->prev->left;
                        }
                        if(is_black(w->left) && is_black(w->right)){
                            w->color=RED;
                            x=x->prev;
                        }
                        else{
                            if(is_black(w->left)){
                                w->right->color=BLACK;
                                w->color=RED;
                                left_rotate(w);
                                w=x->prev->left;
                            }
                            w->color=x->prev->color;
                            x->prev->color=BLACK;
                            w->left->color=BLACK;
                            right_rotate(x->prev);
                            x=root;
                        }
                    }
                }
                x->color=BLACK;
            }
            constexpr void right_rotate(NodePtr x)noexcept{
                NodePtr y=x->left;
                if(x->prev==nullptr)
                    this->root=y;
                else{
                    if(x==x->prev->right)
                        x->prev->right=y;
                    else/*(x==x->prev->left)*/
                        x->prev->left=y;
                }
                y->prev=x->prev;
                x->left=y->right;
                //assert(x->left!=sent);
                if(x->left!=nullptr)
                    x->left->prev=x;
                y->right=x;
                x->prev=y;
            }
            constexpr void left_rotate(NodePtr x)noexcept{
                NodePtr y=x->right;
                if(x->prev==nullptr)
                    this->root=y;	
                else{
                    if(x==x->prev->left)
                        x->prev->left=y;
                    else/*(x==x->prev->right)*/
                        x->prev->right=y;
                }
                y->prev=x->prev;
                x->right=y->left;
                //assert(x->right!=sent);
                if(x->right!=nullptr)
                    x->right->prev=x;
                y->left=x;
                x->prev=y;
            }
            constexpr void transplant(NodePtr u,NodePtr v)noexcept{//Desenlaza u con su padre y enlaza al padre con v
                if(u->prev==nullptr)
                    this->root=v;
                else if(u==u->prev->left)
                    u->prev->left=v;
                else
                    u->prev->right=v;
                if(v!=nullptr)
                    v->prev=u->prev;
            }
            template<typename Lambda>
            constexpr void iterative_in_order(Lambda&& func=Lambda{}){
                if(root==sent) return;
                NodePtr ptr=left_most;
                while(true){
                    func(ptr->data);
                    if(ptr->right!=nullptr){
                        if(ptr==ptr->right->left) return;
                        else ptr=ptr->right->minimum();
                    }
                    else if(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    else{
                        while(ptr==ptr->prev->right)
                            ptr=ptr->prev;
                        ptr=ptr->prev;
                    }
                }
            }
            constexpr void iterative_deletion()noexcept{
                if(root==sent) return;
                NodePtr ptr=left_most,aux;
                while(true){
                    if(ptr->right!=nullptr){
                        if(ptr->right==sent){
                            while(ptr!=root){
                                aux=ptr;
                                ptr=ptr->prev;
                                delete_node(aux);
                            }
                            delete_node(ptr);
                            return;
                        }
                        else{
                            ptr=ptr->right->minimum();
                        } 
                    }
                    else if(ptr==ptr->prev->left){
                        aux=ptr;
                        ptr=ptr->prev;
                        delete_node(aux);
                    }
                    else{
                        while(ptr==ptr->prev->right){
                            aux=ptr;
                            ptr=ptr->prev;
                            delete_node(aux);
                        }
                        aux=ptr;
                        ptr=ptr->prev;
                        delete_node(aux);
                    }
                }
            }
            template<typename Lambda>
            constexpr void iterative_in_order(Lambda&& func=Lambda{})const{
                if(root==sent) return;
                const_NodePtr ptr=left_most;
                while(true){
                    func(ptr->data);
                    if(ptr->right!=nullptr){
                        if(ptr==ptr->right->left) return;
                        else ptr=ptr->right->minimum();
                    }
                    else if(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    else{
                        while(ptr==ptr->prev->right)
                            ptr=ptr->prev;
                        ptr=ptr->prev;
                    }
                }
            }
            struct CpyFunc{
                Tree& self;
                constexpr CpyFunc(Tree& ref)noexcept:self(ref){}
                constexpr void operator()(const value_type& v){
                    self.insert(v);
                }
            };
            struct MovFunc{
                Tree& self;
                constexpr MovFunc(Tree& ref)noexcept:self(ref){}
                constexpr void operator()(value_type& v){
                    self.insert(static_cast<value_type&&>(v));
                }
            };
            template<typename val_type>
            constexpr iterator insert_without_repeated(val_type&& val){//universal reference of value_type
                if constexpr(is_same_v<val_type,node_type>){
                    if(val.empty()) return end();
                }
                NodePtr x;
                If_t<is_same_v<val_type,node_type>,handle_value_compare,value_compare> vcmp(ct.getV1());
                if(root==sent){
                    x=root=left_most=right_most=create_node(static_cast<val_type&&>(val));
                    x->right=x->left=sent;
                }
                else if(!vcmp(val,right_most->data)){//val>=right_most->data
                    if(vcmp(right_most->data,val)){//val>right_most->data
                        right_most->right=create_node(static_cast<val_type&&>(val));
                        right_most->right->prev=right_most;
                        right_most=right_most->right;
                        right_most->right=sent;
                        x=right_most;
                    }
                    else/*val==right_most->data*/ return iterator(right_most);
                }
                else if(!vcmp(left_most->data,val)){//val<=left_most->data
                    if(vcmp(val,left_most->data)){//val<left_most->data
                        left_most->left=create_node(static_cast<val_type&&>(val));
                        left_most->left->prev=left_most;
                        left_most=left_most->left;
                        left_most->left=sent;
                        x=left_most;
                    }
                    else/*val==left_most->data*/ return iterator(left_most);
                }
                else{
                    x=root;
                    while(true){
                        if(vcmp(val,x->data)){
                            if(x->left==nullptr){
                                x->left=create_node(static_cast<val_type&&>(val));
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else if(vcmp(x->data,val)){
                            if(x->right==nullptr){
                                x->right=create_node(static_cast<val_type&&>(val));
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                        else return iterator(x);
                    }
                }
                length++;
                this->insert_fixup(x);
                return iterator(x);
            }
            template<typename val_type>
            constexpr iterator insert_with_repeated(val_type&& val){//universal reference of value_type
                if constexpr(is_same_v<val_type,node_type>){
                    if(val.empty()) return end();
                }
                NodePtr x;
                If_t<is_same_v<val_type,node_type>,handle_value_compare,value_compare> vcmp(ct.getV1());
                if(root==sent){
                    x=root=left_most=right_most=create_node(static_cast<val_type&&>(val));
                    x->right=x->left=sent;
                }
                else if(!vcmp(val,right_most->data)){//val>=right_most->data
                    right_most->right=create_node(static_cast<val_type&&>(val));
                    right_most->right->prev=right_most;
                    right_most=right_most->right;
                    right_most->right=sent;
                    x=right_most;
                }
                else if(vcmp(val,left_most->data)){//val<left_most->data
                    left_most->left=create_node(static_cast<val_type&&>(val));
                    left_most->left->prev=left_most;
                    left_most=left_most->left;
                    left_most->left=sent;
                    x=left_most;
                }
                else{
                    x=root;
                    while(true){
                        if(vcmp(val,x->data)){
                            if(x->left==nullptr){
                                x->left=create_node(static_cast<val_type&&>(val));
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else{
                            if(x->right==nullptr){
                                x->right=create_node(static_cast<val_type&&>(val));
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                    }
                }
                length++;
                this->insert_fixup(x);
                return iterator(x);
            }
        public:
            constexpr Tree()noexcept(is_nothrow_default_constructible_v<AlTy> && is_nothrow_default_constructible_v<key_compare>){root=left_most=right_most=sent;}
            constexpr explicit Tree(const Alloc& al)noexcept(is_nothrow_default_constructible_v<key_compare>):ct(arg2_tag{},al){root=left_most=right_most=sent;}
            constexpr explicit Tree(const key_compare& comp,const Alloc& al=Alloc{})noexcept:ct(arg1and2_tag{},comp,al){root=left_most=right_most=sent;}
            template<typename It, enable_if_t<is_iterator_v<It>,int> = 0>
            constexpr Tree(It first,It last,const key_compare& comp=key_compare{},const Alloc& al=Alloc{}):ct(arg1and2_tag{},comp,al){
                root=left_most=right_most=sent;
                while(first!=last) insert(*first++);
            }
            constexpr Tree(std::initializer_list<value_type> ilist,const key_compare& comp=key_compare{},const Alloc& al=Alloc{}):ct(arg1and2_tag{},comp,al){
                root=left_most=right_most=sent;
                for(const_reference v:ilist) insert(v);
            }
            constexpr Tree(const Tree& other):ct(arg1and2_tag{},other.ct.getV1(),AlNode_traits::select_on_container_copy_construction(other.alloc)){
                root=left_most=right_most=sent;
                other.iterative_in_order(CpyFunc(*this));
            }
            constexpr Tree(const Tree& other,const Alloc& al):ct(arg1and2_tag{},other.ct.getV1(),al){
                root=left_most=right_most=sent;
                other.iterative_in_order(CpyFunc(*this));
            }
            constexpr Tree(Tree&& other)noexcept:ct(arg1and2_tag{},static_cast<key_compare&&>(other.ct.getV1()),static_cast<AlNode&&>(other.alloc)),length(other.length){
                if(other.root==other.sent) root=left_most=right_most=sent;
                else{
                    root=other.root;
                    left_most=other.left_most;
                    right_most=other.right_most;
                    left_most->left=right_most->right=sent;
                    other.reset();
                }
            }
            constexpr Tree(Tree&& other,const Alloc& al)noexcept(ALWAYS_EQ):ct(arg1and2_tag{},static_cast<key_compare&&>(other.ct.getV1()),al){
                if(this->alloc==other.alloc){
                    length=other.length;
                    if(other.root==other.sent) root=left_most=right_most=sent;
                    else{
                        root=other.root;
                        left_most=other.left_most;
                        right_most=other.right_most;
                        left_most->left=right_most->right=sent;
                        other.reset();
                    }                    
                }
                else{
                    root=left_most=right_most=sent;
                    other.iterative_in_order(MovFunc(*this));
                    other.clear();
                }
            }
            constexpr Tree& operator=(const Tree& other){
                if(this==&other) return *this;
                this->clear();
                this->ct.getV1()=other.ct.getV1();
                pocca(this->alloc,other.alloc);
                other.iterative_in_order(CpyFunc(*this));
                return *this;
            }
            constexpr Tree& operator=(Tree&& other)noexcept(ALWAYS_EQ || POCMA){
                if(this==&other) return *this;
                this->clear();
                this->ct.getV1()=static_cast<key_compare&&>(other.ct.getV1());
                if constexpr(!ALWAYS_EQ && !POCMA){
                    if(this->alloc!=other.alloc){
                        other.iterative_in_order(MovFunc(*this));
                        other.clear();
                        return *this;
                    }
                }
                pocma(this->alloc,other.alloc);
                length=other.length;
                if(other.root!=other.sent){
                    root=other.root;
                    left_most=other.left_most;
                    right_most=other.right_most;
                    left_most->left=right_most->right=sent;
                    other.reset();
                }
                return *this;
            }
            constexpr iterator insert(const value_type& val){
                if constexpr(allow_repeated) return insert_with_repeated(val);
                else return insert_without_repeated(val);
            }
            constexpr iterator insert(value_type&& val){
                if constexpr(allow_repeated) return insert_with_repeated(static_cast<value_type&&>(val));
                else return insert_without_repeated(static_cast<value_type&&>(val));
            }
            constexpr iterator insert(node_type&& handle){
                if constexpr(allow_repeated) return insert_with_repeated(static_cast<node_type&&>(handle));
                else return insert_without_repeated(static_cast<node_type&&>(handle));
            }
            template<typename It>
            constexpr enable_if_t<is_iterator_v<It>> insert(It first,It last){
                while(first!=last)
                    insert(*first++);
            }
            template<typename... Args>
            constexpr iterator emplace(Args&&... args){
                value_type val(static_cast<Args&&>(args)...);
                if constexpr(allow_repeated) return insert_with_repeated(static_cast<value_type&&>(val));
                else return insert_without_repeated(static_cast<value_type&&>(val));
            }
            constexpr bool del(const_iterator pos)noexcept{//pos should be an iterator to an element of this or the end() iterator.
                if(pos==end()) return false;
                NodePtr z=const_cast<NodePtr>(pos.ptr);//the node that will be deleted
                if(z==root && z->left==sent && z->right==sent){
                    delete_node(z);
                    reset();
                    return true;
                }
                NodePtr x;//the node that repaces z
                RedOrBlack original_color=z->color;
                TempNode tempnode;
                if(is_null(z->left) && is_null(z->right)){
                    if(z->right==sent){
                        right_most=z->prev;
                        tempnode.link_to_right_of(z);
                        x=z->right;
                    }
                    else if(z->left==sent){
                        left_most=z->prev;
                        tempnode.link_to_left_of(z);
                        x=z->left;
                    }
                    else{
                        tempnode.link_to_left_of(z);
                        x=z->left;
                    }
                    transplant(z,x);
                }
                else if(is_null(z->left)){
                    x=z->right;
                    transplant(z,x);
                    if(left_most==z){
                        left_most=x;
                        x->left=sent;
                    }
                }
                else if(is_null(z->right)){
                    x=z->left;
                    transplant(z,x);
                    if(right_most==z){
                        right_most=x;
                        x->right=sent;
                    }
                }
                else{// In this case, z isn't right_most or left_most
                    NodePtr y=z->right->minimum();
                    original_color=y->color;
                    if(is_null(y->right)){
                        tempnode.link_to_right_of(y);
                    }
                    x=y->right;
                    if(y->prev!=z){
                        transplant(y, x);
                        y->right=z->right;
                        y->right->prev=y;
                    }
                    transplant(z,y);
                    y->left=z->left;
                    y->left->prev=y;
                    y->color=z->color;
                }
                delete_node(z);
                if(original_color==BLACK)
                    delete_fixup(x);
                length--;
                return true;
            }
            constexpr node_type extract(const_iterator pos)noexcept{//pos should be an iterator to an element of this or the end() iterator.
                if(pos==end()) return {};
                NodePtr z=const_cast<NodePtr>(pos.ptr);
                if(z==root && z->left==sent && z->right==sent){
                    z->reset();
                    reset();
                    return node_type(z,alloc);
                }
                NodePtr x;//the node that repaces z
                RedOrBlack original_color=z->color;
                TempNode tempnode;
                if(is_null(z->left) && is_null(z->right)){
                    if(z->right==sent){
                        right_most=z->prev;
                        tempnode.link_to_right_of(z);
                        x=z->right;
                    }
                    else if(z->left==sent){
                        left_most=z->prev;
                        tempnode.link_to_left_of(z);
                        x=z->left;
                    }
                    else{
                        tempnode.link_to_left_of(z);
                        x=z->left;
                    }
                    transplant(z,x);
                }
                else if(is_null(z->left)){
                    x=z->right;
                    transplant(z,x);
                    if(left_most==z){
                        left_most=x;
                        x->left=sent;
                    }
                }
                else if(is_null(z->right)){
                    x=z->left;
                    transplant(z,x);
                    if(right_most==z){
                        right_most=x;
                        x->right=sent;
                    }
                }
                else{// In this case, z isn't right_most or left_most
                    NodePtr y=z->right->minimum();
                    original_color=y->color;
                    if(is_null(y->right)){
                        tempnode.link_to_right_of(y);
                    }
                    x=y->right;
                    if(y->prev!=z){
                        transplant(y, x);
                        y->right=z->right;
                        y->right->prev=y;
                    }
                    transplant(z,y);
                    y->left=z->left;
                    y->left->prev=y;
                    y->color=z->color;
                }
                z->reset();
                if(original_color==BLACK)
                    delete_fixup(x);
                length--;
                return node_type(z,alloc);
            }
            constexpr node_type extract(const key_type& val){
                return extract(find(val));
            }
            constexpr Optional<value_type> pop(const_iterator pos)noexcept(is_nothrow_move_constructible_v<value_type>){
                if(pos==end()) return {};
                NodePtr z=const_cast<NodePtr>(pos.ptr);//the node that will be deleted
                if(z==root && z->left==sent && z->right==sent){
                    value_type ret=static_cast<value_type&&>(z->data);
                    delete_node(z);
                    reset();
                    return ret;
                }
                NodePtr x;//the node that repaces z
                RedOrBlack original_color=z->color;
                TempNode tempnode;
                if(is_null(z->left) && is_null(z->right)){
                    if(z->right==sent){
                        right_most=z->prev;
                        tempnode.link_to_right_of(z);
                        x=z->right;
                    }
                    else if(z->left==sent){
                        left_most=z->prev;
                        tempnode.link_to_left_of(z);
                        x=z->left;
                    }
                    else{
                        tempnode.link_to_left_of(z);
                        x=z->left;
                    }
                    transplant(z,x);
                }
                else if(is_null(z->left)){
                    x=z->right;
                    transplant(z,x);
                    if(left_most==z){
                        left_most=x;
                        x->left=sent;
                    }
                }
                else if(is_null(z->right)){
                    x=z->left;
                    transplant(z,x);
                    if(right_most==z){
                        right_most=x;
                        x->right=sent;
                    }
                }
                else{// In this case, z isn't right_most or left_most
                    NodePtr y=z->right->minimum();
                    original_color=y->color;
                    if(is_null(y->right)){
                        tempnode.link_to_right_of(y);
                    }
                    x=y->right;
                    if(y->prev!=z){
                        transplant(y, x);
                        y->right=z->right;
                        y->right->prev=y;
                    }
                    transplant(z,y);
                    y->left=z->left;
                    y->left->prev=y;
                    y->color=z->color;                    
                }
                value_type ret=static_cast<value_type&&>(z->data);
                delete_node(z);
                if(original_color==BLACK)
                    delete_fixup(x);
                length--;
                return ret;
            }
            constexpr bool del_elem(const key_type& val){
                return del(find(val));
            }
            constexpr Optional<value_type> pop_elem(const key_type& val){
                return pop(find(val));
            }
            constexpr iterator erase(const_iterator first,const_iterator last)noexcept{
                while(first!=last){
                    iterator aux(first.ptr);
                    ++first;
                    del(aux);
                }
                return last;
            }
            constexpr bool contains(const key_type& val)const{
                if(root==sent || cmp(Traits::getKey(right_most->data),val) || cmp(val,Traits::getKey(left_most->data))) return false;
                else if((!cmp(val,Traits::getKey(right_most->data)) && !cmp(Traits::getKey(right_most->data),val)) || (!cmp(val,Traits::getKey(left_most->data)) && !cmp(Traits::getKey(left_most->data),val)))/*val==right_most->data || val==left_most->data*/ return true;
                NodePtr x=root;
                while(true){
                    if(cmp(val,Traits::getKey(x->data))){
                        if(x->left==nullptr)
                            return false;						
                        else
                            x=x->left;
                    }
                    else if(cmp(Traits::getKey(x->data),val)){
                        if(x->right==nullptr)
                            return false;							
                        else
                            x=x->right;
                    }
                    else return true;
                }
            }
            constexpr iterator find(const key_type& val){
                if(root==sent || cmp(Traits::getKey(right_most->data),val) || cmp(val,Traits::getKey(left_most->data))) return end();
                if constexpr(!allow_repeated){
                    if(!cmp(val,Traits::getKey(right_most->data)) && !cmp(Traits::getKey(right_most->data),val))/*val==right_most->data*/ return iterator(right_most);
                    else if(!cmp(val,Traits::getKey(left_most->data)) && !cmp(Traits::getKey(left_most->data),val))/*val==left_most->data*/ return iterator(left_most);
                }
                NodePtr x=root;
/*  Aclaracion
    if(x->left==nullptr) y su contraparte con right estan bien asi.
    No hace falta hacer is_null(x->left) ni is_null(x->right) porque el if de la primera linea checkea si val es mayor al
    elemento mas grande o menor al elemento mas chico y deviuelve this->end() si alguna es verdadera, por lo que nunca se 
    va a necesitar leer left_most->left ni right_mosat->right, que son los unicos nodos nulos que no apuntan a nullptr sino 
    que apuntan al nodo sentinela
*/
                while(true){
                    if(cmp(val,Traits::getKey(x->data))){
                        if(x->left==nullptr)
                            return end();
                        else
                            x=x->left;
                    }
                    else if(cmp(Traits::getKey(x->data),val)){
                        if(x->right==nullptr)
                            return end();							
                        else
                            x=x->right;
                    }
                    else{
                        if constexpr(!allow_repeated) return iterator(x);
                        else{
                            iterator it(x),aux(x);
                            --aux;
                            while(aux!=end() && !(cmp(Traits::getKey(*aux),val) || cmp(val,Traits::getKey(*aux)))){
                                it=aux;
                                --aux;
                            }
                            return it;
                        }
                    }
                }
            }
            constexpr const_iterator find(const key_type& val)const{
                if(root==sent || cmp(Traits::getKey(right_most->data),val) || cmp(val,Traits::getKey(left_most->data))) return end();
                if constexpr(!allow_repeated){
                    if(!cmp(val,Traits::getKey(right_most->data)) && !cmp(Traits::getKey(right_most->data),val))/*val==right_most->data*/ return iterator(right_most);
                    else if(!cmp(val,Traits::getKey(left_most->data)) && !cmp(Traits::getKey(left_most->data),val))/*val==left_most->data*/ return iterator(left_most);
                }
                NodePtr x=root;
                while(true){
                    if(cmp(val,Traits::getKey(x->data))){
                        if(x->left==nullptr)
                            return end();
                        else
                            x=x->left;
                    }
                    else if(cmp(Traits::getKey(x->data),val)){
                        if(x->right==nullptr)
                            return end();							
                        else
                            x=x->right;
                    }
                    else{
                        if constexpr(!allow_repeated) return const_iterator(x);
                        else{
                            const_iterator it(x),aux(x);
                            --aux;
                            while(aux!=end() && !(cmp(Traits::getKey(*aux),val) || cmp(val,Traits::getKey(*aux)))){
                                it=aux;
                                --aux;
                            }
                            return it;
                        }
                    }
                }
            }
            template<bool enable=allow_repeated>
            constexpr enable_if_t<enable,size_type> count(const key_type& val)const{
                const_iterator it=find(val);
                if(it==end())return 0;
                else{
                    size_type count=1;
                    ++it;
                    while(it!=end() && !(cmp(Traits::getKey(*it),val) || cmp(val,Traits::getKey(*it)))){
                        ++it;
                        ++count;
                    }
                    return count;
                }
            }
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const{
                for(const_reference x:*this)
                    if(func(x))return 1;
                return 0;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{}){
                if constexpr(is_same_v<key_type,value_type>) const_cast<const Tree*>(this)->iterative_in_order(func);
                else iterative_in_order(func);
			}            
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{})const{
                iterative_in_order(func);
			}
			template<typename Lambda>
			constexpr Tree filter(Lambda&& func=Lambda{})const{
				Tree new_tree(ct.getV1(),AlNode_traits::select_on_container_copy_construction(alloc));
                auto f=[&new_tree,&func](const value_type& v){if(func(v))new_tree.insert(v);};
                this->iterative_in_order(f);
				return new_tree;
			}
            template<typename Lambda>
            constexpr void pre_order(Lambda&& func=Lambda{})const{
                pre_order_helper(root,func);
            }
            template<typename Lambda>
            constexpr void in_order(Lambda&& func=Lambda{})const{
                in_order_helper(root,func);
            }
            template<typename Lambda>
            constexpr void post_order(Lambda&& func=Lambda{})const{
                post_order_helper(root,func);
            }
			constexpr bool operator==(const Tree& other)const{
				if(this->length!=other.length) return false;
				else return equal(this->begin(),this->end(),other.begin());
			}
			constexpr bool operator!=(const Tree& other)const{
				if(this->length!=other.length) return true;
				else return !equal(this->begin(),this->end(),other.begin());
			}
            constexpr bool operator<(const Tree& other)const{
                return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool operator>(const Tree& other)const{
                return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator<=(const Tree& other)const{
                return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator>=(const Tree& other)const{
                return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool empty()const noexcept{
                return length==0;
            }
            constexpr iterator get_root()noexcept{
                return iterator(root);
            }
            constexpr const_iterator get_root()const noexcept{
                return const_iterator(root);
            }
            constexpr iterator get_min()noexcept{
                return iterator(left_most);
            }
            constexpr const_iterator get_min()const noexcept{
                return const_iterator(left_most);
            }
            constexpr iterator get_max()noexcept{
                return iterator(right_most);
            }
            constexpr const_iterator get_max()const noexcept{
                return const_iterator(right_most);
            }
            constexpr allocator_type get_allocator()const noexcept{
                return this->alloc;
            }
            constexpr size_type get_size()const{
                size_t len=0;
                iterative_in_order([&len](const value_type&){++len;});
                return len;
            }
            constexpr size_type size()const noexcept{
#if defined(DEBUG) && (defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_))
                if(length!=get_size()){
                    std::cerr<<"\nSIZE AND NODE_COUNT DO NOT MATCH\a\n"
                            <<"LEN: "<<length<<"\nNODE_COUNT: "<<get_size()<<std::endl;
                    exit(-1);
                }
#endif
                return length;
            }
            constexpr void clear()noexcept{
                iterative_deletion();
                reset();
            }
            ~Tree()noexcept{
                iterative_deletion();
            }
            constexpr const_iterator begin()const noexcept{
                return const_iterator(left_most);
            }
            constexpr const_iterator end()const noexcept{
                return const_iterator(sent);
            }
            constexpr iterator begin()noexcept{
                return iterator(left_most);
            }
            constexpr iterator end()noexcept{
                return iterator(sent);
            }
            template<typename Out,typename Traits_> friend Out& operator<<(Out&,const Tree<Traits_>&);

#undef root
#undef alloc
#undef left_most
#undef right_most
#undef sent
    };
    template<typename Out,typename Traits>
    Out& operator<<(Out& stream,const Tree<Traits>& tree){
        stream<<"[";
        auto it(tree.begin());
        auto it_end(tree.end());
        while(it!=it_end){
            stream<<*it;
            if(++it!=it_end)
                stream<<", ";
        }
        stream<<"]";
        return stream;
    }
}

namespace mpv{
	template<typename keyT,typename valT>
    struct MapPair COUNT_IT{
        using mapped_type=valT;
        using key_type=keyT;
        mapped_type val;
        const key_type key;
        MapPair()=delete;
        template<typename kT,typename vT,enable_if_t<is_convertible_v<kT,key_type> && is_convertible_v<vT,mapped_type>>* = nullptr>
        constexpr MapPair(kT&& key,vT&& val):val(static_cast<vT&&>(val)),key(static_cast<kT&&>(key)){}
        template<typename kT,enable_if_t<is_convertible_v<kT,key_type>>* = nullptr>
        constexpr explicit MapPair(kT&& key):key(static_cast<kT&&>(key)){}
    };
    template<typename K,typename T,typename Cmp,typename Alloc,bool repeated> struct TreeMapTraits{
        using key_type=K;
        using value_type=MapPair<K,T>;
        using key_compare=Cmp;
        using allocator_type=Alloc;
        static constexpr bool allow_repeated=repeated;
        static constexpr const key_type& getKey(const value_type& val){
            return val.key;
        }
        struct value_compare{
            protected:
                friend Tree<TreeMapTraits>;
                key_compare cmp;
            public:
                constexpr value_compare(key_compare cmp):cmp(cmp){}
                constexpr bool operator()(const value_type& fst,const value_type& scnd)const{
                    return cmp(fst.key,scnd.key);
                }
        };
    };
    template<typename> class MapKeys;
    template<typename> class MapValues;
    template<typename> class ConstMapValues;
    template<typename K,typename T,typename Alloc=allocator<MapPair<K,T>>,typename Cmp=less<K>>
    class Map:public Tree<TreeMapTraits<K,T,Cmp,Alloc,false>>{
        public:
        	using base=Tree<TreeMapTraits<K,T,Cmp,Alloc,false>>;
        	using base::base;
            using typename base::key_type;
            using typename base::value_type;
            using typename base::key_compare;
            using typename base::value_compare;
			using typename base::allocator_type;
			using typename base::size_type;
			using typename base::difference_type;
			using typename base::pointer;
			using typename base::const_pointer;
			using typename base::reference;
			using typename base::const_reference;
            using typename base::iterator;
            using typename base::const_iterator;
            using typename base::node_type;
            using mapped_type=T;
        private: 
            using Traits = TreeMapTraits<K,T,Cmp,Alloc,false>;
            using typename base::NodePtr;
        public:
            constexpr const mapped_type& at(const key_type& key)const{
                return this->find(key)->val;
            }
            constexpr mapped_type& at(const key_type& key){
                return this->find(key)->val;
            }
#define alloc       this->ct.getV2()
#define root        this->ct.getV3().prev
#define left_most   this->ct.getV3().right
#define right_most  this->ct.getV3().left
#define sent        this->ct.getV3().address()
            constexpr mapped_type& operator[](const key_type& key){
                NodePtr x;
                if(root==sent){
                    x=root=left_most=right_most=this->create_node(value_type(key));
                    x->right=x->left=sent;
                }
                else if(!this->cmp(key,right_most->data.key)){//val>=right_most->data
                    if(this->cmp(right_most->data.key,key)){//val>right_most->data
                        right_most->right=this->create_node(value_type(key));
                        right_most->right->prev=right_most;
                        right_most=right_most->right;
                        right_most->right=sent;
                        x=right_most;
                    }
                    else/*val==right_most->data*/ return right_most->data.val;
                }
                else if(!this->cmp(left_most->data.key,key)){//val<=left_most->data
                    if(this->cmp(key,left_most->data.key)){//val<left_most->data
                        left_most->left=this->create_node(value_type(key));
                        left_most->left->prev=left_most;
                        left_most=left_most->left;
                        left_most->left=sent;
                        x=left_most;
                    }
                    else/*val==left_most->data*/ return left_most->data.val;
                }
                else{
                    x=root;
                    while(true){
                        if(this->cmp(key,x->data.key)){
                            if(x->left==nullptr){
                                x->left=this->create_node(value_type(key));
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else if(this->cmp(x->data.key,key)){
                            if(x->right==nullptr){
                                x->right=this->create_node(value_type(key));
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                        else return x->data.val;
                    }
                }
                this->length++;
                this->insert_fixup(x);
                return x->data.val;
            }
#undef root
#undef alloc
#undef left_most
#undef right_most
#undef sent
            constexpr Optional<mapped_type> get(const key_type& key)const{
                const_iterator p=this->find(key);
                if(p!=this->end()) return p->val;
                else return {};
            }
            constexpr Optional<mapped_type> pop_at(const key_type& key){
                Optional<value_type> x=this->pop_elem(key);
                if(x.has_value())return mpv::move(x->val);
                else return {};
            }
            constexpr MapKeys<Map> keys()const{
                return *this;
            }
            constexpr const MapValues<Map> values()const{
                return *this;
            }
            constexpr MapValues<Map> values(){
                return *this;
            }
    };
    template<typename K,typename T,typename Alloc=allocator<MapPair<K,T>>,typename Cmp=less<K>>
    class MultiMap:public Tree<TreeMapTraits<K,T,Cmp,Alloc,true>>{
        public:
        	using base=Tree<TreeMapTraits<K,T,Cmp,Alloc,true>>;
        	using base::base;
            using typename base::key_type;
            using typename base::value_type;
            using typename base::key_compare;
            using typename base::value_compare;
			using typename base::allocator_type;
			using typename base::size_type;
			using typename base::difference_type;
			using typename base::pointer;
			using typename base::const_pointer;
			using typename base::reference;
			using typename base::const_reference;
            using typename base::iterator;
            using typename base::const_iterator;
            using typename base::node_type;
            using mapped_type=T;
        private: 
            using Traits = TreeMapTraits<K,T,Cmp,Alloc,true>;
            using typename base::NodePtr;
        public:
            constexpr MapKeys<MultiMap> keys()const{
                return *this;
            }
            constexpr const MapValues<MultiMap> values()const{
                return *this;
            }
            constexpr MapValues<MultiMap> values(){
                return *this;
            }
    };
    template<typename Out,typename T,typename D>
    Out& operator<<(Out& stream,const MapPair<T,D>& pair){
        stream<<"{"<<pair.key<<" : "<<pair.val<<"}";
        return stream;
    }
    template<typename Map_t>
    class MapKeys{
        public:
            using MapType=Map_t;
            using value_type=typename MapType::key_type;
            using size_type=typename MapType::size_type;
            using difference_type=typename MapType::difference_type;
            using reference=typename MapType::key_type&;
            using const_reference=const typename MapType::key_type&;
            using pointer=typename MapType::key_type*;
            using const_pointer=const typename MapType::key_type*;
        private:
            const MapType& map;
        public:
            class iterator{// MapKeys iterator is readonly
                public:
                    using Ptr=typename MapType::const_iterator;
                    using iterator_category=bidirectional_iterator_tag;
                    using value_type=typename MapType::key_type;
                    using difference_type=typename MapType::difference_type;
                    using pointer=const typename MapType::key_type*;
                    using reference=const typename MapType::key_type&;
                private:
                    Ptr ptr;
                public:
                    constexpr iterator(typename MapType::const_iterator ptr)noexcept:ptr(ptr){}
                    constexpr reference operator*()const noexcept{
                        return ptr->key;
                    }
                    constexpr pointer operator->()const noexcept{
                        return pointer_traits<pointer>::pointer_to(ptr->key);
                    }
                    constexpr iterator& operator++()noexcept{
                        ++ptr;
                        return *this;
                    }
                    constexpr iterator operator++(int)noexcept{
                        iterator aux=*this;
                        ++ptr;
                        return aux;
                    }
                    constexpr iterator& operator--()noexcept{
                        --ptr;
                        return *this;
                    }
                    constexpr iterator operator--(int)noexcept{
                        iterator aux=*this;
                        --ptr;
                        return aux;
                    }
                    constexpr bool operator==(const iterator& other)const noexcept{
                        return this->ptr==other.ptr;
                    }
                    constexpr bool operator!=(const iterator& other)const noexcept{
                        return this->ptr!=other.ptr;
                    }
            };
            using const_iterator=iterator;
            constexpr MapKeys(const MapType& map)noexcept:map(map){}
            template<typename MT=MapType>
            constexpr size_type size()const noexcept{
                return map.size();
            }
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const{
                for(const_reference x:*this)
                    if(func(x))return true;
                return false;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{})const{
				for(const_reference x:*this)
					func(x);
			}
			constexpr bool contains(const_reference val)const{
                return map.contains(val);
				// for(const_reference x:*this)
				// 	if(x==val) return true;
				// return false;
			}
            template<bool enable=MapType::allow_repeated>
            constexpr enable_if_t<enable,size_type> count(const_reference val)const{
                return map.count(val);
                // size_type counter=0;
                // for(const_reference x:*this)
                //     if(x==val) counter++;
                // return counter;
            }
            constexpr iterator begin()const noexcept{
                return map.begin();
            }
            constexpr iterator end()const noexcept{
                return map.end();
            }
    };
    template<typename Map_t>
    class MapValues{
        public:
            using MapType=Map_t;
            using value_type=typename MapType::mapped_type;
            using size_type=typename MapType::size_type;
            using difference_type=typename MapType::difference_type;
            using reference=typename MapType::mapped_type&;
            using const_reference=const typename MapType::mapped_type&;
            using pointer=typename MapType::mapped_type*;
            using const_pointer=const typename MapType::mapped_type*;
        private:
            MapType& map;
        public:
        	class const_iterator;
            class iterator{
                public:
                    using Ptr=typename MapType::iterator;
                    using iterator_category=bidirectional_iterator_tag;
                    using value_type=typename MapType::mapped_type;
                    using difference_type=typename MapType::difference_type;
                    using pointer=typename MapType::mapped_type*;
                    using reference=typename MapType::mapped_type&;
                private:
                    Ptr ptr;
                public:
                    constexpr iterator(typename MapType::iterator ptr)noexcept:ptr(ptr){}
                    constexpr reference operator*()const noexcept{
                        return ptr->val;
                    }
                    constexpr pointer operator->()const noexcept{
                        return pointer_traits<pointer>::pointer_to(ptr->val);
                    }
                    constexpr iterator& operator++()noexcept{
                        ++ptr;
                        return *this;
                    }
                    constexpr iterator operator++(int)noexcept{
                        iterator aux=*this;
                        ptr++;
                        return aux;
                    }
                    constexpr iterator& operator--()noexcept{
                        --ptr;
                        return *this;
                    }
                    constexpr iterator operator--(int)noexcept{
                        iterator aux=*this;
                        ptr--;
                        return aux;
                    }
                    constexpr bool operator==(const iterator& other)const noexcept{
                        return this->ptr==other.ptr;
                    }
                    constexpr bool operator!=(const iterator& other)const noexcept{
                        return this->ptr!=other.ptr;
                    }
                    friend class MapValues<Map_t>::const_iterator;
            };
            class const_iterator{
                public:
                    using Ptr=typename MapType::const_iterator;
                    using iterator_category=bidirectional_iterator_tag;
                    using value_type=const typename MapType::mapped_type;
                    using difference_type=typename MapType::difference_type;
                    using pointer=const typename MapType::mapped_type*;
                    using reference=const typename MapType::mapped_type&;
                private:
                    Ptr ptr;
                public:
                    constexpr const_iterator(typename MapType::const_iterator ptr)noexcept:ptr(ptr){}
                    constexpr const_iterator(iterator nonconst_it)noexcept:ptr(nonconst_it.ptr){}
                    constexpr reference operator*()const noexcept{
                        return ptr->val;
                    }
                    constexpr pointer operator->()const noexcept{
                        return pointer_traits<pointer>::pointer_to(ptr->val);
                    }
                    constexpr const_iterator& operator++()noexcept{
                        ++ptr;
                        return *this;
                    }
                    constexpr const_iterator operator++(int)noexcept{
                        const_iterator aux=*this;
                        ptr++;
                        return aux;
                    }
                    constexpr const_iterator& operator--()noexcept{
                        --ptr;
                        return *this;
                    }
                    constexpr const_iterator operator--(int)noexcept{
                        const_iterator aux=*this;
                        ptr--;
                        return aux;
                    }
                    constexpr bool operator==(const const_iterator& other)const noexcept{
                        return this->ptr==other.ptr;
                    }
                    constexpr bool operator!=(const const_iterator& other)const noexcept{
                        return this->ptr!=other.ptr;
                    }
            };
            constexpr MapValues(const MapType& map)noexcept:map(const_cast<MapType&>(map)){}
            template<typename MT=MapType>
            constexpr size_type size()const noexcept{
                return map.size();
            }
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const{
                for(const_reference x:*this)
                    if(func(x)) return true;
                return false;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{})const{
				for(const_reference x:*this)
					func(x);
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{}){
				for(reference x:*this)
					func(x);
			}
			constexpr bool contains(const_reference val)const{
				for(const_reference x:*this)
					if(x==val) return true;
				return false;
			}
            constexpr size_type count(const_reference val)const{
                size_type counter=0;
                for(const_reference x:*this)
                    if(x==val) counter++;
                return counter;
            }
            constexpr const_iterator begin()const noexcept{
                return const_cast<const MapType&>(map).begin();
            }
            constexpr const_iterator end()const noexcept{
                return const_cast<const MapType&>(map).end();
            }
            constexpr iterator begin()noexcept{
                return map.begin();
            }
            constexpr iterator end()noexcept{
                return map.end();
            }
    };
    template<typename Out,typename Map_t>
    Out& operator<<(Out& stream,const MapKeys<Map_t>& keys){
        stream<<"[";
        auto it(keys.begin());
        auto it_end(keys.end());
        while(it!=it_end){
            stream<<""<<*it;
            if(++it!=it_end)
                stream<<", ";
        }
        stream<<"]";
        return stream;
    }
    template<typename Out,typename Map_t>
    Out& operator<<(Out& stream,const MapValues<Map_t>& values){
        stream<<"[";
        auto it(values.begin());
        auto it_end(values.end());
        while(it!=it_end){
            stream<<""<<*it;
            if(++it!=it_end)
                stream<<", ";
        }
        stream<<"]";
        return stream;
    }
}

namespace mpv{
    template<typename T,typename Cmp,typename Alloc,bool repeated> struct TreeSetTraits{
        using key_type=T;
        using value_type=T;
        using key_compare=Cmp;
        using value_compare=Cmp;
        using allocator_type=Alloc;
        static constexpr bool allow_repeated=repeated;
        static constexpr const key_type& getKey(const value_type& val){
            return val;
        }
    };
    template<typename T,typename Alloc=allocator<T>,typename Cmp=less<T>>
    class Set:public Tree<TreeSetTraits<T,Cmp,Alloc,false>>{
        public:
        	using base=Tree<TreeSetTraits<T,Cmp,Alloc,false>>;
        	using base::base;
            using typename base::key_type;
            using typename base::value_type;
            using typename base::key_compare;
            using typename base::value_compare;
			using typename base::allocator_type;
			using typename base::size_type;
			using typename base::difference_type;
			using typename base::pointer;
			using typename base::const_pointer;
			using typename base::reference;
			using typename base::const_reference;
            using typename base::iterator;
            using typename base::const_iterator;
            using typename base::node_type;
            template<typename,typename,typename> friend class Set;
			template<typename Lambda>
			constexpr auto map(Lambda&& func=Lambda{})const{
                using func_ret=decltype(func(declval<const_reference>()));
                Set<func_ret,rebind_alloc<Alloc,func_ret>,rebind_t<Cmp,func_ret>> new_set(base::AlTy_traits::select_on_container_copy_construction(this->get_allocator()));
                for(const_reference x:*this)
                    new_set.insert(func(x));
				return new_set;
			}
            constexpr Set operator|(const Set& other)const{
                Set new_set(*this);
                for(const_reference x:other)
                    new_set.insert(x);
                return new_set;
            }
            constexpr Set operator&(const Set& other)const{
                Set new_set(base::AlTy_traits::select_on_container_copy_construction(this->get_allocator()));
                for(const_reference x:*this)
                    if(other.contains(x)) new_set.insert(x);
                return new_set;
            }
            constexpr Set operator-(const Set& other)const{
                Set new_set(base::AlTy_traits::select_on_container_copy_construction(this->get_allocator()));
                for(const_reference x:*this)
                    if(!other.contains(x)) new_set.insert(x);
                return new_set;
            }
            constexpr Set operator^(const Set& other)const{
                Set new_set(base::AlTy_traits::select_on_container_copy_construction(this->get_allocator()));
                for(const_reference x:*this)
                    if(!other.contains(x)) new_set.insert(x);
                for(const_reference x:other)
                    if(!this->contains(x)) new_set.insert(x);
                return new_set;
            }
            constexpr Set& operator|=(const Set& other){
                for(const_reference x:other)
                    this->insert(x);
                return *this;
            }
            constexpr Set& operator&=(const Set& other){
                *this=static_cast<Set&&>(*this)&other;
                return *this;
            }
            constexpr Set& operator-=(const Set& other){
                for(const_reference x:other)
                    this->del_elem(x);
                return *this;
            }
            constexpr Set& operator^=(const Set& other){
                for(const_reference x:other)
                    if(this->contains(x)) this->del_elem(x);
                    else this->insert(x);
                return *this;
            }
    };
    template<typename T,typename Alloc=allocator<T>,typename Cmp=less<T>>
    class MultiSet:public Tree<TreeSetTraits<T,Cmp,Alloc,true>>{
        public:
        	using base=Tree<TreeSetTraits<T,Cmp,Alloc,true>>;
        	using base::base;
            using typename base::key_type;
            using typename base::value_type;
            using typename base::key_compare;
            using typename base::value_compare;
			using typename base::allocator_type;
			using typename base::size_type;
			using typename base::difference_type;
			using typename base::pointer;
			using typename base::const_pointer;
			using typename base::reference;
			using typename base::const_reference;
            using typename base::iterator;
            using typename base::const_iterator;
            using typename base::node_type;
            template<typename,typename,typename> friend class Set;
			template<typename Lambda>
			constexpr auto map(Lambda&& func=Lambda{})const{
                using func_ret=decltype(func(declval<const_reference>()));
                MultiSet<func_ret,rebind_alloc<Alloc,func_ret>,rebind_t<Cmp,func_ret>> new_set(base::AlTy_traits::select_on_container_copy_construction(this->get_allocator()));
                for(const_reference x:*this)
                    new_set.insert(func(x));
				return new_set;
			}
    };
}
//#include"Classes/BitList.hpp"

namespace mpv{
template<typename T,size_t len>
    class Array COUNT_IT{
        static_assert(len!=0,"array length cannot be 0");
        protected:
            T elems[len];
        public:
            using value_type=T;
            using size_type=size_t;
            using difference_type=ptrdiff_t;
            using pointer=T*;
            using const_pointer=const T*;
            using reference=T&;
            using const_reference=const T&;
            using iterator=T*;
            using const_iterator=const T*;
            static constexpr size_t length=len;
            static constexpr size_t size(){
                return length;
            }
            constexpr Array(const T (&list)[length]){
                for(size_t i=0;i<length;i++)
                    elems[i]=list[i];
            }
            template<typename... Args,enable_if_t<conjunction_v<is_constructible<T,Args>...>>* = nullptr>
            constexpr Array(Args&&... args):elems{static_cast<Args&&>(args)...}{}
            //Array(std::initializer_list<T> ilist):elems(ilist){}
            constexpr const T& operator[](size_t index)const{
                return elems[index];
            }
            constexpr T& operator[](size_t index){
                return elems[index];
            }
			constexpr bool operator==(const Array& other)const{
				return equal(this->begin(),this->end(),other.begin());
			}
			constexpr bool operator!=(const Array& other)const{
				return !equal(this->begin(),this->end(),other.begin());
			}
            constexpr bool operator<(const Array& other)const{
                return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool operator>(const Array& other)const{
                return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator<=(const Array& other)const{
                return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator>=(const Array& other)const{
                return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            void sort(){
                dflt::insertion_sort(elems,length);
            }
            void sort_reverse(){
                dflt::insertion_sort_reverse(elems,length);
            }
            template<typename D>
            void sort(D(*func)(const T&)){
                dflt::insertion_sort(elems,length,func);
            }
            template<typename Cmp>
            void sort(Cmp&& cmp=Cmp{}){
                dflt::insertion_sort<T,Cmp>(elems,length,static_cast<Cmp&&>(cmp));
            }
            constexpr Array operator+(const Array& other)const{
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=(*this)[i]+other[i];
                }
                return new_array;
            }
            constexpr Array operator-(const Array& other)const{
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=(*this)[i]-other[i];
                }
                return new_array;
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            constexpr Array operator*(U scalar)const{
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=(*this)[i]*scalar;
                }
                return new_array;
            }
            constexpr Array operator-()const{
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=-(*this)[i];
                }
                return new_array;
            }
            constexpr Array operator+()const{
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=+(*this)[i];
                }
                return new_array;
            }
            constexpr T operator*(const Array& other)const{
                T n=0;
                for(size_t i=0;i<length;i++){
                    n+=(*this)[i]*other[i];
                }
                return n;
            }
            constexpr void operator+=(const Array& other){
                for(size_t i=0;i<length;i++){
                    (*this)[i]+=other[i];
                }
            }
            constexpr void operator-=(const Array& other){
                for(size_t i=0;i<length;i++){
                    (*this)[i]-=other[i];
                }
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            constexpr void operator*=(U scalar){
                for(size_t i=0;i<length;i++){
                    (*this)[i]*=scalar;
                }
            }
            constexpr double norm()const{
                double sum=0;
                for(size_t i=0;i<length;i++){
                    sum+=(*this)[i]*(*this)[i];
                }
                return mpv::sqrt(sum);
            }
            constexpr double distance(const Array& other)const{
                T sum=0;
                for(size_t i=0;i<length;i++){
                    T res=other[i]-(*this)[i];
                    sum+=res*res;
                }
                return sqrt(sum);
            }
            constexpr Array<double,length> make_versor(){
                Array<double,length> new_vec;
                double n=1/(*this).norm();
                for(size_t i=0;i<length;i++){
                    new_vec[i]=(*this)[i]*n;
                }
                return new_vec;
            }
            constexpr Array<double,length> make_neg_versor(){
                Array<double,length> new_vec;
                double n=1/(*this).norm();
                for(size_t i=0;i<length;i++){
                    new_vec[i]=-(*this)[i]*n;
                }
                return new_vec;
            }
            template<size_t s=length,typename=enable_if_t<s==3>>
            constexpr Array<T,3> operator%(const Array<T,3>& other)const{
                Array<T,3> new_array;
                new_array[0]=(*this)[1]*other[2]-other[1]*(*this)[2];
                new_array[1]=other[0]*(*this)[2]-(*this)[0]*other[2];
                new_array[2]=(*this)[0]*other[1]-other[0]*(*this)[1];
                return new_array;
            }
            template<size_t s=length,typename=enable_if_t<s==3>>
            constexpr void operator%=(const Array<T,3>& other){
                Array<T,3> aux=*this;
                (*this)[0]=aux[1]*other[2]-other[1]*aux[2];
                (*this)[1]=other[0]*aux[2]-aux[0]*other[2];
                (*this)[2]=aux[0]*other[1]-other[0]*aux[1];
            }
            constexpr iterator begin(){
                return elems;
            }
            constexpr iterator end(){
                return elems+length;
            }
            constexpr const_iterator begin()const{
                return elems;
            }
            constexpr const_iterator end()const{
                return elems+length;
            }
    };
    template<typename T,size_t length,typename U,typename=enable_if_t<is_convertible_v<U,T>,U>>
    constexpr Array<T,length> operator*(U scalar,const Array<T,length>& vec){
        return vec*scalar;
    }
#if __cplusplus>=201703
    template<typename T,typename... Args> Array(T,Args...)->Array<enable_if_t<are_same_v<T,Args...>,T>,sizeof...(Args)+1>;
#endif
    template<typename Out,typename T,size_t length>
    Out& operator<<(Out& stream,const Array<T,length>& array){
        stream<<"(";
        for(size_t i=0;i<length;i++){
            stream<<""<<array[i];
            if(i<length-1)
                stream<<", ";		
        }
        stream<<")";
        return stream;
    }
}

namespace mpv{
    template<typename T,size_t R,size_t C>
    class Matrix:public Array<Array<T,C>,R>{
        public:
            using base=Array<Array<T,C>,R>;
            using base::base;
            using value_type=T;
            using size_type=size_t;
            using difference_type=ptrdiff_t;
            using pointer=T*;
            using const_pointer=const T*;
            using reference=T&;
            using const_reference=const T&;
            using iterator=typename base::iterator;
            using const_iterator=typename base::const_iterator;
            static constexpr size_t rows=R;
            static constexpr size_t columns=C;
            static constexpr size_t length=R*C;
            constexpr Matrix operator+(const Matrix& other)const{
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=(*this)[i]+other[i];
                }
                return new_matrix;
            }
            constexpr Matrix operator-(const Matrix& other)const{
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=(*this)[i]-other[i];
                }
                return new_matrix;
            }
            constexpr Matrix operator-()const{
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=-(*this)[i];
                }
                return new_matrix;
            }
            constexpr Matrix operator+()const{
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=+(*this)[i];
                }
                return new_matrix;
            }
            constexpr void triangulate(){
                size_t zc=0;// column of zeros counter
                for(size_t i=0;i<R-1+zc && i<C;i++){
                    if(is_zero(this->elems[i-zc][i])){
                        bool column_of_zeros=true;
                        for(size_t j=i+1-zc;j<R;j++){
                            if(!is_zero(this->elems[j][i])){
                                column_of_zeros=false;
                                swap(this->elems[j],this->elems[i-zc]);
                                break;
                            }
                        }
                        if(column_of_zeros){
                            zc++;
                            continue;
                        }
                    }
                    for(size_t j=i+1-zc;j<R;j++){
                        if(!is_zero(this->elems[j][i])){
                            this->elems[j]=this->elems[j]*this->elems[i-zc][i]-this->elems[i-zc]*this->elems[j][i];
                        }
                    }
                }
            }
            constexpr Matrix triangulated()const{
                Matrix new_matrix(*this);
                new_matrix.triangulate();
                return new_matrix;
            }
            template<size_t Rows,size_t Columns,typename=enable_if_t<C==Rows>>
            constexpr Matrix<T,R,Columns> operator*(const Matrix<T,Rows,Columns>& other){
                Matrix<T,R,Columns> new_matrix;
                for(size_t i=0;i<R;i++){
                    for(size_t j=0;j<Columns;j++){
                        new_matrix[i][j]=0;
                        for(size_t k=0;k<C;k++){
                            new_matrix[i][j]+=(*this)[i][k]*other[k][j];
                        }
                    }
                }
                return new_matrix;
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            constexpr Matrix operator*(U scalar)const{
                Matrix new_matrix;
                for(size_t i=0;i<R;i++)
                    new_matrix[i]=this->elems[i]*scalar;
                return new_matrix;
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            constexpr void operator*=(U scalar){
                for(size_t i=0;i<R;i++)
                    this->elems[i]*=scalar;
            }
            template<size_t R1=R-1,size_t C1=C-1>
            constexpr enable_if_t<(R1>0 && C1>0),Matrix<T,R-1,C-1>> subm(const size_t I,const size_t J)const{
                Matrix<T,R1,C1> subm;
                for(size_t j=0;j<J;j++){
                    for(size_t i=0;i<I;i++){
                        subm[i][j]=this->elems[i][j];
                    }
                }
                for(size_t j=J+1;j<C;j++){
                    for(size_t i=0;i<I;i++){
                        subm[i][j-1]=this->elems[i][j];
                    }
                }
                for(size_t j=0;j<J;j++){
                    for(size_t i=I+1;i<R;i++){
                        subm[i-1][j]=this->elems[i][j];
                    }
                }
                for(size_t j=J+1;j<C;j++){
                    for(size_t i=I+1;i<R;i++){
                        subm[i-1][j-1]=this->elems[i][j];
                    }
                }
                return subm;
            }
            template<size_t N=R>
            constexpr enable_if_t<N==C && N!=1,T> determinant()const{
                if constexpr(N==2){
                    return this->elems[0][0]*this->elems[1][1]-this->elems[0][1]*this->elems[1][0];
                }
                else{
                    T result=0;
                    for(size_t k=0;k<R;k++){
                        Matrix<T,N-1,N-1> subm;
                        for(size_t j=0;j<k;j++){
                            for(size_t i=1;i<R;i++){
                                subm[i-1][j]=this->elems[i][j];
                            }
                        }
                        for(size_t j=k+1;j<C;j++){
                            for(size_t i=1;i<R;i++){
                                subm[i-1][j-1]=this->elems[i][j];
                            }
                        }
                        //std::cout<<this->elems[0][k]*minus1pow(k)<<" * "<<subm.determinant()<<std::endl;
                        result+=this->elems[0][k]*minus1pow(k)*subm.determinant();
                    }
                    return result;
                }
            }
            template<size_t N=R>
            constexpr enable_if_t<N==C && N!=1,Matrix> adj()const{
                Matrix new_matrix;
                if constexpr(N==2){
                    new_matrix[0][0]=(*this)[1][1];
                    new_matrix[1][1]=(*this)[0][0];
                    new_matrix[1][0]=-(*this)[0][1];
                    new_matrix[0][1]=-(*this)[1][0];
                }
                else{
                    for(size_t i=0;i<R;i++){
                        for(size_t j=0;j<C;j++){
                            new_matrix[i][j]=minus1pow(i+j)*this->subm(i,j).determinant();
                        }
                    }
                }
                return new_matrix;
            }
            constexpr Matrix<T,C,R> transposed()const{
                Matrix<T,C,R> new_matrix;
                for(size_t i=0;i<R;i++){
                    for(size_t j=0;j<C;j++){
                        new_matrix[j][i]=(*this)[i][j];
                    }
                }
                return new_matrix;
            }
            template<size_t N=R>
            constexpr enable_if_t<N==C && N!=1,Optional<Matrix>> inverse()const{
                const T det=this->determinant();
                if(det==0) return {};
                else return this->transposed().adj()*(1/det);
            }
    };
    template<typename T,size_t R,size_t C,typename U,typename=enable_if_t<is_convertible_v<U,T>,U>>
    constexpr Matrix<T,R,C> operator*(U scalar,const Matrix<T,R,C>& m){
        return m*scalar;
    }
    template<typename Out,typename T,size_t R,size_t C>
    Out& operator<<(Out& stream,const Matrix<T,R,C>& matrix){
        stream<<"(";
        for(size_t i=0;i<R;i++){
            for(size_t j=0;j<C;j++){
                stream<<""<<matrix[i][j];
                if(j!=C-1) stream<<",\t";
            }
            if(i!=R-1) stream<<"\n";
        }
        stream<<")";
        return stream;
    }
}

namespace mpv{
    template<typename StateType>
    class StateMachine{

        template<typename T,typename=void> struct has_enter:false_type{};
        template<typename T> struct has_enter<T,void_t<decltype(declval<T>().enter())>>:true_type{};
        template<typename T,typename=void> struct has_exit:false_type{};
        template<typename T> struct has_exit<T,void_t<decltype(declval<T>().exit())>>:true_type{};
        static_assert(has_enter<StateType>::value,"StateType must have a enter() method");
        static_assert(has_exit<StateType>::value,"StateType must have a exit() method");
        enum class Action:char{NONE,PUSH,POP,REPLACE,POP_UNTILL_FIND} incoming_action=Action::NONE;
        Stack<uPtr<StateType>> stack;
        uPtr<StateType> incoming_state;
        bool(*func)(StateType*);
        template<typename Derived>
        static bool is(StateType* p){
            return (bool)dynamic_cast<Derived*>(p);
        }        
    public:
        void push(StateType* new_state){
            incoming_state=new_state;
            incoming_action=Action::PUSH;
        }
        void pop(){
            incoming_action=Action::POP;
        }
        void replace_current(StateType* new_state){
            incoming_state=new_state;
            incoming_action=Action::REPLACE;
        }

        template<typename Derived>
        void pop_until_find(){
            func = &is<Derived>;
            incoming_action = Action::POP_UNTILL_FIND;
        }
        void process_change(){
            switch(incoming_action){
                case Action::NONE:break;
                case Action::PUSH:
                    if(!stack.empty())
                        stack.top()->exit();
                    stack.push(move(incoming_state));
                    stack.top()->enter();
                    break;
                case Action::REPLACE:
                    if(stack.empty()){
                        stack.push(move(incoming_state));
                    }
                    else{
                        stack.top()->exit();
                        stack.top()=move(incoming_state);
                    }
                    stack.top()->enter();
                    break;
                case Action::POP:
                    if(!stack.empty()){
                        stack.top()->exit();
                        stack.del();
                        if(!stack.empty())
                            stack.top()->enter();
                    }break;
                case Action::POP_UNTILL_FIND:
                    if(!stack.empty() && !func(stack.top().get())){
                        stack.top()->exit();
                        stack.del();
                    }
                    while(!stack.empty() && !func(stack.top().get())){
                        stack.del();
                    }
                    if(!stack.empty())
                        stack.top()->enter();
            }
            incoming_action=Action::NONE;
        }
        StateType& current(){
            return *stack.top();
        }
        StateType* get_current(){
            return stack.top().get();
        }
        bool empty()const{
            return stack.empty();
        }
    };
}

//#include"Classes/BigInt.hpp"

namespace mpv{
	struct testObj COUNT_IT {
		static long long objCount;
		int a;
		int* array;
		int get(){return a;}
		testObj() :a(-99),array(new int[10]){
			objCount++;
	        PUTS("\ndefault constructor");
		}
		testObj(int a):a(a),array(new int[10]){objCount++;
			for(int i=0;i<10;i++)
				array[i]=a;
			PUTS("\nassignment constructor");
		}
		testObj(const testObj& other):a(other.a),array(new int[10]){
			//if(objCount==8) throw "objCount==8";
			objCount++;
			for(int i=0;i<10;i++)
				array[i]=other.array[i];
			PUTS("\ncopy constructor");
		}
		testObj(testObj&& other)noexcept:a(other.a){objCount++;
			this->array=other.array;
			other.a=-100;
			other.array=nullptr;
			PUTS("\nmovement constructor");
		}
		void operator=(const testObj& other){
			delete[] this->array;
			this->array=new int[10];
			for(int i=0;i<10;i++)
				this->array[i]=other.array[i];
			this->a=other.a;
			PUTS("\ncopy operator");
		}
		void operator=(testObj&& other)noexcept{
			if(this==&other)return;
			if(array) delete[] this->array;
			this->array=other.array;
			other.array=nullptr;
			this->a=other.a;
			other.a=-100;
			PUTS("\nmovement operator");
		}
		~testObj(){
			if(array) delete[] this->array;
			//if(--objCount==0)
				PRINTF("\nOBJECT COUNTER=%lli\n",--objCount);
		}
		bool operator==(const testObj& other)const{
			return this->a==other.a;
		}
		bool operator!=(const testObj& other)const{
			return this->a!=other.a;
		}
		bool operator<=(const testObj& other)const{
			return this->a<=other.a;
		}
		bool operator>=(const testObj& other)const{
			return this->a>=other.a;
		}
		bool operator<(const testObj& other)const{
			return this->a<other.a;
		}
		bool operator>(const testObj& other)const{
			return this->a>other.a;
		}
		explicit operator int()const{
			return a;
		}
	};
	inline long long testObj::objCount=0;
	template<typename Out>
	Out& operator<<(Out& stream,const testObj& obj){
		stream<<obj.a;
		return stream;
	}
}

namespace mpv{
    namespace serialization{
        class Bytes;
        template<typename T> constexpr enable_if_t<is_trivially_copyable_v<T>,Bytes> to_bytes(const T& val);
        template<typename T,typename U> Bytes to_bytes(const MapPair<T,U>& pair);
        template<typename Container> enable_if_t<!is_trivially_copyable_v<Container> && is_trivially_serializable_container_v<Container>,Bytes> to_bytes(const Container& cont);
        template<typename Container> enable_if_t<!is_trivially_copyable_v<Container> && !is_trivially_serializable_container_v<Container>,Bytes> to_bytes(const Container& cont);
        
        template<typename T>
        struct NullTerminatedArray{
            static_assert(is_trivially_copyable_v<T>,"T mus be trivially copyable");
            const T* ptr;
            size_t size;
            NullTerminatedArray(const T* ptr,size_t size):ptr(ptr),size(size){}
        };
        template<typename T>
        union Serialized{
            static_assert(is_trivially_copyable_v<T>,"T must be trivially copyable");
            T value;
            unsigned char bytearray[sizeof(T)];
            Serialized(const T& val):value(val){}
        };
        template<typename Container,typename Uint>
        struct Wrapper:public Container{
            static_assert(is_iterable_v<Container>,"Container must be iterable");
            static_assert(is_unsigned_v<Uint> && !is_bool_v<Uint>,"Uint must be an unsigned integer");
            using Base=Container;
            using Container::Container;
            using Container::operator=;
            Wrapper(const Base& b):Base(b){}
            Wrapper(Base&& b):Base(static_cast<Base&&>(b)){}
            using typename Container::value_type;
            using typename Container::reference;
            using typename Container::pointer;
            constexpr Uint size()const noexcept{
                return static_cast<Uint>(Container::size());
            }
        };
        template<typename T>
        struct unwrapped{
            using type=T;
        };
        template<typename T> using unwrapped_t=typename unwrapped<T>::type;
        
        template<template<typename...> class Container,typename... Args>
        struct unwrapped<Container<Args...>>{
            using type=Container<unwrapped_t<Args>...>;
        };
        template<template<typename...> class Container,typename Uint,typename... Args>
        struct unwrapped<Wrapper<Container<Args...>,Uint>>{
            using type=Container<unwrapped_t<Args>...>;
        };
        class Bytes:public Vector<unsigned char>{
        public:
            using Base = Vector<unsigned char>;
            using Base::Base;
            Bytes(const Vector<unsigned char>& vec):Base(vec){}
            Bytes operator+(const Bytes& other)const&{
                return this->Base::operator+(other);
            }
            Bytes operator+(Bytes&& other)const&{
                return this->Base::operator+(static_cast<Bytes&&>(other));
            }
            Bytes operator+(const Bytes& other)&&{
                return static_cast<Bytes&&>(*this).Base::operator+(other);
            }
            Bytes operator+(Bytes&& other)&&{
                return static_cast<Bytes&&>(*this).Base::operator+(static_cast<Bytes&&>(other));
            }
            Bytes& operator+=(const Bytes& other){
                this->Base::operator+=(other);
                return *this;
            }
            Bytes& operator+=(Bytes&& other){
                this->Base::operator+=(static_cast<Bytes&&>(other));
                return *this;
            }
            template<typename T>
            Bytes operator+(const Serialized<T> trivial)const{
                Bytes res(sizeof(trivial)+this->size());
                mpv::copy(res.begin(),this->begin(),this->end());
                *reinterpret_cast<Serialized<T>*>(res.get_array()+this->size())=trivial;
                return res;
            }
            template<typename T>
            Bytes& operator+=(Serialized<T> trivial){
                this->insert(this->end(),mpv::begin(trivial.bytearray),mpv::end(trivial.bytearray));
                return *this;
            }
        };
        template<typename T>
        Bytes operator+(const Serialized<T> fst,const Bytes& scnd){
            Bytes res(sizeof(fst)+scnd.size());
            *reinterpret_cast<Serialized<T>*>(res.get_array())=fst;
            mpv::copy(res.begin()+sizeof(fst),scnd.begin(),scnd.end());
            return res;
        }
        template<typename T>
        constexpr enable_if_t<is_trivially_copyable_v<T>,Bytes> to_bytes(const T& val){
            const unsigned char* p = reinterpret_cast<const unsigned char*>(&val);
            return Vector<unsigned char>(p,p+sizeof(T));
        }
        template<typename T,typename U>
        Bytes to_bytes(const MapPair<T,U>& pair){
            return to_bytes(pair.key)+to_bytes(pair.val);
        }
        template<typename Container>
        enable_if_t<!is_trivially_copyable_v<Container> && is_trivially_serializable_container_v<Container>,Bytes> to_bytes(const Container& cont){
            Vector<unsigned char> data(sizeof(decltype(cont.size())) + cont.size() * sizeof(typename Container::value_type));
            using size_type=decltype(cont.size());
            using value_type=typename Container::value_type;
            unsigned char* ptr=data.get_array();
            *reinterpret_cast<size_type*>(ptr)=cont.size();
            ptr+=sizeof(size_type);
            copy_n(reinterpret_cast<value_type*>(ptr),cont.begin(),cont.size());
            return Bytes(data);
        }
        template<typename Container>
        enable_if_t<!is_trivially_copyable_v<Container> && !is_trivially_serializable_container_v<Container>,Bytes> to_bytes(const Container& cont){
            Vector<unsigned char> data(sizeof(decltype(cont.size())));
            using size_type=decltype(cont.size());
            *reinterpret_cast<size_type*>(data.get_array()) = cont.size();
            for(const auto& val:cont){
                data+=to_bytes(val);
            }
            return Bytes(data);
        }
        template<typename T>
        enable_if_t<is_trivially_copyable_v<T>,Bytes> to_bytes(NullTerminatedArray<T> array) {
            return Bytes(reinterpret_cast<const unsigned char*>(array.ptr),reinterpret_cast<const unsigned char*>(array.ptr)+(array.size+1)*sizeof(T));
        }
        template<typename,typename=void> struct Deserialize{};
        template<typename T> 
        struct Deserialize<T,enable_if_t<is_trivially_copyable_v<T>>>{
            template<typename except_t, bool runtime_check=true>
            static constexpr T from_bytes(const unsigned char*& data,const unsigned char* end)noexcept(runtime_check==false){
                if constexpr(runtime_check){
                    if(data+sizeof(T)>end) throw except_t();
                }
                T val=*reinterpret_cast<const T*>(data);
                data+=sizeof(T);
                return val;
            }
        };
        template<typename K,typename V>
        struct Deserialize<MapPair<K,V>>{
        	template<typename except_t,bool runtime_check=true>
            static constexpr MapPair<unwrapped_t<K>,unwrapped_t<V>> from_bytes(const unsigned char*& data,const unsigned char* end){
                unwrapped_t<K> key=Deserialize<K>::template from_bytes<except_t,runtime_check>(data,end);
                return MapPair<unwrapped_t<K>,unwrapped_t<V>>(static_cast<unwrapped_t<K>&&>(key),Deserialize<V>::template from_bytes<except_t,runtime_check>(data,end));
            }
        };
        template<typename Sequence>
        struct Deserialize<Sequence,enable_if_t<!is_trivially_copyable_v<Sequence> && is_trivially_serializable_container_v<Sequence>>>{
            template<typename except_t,bool runtime_check=true>
            static constexpr unwrapped_t<Sequence> from_bytes(const unsigned char*& data,const unsigned char* end){
                using size_type=decltype(mpv::declval<Sequence>().size());
                using value_type=typename Sequence::value_type;
                if constexpr(runtime_check){
                    if(data+sizeof(size_type)>end) throw except_t();
                }
                size_type size=*reinterpret_cast<const size_type*>(data);
                data+=sizeof(size_type);
                if constexpr(runtime_check){
                    if(data+size*sizeof(value_type)>end) throw except_t();
                }
                const value_type* rebound=reinterpret_cast<const value_type*>(data);
                data+=size*sizeof(value_type);
                return unwrapped_t<Sequence>(rebound,rebound+size);
            }            
        };

        template<typename Sequence>
        struct Deserialize<Sequence,enable_if_t<has_push_back_v<Sequence> && is_iterable_v<Sequence> && has_size_v<Sequence> && !is_trivially_copyable_v<Sequence> && !is_trivially_serializable_container_v<Sequence>>>{
            template<typename except_t,bool runtime_check=true>
            static constexpr unwrapped_t<Sequence> from_bytes(const unsigned char*& data,const unsigned char* end){
                using size_type=decltype(mpv::declval<Sequence>().size());
                if constexpr(runtime_check){
                    if(data+sizeof(size_type)>end) throw except_t();
                }
                size_type size=*reinterpret_cast<const size_type*>(data);
                unwrapped_t<Sequence> cont;
                if constexpr(has_reserve_v<Sequence>){
                    cont.reserve(size);
                }
                
                data+=sizeof(size_type);
                for(size_type i=0;i<size;i++)
                    cont.push_back(Deserialize<typename Sequence::value_type>::template from_bytes<except_t,runtime_check>(data,end));
                return cont;
            }
        };
        template<typename Tree>
        struct Deserialize<Tree,enable_if_t<has_get_root_v<Tree> && is_iterable_v<Tree> && has_size_v<Tree> && !is_trivially_copyable_v<Tree> && !is_trivially_serializable_container_v<Tree>>>{
            template<typename except_t,bool runtime_check=true>
            static constexpr unwrapped_t<Tree> from_bytes(const unsigned char*& data,const unsigned char* end){
                using size_type=decltype(mpv::declval<Tree>().size());
                if constexpr(runtime_check){
                    if(data+sizeof(size_type)>end) throw except_t();
                }
                size_type size=*reinterpret_cast<const size_type*>(data);
                unwrapped_t<Tree> cont;
                data+=sizeof(size_type);
                for(size_type i=0;i<size;i++)
                    cont.insert(Deserialize<typename Tree::value_type>::template from_bytes<except_t,runtime_check>(data,end));
                return cont;
            }
        };
        template<typename except_t=int,bool runtime_check=true>
        class Deserializer{
            const unsigned char* data,* end;
        public:
            Deserializer(const void* data,size_t size):data(reinterpret_cast<const unsigned char*>(data)),end(this->data+size){}
            template<typename T>
            unwrapped_t<T> read(){
                return Deserialize<T>::template from_bytes<except_t,runtime_check>(data,end);
            }
            template<typename T>
            Str<T> read_null_terminated_array(){
                Str<T> str;
                if constexpr(runtime_check){
                    if(data+sizeof(T)>end) throw except_t();
                }
                while(*reinterpret_cast<const T*>(data)!=T(0)){
                    str+=*reinterpret_cast<const T*>(data);
                    data+=sizeof(T);
                    if constexpr(runtime_check){
                        if(data+sizeof(T)>end) throw except_t();
                    }
                }
                data+=sizeof(T);
                return str;
            }
            constexpr size_t get_remaining_bytes()const noexcept{
                return end-data;
            }
        };
    }
}
#undef CONSTRUCT_VARARGS
#undef CONSTRUCT
#undef DEFAULT_CONSTRUCT
#undef DESTROY
#undef MEMDEFAULTCONSTRUCT
#undef MEMFILLCONSTRUCT
#undef MEMCOPYCONSTRUCT
#undef MEMMOVECONSTRUCT
#undef MEMDESTRUCTOR
#endif