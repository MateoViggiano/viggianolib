#pragma once
namespace mpv{
	template<typename keyT,typename valT>
    struct MapPair COUNT_IT{
        using mapped_type=valT;
        using key_type=keyT;
        mapped_type val{};
        const key_type key;
        MapPair()=delete;
        template<typename kT,typename vT,enable_if_t<is_convertible_v<kT,key_type> && is_convertible_v<vT,mapped_type>>* = nullptr>
        constexpr MapPair(kT&& key,vT&& val)noexcept(is_nothrow_constructible_v<mapped_type,vT> && is_nothrow_constructible_v<key_type,kT>):val(static_cast<vT&&>(val)),key(static_cast<kT&&>(key)){}
        template<typename kT,enable_if_t<is_convertible_v<kT,key_type>>* = nullptr>
        constexpr explicit MapPair(kT&& key)noexcept(is_nothrow_default_constructible_v<mapped_type> && is_nothrow_constructible_v<key_type,kT>):key(static_cast<kT&&>(key)){}
        constexpr bool operator==(const MapPair& other)const noexcept(noexcept(fake_copy_init<bool>(this->key==other.key && this->val==other.val))){
            return this->key==other.key && this->val==other.val;
        }
        constexpr bool operator!=(const MapPair& other)const noexcept(noexcept(fake_copy_init<bool>(this->key!=other.key || this->val!=other.val))){
            return this->key!=other.key || this->val!=other.val;
        }
        constexpr bool operator<(const MapPair& other)const noexcept(noexcept(this->key<other.key ? true : other.key<this->key ? false : bool(this->val<other.val))){
            return this->key<other.key ? true : other.key<this->key ? false : bool(this->val<other.val);
        }
        constexpr bool operator>(const MapPair& other)const noexcept(noexcept(this->key>other.key ? true : other.key>this->key ? false : bool(this->val>other.val))){
            return this->key>other.key ? true : other.key>this->key ? false : bool(this->val>other.val);
        }
        constexpr bool operator<=(const MapPair& other)const noexcept(noexcept(*this>other)){
            return !(*this>other);
        }
        constexpr bool operator>=(const MapPair& other)const noexcept(noexcept(*this<other)){
            return !(*this<other);
        }
    };
    template<typename K,typename T,typename Cmp,typename Alloc,bool repeated> struct TreeMapTraits{
        using key_type=K;
        using value_type=MapPair<K,T>;
        using key_compare=Cmp;
        using allocator_type=Alloc;
        static constexpr bool allow_repeated=repeated;
        static constexpr const key_type& getKey(const value_type& val)noexcept{
            return val.key;
        }
        struct value_compare{
            protected:
                friend Tree<TreeMapTraits>;
                key_compare cmp;
            public:
                constexpr value_compare(key_compare cmp)noexcept(is_nothrow_copy_constructible_v<key_compare>):cmp(cmp){}
                constexpr bool operator()(const value_type& fst,const value_type& scnd)const noexcept(noexcept(fake_copy_init<bool>(cmp(fst.key,scnd.key)))){
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
            using typename base::BaseNodePtr;
            using mapped_type=T;
        private: 
            using Traits = TreeMapTraits<K,T,Cmp,Alloc,false>;
            using typename base::NodePtr;
            using base::to_node;
        public:
            constexpr const mapped_type& at(const key_type& key)const noexcept(noexcept(this->find(key)->val)){// key must be in the map
                return this->find(key)->val;
            }
            constexpr mapped_type& at(const key_type& key)noexcept(noexcept(this->find(key)->val)){// key must be in the map
                return this->find(key)->val;
            }
#define alloc       this->ct.getV2()
#define root        this->ct.getV3().prev
#define left_most   this->ct.getV3().right
#define right_most  this->ct.getV3().left
#define sent        this->ct.getV3().address()
            constexpr mapped_type& operator[](const key_type& key){
                BaseNodePtr x;
                if(root==sent){
                    x=root=left_most=right_most=this->create_node(key);
                    x->right=x->left=sent;
                }
                else if(!this->cmp(key,to_node(right_most)->data.key)){//val>=right_most->data
                    if(this->cmp(to_node(right_most)->data.key,key)){//val>right_most->data
                        right_most->right=this->create_node(key);
                        right_most->right->prev=right_most;
                        right_most=right_most->right;
                        right_most->right=sent;
                        x=right_most;
                    }
                    else/*val==right_most->data*/ return to_node(right_most)->data.val;
                }
                else if(!this->cmp(to_node(left_most)->data.key,key)){//val<=left_most->data
                    if(this->cmp(key,to_node(left_most)->data.key)){//val<left_most->data
                        left_most->left=this->create_node(key);
                        left_most->left->prev=left_most;
                        left_most=left_most->left;
                        left_most->left=sent;
                        x=left_most;
                    }
                    else/*val==left_most->data*/ return to_node(left_most)->data.val;
                }
                else{
                    x=root;
                    while(true){
                        if(this->cmp(key,to_node(x)->data.key)){
                            if(x->left==nullptr){
                                x->left=this->create_node(key);
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else if(this->cmp(to_node(x)->data.key,key)){
                            if(x->right==nullptr){
                                x->right=this->create_node(key);
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                        else return to_node(x)->data.val;
                    }
                }
                this->length++;
                this->insert_fixup(x);
                return to_node(x)->data.val;
            }
#undef root
#undef alloc
#undef left_most
#undef right_most
#undef sent
            constexpr Optional<mapped_type> get(const key_type& key)const noexcept(noexcept(fake_copy_init<Optional<mapped_type>>(this->find(key)->val))){
                const_iterator p=this->find(key);
                if(p!=this->end()) return p->val;
                else return {};
            }
            constexpr Optional<mapped_type> pop_at(const key_type& key)noexcept(noexcept(Optional<value_type>(this->pop_elem(key))) && noexcept(fake_copy_init<Optional<mapped_type>>(mpv::move(mpv::declval<mapped_type&>())))){
                Optional<value_type> x=this->pop_elem(key);
                if(x.has_value())return mpv::move(x->val);
                else return {};
            }
            constexpr MapKeys<Map> keys()const& noexcept{
                return *this;
            }
            constexpr ConstMapValues<Map> values()const& noexcept{
                return *this;
            }
            constexpr MapValues<Map> values()& noexcept{
                return *this;
            }
            MapKeys<Map> keys()const&&=delete;
            ConstMapValues<Map> values()const&&=delete;
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
            constexpr MapKeys<MultiMap> keys()const& noexcept{
                return *this;
            }
            constexpr ConstMapValues<MultiMap> values()const& noexcept{
                return *this;
            }
            constexpr MapValues<MultiMap> values()& noexcept{
                return *this;
            }
            MapKeys<MultiMap> keys()const&&=delete;
            ConstMapValues<MultiMap> values()const&&=delete;
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
            using reference=const typename MapType::key_type&;
            using const_reference=const typename MapType::key_type&;
            using pointer=const typename MapType::key_type*;
            using const_pointer=pointer;
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
                    Ptr ptr{};
                public:
                    constexpr iterator()noexcept=default;
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
			constexpr bool contains(const_reference val)const noexcept(noexcept(this->map.contains(val))){
                return map.contains(val);
			}
            template<bool enable=MapType::allow_repeated>
            constexpr enable_if_t<enable,size_type> count(const_reference val)const noexcept(noexcept(this->map.count(val))){
                return map.count(val);
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
            using reference=value_type&;
            using const_reference=const value_type&;
            using pointer=value_type*;
            using const_pointer=const value_type*;
        private:
            MapType& map;
        public:
        	class const_iterator;
            class iterator{
                friend class MapValues<Map_t>::const_iterator;
                public:
                    using Ptr=typename MapType::iterator;
                    using iterator_category=bidirectional_iterator_tag;
                    using value_type=typename MapType::mapped_type;
                    using difference_type=typename MapType::difference_type;
                    using pointer=typename MapType::mapped_type*;
                    using reference=typename MapType::mapped_type&;
                private:
                    Ptr ptr{};
                public:
                    constexpr iterator()noexcept=default;
                    constexpr iterator(Ptr ptr)noexcept:ptr(ptr){}
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
                    constexpr bool operator==(const const_iterator& other)const noexcept{
                        return this->ptr==other.ptr;
                    }
                    constexpr bool operator!=(const const_iterator& other)const noexcept{
                        return this->ptr!=other.ptr;
                    }
            };
            class const_iterator{
                friend class MapValues<Map_t>::iterator;
                public:
                    using Ptr=typename MapType::const_iterator;
                    using iterator_category=bidirectional_iterator_tag;
                    using value_type=typename MapType::mapped_type;
                    using difference_type=typename MapType::difference_type;
                    using pointer=const typename MapType::mapped_type*;
                    using reference=const typename MapType::mapped_type&;
                private:
                    Ptr ptr{};
                public:
                    constexpr const_iterator()noexcept=default;
                    constexpr const_iterator(Ptr ptr)noexcept:ptr(ptr){}
                    constexpr const_iterator(MapValues<Map_t>::iterator nonconst_it)noexcept:ptr(nonconst_it.ptr){}
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
            constexpr MapValues(MapType& map)noexcept:map(map){}
            template<typename MT=MapType>
            constexpr size_type size()const noexcept{
                return map.size();
            }
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const noexcept(noexcept(fake_copy_init<bool>(func(*this->begin())))){
                for(const_reference x:*this)
                    if(func(x)) return true;
                return false;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{})const noexcept(noexcept(func(*this->begin()))){
				for(const_reference x:*this)
					func(x);
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{})noexcept(noexcept(func(*this->begin()))){
				for(reference x:*this)
					func(x);
			}
			constexpr bool contains(const_reference val)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()==val))){
				for(const_reference x:*this)
					if(x==val) return true;
				return false;
			}
            constexpr size_type count(const_reference val)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()==val))){
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
    template<typename Map_t>
    class ConstMapValues{
        public:
            using MapType=Map_t;
            using value_type=typename MapType::mapped_type;
            using size_type=typename MapType::size_type;
            using difference_type=typename MapType::difference_type;
            using reference=const value_type&;
            using const_reference=reference;
            using pointer=const value_type*;
            using const_pointer=pointer;
        private:
            const MapType& map;
        public:
            class const_iterator{
                public:
                    using Ptr=typename MapType::const_iterator;
                    using iterator_category=bidirectional_iterator_tag;
                    using value_type=typename MapType::mapped_type;
                    using difference_type=typename MapType::difference_type;
                    using pointer=const typename MapType::mapped_type*;
                    using reference=const typename MapType::mapped_type&;
                private:
                    Ptr ptr{};
                public:
                    constexpr const_iterator()noexcept=default;
                    constexpr const_iterator(Ptr ptr)noexcept:ptr(ptr){}
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
            using iterator=const_iterator;
            constexpr ConstMapValues(const MapType& map)noexcept:map(map){}
            template<typename MT=MapType>
            constexpr size_type size()const noexcept{
                return map.size();
            }
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const noexcept(noexcept(fake_copy_init<bool>(func(*this->begin())))){
                for(const_reference x:*this)
                    if(func(x)) return true;
                return false;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{})const noexcept(noexcept(func(*this->begin()))){
				for(const_reference x:*this)
					func(x);
			}
			constexpr bool contains(const_reference val)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()==val))){
				for(const_reference x:*this)
					if(x==val) return true;
				return false;
			}
            constexpr size_type count(const_reference val)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()==val))){
                size_type counter=0;
                for(const_reference x:*this)
                    if(x==val) counter++;
                return counter;
            }
            constexpr const_iterator begin()const noexcept{
                return map.begin();
            }
            constexpr const_iterator end()const noexcept{
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
    template<typename Out,typename Map_t>
    Out& operator<<(Out& stream,const ConstMapValues<Map_t>& values){
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