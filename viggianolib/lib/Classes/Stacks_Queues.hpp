#pragma once
namespace mpv{
	template<typename T,typename VoidPtr>
	struct Basic_Node COUNT_IT{
		using NodePtr=rebind_pointer<VoidPtr,Basic_Node>;
		using const_NodePtr=rebind_pointer<VoidPtr,const Basic_Node>;
		NodePtr next=nullptr;
		T data;
		template<typename... Args>
		constexpr Basic_Node(Args&&... args)noexcept(is_nothrow_constructible_v<T,Args...>):data(static_cast<Args&&>(args)...){}
		Basic_Node(const Basic_Node&)=delete;
		Basic_Node& operator=(const Basic_Node&)=delete;
	};
	template<typename Types>
	class Node_iterator{
		public:
			using iterator_category=forward_iterator_tag;
			using NodePtr=typename Types::NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=typename Types::reference;
		private:
			NodePtr ptr;
		public:
			constexpr Node_iterator(NodePtr ptr)noexcept:ptr(ptr){}
			constexpr reference operator*()const noexcept{
				return ptr->data;
			}
			constexpr pointer operator->()const noexcept{
				return pointer_traits<pointer>::pointer_to(ptr->data);
			}
			constexpr Node_iterator& operator++()noexcept{
				ptr=ptr->next;
				return *this;
			}
			constexpr Node_iterator operator++(int)noexcept{
				Node_iterator aux=*this;
				ptr=ptr->next;
				return *this;
			}
			constexpr bool operator==(const Node_iterator& other)const noexcept{
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const Node_iterator& other)const noexcept{
				return this->ptr!=other.ptr;
			}
			template<typename> friend class const_Node_iterator;
	};
	template<typename Types>
	class const_Node_iterator{
		public:
			using iterator_category=forward_iterator_tag;
			using NodePtr=typename Types::const_NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
		private:
			NodePtr ptr;
		public:
			constexpr const_Node_iterator(NodePtr ptr)noexcept:ptr(ptr){}
			constexpr const_Node_iterator(Node_iterator<Types> nonconst_it)noexcept:ptr(nonconst_it.ptr){}
			constexpr reference operator*()const noexcept{
				return ptr->data;
			}
			constexpr pointer operator->()const noexcept{
				return pointer_traits<pointer>::pointer_to(ptr->data);
			}
			constexpr const_Node_iterator& operator++()noexcept{
				ptr=ptr->next;
				return *this;
			}
			constexpr const_Node_iterator operator++(int)noexcept{
				const_Node_iterator aux=*this;
				ptr=ptr->next;
				return aux;
			}
			constexpr bool operator==(const const_Node_iterator& other)const noexcept{
				return this->ptr==other.ptr;
			}
			constexpr bool operator!=(const const_Node_iterator& other)const noexcept{
				return this->ptr!=other.ptr;
			}
	};
#define head cp.V2
#define alloc cp.getV1()
	template<typename T,typename Alloc=allocator<T>>
	class Queue COUNT_IT{
		private:
			using Node=Basic_Node<T,typename allocator_traits<Alloc>::void_pointer>;
			using NodePtr=typename Node::NodePtr;
			using const_NodePtr=typename Node::const_NodePtr;
			using AlTy=rebind_alloc<Alloc,T>;
			using AlTy_traits=allocator_traits<AlTy>;
			using AlNode=rebind_alloc<Alloc,Node>;
			using AlNode_traits=allocator_traits<AlNode>;
			struct Val_types{
				using value_type=T;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=T&;
				using const_reference=const T&;
				using NodePtr=typename Node::NodePtr;
				using const_NodePtr=typename Node::const_NodePtr;
			};
            static constexpr bool POCCA=AlTy_traits::propagate_on_container_copy_assignment::value;
            static constexpr bool POCMA=AlTy_traits::propagate_on_container_move_assignment::value;
            static constexpr bool POCS=AlTy_traits::propagate_on_container_swap::value;
            static constexpr bool ALWAYS_EQ=AlTy_traits::is_always_equal::value;
		public:
			using allocator_type=Alloc;
			using value_type=typename Val_types::value_type;
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
			using iterator=Node_iterator<Val_types>;
			using const_iterator=const_Node_iterator<Val_types>;
		private:
			CompressedPair<AlNode,NodePtr> cp=CompressedPair<AlNode,NodePtr>(nullptr);
			NodePtr tail=nullptr;
			constexpr void delete_node(NodePtr node)noexcept{
				DESTROY(this->alloc,node);
				AlNode_traits::deallocate(this->alloc,node,1);
			}
			template<typename... Args>
			constexpr NodePtr create_node(Args&&... args){
                AllocConstructPtr guard(alloc);
                guard.allocate();
				CONSTRUCT_VARARGS(this->alloc,guard.ptr,static_cast<Args&&>(args));
				return guard.release();
			}
			constexpr void reset()noexcept{
				head=nullptr;
				tail=nullptr;
			}
			template<typename... Args>
			constexpr void create_first_node(Args&&... args){
				this->head=this->create_node(static_cast<Args&&>(args)...);
				this->tail=this->head;
			}
			template<typename... Args>
			constexpr void link_back(Args&&... args){
				this->tail->next=this->create_node(static_cast<Args&&>(args)...);
				this->tail=this->tail->next;
			}
			constexpr void copy_elements(const Queue& other){
				NodePtr p=other.head;
				if(p!=nullptr){
					this->create_first_node(p->data);
					p=p->next;
					while(p!=nullptr){
						this->link_back(p->data);
						p=p->next;
					}
				}
			}
			constexpr void copy_elements(Queue&& other){
				if(other.head!=nullptr){
					this->create_first_node(static_cast<value_type&&>(other.head->data));
					NodePtr aux=other.head;
					other.head=other.head->next;
					other.delete_node(aux);
					while(other.head!=nullptr){
						this->link_back(other.head->data);
						aux=other.head;
						other.head=other.head->next;
						other.delete_node(aux);
					}
				}
				other.reset();
			}
			constexpr void move_elements(Queue& other){
				this->head=other.head;
				this->tail=other.tail;
				other.reset();
			}
		public:
			constexpr Queue()noexcept=default;
			constexpr explicit Queue(const Alloc& al)noexcept:cp(arg1_tag{},al){}
			constexpr Queue(const Queue& other):cp(arg1_tag{},AlNode_traits::select_on_container_copy_construction(other.alloc)){
				this->copy_elements(other);
			}
			constexpr Queue(const Queue& other,const Alloc& al):cp(arg1_tag{},al){
				this->copy_elements(other);
			}
			constexpr Queue(Queue&& other)noexcept:cp(arg1_tag{},static_cast<AlNode&&>(other.alloc),other.head),tail(other.tail){
				other.reset();
			}
			constexpr Queue(Queue&& other,const Alloc& al)noexcept(ALWAYS_EQ):cp(arg1_tag{},al){
				if(this->alloc==other.alloc){
					head=other.head;
					tail=other.tail;
					other.reset();
				}
				else{
					if(other.head!=nullptr){
						this->create_first_node(static_cast<value_type&&>(other.head->data));
						NodePtr aux=other.head;
						other.head=other.head->next;
						other.delete_node(aux);
						while(other.head!=nullptr){
							this->link_back(static_cast<value_type&&>(other.head->data));
							aux=other.head;
							other.head=other.head->next;
							other.delete_node(aux);
						}
						other.tail=nullptr;
					}
				}
			}
			constexpr Queue& operator=(const Queue& other){
				if(this==&other) return *this;
				this->clear();
				pocca(this->alloc,other.alloc);
				this->copy_elements(other);
				return *this;
			}
			constexpr Queue& operator=(Queue&& other)noexcept(ALWAYS_EQ || POCMA){
				if(this==&other) return *this;
				this->clear();
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->alloc!=other.alloc){
						this->copy_elements(static_cast<Queue&&>(other));
						return *this;
					}
				}
				pocma(this->alloc,other.alloc);
				this->move_elements(other);
				return *this;
			}
			template<typename... Args>
			constexpr void emplace(Args&&... args){
				NodePtr new_node=create_node(static_cast<Args&&>(args)...);
				if(this->head==nullptr)
					this->head=new_node;	
				else
					this->tail->next=new_node;
				this->tail=new_node;
			}
			constexpr void push(const_reference val){
				NodePtr new_node=create_node(val);
				if(this->head==nullptr)
					this->head=new_node;	
				else
					this->tail->next=new_node;
				this->tail=new_node;	
			}
			constexpr void push(value_type&& val){
				NodePtr new_node=create_node(static_cast<value_type&&>(val));
				if(this->head==nullptr)
					this->head=new_node;	
				else
					this->tail->next=new_node;
				this->tail=new_node;	
			}
			constexpr const_reference front()const noexcept{
				return this->head->data;
			}
			constexpr const_reference back()const noexcept{
				return this->tail->data;
			}
			constexpr reference front()noexcept{
				return this->head->data;
			}
			constexpr reference back()noexcept{
				return this->tail->data;
			}
			constexpr bool empty()const noexcept{
				return this->head==nullptr;
			}
			constexpr value_type pop()noexcept(is_nothrow_move_constructible_v<value_type>){
				NodePtr aux=this->head;
				value_type aux_data=static_cast<value_type&&>(this->head->data);
				this->head=this->head->next;
				if(this->head==nullptr) this->tail=nullptr;
				delete_node(aux);
				return aux_data;
			}
			constexpr void del()noexcept{
				NodePtr aux=this->head;
				this->head=this->head->next;
				if(this->head==nullptr) this->tail=nullptr;
				delete_node(aux);
			}
			constexpr void clear()noexcept{
				while(head!=nullptr){
					NodePtr aux=this->head;
					this->head=this->head->next;
					delete_node(aux);
				}
				tail=nullptr;
			}
			~Queue()noexcept{
				while(head!=nullptr){
					NodePtr aux=this->head;
					this->head=this->head->next;
					delete_node(aux);
				}
			}
			constexpr iterator begin()noexcept{
				return NodePtr(head);
			}
			constexpr iterator last()noexcept{
				return NodePtr(tail);
			}
			constexpr iterator end()noexcept{
				return NodePtr(nullptr);
			}
			constexpr const_iterator begin()const noexcept{
				return const_NodePtr(head);
			}
			constexpr const_iterator last()const noexcept{
				return const_NodePtr(tail);
			}
			constexpr const_iterator end()const noexcept{
				return const_NodePtr(nullptr);
			}
	};
	template<typename Out,typename T,typename Alloc>
	Out& operator<<(Out& stream,const Queue<T,Alloc>& queue){
		stream<<'[';
		for(typename Queue<T,Alloc>::const_iterator i=queue.begin();i!=queue.end();){
			stream<<*i;
			if(++i!=queue.end())
				stream<<", ";		
		}
		return stream<<']';
	}
	template<typename T,typename Alloc=allocator<T>>
	class Stack COUNT_IT{
		private:
			using Node=Basic_Node<T,typename allocator_traits<Alloc>::void_pointer>;
			using NodePtr=typename Node::NodePtr;
			using const_NodePtr=typename Node::const_NodePtr;
			using AlTy=rebind_alloc<Alloc,T>;
			using AlTy_traits=allocator_traits<AlTy>;
			using AlNode=rebind_alloc<Alloc,Node>;
			using AlNode_traits=allocator_traits<AlNode>;
			struct Val_types{
				using value_type=T;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=T&;
				using const_reference=const T&;
				using NodePtr=typename Node::NodePtr;
				using const_NodePtr=typename Node::const_NodePtr;
			};
            static constexpr bool POCCA=AlTy_traits::propagate_on_container_copy_assignment::value;
            static constexpr bool POCMA=AlTy_traits::propagate_on_container_move_assignment::value;
            static constexpr bool POCS=AlTy_traits::propagate_on_container_swap::value;
            static constexpr bool ALWAYS_EQ=AlTy_traits::is_always_equal::value;
		public:
			using allocator_type=Alloc;
			using value_type=typename Val_types::value_type;
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
			using iterator=Node_iterator<Val_types>;
			using const_iterator=const_Node_iterator<Val_types>;
		private:
			CompressedPair<AlNode,NodePtr> cp=CompressedPair<AlNode,NodePtr>(nullptr);
			constexpr void delete_node(NodePtr node)noexcept{
				DESTROY(this->alloc,node);
				AlNode_traits::deallocate(this->alloc,node,1);
			}
			template<typename... Args>
			constexpr NodePtr create_node(Args&&... args){
                AllocConstructPtr guard(alloc);
                guard.allocate();
				CONSTRUCT_VARARGS(this->alloc,guard.ptr,static_cast<Args&&>(args));
				return guard.release();
			}
			constexpr void copy_elements(const Stack& other){
				NodePtr p=other.head;
				if(p!=nullptr){
					this->head=this->create_node(p->data);
					p=p->next;
					for(NodePtr last=this->head;p!=nullptr;last=last->next){
						last->next=this->create_node(p->data);
						p=p->next;
					}
				}
			}
			constexpr void copy_elements(Stack&& other){
				if(other.head!=nullptr){
					this->head=this->create_node(static_cast<value_type&&>(other.head->data));
					NodePtr aux=other.head;
					other.head=other.head->next;
					other.delete_node(aux);
					for(NodePtr last=this->head;other.head!=nullptr;last=last->next){
						last->next=this->create_node(other.head->data);
						aux=other.head;
						other.head=other.head->next;
						other.delete_node(aux);
					}
				}				
			}
		public:
			constexpr Stack()noexcept=default;
			constexpr explicit Stack(const Alloc& al)noexcept:cp(arg1_tag{},al){}
			constexpr Stack(const Stack& other):cp(arg1_tag{},AlNode_traits::select_on_container_copy_construction(other.alloc)){
				this->copy_elements(other);
			}
			constexpr Stack(const Stack& other,const Alloc& al):cp(arg1_tag{},al){
				this->copy_elements(other);
			}
			constexpr Stack(Stack&& other)noexcept:cp(arg1_tag{},static_cast<AlNode&&>(other.alloc),other.head){
				other.head=nullptr;
			}
			constexpr Stack(Stack&& other,const Alloc& al)noexcept(ALWAYS_EQ):cp(arg1_tag{},al){
				if(this->alloc==other.alloc){
					this->head=other.head;
					other.head=nullptr;
				}
				else
					this->copy_elements(static_cast<Stack&&>(other));
			}
			constexpr Stack& operator=(const Stack& other){
				if(this==&other) return *this;
				this->clear();
				pocca(this->alloc,other.alloc);
				this->copy_elements(other);
				return *this;
			}
			constexpr Stack& operator=(Stack&& other)noexcept(ALWAYS_EQ || POCMA){
				if(this==&other) return *this;
				this->clear();
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->alloc!=other.alloc){
						this->copy_elements(static_cast<Stack&&>(other));
						return *this;
					}
				}
				pocma(this->alloc,other.alloc);
				this->head=other.head;
				other.head=nullptr;
				return *this;
			}
			template<typename... Args>
			constexpr void emplace(Args&&... args){
				NodePtr new_node=create_node(static_cast<Args&&>(args)...);
				new_node->next=this->head;
				this->head=new_node;
			}
			constexpr void push(const_reference val){
				NodePtr new_node=create_node(val);
				new_node->next=this->head;
				this->head=new_node;
			}
			constexpr void push(value_type&& val){
				NodePtr new_node=create_node(static_cast<value_type&&>(val));
				new_node->next=this->head;
				this->head=new_node;
			}
			constexpr const_reference top()const noexcept{
				return this->head->data;
			}
			constexpr reference top()noexcept{
				return this->head->data;
			}
			constexpr bool empty()const noexcept{
				return this->head==nullptr;
			}
			constexpr value_type pop()noexcept(is_nothrow_move_constructible_v<value_type>){
				NodePtr aux=this->head;
				value_type aux_data=static_cast<value_type&&>(head->data);
				this->head=this->head->next;
				delete_node(aux);
				return aux_data;
			}
			constexpr void del()noexcept{
				NodePtr aux=this->head;
				this->head=this->head->next;
				delete_node(aux);
			}
			constexpr void clear()noexcept{
				while(head!=nullptr){
					NodePtr aux=head;
					head=head->next;
					delete_node(aux);
				}
			}
			~Stack()noexcept{
				clear();
			}
			constexpr iterator begin()noexcept{
				return NodePtr(head);
			}
			constexpr iterator last()noexcept{
				NodePtr p=head;
				if(head!=nullptr){
					while(p->next!=nullptr){
						p=p->next;
					}
					return p;
				}
				else return NodePtr(nullptr);
			}
			constexpr iterator end()noexcept{
				return NodePtr(nullptr);
			}
			constexpr const_iterator begin()const noexcept{
				return const_NodePtr(head);
			}
			constexpr const_iterator last()const noexcept{
				const_NodePtr p=head;
				if(head!=nullptr){
					while(p->next!=nullptr){
						p=p->next;
					}
					return const_iterator(p);
				}
				else return const_NodePtr(nullptr);
			}
			constexpr const_iterator end()const noexcept{
				return const_NodePtr(nullptr);
			}
	};
	template<typename Out,typename T,typename Alloc>
	Out& operator<<(Out& stream,const Stack<T,Alloc>& queue){
		stream<<'[';
		for(typename Stack<T,Alloc>::const_iterator i=queue.begin();i!=queue.end();){
			stream<<*i;
			if(++i!=queue.end())
				stream<<", ";		
		}
		return stream<<']';
	}
#undef alloc
#undef head
}