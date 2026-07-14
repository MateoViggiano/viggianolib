#pragma once
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
    	if(opt.has_value()) stream<<opt.value();
    	else stream<<"<empty>";
    	return stream;
    }
}
  