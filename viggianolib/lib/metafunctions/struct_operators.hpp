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