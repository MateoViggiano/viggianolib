#pragma once
#ifdef __clang__
#define NOEXCEPT(x) noexcept(x)
#else
#define NOEXCEPT(x) 
#endif
namespace mpv{
    template<typename T> struct non_trivial_copy_ctor:T{
        using T::T;
        non_trivial_copy_ctor()=default;
        constexpr non_trivial_copy_ctor(const non_trivial_copy_ctor& other)NOEXCEPT(noexcept(T::construct(static_cast<const T&>(other)))){
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
        constexpr non_trivial_move_ctor(non_trivial_move_ctor&& other)NOEXCEPT(noexcept(base_type::construct(static_cast<T&&>(other)))){
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
        constexpr non_trivial_copy_assign& operator=(const non_trivial_copy_assign& other)NOEXCEPT(noexcept(base_type::assign(static_cast<const T&>(other)))){
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
        constexpr non_trivial_move_assign& operator=(non_trivial_move_assign&& other)NOEXCEPT(noexcept(base_type::assign(static_cast<T&&>(other)))){
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

    template<typename T,typename... Args> using smf_control=
    If_t<conjunction_v<is_trivially_destructible<Args>...,is_trivially_move_constructible<Args>...,is_trivially_move_assignable<Args>...>,
        smf_control_copy_assign<T,Args...>
        ,
        If_t<conjunction_v<is_move_constructible<Args>...,is_move_assignable<Args>...>,
            non_trivial_move_assign<T,Args...>
            ,
            deleted_move_assign<T,Args...>
        >
    >;
}