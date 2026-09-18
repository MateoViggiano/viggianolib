#pragma once
namespace mpv{
	template<typename Types> class const_random_access_iterator;
	template<typename Types>
	class random_access_iterator{
		friend class const_random_access_iterator<Types>;
		using const_iterator=const_random_access_iterator<Types>;
		public:
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=typename Types::reference;
			using iterator_category=typename iterator_traits<pointer>::iterator_category;
		private:
			pointer ptr{};
		public:
			constexpr random_access_iterator()noexcept(mpv::is_nothrow_default_constructible_v<pointer>)=default;
			constexpr random_access_iterator(pointer ptr)noexcept(mpv::is_nothrow_copy_constructible_v<pointer>):ptr(ptr){}
			constexpr reference operator*()const noexcept(noexcept(reference(*ptr))){
				return *ptr;
			}
			constexpr reference operator[](difference_type n)const noexcept(noexcept(reference(*(ptr+n)))){
				return *(ptr+n);
			}
			constexpr pointer operator->()const noexcept(mpv::is_nothrow_copy_constructible_v<pointer>){
				return ptr;
			}
			constexpr random_access_iterator& operator++()noexcept(noexcept(++ptr)){
				++ptr;
				return *this;
			}
			constexpr random_access_iterator operator++(int)noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(++ptr)){
				random_access_iterator aux=*this;
				++ptr;
				return aux;
			}
			constexpr random_access_iterator& operator--()noexcept(noexcept(--ptr)){
				--ptr;
				return *this;
			}
			constexpr random_access_iterator operator--(int)noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(--ptr)){
				random_access_iterator aux=*this;
				--ptr;
				return aux;
			}
			constexpr random_access_iterator operator+(difference_type n)const noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(random_access_iterator(this->ptr+n))){
				return random_access_iterator(this->ptr+n);
			}
			constexpr random_access_iterator operator-(difference_type n)const noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(random_access_iterator(this->ptr-n))){
				return random_access_iterator(this->ptr-n);
			}
			friend constexpr random_access_iterator operator+(difference_type n,random_access_iterator it)noexcept(mpv::is_nothrow_copy_constructible_v<pointer> && noexcept(it+=n)){
				return it+=n;
			}
			constexpr random_access_iterator& operator+=(difference_type n)noexcept(noexcept(ptr+=n)){
				ptr+=n;
				return *this;
			}
			constexpr random_access_iterator& operator-=(difference_type n)noexcept(noexcept(ptr-=n)){
				ptr-=n;
				return *this;
			}
			constexpr difference_type operator-(const random_access_iterator& other)const noexcept(noexcept(this->ptr-other.ptr)){
				return this->ptr-other.ptr;
			}
			constexpr difference_type operator-(const const_random_access_iterator<Types>& other)const noexcept(noexcept(this->ptr-other.ptr)){
				return this->ptr-other.ptr;
			}
			constexpr bool operator==(const random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr==other.ptr))){
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr!=other.ptr))){
				return this->ptr!=other.ptr;
			}
			constexpr bool operator<=(const random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr<=other.ptr))){
				return this->ptr<=other.ptr;
			}
			constexpr bool operator>=(const random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr>=other.ptr))){
				return this->ptr>=other.ptr;
			}
			constexpr bool operator<(const random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr<other.ptr))){
				return this->ptr<other.ptr;
			}
			constexpr bool operator>(const random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr>other.ptr))){
				return this->ptr>other.ptr;
			}
			constexpr bool operator==(const const_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr==other.ptr))){
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const const_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr!=other.ptr))){
				return this->ptr!=other.ptr;
			}
			constexpr bool operator<=(const const_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr<=other.ptr))){
				return this->ptr<=other.ptr;
			}
			constexpr bool operator>=(const const_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr>=other.ptr))){
				return this->ptr>=other.ptr;
			}
			constexpr bool operator<(const const_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr<other.ptr))){
				return this->ptr<other.ptr;
			}
			constexpr bool operator>(const const_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr>other.ptr))){
				return this->ptr>other.ptr;
			}
	};
	template<typename Types>
	class const_random_access_iterator{
		friend class random_access_iterator<Types>;
		public:
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
			using iterator_category=typename iterator_traits<pointer>::iterator_category;
		private:
			pointer ptr{};
		public:
			constexpr const_random_access_iterator()noexcept(mpv::is_nothrow_default_constructible_v<pointer>)=default;
			constexpr const_random_access_iterator(pointer ptr)noexcept(mpv::is_nothrow_copy_constructible_v<pointer>):ptr(ptr){}
			constexpr const_random_access_iterator(const random_access_iterator<Types>& nonconst_it)noexcept(noexcept(pointer(nonconst_it.ptr))):ptr(nonconst_it.ptr){}
			constexpr reference operator*()const noexcept(noexcept(reference(*ptr))){
				return *ptr;
			}
			constexpr reference operator[](difference_type n)const noexcept(noexcept(reference(*(ptr+n)))){
				return *(ptr+n);
			}
			constexpr pointer operator->()const noexcept(mpv::is_nothrow_copy_constructible_v<pointer>){
				return ptr;
			}
			constexpr const_random_access_iterator& operator++()noexcept(noexcept(++ptr)){
				++ptr;
				return *this;
			}
			constexpr const_random_access_iterator operator++(int)noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(++ptr)){
				const_random_access_iterator aux=*this;
				++ptr;
				return aux;
			}
			constexpr const_random_access_iterator& operator--()noexcept(noexcept(--ptr)){
				--ptr;
				return *this;
			}
			constexpr const_random_access_iterator operator--(int)noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(--ptr)){
				const_random_access_iterator aux=*this;
				--ptr;
				return aux;
			}
			constexpr const_random_access_iterator operator+(difference_type n)const noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(const_random_access_iterator(this->ptr+n))){
				return const_random_access_iterator(this->ptr+n);
			}
			constexpr const_random_access_iterator operator-(difference_type n)const noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(const_random_access_iterator(this->ptr-n))){
				return const_random_access_iterator(this->ptr-n);
			}
			friend constexpr const_random_access_iterator operator+(difference_type n,const_random_access_iterator it)noexcept(mpv::is_nothrow_copy_constructible_v<pointer> && noexcept(it+=n)){
				return it+=n;
			}
			constexpr const_random_access_iterator& operator+=(difference_type n)noexcept(noexcept(ptr+=n)){
				ptr+=n;
				return *this;
			}
			constexpr const_random_access_iterator& operator-=(difference_type n)noexcept(noexcept(ptr-=n)){
				ptr-=n;
				return *this;
			}
			constexpr difference_type operator-(const const_random_access_iterator& other)const noexcept(noexcept(this->ptr-other.ptr)){
				return this->ptr-other.ptr;
			}
			constexpr difference_type operator-(const random_access_iterator<Types>& other)const noexcept(noexcept(this->ptr-other.ptr)){
				return this->ptr-other.ptr;
			}
			constexpr bool operator==(const const_random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr==other.ptr))){
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const const_random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr!=other.ptr))){
				return this->ptr!=other.ptr;
			}
			constexpr bool operator<=(const const_random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr<=other.ptr))){
				return this->ptr<=other.ptr;
			}
			constexpr bool operator>=(const const_random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr>=other.ptr))){
				return this->ptr>=other.ptr;
			}
			constexpr bool operator<(const const_random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr<other.ptr))){
				return this->ptr<other.ptr;
			}
			constexpr bool operator>(const const_random_access_iterator& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr>other.ptr))){
				return this->ptr>other.ptr;
			}
	};
    template<typename It>
	class move_iterator{
		public:
			using base_category=iter_cat_t<It>;
			using iterator_category=If_t<is_base_of_v<random_access_iterator_tag,base_category>,random_access_iterator_tag,base_category>;
			using iterator_type=It;
			using value_type=iter_value_t<It>;
			using difference_type=iter_diff_t<It>;
			using pointer=It;
			using reference=If_t<is_reference_v<iter_ref_t<It>>,remove_reference_t<iter_ref_t<It>>&&,iter_ref_t<It>>;
			pointer ptr{};
			constexpr move_iterator()noexcept(mpv::is_nothrow_default_constructible_v<pointer>)=default;
			constexpr explicit move_iterator(pointer ptr)noexcept(mpv::is_nothrow_move_constructible_v<pointer>):ptr(mpv::move(ptr)){}
			template<typename It2,enable_if_t<mpv::is_constructible_v<pointer,const It2&>>* = nullptr>
			constexpr move_iterator(const move_iterator<It2>& other)noexcept(mpv::is_nothrow_constructible_v<pointer,const It2&>):ptr(other.base()){}
			template<typename It2,enable_if_t<mpv::is_assignable_v<pointer&,const It2&>>* = nullptr>
			constexpr move_iterator& operator=(const move_iterator<It2>& other)noexcept(mpv::is_nothrow_assignable_v<pointer&,const It2&>){
				ptr=other.base();
				return *this;
			}
			constexpr const iterator_type& base()const& noexcept{
				return ptr;
			}
			constexpr iterator_type base()&& noexcept(mpv::is_nothrow_move_constructible_v<iterator_type>){
				return mpv::move(ptr);
			}
			constexpr reference operator*()const noexcept(noexcept(reference(*ptr))){
				return reference(*ptr);
			}
			constexpr reference operator[](const difference_type n)const noexcept(noexcept(reference(ptr[n]))){
				return reference(ptr[n]);
			}
			constexpr pointer operator->()const noexcept(mpv::is_nothrow_copy_constructible_v<pointer>){
				return ptr;
			}
			constexpr move_iterator& operator++()noexcept(noexcept(++ptr)){
				++ptr;
				return *this;
			}
			constexpr move_iterator operator++(int)noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(++ptr)){
				move_iterator aux=*this;
				++ptr;
				return aux;
			}
			constexpr move_iterator& operator--()noexcept(noexcept(--ptr)){
				--ptr;
				return *this;
			}
			constexpr move_iterator operator--(int)noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(--ptr)){
				move_iterator aux=*this;
				--ptr;
				return aux;
			}
			constexpr move_iterator& operator+=(const difference_type n)noexcept(noexcept(ptr+=n)){
				ptr+=n;
				return *this;
			}
			constexpr move_iterator& operator-=(const difference_type n)noexcept(noexcept(ptr-=n)){
				ptr-=n;
				return *this;
			}
			constexpr move_iterator operator+(const difference_type n)const noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(move_iterator(ptr+n))){
				return move_iterator(ptr+n);
			}
			constexpr move_iterator operator-(const difference_type n)const noexcept(mpv::is_nothrow_copy_move_constructible_v<pointer> && noexcept(move_iterator(ptr-n))){
				return move_iterator(ptr-n);
			}
			friend constexpr move_iterator operator+(difference_type n,move_iterator iter)noexcept(mpv::is_nothrow_copy_constructible_v<pointer> && noexcept(iter+=n)){
				return iter+=n;
			}
			template<typename It2> constexpr auto operator-(const move_iterator<It2>& other)const noexcept(noexcept(ptr-other.base())) -> decltype(ptr-other.base()){
				return ptr-other.base();
			}
			template<typename It2> constexpr bool operator==(const move_iterator<It2>& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr==other.base()))){
				return this->ptr==other.base();
			}
			template<typename It2> constexpr bool operator!=(const move_iterator<It2>& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr!=other.base()))){
				return this->ptr!=other.base();
			}
			template<typename It2> constexpr bool operator<=(const move_iterator<It2>& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr<=other.base()))){
				return this->ptr<=other.base();
			}
			template<typename It2> constexpr bool operator>=(const move_iterator<It2>& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr>=other.base()))){
				return this->ptr>=other.base();
			}
			template<typename It2>constexpr bool operator<(const move_iterator<It2>& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr<other.base()))){
				return this->ptr<other.base();
			}
			template<typename It2> constexpr bool operator>(const move_iterator<It2>& other)const noexcept(noexcept(fake_copy_init<bool>(this->ptr>other.base()))){
				return this->ptr>other.base();
			}
	};
}