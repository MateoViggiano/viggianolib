#pragma once
namespace mpv{
	template<typename> class const_HoleIterator;
	template<typename Types>
	class HoleIterator{
		friend class const_HoleIterator<Types>;
		public:
			using vector_optional=typename Types::vector_optional;
			using iterator_category=bidirectional_iterator_tag;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=typename Types::reference;
			using size_type=typename Types::size_type;
			using Ptr=typename Types::vector_optional::iterator;
		private:
			vector_optional* vecptr{};
			Ptr it{};
		public:
			constexpr HoleIterator()noexcept=default;
			constexpr HoleIterator(vector_optional* vecptr,Ptr iter)noexcept:vecptr(vecptr),it(iter){
				while(it<vecptr->end() && !it->has_value()) ++it;
			}
			constexpr reference operator*()const noexcept{
				return **it;//primer * para desreferenciar el iterador y segundo * para ir al valor del Optional
			}
			constexpr pointer operator->()const noexcept{
				return pointer_traits<pointer>::pointer_to(**it);
			}
			constexpr size_type get_raw_index()const noexcept{
				return it-vecptr->begin();
			}
			constexpr HoleIterator& operator++()noexcept{
				if(it<vecptr->end()) ++it;
				while(it<vecptr->end() && !it->has_value()){
					++it;
				}
				return *this;
			}
			constexpr HoleIterator operator++(int)noexcept{
				HoleIterator aux=*this;
				++(*this);
				return aux;
			}
			constexpr HoleIterator& operator--()noexcept{
				if(it>=vecptr->begin()) --it;
				while(it>=vecptr->begin() && !it->has_value()){
					--it;
				}
				return *this;
			}
			constexpr HoleIterator operator--(int)noexcept{
				HoleIterator aux=*this;
				--(*this);
				return aux;
			}
			constexpr bool operator==(const HoleIterator& other)const noexcept{
				return this->it==other.it;
			}
			constexpr bool operator!=(const HoleIterator& other)const noexcept{
				return this->it!=other.it;
			}
			constexpr bool operator>=(const HoleIterator& other)const noexcept{
				return this->it>=other.it;
			}
			constexpr bool operator<=(const HoleIterator& other)const noexcept{
				return this->it<=other.it;
			}
			constexpr bool operator>(const HoleIterator& other)const noexcept{
				return this->it>other.it;
			}
			constexpr bool operator<(const HoleIterator& other)const noexcept{
				return this->it<other.it;
			}
			constexpr bool operator==(const const_HoleIterator<Types>& other)const noexcept{
				return this->it==other.it;
			}
			constexpr bool operator!=(const const_HoleIterator<Types>& other)const noexcept{
				return this->it!=other.it;
			}
			constexpr bool operator>=(const const_HoleIterator<Types>& other)const noexcept{
				return this->it>=other.it;
			}
			constexpr bool operator<=(const const_HoleIterator<Types>& other)const noexcept{
				return this->it<=other.it;
			}
			constexpr bool operator>(const const_HoleIterator<Types>& other)const noexcept{
				return this->it>other.it;
			}
			constexpr bool operator<(const const_HoleIterator<Types>& other)const noexcept{
				return this->it<other.it;
			}
	};
	template<typename Types>
	class const_HoleIterator{
		friend class HoleIterator<Types>;
		public:
			using vector_optional=typename Types::vector_optional;
			using iterator_category=bidirectional_iterator_tag;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
			using size_type=typename Types::size_type;
			using Ptr=typename Types::vector_optional::const_iterator;
		private:
			const vector_optional* vecptr{};
			Ptr it{};
		public:
			constexpr const_HoleIterator()noexcept=default;
			constexpr const_HoleIterator(const HoleIterator<Types>& nonconst_it)noexcept:vecptr(nonconst_it.vecptr),it(nonconst_it.it){
				while(it<vecptr->end() && !it->has_value()) ++it;
			}
			constexpr const_HoleIterator(const vector_optional* vecptr,Ptr iter)noexcept:vecptr(vecptr),it(iter){
				while(it<vecptr->end() && !it->has_value()) ++it;
			}
			constexpr reference operator*()const noexcept{
				return **it;//primer * para desreferenciar el iterador y segundo * para ir al valor del Optional
			}
			constexpr pointer operator->()const noexcept{
				return pointer_traits<pointer>::pointer_to(**it);
			}
			constexpr size_type get_raw_index()const noexcept{
				return it-vecptr->begin();
			}
			constexpr const_HoleIterator& operator++()noexcept{
				if(it<vecptr->end()) ++it;
				while(it<vecptr->end() && !it->has_value()){
					++it;
				}
				return *this;
			}
			constexpr const_HoleIterator operator++(int)noexcept{
				const_HoleIterator aux=*this;
				++(*this);
				return aux;
			}
			constexpr const_HoleIterator& operator--()noexcept{
				if(it>=vecptr->begin()) --it;
				while(it>=vecptr->begin() && !it->has_value()){
					--it;
				}
				return *this;
			}
			constexpr const_HoleIterator operator--(int)noexcept{
				const_HoleIterator aux=*this;
				--(*this);
				return aux;
			}
			constexpr bool operator==(const const_HoleIterator& other)const noexcept{
				return this->it==other.it;
			}
			constexpr bool operator!=(const const_HoleIterator& other)const noexcept{
				return this->it!=other.it;
			}
			constexpr bool operator>=(const const_HoleIterator& other)const noexcept{
				return this->it>=other.it;
			}
			constexpr bool operator<=(const const_HoleIterator& other)const noexcept{
				return this->it<=other.it;
			}
			constexpr bool operator>(const const_HoleIterator& other)const noexcept{
				return this->it>other.it;
			}
			constexpr bool operator<(const const_HoleIterator& other)const noexcept{
				return this->it<other.it;
			}
	};
    template<typename T,typename Alloc=allocator<T>,typename realloc_params=params<typename allocator_traits<Alloc>::template rebind_traits<T>::size_type,VECTOR_MMS,2>>
    class HoleVector{
        private:
            using AlTy=rebind_alloc<Alloc,T>;
            using AlTy_traits=allocator_traits<AlTy>;		
			using OptVec=Vector<Optional<T>,rebind_alloc<Alloc,Optional<T>>,realloc_params>;
			struct Val_types{
				using value_type=T;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=T&;
				using const_reference=const T&;
				using vector_optional=OptVec;
			};
			template<typename,typename,typename> friend class HoleVector;
        public:
			using allocator_type=Alloc;
			using value_type=typename Val_types::value_type;
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
            using iterator=HoleIterator<Val_types>;
            using const_iterator=const_HoleIterator<Val_types>;
		private:
			using StackVec=Vector<size_type,rebind_alloc<Alloc,size_type>,realloc_params>;
            OptVec vec;
            StackVec hole_stack;
		public:
			constexpr HoleVector()noexcept(is_nothrow_default_constructible_v<StackVec> && is_nothrow_default_constructible_v<OptVec>) = default;
			constexpr explicit HoleVector(const Alloc& al)noexcept:vec(al),hole_stack(al){}
			constexpr HoleVector(const HoleVector& other) = default;
			constexpr HoleVector(HoleVector&& other)noexcept(is_nothrow_move_constructible_v<OptVec> && is_nothrow_move_constructible_v<StackVec>) = default;
			constexpr HoleVector& operator=(const HoleVector& other){
				ClearGuard cg1(vec);
				ClearGuard cg2(hole_stack);
				this->vec=other.vec;
				this->hole_stack=other.hole_stack;
				cg1.cont=nullptr;
				cg2.cont=nullptr;
				return *this;
			}
			constexpr HoleVector& operator=(HoleVector&& other)noexcept(noexcept(vec=static_cast<OptVec&&>(other.vec)) && noexcept(hole_stack=static_cast<StackVec&&>(other.hole_stack))){
				ClearGuard cg1(vec);
				ClearGuard cg2(hole_stack);
				this->vec=static_cast<OptVec&&>(other.vec);
				this->hole_stack=static_cast<StackVec&&>(other.hole_stack);
				cg1.cont=nullptr;
				cg2.cont=nullptr;
				return *this;
			}

            template<typename It, enable_if_t<is_iterator_v<It> && !is_specialization_v<remove_cvref_t<iter_ref_t<It>>,Optional>,int> = 0>
            constexpr HoleVector(It first,It last,const Alloc& al=Alloc{}):vec(first,last,al),hole_stack(al){}
            constexpr HoleVector(std::initializer_list<value_type> initlist,const Alloc& al=Alloc()):vec(initlist.begin(),initlist.end(),al),hole_stack(al){}
			constexpr void copy_dist(const HoleVector& other){
				vec.clear();
				ClearGuard cg1(vec);
				ClearGuard cg2(hole_stack);
				hole_stack=other.hole_stack;
				vec.resize(other.vec.size());
				for(const_iterator it=--other.end();it>=other.begin();--it){
					hole_stack.push_back(it.get_raw_index());
				}
				cg1.cont=nullptr;
				cg2.cont=nullptr;
			}
			template<typename... Args>
			constexpr iterator emplace(Args&&... args){
				if(hole_stack.empty()){
					vec.emplace_back(value_type(static_cast<Args&&>(args)...));
					return iterator(&vec,vec.end()-1);
				}
				else{
					size_type raw_index=hole_stack.back();
					vec[raw_index].emplace(static_cast<Args&&>(args)...);
					hole_stack.del_back();
					return iterator(&vec,vec.begin()+raw_index);
				}
			}
			constexpr iterator insert(const T& val){
				return emplace(val);
			}
			constexpr iterator insert(T&& val){
				return emplace(static_cast<T&&>(val));
			}
			constexpr void del(const_iterator pos){
				hole_stack.emplace_back(pos.get_raw_index());
				vec[pos.get_raw_index()].reset();
			}
			constexpr void del_at(size_type raw_index){
				hole_stack.emplace_back(raw_index);
				vec[raw_index].reset();
			}
			constexpr bool remove(const T& val){
				for(iterator it=begin();it!=end();++it){
					if(*it==val){
						del(it);
						return true;
					}
				}
				return false;
			}
			constexpr value_type pop(const_iterator pos){
				value_type aux=static_cast<value_type&&>(vec[pos.get_raw_index()].value());
				hole_stack.emplace_back(pos.get_raw_index());
				vec[pos.get_raw_index()].reset();
				return aux;
			}
			constexpr value_type pop_at(size_type raw_index){
				value_type aux=static_cast<value_type&&>(vec[raw_index].value());
				hole_stack.emplace_back(raw_index);
				vec[raw_index].reset();
				return aux;
			}			
			constexpr size_type remove_all(const T& val){
				size_type prev_size=size();
				iterator pos=end();
				for(iterator it=begin();it!=end();++it){
					if(mpv::addressof(val)==mpv::addressof(*it)){
						pos=it;
					}
					else if(*it==val) del(it);
				}
				if(pos!=end()) del(pos);
				return prev_size-size();
			}
			constexpr const_reference operator[](size_type raw_index)const noexcept{
                return vec[raw_index].value();
            }//Este contenedor NO tiene acceso aleatorio. Si se indexa una posicion tiene que ser porque se sabe de antemano que hay un valor valido ahi
            constexpr reference operator[](size_type raw_index)noexcept{
                return vec[raw_index].value();
            }//Este contenedor NO tiene acceso aleatorio. Si se indexa una posicion tiene que ser porque se sabe de antemano que hay un valor valido ahi
            constexpr const_reference back()const noexcept{
                return *(--end());
            }
            constexpr reference back()noexcept{
                return *(--end());
            }
            bool has_value_at(size_type raw_index)const noexcept{
                return vec[raw_index].has_value();
            }
            constexpr size_type size()const noexcept{
                return vec.size()-hole_stack.size();
            }
			constexpr bool empty()const noexcept{
				return vec.size()-hole_stack.size()==0;
			}
            constexpr void clear()noexcept{
                vec.clear();
				hole_stack.clear();
            }
            constexpr bool operator==(const HoleVector& other)const noexcept(noexcept(equal(this->begin(),this->end(),other.begin()))){
                if(this->size()!=other.size()) return false;
                else return equal(this->begin(),this->end(),other.begin());
            }
            constexpr bool operator!=(const HoleVector& other)const noexcept(noexcept(equal(this->begin(),this->end(),other.begin()))){
                if(this->size()!=other.size()) return true;
                else return !equal(this->begin(),this->end(),other.begin());
            }
            constexpr bool operator<(const HoleVector& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool operator>(const HoleVector& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator<=(const HoleVector& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator>=(const HoleVector& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const noexcept(noexcept(fake_copy_init<bool>(func(*this->begin())))){
                for(const_reference x:*this)
                    if(func(x))return true;
                return false;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{})const noexcept(noexcept(func(*this->begin()))){
				for(const_reference x:*this)
					func(x);
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{}) noexcept(noexcept(func(*this->begin()))){
				for(reference x:*this)
					func(x);
			}
			template<typename Lambda>
			constexpr auto map(Lambda&& func=Lambda{})const{
				using U=mpv::remove_cvref_t<decltype(func(declval<const_reference>()))>;
				HoleVector<U,rebind_alloc<Alloc,U>,realloc_params> new_vec(get_allocator());
				new_vec.vec.resize(this->vec.size());
				new_vec.hole_stack=this->hole_stack;
				for(size_type i=0;i<vec.size();i++){
					if(vec[i].has_value())
						new_vec.vec[i]=func(*vec[i]);
				}
				return new_vec;
			}
			template<typename Lambda>
			constexpr HoleVector filter(Lambda&& func=Lambda{})const{
				HoleVector new_vec(get_allocator());
				new_vec.vec.resize(this->vec.size());
				new_vec.hole_stack=this->hole_stack;
				for(size_type i=0;i<vec.size();i++){
					if(vec[i].has_value()){
						if(func(*vec[i])) new_vec.vec[i]=this->vec[i];
						else new_vec.hole_stack.push_back(i);
					}
				}
				return new_vec;
			}
			constexpr HoleVector& reverse()noexcept(noexcept(mpv::reverse(this->begin(),this->end()))){
				mpv::reverse(begin(),end());
				return *this;
			}
			template<typename Pred=less<>>
			constexpr void sort(Pred pred=Pred{}){
				mpv::insertion_sort(begin(),end(),pred);
			}
            constexpr size_type count(const T& val)const noexcept(noexcept(mpv::count<const_iterator,T,size_type>(this->begin(),this->end(),val))){
                return mpv::count<const_iterator,T,size_type>(begin(),end(),val);
            }
			constexpr bool contains(const T& val)const noexcept(noexcept(mpv::contains<const_iterator,T>(this->begin(),this->end(),val))){
				return mpv::contains<const_iterator,T>(begin(),end(),val);
			}
            constexpr const_iterator find(const T& val)const noexcept(noexcept(mpv::find(this->begin(),this->end(),val))){
                return mpv::find(begin(),end(),val);
            }
            constexpr iterator find(const T& val)noexcept(noexcept(mpv::find(this->begin(),this->end(),val))){
                return mpv::find(begin(),end(),val);
            }
            constexpr allocator_type get_allocator()const noexcept{
                return vec.get_allocator();
            }
			constexpr const OptVec& get_internal_vector()const noexcept{
				return vec;
			}
            constexpr iterator begin()noexcept{
                return iterator(&vec,vec.begin());
            }
            constexpr iterator end()noexcept{
                return iterator(&vec,vec.end());
            }
            constexpr const_iterator begin()const noexcept{
                return const_iterator(&vec,vec.begin());
            }
            constexpr const_iterator end()const noexcept{
                return const_iterator(&vec,vec.end());
            }

    };
    template<typename Out,typename T,typename Alloc,typename realloc_params>
    Out& operator<<(Out& stream,const HoleVector<T,Alloc,realloc_params>& hvec){
		using reference=typename HoleVector<T,Alloc,realloc_params>::const_iterator::reference;
		typename HoleVector<T,Alloc,realloc_params>::size_type remaining=hvec.size();
		stream<<"[";
		for(reference x:hvec){
			stream<<""<<x;
			if(--remaining)
				stream<<", ";
		}
		stream<<"]";
		return stream;
    }
}