#pragma once
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