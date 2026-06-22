#pragma once
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