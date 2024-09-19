#pragma once
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
    template<typename T> struct is_integral:is_integral__<remove_cv_t<T>>::type{};
    template<typename T> constexpr bool is_integral_v=is_integral<T>::value;

    template<typename> struct is_floating_point__:false_type{};
    template<> struct is_floating_point__<float>:true_type{};
    template<> struct is_floating_point__<double>:true_type{};
    template<> struct is_floating_point__<long double>:true_type{};
    template<typename T> struct is_floating_point:is_floating_point__<remove_cv_t<T>>::type{};
    template<typename T> constexpr bool is_floating_point_v=is_floating_point<T>::value;

    template<typename T> constexpr bool is_arithmetic_v=is_integral_v<T> or is_floating_point_v<T>;
    template<typename T> struct is_arithmetic:bool_constant<is_arithmetic_v<T>>{};

    template<typename T,typename... Args> struct are_same:true_type{};
	template<typename T,typename U,typename... Args> struct are_same<T,U,Args...>   :bool_constant<is_same_v<T,U> and are_same<U,Args...>::value>{};
    template<typename T,typename... Args> constexpr bool are_same_v=are_same<T,Args...>::value;


    template<typename  > struct is_const            :false_type{};
    template<typename T> struct is_const<const T>   :true_type{};
    template<typename T> constexpr bool is_const_v=is_const<T>::value;

    template<typename  > struct is_lvalue_ref       :false_type{};
    template<typename T> struct is_lvalue_ref<T&>   :true_type{};
    template<typename T> constexpr bool is_lvalue_ref_v=is_lvalue_ref<T>::value;


    template<typename  > struct is_rvalue_ref       :false_type{};
    template<typename T> struct is_rvalue_ref<T&&>  :true_type{};
    template<typename T> constexpr bool is_rvalue_ref_v=is_rvalue_ref<T>::value;

    template<typename T> struct is_reference:bool_constant<is_lvalue_ref_v<T> or is_rvalue_ref_v<T>>{};
    template<typename T> constexpr bool is_reference_v=is_reference<T>::value;

    
    template<typename T> constexpr bool is_function_v=!is_const_v<const T> && !is_reference_v<T>;
    template<typename T> struct is_function:bool_constant<is_function_v<T>>{};
// only function types and reference types can't be const qualified
    template<typename T> constexpr bool is_object_v=is_const_v<const T> && !is_void_v<T>;
    template<typename T> struct is_object:bool_constant<is_object_v<T>> {};


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


    template<typename T,typename U>          struct have_same_extent           :bool_constant<not(is_array_v<T> or is_array_v<U>)>{};
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
        static_assert(is_enum_v<T> || (is_integral_v<T> and not is_bool_v<T>),"template parameter must be a non bool integer or an enum");
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
        static_assert(is_enum_v<T> || (is_integral_v<T> and not is_bool_v<T>),"template parameter must be a non bool integer or an enum");
        using type=rebind_cv_t<T,Make_Signed_Helper_t<remove_cv_t<T>>>;
    };
    template<typename T> using make_signed_t=typename make_signed<T>::type;

    template<typename,typename=void> struct Sign{
        static constexpr bool Signed=false;
        static constexpr bool Unsigned=false;
    };
    template<typename T> struct Sign<T,enable_if_t<is_integral_v<T> or is_floating_point_v<T>>>{
        static constexpr bool Signed=T(-1)<T(0);
        static constexpr bool Unsigned=not Signed;
    };
    template<typename T> struct is_unsigned:bool_constant<Sign<T>::Unsigned>{};
    template<typename T> constexpr bool is_unsigned_v=is_unsigned<T>::value;
    template<typename T> struct is_signed:bool_constant<Sign<T>::Signed>{};
    template<typename T> constexpr bool is_signed_v=is_signed<T>::value;

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

    template<typename T> constexpr bool is_member_pointer_v=is_member_object_pointer_v<T> or is_member_function_pointer_v<T>;
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
        //template<typename Callable,typename... Args> static constexpr auto call(Callable&& obj,Args&&... args){return static_cast<Callable&&>(obj)(static_cast<Args>(args)...);}
    };
    struct Invoker_pmf_object{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmf_object;
        //template<typename Decayed,typename T1,typename... Args> static constexpr auto call(Decayed pmf,T1&& arg1,Args&&... args){return (static_cast<T1&&>(arg1).*pmf)(static_cast<Args&&>(args)...);}
    };
    struct Invoker_pmf_refwrap{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmf_refwrap;
        //template<typename Callable,typename Refwrap,typename... Args> static constexpr auto call(Decayed pmf,Refwrap rw,Args&&... args){return (rw.get().*pmf)(static_cast<Args&&>(args)...);}
    };
    struct Invoker_pmf_pointer{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmf_pointer;
        //template<typename Decayed,typename T1,typename... Args> static constexpr auto call(Decayed pmf,T1&& arg,Args&&... args){return (static_cast<T1&&>(arg)->*pmf)(static_cast<Args&&>(args)...);}
    };
    struct Invoker_pmd_object{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmd_object;
        //template<typename Decayed,typename T1> static constexpr auto call(Decayed pmd,T1&& arg){return static_cast<T1&&>(arg).*pmd;}
    };
    struct Invoker_pmd_refwrap{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmd_refwrap;
        //template<typename Refwrap,typename T1> static constexpr auto call(Refwrap pmd,T1&& arg){return rw.get().*pmd;}
    };
    struct Invoker_pmd_pointer{
        static constexpr Invoker_strategy strat=Invoker_strategy::Pmd_pointer;
        //template<typename Decayed,typename T1> static constexpr auto call(Decayed pmd,T1&& arg){return static_cast<T1&&>(arg)->*pmd;}
    };
    template<typename Callable,typename T1,
                                        bool Is_pmf=is_member_function_pointer_v<Callable>,
                                        bool Is_pmd=is_member_object_pointer_v  <Callable>>
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
    auto invoke(Callable&& obj){
        return static_cast<Callable&&>(obj)();
    }
    template<typename Callable,typename T1,typename... Args>
    auto invoke(Callable&& obj,T1&& arg1,Args&&... args){
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

    template<typename T> class reference_wrapper{static_assert(is_object_v<T> || is_function_v<T>,"T must be an object or function type");
        private:
            T* data;
        public:
            using type=T;
            template<typename U,typename=enable_if_t<!is_same_v<reference_wrapper,remove_cvref_t<U>> and !is_rvalue_ref_v<U&&>>>
            reference_wrapper(U&& ref):data(&static_cast<U&&>(ref)){}
            reference_wrapper(const reference_wrapper&)=default;
            reference_wrapper& operator=(const reference_wrapper&)=default;
            operator T&()const noexcept{return *data;}
            T& get()const noexcept{return *data;}
            template<typename... Args>
            auto operator()(Args&&... args)const{
                return mpv::invoke(*data,static_cast<Args&&>(args)...);
            }
    };
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
    template<typename T>
    constexpr remove_reference_t<T>&& move(T&& val)noexcept{
        return static_cast<remove_reference_t<T>&&>(val);
    }
    template<typename T,typename=void> struct check_has_dtor:false_type{};
    template<typename T> struct check_has_dtor<T,void_t<decltype(declval<T&>().~T())>>:true_type{};
    template<typename T> struct is_destructible:
        If_t<is_void_v<T> || is_no_size_array_v<T> || is_function_v<T>,
            false_type
            ,
            If_t<is_reference_v<T> || is_scalar_v<T>,
                true_type
                ,
                check_has_dtor<T>
            >
        >{};
    template<typename T> constexpr bool is_destructible_v=is_destructible<T>::value;

#ifdef __clang__
    template<typename T> constexpr bool is_trivially_destructible_v=__is_trivially_destructible(T);
#else
    template<typename T> constexpr bool is_trivially_destructible_v=is_destructible_v<T> and __has_trivial_destructor(T);
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

    struct is_constructible_impl{
        template<typename T,typename... Args,typename=decltype(T(declval<Args>()...))> static true_type test(int);
        template<typename T,typename... Args> static false_type test(...);
        template<typename T,typename Arg,typename=decltype(::new T(declval<Arg>()))> static true_type test_single_arg(int);
        template<typename T,typename Arg> static false_type test_single_arg(...);
        template<typename T> static true_type ref_test(T);
        template<typename T> static false_type ref_test(...);

    };
    template<typename T,typename... Args> struct is_constructible:decltype(is_constructible_impl::test<T,Args...>(0)){};
    template<typename T,typename Arg> struct is_constructible<T,Arg>:decltype(is_constructible_impl::test_single_arg<T,Arg>(0)){};
    template<typename T,typename Arg> struct is_constructible<T&,Arg>:decltype(is_constructible_impl::ref_test<T&>(declval<Arg>())){};
    template<typename T,typename Arg> struct is_constructible<T&&,Arg>:decltype(is_constructible_impl::ref_test<T&&>(declval<Arg>())){};
    template<> struct is_constructible<void>:false_type{};
    template<> struct is_constructible<const void>:false_type{};
    template<> struct is_constructible<volatile void>:false_type{};
    template<> struct is_constructible<const volatile void>:false_type{};
    template<typename T,typename... Args> constexpr bool is_constructible_v=is_constructible<T,Args...>::value;
    template<typename T> using is_default_constructible=is_constructible<T>;
    template<typename T> constexpr bool is_default_constructible_v=is_default_constructible<T>::value;
    
    struct is_assignable_impl{//esto no funciona como deberia
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
    template<typename T,typename=void> struct is_iterable:false_type{};
    template<typename T> struct is_iterable<T,void_t<decltype(declval<T>().begin()),decltype(declval<T>().end())>>:true_type{};
    template<typename T> constexpr bool is_iterable_v=is_iterable<T>::value;

    template<typename T,typename=void> struct has_len:false_type{};
    template<typename T> struct has_len<T,void_t<decltype(declval<T>().len())>>:true_type{};
    template<typename T> constexpr bool has_len_v=has_len<T>::value;

    template<typename T,typename=void> struct has_size:false_type{};
    template<typename T> struct has_size<T,void_t<decltype(declval<T>().size())>>:true_type{};
    template<typename T> constexpr bool has_size_v=has_size<T>::value;
}