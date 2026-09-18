#pragma once
namespace mpv{
	template<typename,typename> class List;
	template<typename,typename> struct List_Node;

	template<typename T,typename VoidPtr>
	struct Base_List_Node{
		using BaseNodePtr=rebind_pointer<VoidPtr,Base_List_Node>;
		using const_BaseNodePtr=rebind_pointer<VoidPtr,const Base_List_Node>;
		using NodePtr=rebind_pointer<VoidPtr,List_Node<T,VoidPtr>>;
		BaseNodePtr next;
		BaseNodePtr prev;
		constexpr void link_right(BaseNodePtr other)noexcept{//links other to the right of this
			this->next->prev=other;
			other->next=this->next;
			this->next=other;
			other->prev=this->address();
		}
		constexpr void link_left(BaseNodePtr other)noexcept{//links other to the left of this
			this->prev->next=other;
			other->prev=this->prev;
			this->prev=other;
			other->next=this->address();
		}
		constexpr void unlink()noexcept{
			this->prev->next=this->next;
			this->next->prev=this->prev;
		}
		constexpr BaseNodePtr address()const noexcept{
			return pointer_traits<BaseNodePtr>::pointer_to(*const_cast<Base_List_Node*>(this));
		}
	};
	template<typename T,typename VoidPtr>
	struct List_Node:public Base_List_Node<T,VoidPtr>{
		T data;
		template<typename... Args>
		constexpr List_Node(Args&&... args)noexcept(is_nothrow_constructible_v<T,Args...>):data(static_cast<Args&&>(args)...){}
		List_Node(const List_Node&)=delete;
		List_Node& operator=(const List_Node&)=delete;
	};
	template<typename> class const_List_iterator;
	template<typename Types>
	class List_iterator{
		template<typename,typename> friend class List;
		friend class const_List_iterator<Types>;
		using const_iterator=const_List_iterator<Types>;
		public:
			using iterator_category=bidirectional_iterator_tag;
			using BaseNodePtr=typename Types::BaseNodePtr;
			using Node=typename Types::Node;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=typename Types::reference;
		private:
			BaseNodePtr ptr{};
		public:
			constexpr List_iterator()=default;
			constexpr List_iterator(BaseNodePtr ptr)noexcept:ptr(ptr){}
			constexpr reference operator*()const noexcept{
				return static_cast<Node&>(*ptr).data;
			}
			constexpr pointer operator->()const noexcept{
				return pointer_traits<pointer>::pointer_to(static_cast<Node&>(*ptr).data);
			}
			constexpr List_iterator& operator++()noexcept{
				ptr=ptr->next;
				return *this;
			}
			constexpr List_iterator operator++(int)noexcept{
				List_iterator aux=*this;
				ptr=ptr->next;
				return aux;
			}
			constexpr List_iterator& operator--()noexcept{
				ptr=ptr->prev;
				return *this;
			}
			constexpr List_iterator operator--(int)noexcept{
				List_iterator aux=*this;
				ptr=ptr->prev;
				return aux;
			}
			constexpr List_iterator operator+(difference_type n)const noexcept{
				List_iterator ret(this->ptr);
				advance(ret,n);
				return ret;
			}
			constexpr List_iterator operator-(difference_type n)const noexcept{
				List_iterator ret(this->ptr);
				advance(ret,-n);
				return ret;
			}
			constexpr List_iterator& operator+=(difference_type n)noexcept{
				advance(*this,n);
				return *this;
			}
			constexpr List_iterator& operator-=(difference_type n)noexcept{
				advance(*this,-n);
				return *this;
			}
			constexpr bool operator==(const const_iterator& other)const noexcept{
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const const_iterator& other)const noexcept{
				return this->ptr!=other.ptr;
			}
	};
	template<typename Types>
	class const_List_iterator{
		template<typename,typename> friend class List;
		friend class List_iterator<Types>;
		public:
			using iterator_category=bidirectional_iterator_tag;
			using BaseNodePtr=typename Types::BaseNodePtr;
			using Node=typename Types::Node;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
		private:
			BaseNodePtr ptr{};
		public:
			constexpr const_List_iterator()=default;
			constexpr const_List_iterator(BaseNodePtr ptr)noexcept:ptr(ptr){}
			constexpr const_List_iterator(List_iterator<Types> nonconst_it)noexcept:ptr(nonconst_it.ptr){}
			constexpr reference operator*()const noexcept{
				return static_cast<Node&>(*ptr).data;
			}
			constexpr pointer operator->()const noexcept{
				return pointer_traits<pointer>::pointer_to(static_cast<Node&>(*ptr).data);
			}
			constexpr const_List_iterator& operator++()noexcept{
				ptr=ptr->next;
				return *this;
			}
			constexpr const_List_iterator operator++(int)noexcept{
				const_List_iterator aux=*this;
				ptr=ptr->next;
				return aux;
			}
			constexpr const_List_iterator& operator--()noexcept{
				ptr=ptr->prev;
				return *this;
			}
			constexpr const_List_iterator operator--(int)noexcept{
				const_List_iterator aux=*this;
				ptr=ptr->prev;
				return aux;
			}
			constexpr const_List_iterator operator+(difference_type n)const noexcept{
				const_List_iterator ret(this->ptr);
				advance(ret,n);
				return ret;
			}
			constexpr const_List_iterator operator-(difference_type n)const noexcept{
				const_List_iterator ret(this->ptr);
				advance(ret,-n);
				return ret;
			}
			constexpr const_List_iterator& operator+=(difference_type n)noexcept{
				advance(*this,n);
				return *this;
			}
			constexpr const_List_iterator& operator-=(difference_type n)noexcept{
				advance(*this,-n);
				return *this;
			}
			constexpr bool operator==(const const_List_iterator& other)const noexcept{
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const const_List_iterator& other)const noexcept{
				return this->ptr!=other.ptr;
			}
	};
	template<typename T,typename Alloc=allocator<T>>
	class List COUNT_IT{
		private:
			using AlTy=rebind_alloc<Alloc,T>;
			using AlTy_traits=allocator_traits<AlTy>;		
			using BaseNode=Base_List_Node<T,typename allocator_traits<Alloc>::void_pointer>;
			using AlBase=rebind_alloc<Alloc,BaseNode>;
			using AlBase_traits=allocator_traits<AlBase>;
			using BaseNodePtr=typename AlBase_traits::pointer;
			using Node=List_Node<T,typename allocator_traits<Alloc>::void_pointer>;
			using AlNode=rebind_alloc<Alloc,Node>;
			using AlNode_traits=allocator_traits<AlNode>;			
			using NodePtr=typename AlNode_traits::pointer;
			using const_NodePtr=typename AlNode_traits::const_pointer;

			struct Val_types{
				using value_type=T;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=T&;
				using const_reference=const T&;
				using Node=List_Node<T,typename allocator_traits<Alloc>::void_pointer>;
				using BaseNodePtr=typename AlBase_traits::pointer;
			};
		public:
            static constexpr bool POCCA=AlTy_traits::propagate_on_container_copy_assignment::value;
            static constexpr bool POCMA=AlTy_traits::propagate_on_container_move_assignment::value;
            static constexpr bool POCS=AlTy_traits::propagate_on_container_swap::value;
            static constexpr bool ALWAYS_EQ=AlTy_traits::is_always_equal::value;
			using allocator_type=Alloc;
			using value_type=typename Val_types::value_type;
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
			using iterator=List_iterator<Val_types>;
			using const_iterator=const_List_iterator<Val_types>;
			using move_iterator=move_iterator<iterator>;
		private:
#define HEAD cp.V2.next
#define TAIL cp.V2.prev
#define NULLPTR cp.V2.address()
#define alloc cp.getV1()
			CompressedPair<AlNode,BaseNode> cp;
			size_type length=0;

			constexpr static NodePtr to_node(BaseNodePtr bp)noexcept{
				return pointer_traits<NodePtr>::pointer_to(static_cast<Node&>(*bp));
			}
			constexpr static BaseNodePtr to_base(NodePtr p)noexcept{
				return pointer_traits<BaseNodePtr>::pointer_to(static_cast<BaseNode&>(*p));
			}
			template<typename... Args>
			constexpr BaseNodePtr create_node(Args&&... args){
                AllocConstructPtr guard(alloc);
                guard.allocate();
				CONSTRUCT_VARARGS(this->alloc,guard.ptr,static_cast<Args&&>(args));
				guard.ptr->next=guard.ptr->prev=NULLPTR;
				return to_base(guard.release());
			}
			constexpr void delete_node(NodePtr node)noexcept{
				/* AlNode_traits::destroy */DESTROY(this->alloc,node);
				AlNode_traits::deallocate(this->alloc,node,1);
			}
			constexpr void destroy_and_free()noexcept{
				TAIL=NULLPTR;
				while(HEAD!=NULLPTR){
					NodePtr aux=to_node(HEAD);
					HEAD=HEAD->next;
					DESTROY(this->alloc,aux);
					AlNode_traits::deallocate(this->alloc,aux,1);
				}//TAIL and HEAD en up pointing to NULLPTR
			}
			constexpr void reset()noexcept{
				HEAD=TAIL=NULLPTR;
				length=0;
			}
			constexpr void set_null_sides()noexcept{
				this->HEAD->prev=this->TAIL->next=this->NULLPTR;
			}
			constexpr void link_back(BaseNodePtr node)noexcept{
				TAIL->next=node;// node is allready pointing to null when created
				TAIL->next->prev=TAIL;
				TAIL=TAIL->next;
			}
			constexpr NodePtr unlink_back()noexcept{
				NodePtr p(to_node(TAIL));
				p->unlink();
				return p;
			}
			constexpr NodePtr unlink_at(size_type index)noexcept{
				BaseNodePtr p(HEAD);
				for(size_type i=0;i<index;i++) p=p->next;
				p->unlink();
				return to_node(p);
			}
/*	Cada una de las funciones que siguen asume que recibe una lista valida en la que length coincide con 
	la actual cantidad de nodos  en la lista, sus nodos correctamente enlazados y apuntando a null 
	en caso de que size sea 0. 
	Modifican la lista this, dejan length coincidiendo con la cantidad de nodos y en caso de que modifiquen la
	segunda lista(porque la primera lista se apropia de los elementos de la segunda), se encargan de que esta
	siga siendo valida, dejando length en 0 y haciendo que HEAD y TAIL apunten a other.null

*/
			constexpr void copy_elements(const List& other){
				Guard guard(this->alloc,other.begin(),other.end());
				this->length=guard.transfer_at(HEAD);
			}
			constexpr void copy_elements(List&& other){
				Guard guard(this->alloc,move_iterator(other.begin()),move_iterator(other.end()));
				this->length=guard.transfer_at(HEAD);
			}
			constexpr void move_elements(List& other)noexcept{
				this->length=other.length;
				if(other.length){
					this->HEAD=other.HEAD;
					this->TAIL=other.TAIL;
					this->set_null_sides();
					other.reset();					
				}
				else{
					this->HEAD=this->NULLPTR;
					this->TAIL=this->NULLPTR;
				}
			}
			constexpr void copy_elements_keeping_this_allocation(const List& other){
				if(this->length>=other.length){
					BaseNodePtr i=this->HEAD, j=other.HEAD;
					while(j!=other.NULLPTR){
						to_node(i)->data=to_node(j)->data;
						i=i->next;
						j=j->next;
					}
					this->TAIL=i->prev;
					TAIL->next=this->NULLPTR;
					while(i!=this->NULLPTR){
						NodePtr aux=to_node(i);
						i=i->next;
						this->delete_node(aux);
					}
					this->length=other.length;
				}
				else{
					BaseNodePtr i=this->HEAD, j=other.HEAD;
					while(i!=this->NULLPTR){
						to_node(i)->data=to_node(j)->data;
						i=i->next;
						j=j->next;
					}
					Guard guard(this->alloc,const_iterator(j),other.end());
					this->length+=guard.transfer_at(this->NULLPTR);
				}
			}
			constexpr void copy_elements_keeping_this_allocation(List&& other){
				if(this->length>=other.length){
					BaseNodePtr i=this->HEAD, j=other.HEAD;
					while(j!=other.NULLPTR){
						to_node(i)->data=static_cast<value_type&&>(to_node(j)->data);
						i=i->next;
						j=j->next;
					}
					this->TAIL=i->prev;
					TAIL->next=this->NULLPTR;
					while(i!=this->NULLPTR){
						NodePtr aux=to_node(i);
						i=i->next;
						this->delete_node(aux);
					}
					this->length=other.length;
					other.clear();
				}
				else{
					BaseNodePtr i=this->HEAD, j=other.HEAD;
					while(i!=this->NULLPTR){
						to_node(i)->data=static_cast<value_type&&>(to_node(j)->data);
						i=i->next;
						j=j->next;
					}
					Guard guard(this->alloc,move_iterator(iterator(j)),move_iterator(other.end()));
					this->length+=guard.transfer_at(this->NULLPTR);
				}
			}
			constexpr void add_back(const List& other){
				Guard guard(this->alloc,other.begin(),other.end());
				this->length+=guard.transfer_at(this->NULLPTR);
			}
			constexpr void add_back(List&& other){
				Guard guard(this->alloc,move_iterator(other.begin()),move_iterator(other.end()));
				this->length+=guard.transfer_at(this->NULLPTR);
			}
			constexpr void move_back(List& other)noexcept{
				this->length+=other.length;
				this->TAIL->next=other.HEAD;
				other.HEAD->prev=this->TAIL;
				this->TAIL=other.TAIL;
				this->TAIL->next=this->NULLPTR;
				other.reset();
			}
			constexpr void add_front(const List& other){
				Guard guard(this->alloc,other.begin(),other.end());
				this->length+=guard.transfer_at(this->HEAD);
			}
			constexpr void add_front(List&& other){
				Guard guard(this->alloc,move_iterator(other.begin()),move_iterator(other.end()));
				this->length+=guard.transfer_at(this->HEAD);
			}
			constexpr void move_front(List& other)noexcept{
				this->length+=other.length;
				this->HEAD->prev=other.TAIL;
				other.TAIL->next=this->HEAD;
				this->HEAD=other.HEAD;
				this->HEAD->prev=this->NULLPTR;
				other.reset();
			}
			constexpr void move_at(BaseNodePtr p,List& other)noexcept{
				this->length+=other.length;
				p->prev->next=other.HEAD;
				other.HEAD->prev=p->prev;
				p->prev=other.TAIL;
				other.TAIL->next=p;
				other.reset();
			}
			struct Guard{
				AlNode& al;
				size_type length=0;
				BaseNodePtr head=nullptr,tail=nullptr;
				template<typename... Args>
				constexpr BaseNodePtr create_node(Args&&... args){
					AllocConstructPtr guard(al);
					guard.allocate();
					CONSTRUCT_VARARGS(this->al,guard.ptr,static_cast<Args&&>(args));
					guard.ptr->next=guard.ptr->prev=nullptr;
					return to_base(guard.release());
				}
				Guard(const Guard&)=delete;
				Guard& operator=(const Guard&)=delete;
				constexpr Guard(AlNode& al)noexcept:al(al){}
				template<typename It>
				constexpr Guard(AlNode& al,It first,It last):al(al){
					Guard guard(al);
					if(first!=last){
						guard.head=guard.tail=guard.create_node(*first++);
						guard.length=1;
						while(first!=last){
							guard.tail->next=guard.create_node(*first++);
							guard.tail->next->prev=guard.tail;
							guard.tail=guard.tail->next;
							++guard.length;
						}
						this->tail=guard.tail;
						this->head=guard.head;
						this->length=guard.length;
						guard.head=nullptr;
					}
				}
				constexpr Guard(AlNode& al,size_type sz):al(al){
					Guard guard(al);
					if(guard.length<sz){
						guard.head=guard.tail=guard.create_node();
						guard.length=1;
						while(guard.length<sz){
							guard.tail->next=guard.create_node();
							guard.tail->next->prev=guard.tail;
							guard.tail=guard.tail->next;
							++guard.length;
						}
						this->tail=guard.tail;
						this->head=guard.head;
						this->length=guard.length;
						guard.head=nullptr;
					}
				}
				constexpr Guard(AlNode& al,size_type sz,const value_type& fillwith):al(al){
					Guard guard(al);
					if(guard.length<sz){
						guard.head=guard.tail=guard.create_node(fillwith);
						guard.length=1;
						while(guard.length<sz){
							guard.tail->next=guard.create_node(fillwith);
							guard.tail->next->prev=guard.tail;
							guard.tail=guard.tail->next;
							++guard.length;
						}
						this->tail=guard.tail;
						this->head=guard.head;
						this->length=guard.length;
						guard.head=nullptr;
					}
				}
				constexpr size_type transfer_at(BaseNodePtr p)noexcept{
					if(length!=0){
						p->prev->next=head;
						head->prev=p->prev;
						p->prev=tail;
						tail->next=p;
						head=nullptr;
					}
					return length;
				}
				template<bool double_link=false,bool calculate_length=false>
				constexpr void push_back(BaseNodePtr p)noexcept{
					if(head==nullptr)
						head=tail=p;
					else{
						tail->next=p;
						if constexpr(double_link) p->prev=tail;
						tail=tail->next;
					}
					if constexpr(calculate_length) ++length;
				}
				~Guard()noexcept{
					while(head!=nullptr){
						NodePtr aux=to_node(head);
						head=head->next;
						DESTROY(al,aux);
						AlNode_traits::deallocate(al,aux,1);
					}
				}
			};
			struct ForwardGuard{
				AlNode& al;
				BaseNodePtr first;
				size_type length;
				ForwardGuard(const ForwardGuard&)=delete;
				ForwardGuard& operator=(const ForwardGuard&)=delete;
				ForwardGuard(AlNode& al,BaseNodePtr first,size_type length)noexcept:al(al),first(first),length(length){}
				~ForwardGuard(){
					while(length--){
						NodePtr aux=to_node(first);
						first=first->next;
						DESTROY(al,aux);
						AlNode_traits::deallocate(al,aux,1);
					}
				}
			};
		 public:
			constexpr List()noexcept(is_nothrow_default_constructible_v<AlNode>){
				HEAD=NULLPTR;
				TAIL=NULLPTR;
			}
			constexpr explicit List(const Alloc& al)noexcept:cp(arg1_tag{},al){
				HEAD=NULLPTR;
				TAIL=NULLPTR;
			}
			constexpr List(const List& other):cp(arg1_tag{},AlNode_traits::select_on_container_copy_construction(other.alloc)){
				HEAD=NULLPTR;
				TAIL=NULLPTR;
				Guard guard(this->alloc,other.begin(),other.end());
				length=guard.transfer_at(HEAD);
			}
			constexpr List(const List& other,const Alloc& al):cp(arg1_tag{},al){
				HEAD=NULLPTR;
				TAIL=NULLPTR;
				Guard guard(this->alloc,other.begin(),other.end());
				length=guard.transfer_at(HEAD);
			}
			constexpr List(List&& other)noexcept:cp(arg1_tag{},static_cast<AlNode&&>(other.alloc)),length(other.length){
				if(other.length){
					HEAD=other.HEAD;
					TAIL=other.TAIL;
					this->set_null_sides();
					other.reset();					
				}
				else{
					HEAD=NULLPTR;
					TAIL=NULLPTR;
				}
			}
			constexpr List(List&& other,const Alloc& al)noexcept(ALWAYS_EQ):cp(arg1_tag{},al),length(other.length){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				if(this->alloc==other.alloc){
					if(other.length){
						this->HEAD=other.HEAD;
						this->TAIL=other.TAIL;
						this->set_null_sides();
						other.reset();						
					}
				}
				else{
					this->copy_elements(static_cast<List&&>(other));
				}
			}
			constexpr explicit List(size_type length,const Alloc& al=Alloc()):cp(arg1_tag{},al){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				Guard guard(this->alloc,length);
				this->length=guard.transfer_at(HEAD);
			}
			constexpr List(size_type length,const value_type& fillwith,const Alloc& al=Alloc()):cp(arg1_tag{},al){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				Guard guard(this->alloc,length,fillwith);
				this->length=guard.transfer_at(HEAD);
			}
			template<typename It, enable_if_t<is_iterator_v<It>,int> = 0>
			constexpr List(It first,It last,const Alloc& al=Alloc{}):cp(arg1_tag{},al){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				Guard guard(this->alloc,first,last);
				length=guard.transfer_at(HEAD);
			}
			constexpr List(std::initializer_list<value_type> initlist){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				Guard guard(this->alloc,initlist.begin(),initlist.end());
				length=guard.transfer_at(HEAD);
			}
			constexpr List(std::initializer_list<value_type> initlist,const Alloc& al):cp(arg1_tag{},al){
				this->HEAD=this->NULLPTR;
				this->TAIL=this->NULLPTR;
				Guard guard(this->alloc,initlist.begin(),initlist.end());
				length=guard.transfer_at(HEAD);
			}
			constexpr List& operator=(const List& other){										//Operador de copia(optimizado)
				if(this!=&other){
					if constexpr(POCCA){
						if constexpr(!ALWAYS_EQ){
							if(this->alloc!=other.alloc){
								this->destroy_and_free();
								this->length=0;
								this->alloc=other.alloc;
								this->copy_elements(other);
								return *this;
							}							
						}
						this->alloc=other.alloc;
					}
					this->copy_elements_keeping_this_allocation(other);
				}
				return *this;
			}
			constexpr List& operator=(List&& other)noexcept(ALWAYS_EQ || POCMA){												//Operador de movimiento
				if(this==&other) return *this;
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->alloc!=other.alloc){
						this->copy_elements_keeping_this_allocation(static_cast<List&&>(other));
						return *this;
					}
				}
				this->destroy_and_free();
				pocma(this->alloc,other.alloc);
				this->move_elements(other);
				return *this;
			}
			constexpr List operator+(const List& other)const&{
				List new_list(*this);
				new_list.add_back(other);
				return new_list;
			}
			constexpr List operator+(List&& other)const&{
				List new_list(static_cast<List&&>(other));
				new_list.add_front(*this);
				return new_list;
			}
			constexpr List operator+(const List& other)&&{
				List new_list(static_cast<List&&>(*this));
				new_list.add_back(other);
				return new_list;
			}
			constexpr List operator+(List&& other)&& noexcept(ALWAYS_EQ){
				List new_list(static_cast<List&&>(*this));
				if constexpr(!ALWAYS_EQ){
					if(new_list.alloc==other.alloc) new_list.move_back(other);
					else new_list.add_back(static_cast<List&&>(other));
				}
				else new_list.move_back(other);
				return new_list;
			}
			constexpr List& operator+=(const List& other){
				this->add_back(other);
				return *this;
			}
			constexpr List& operator+=(List&& other)noexcept(ALWAYS_EQ){
				if constexpr(ALWAYS_EQ){
					this->move_back(other);
				}
				else{
					if(this->alloc==other.alloc) this->move_back(other);
					else this->add_back(static_cast<List&&>(other));
				}
				return *this;
			}
			constexpr List operator*(size_type num)const{
				List new_list(AlNode_traits::select_on_container_copy_construction(alloc));
				for(size_type i=0;i<num;i++)
					new_list+=*this;
				return new_list;
			}
			constexpr List& operator*=(size_type num){
				if(length==0 || num==1)
					return *this;
				else if(num==0){
					clear();
					return *this;
				}
				else{
					Guard guard(alloc,begin(),end());
					for(size_type i=2;i<num;i++){
						BaseNodePtr p=HEAD;
						for(size_type j=0;j<length;j++){
							guard.tail->next=guard.create_node(to_node(p)->data);
							guard.tail->next->prev=guard.tail;
							guard.tail=guard.tail->next;
							p=p->next;
							++guard.length;
						}
					}
					length+=guard.transfer_at(NULLPTR);
					return *this;					
				}
			}
			template<typename... Args>
			constexpr void emplace_back(Args&&... args){
				link_back(create_node(static_cast<Args&&>(args)...));
				++length;
			}
			constexpr void push_back(const value_type& val){
				link_back(create_node(val));
				++length;
			}
			constexpr void push_back(value_type&& val){
				link_back(create_node(static_cast<value_type&&>(val)));
				++length;
			}
			template<typename... Args>
			constexpr iterator emplace(const_iterator pos,Args&&... args){
				BaseNodePtr p=create_node(static_cast<Args&&>(args)...);
				pos.ptr->link_left(p);
				++length;
				return iterator(p);
			}
			constexpr iterator insert(const_iterator pos,const value_type& val){
				BaseNodePtr p=create_node(val);
				pos.ptr->link_left(p);
				++length;
				return iterator(p);
			}
			constexpr iterator insert(const_iterator pos,value_type&& val){
				BaseNodePtr p=create_node(static_cast<value_type&&>(val));
				pos.ptr->link_left(p);
				++length;
				return iterator(p);
			}
            template<typename It>
            constexpr enable_if_t<is_iterator_v<It>,iterator> insert(const_iterator pos,It first,It last){
                Guard guard(alloc,first,last);
				if(guard.head==nullptr) return iterator(pos.ptr);
				else{
					BaseNodePtr p=guard.head;
					this->length+=guard.transfer_at(pos.ptr);
					return iterator(p);
				}
            }
            constexpr iterator insert(const_iterator pos,const size_type count,const T& val){
                Guard guard(alloc,count,val);
				if(guard.head==nullptr) return iterator(pos.ptr);
				else{
					BaseNodePtr p=guard.head;
					this->length+=guard.transfer_at(pos.ptr);
					return iterator(p);
				}
            }
			template<typename... Args>
			constexpr iterator emplace_at(size_type index,Args&&... args){
				return emplace(begin()+index,static_cast<Args&&>(args)...);
			}
			constexpr iterator insert_at(size_type index,const value_type& val){
				return insert(begin()+index,val);
			}
			constexpr iterator insert_at(size_type index,value_type&& val){
				return insert(begin()+index,static_cast<value_type&&>(val));
			}
            template<typename It>
            constexpr enable_if_t<is_iterator_v<It>,iterator> insert_at(size_type index,It first,It last){
                return insert(begin()+index,first,last);
            }
            constexpr iterator insert_at(size_type index,const size_type count,const T& val){
                return insert(begin()+index,count,val);
            }
			constexpr value_type pop(const_iterator pos)noexcept(is_nothrow_move_constructible_v<value_type>){
				NodePtr p(to_node(pos.ptr));
				value_type aux=static_cast<value_type&&>(p->data);
				p->unlink();
				delete_node(p);
				--length;
				return aux;
			}
			constexpr void del(const_iterator pos)noexcept{
				--length;
				pos.ptr->unlink();
				delete_node(to_node(pos.ptr));
			}
			constexpr value_type pop_at(size_type index)noexcept(is_nothrow_move_constructible_v<value_type>){
				return pop(begin()+index);
			}
			constexpr void del_at(size_type index)noexcept{
				--length;
				delete_node(unlink_at(index));
			}
			constexpr value_type pop_back()noexcept(is_nothrow_move_constructible_v<value_type>){
				return pop(const_iterator(TAIL));
			}
			void del_back()noexcept{
				--length;
				delete_node(unlink_back());
			}
			constexpr List concat(const_iterator pos,const List& other)const&{
				List new_list(this->begin(),pos,AlNode_traits::select_on_container_copy_construction(alloc));
				new_list.insert(new_list.end(),other.begin(),other.end());
				new_list.insert(new_list.end(),pos,this->end());
				return new_list;
			}
			constexpr List concat(const_iterator pos,List&& other)const&{
				List new_list(static_cast<List&&>(other));
				new_list.insert(new_list.begin(),this->begin(),pos);
				new_list.insert(new_list.end(),pos,this->end());
				return new_list;
			}
			constexpr List concat(const_iterator pos,const List& other)&&{
				this->insert(pos,other.begin(),other.end());
				return List(static_cast<List&&>(*this));
			}
			constexpr List concat(const_iterator pos,List&& other)&& noexcept(ALWAYS_EQ){
				this->join(pos,static_cast<List&&>(other));
				return List(static_cast<List&&>(*this));
			}
			constexpr void join(const_iterator pos,const List& other){
				this->insert(pos,other.begin(),other.end());
			}
			constexpr void join(const_iterator pos,List&& other)noexcept(ALWAYS_EQ){
				if constexpr(!ALWAYS_EQ){
					if(this->alloc!=other.alloc){
						Guard guard(this->alloc,move_iterator(other.begin()),move_iterator(other.end()));
						length+=guard.transfer_at(pos.ptr);
						return;
					}
				}
				move_at(pos.ptr,other);
			}
			constexpr List sublist(const_iterator first,const_iterator last)const&{
				return List(first,last,AlNode_traits::select_on_container_copy_construction(alloc));
			}
			constexpr List sublist(const_iterator first,const_iterator last)&& noexcept(noexcept(AlNode_traits::select_on_container_copy_construction(this->alloc))){
				if(first==last) return List(AlNode_traits::select_on_container_copy_construction(alloc));
				wipe(last,end());
				wipe(begin(),first);
				return List(static_cast<List&&>(*this));
			}
			constexpr List& wipe(const_iterator first,const_iterator last)noexcept{
				BaseNodePtr fst=first.ptr,lst=last.ptr;
				fst->prev->next=lst;
				lst->prev=fst->prev;
				while(fst!=lst){
					--length;
					NodePtr aux=to_node(fst);
					fst=fst->next;
					delete_node(aux);
				}
				return *this;
			}
			constexpr List cut(const_iterator first,const_iterator last)& noexcept(ALWAYS_EQ && noexcept(AlNode_traits::select_on_container_copy_construction(this->alloc))){
				List new_list(AlNode_traits::select_on_container_copy_construction(alloc));
				if(first==last) return new_list;
				BaseNodePtr fst=first.ptr, lst=last.ptr;
				if constexpr(!ALWAYS_EQ){
					if(this->alloc!=new_list.alloc){
						fst->prev->next=lst;
						lst->prev=fst->prev;
						size_type cut_size=distance(first,last);
						this->length-=cut_size;
						ForwardGuard guard(alloc,fst,cut_size);
						new_list.insert(new_list.begin(),move_iterator(iterator(fst)),move_iterator(iterator(lst)));
						return new_list;
					}
				}
				new_list.length=distance(first,last);
				this->length-=new_list.length;
				new_list.HEAD=fst;
				new_list.TAIL=lst->prev;
				fst->prev->next=lst;
				lst->prev=fst->prev;
				new_list.set_null_sides();
				return new_list;
			}
			constexpr List cut(const_iterator first,const_iterator last)&& noexcept{
				return static_cast<List&&>(*this).sublist(first,last);
			}
			constexpr size_type size()const noexcept{
				return length;
			}
			constexpr bool remove(const T& val)noexcept(noexcept(fake_copy_init<bool>(to_node(this->HEAD)->data==val))){
				BaseNodePtr current=this->HEAD;
				while(current!=NULLPTR){
					if(NodePtr p=to_node(current); p->data==val){
						p->unlink();
						delete_node(p);
						this->length--;
						return true;
					}
					else
						current=current->next;
				}
				return false;
			}
			constexpr size_type remove_all(const T& val)noexcept(noexcept(fake_copy_init<bool>(to_node(this->HEAD)->data==val))){	//devuelve la cantidad removida
				Guard guard(alloc);
				guard.length=this->length;//old length
				BaseNodePtr current=this->HEAD;
				while(current!=NULLPTR){
					if(to_node(current)->data==val){
						guard.push_back(current);
						current->unlink();
						current=current->next;
						guard.tail->next=nullptr;
						this->length--;
					}
					else
						current=current->next;
				}
				return guard.length-this->length;//old length - new length
			}
			constexpr void resize(size_type new_size){
				while(this->length<new_size){
					Guard guard(alloc,new_size-this->length);
					this->length+=guard.transfer_at(NULLPTR);
				}
				while(this->length>new_size){
					delete_node(unlink_back());
					--length;
				}
			}
            constexpr const_iterator find(const T& val)const noexcept(noexcept(mpv::find(this->begin(),this->end(),val))){
                return mpv::find(this->begin(),this->end(),val);
            }
            constexpr iterator find(const T& val)noexcept(noexcept(mpv::find(this->begin(),this->end(),val))){
                return mpv::find(this->begin(),this->end(),val);
            }
            constexpr size_type count(const T& val)const noexcept(noexcept(mpv::count<const_iterator,T,size_type>(this->begin(),this->end(),val))){
                return mpv::count<const_iterator,T,size_type>(this->begin(),this->end(),val);
            }
			constexpr bool contains(const T& val)const noexcept(noexcept(mpv::contains<const_iterator,T>(this->begin(),this->end(),val))){
				return mpv::contains<const_iterator,T>(this->begin(),this->end(),val);
			}
            constexpr size_type index_of(const T& val)const noexcept(noexcept(mpv::index_of<const_iterator,T,size_type>(this->begin(),this->end(),val))){
                return mpv::index_of<const_iterator,T,size_type>(this->begin(),this->end(),val);
            }
			template<typename Pred=less<>>
			constexpr void sort(Pred pred=Pred{}){
				mpv::insertion_sort(this->begin(),this->end(),pred);
			}
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const noexcept(noexcept(fake_copy_init<bool>(func(*this->begin())))){
                for(const_reference x:*this)
                    if(func(x))return 1;
                return 0;
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
			template<typename Lambda>
			constexpr auto map(Lambda&& func=Lambda{})const{
				using U=mpv::remove_cvref_t<decltype(func(declval<const_reference>()))>;
				List<U,rebind_alloc<Alloc,U>> new_list(AlNode_traits::select_on_container_copy_construction(alloc));
				for(const_reference x:*this)
					new_list.push_back(func(x));
				return new_list;
			}
			template<typename Lambda>
			constexpr List filter(Lambda&& func=Lambda{})const{
				List new_list(AlNode_traits::select_on_container_copy_construction(alloc));
				for(const_reference x:*this)
					if(func(x)) new_list.push_back(x);
				return new_list;
			}
			constexpr List& reverse()noexcept(noexcept(mpv::reverse(this->begin(),this->end()))){
				mpv::reverse(this->begin(),this->end());
				return *this;
			}
			constexpr bool empty()const noexcept{
				return length==0;
			}
			constexpr const_reference operator[](size_type index)const noexcept{
				BaseNodePtr current=this->HEAD;
				for(size_type i=0;i<index;i++)
					current=current->next;
				return to_node(current)->data;
			}
			constexpr reference operator[](size_type index)noexcept{
				BaseNodePtr current=this->HEAD;
				for(size_type i=0;i<index;i++)
					current=current->next;
				return to_node(current)->data;
			}
			constexpr const_reference back()const noexcept{
				return to_node(TAIL)->data;
			}
			constexpr reference back()noexcept{
				return to_node(TAIL)->data;
			}
			constexpr bool operator==(const List& other)const noexcept(noexcept(equal(this->begin(),this->end(),other.begin()))){
				if(this->length!=other.length) return false;
				else return equal(this->begin(),this->end(),other.begin());
			}
			constexpr bool operator!=(const List& other)const noexcept(noexcept(equal(this->begin(),this->end(),other.begin()))){
				if(this->length!=other.length) return true;
				else return !equal(this->begin(),this->end(),other.begin());
			}
            constexpr bool operator<(const List& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool operator>(const List& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator<=(const List& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator>=(const List& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
			constexpr void clear()noexcept{
				length=0;
				TAIL=NULLPTR;
				while(HEAD!=NULLPTR){
					NodePtr aux=to_node(HEAD);
					HEAD=HEAD->next;
					delete_node(aux);
				}
			}
            constexpr allocator_type get_allocator()const noexcept{
                return this->alloc;
            }
			~List()noexcept{
				while(HEAD!=NULLPTR){
					NodePtr aux=to_node(HEAD);
					HEAD=HEAD->next;
					delete_node(aux);
				}
			}
			constexpr iterator begin()noexcept{
				return iterator(HEAD);
			}
			constexpr iterator end()noexcept{
				return iterator(NULLPTR);
			}
			constexpr const_iterator begin()const noexcept{
				return const_iterator(HEAD);
			}
			constexpr const_iterator end()const noexcept{
				return const_iterator(NULLPTR);
			}
	};
	template<typename Out,typename T,typename Alloc>
	Out& operator<<(Out& stream,const List<T,Alloc>& list){
		using reference=typename List<T,Alloc>::const_iterator::reference;
		typename List<T,Alloc>::size_type remaining=list.size();
		stream<<"[";
		for(reference x:list){
			stream<<""<<x;
			if(--remaining)
				stream<<", ";
		}
		stream<<"]";
		return stream;
	}
#undef HEAD
#undef TAIL
#undef NULLPTR
#undef alloc
}