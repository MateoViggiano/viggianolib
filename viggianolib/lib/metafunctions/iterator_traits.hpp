#pragma once
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