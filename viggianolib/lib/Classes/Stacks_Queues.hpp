#pragma once
namespace mpv{
	template<typename T,typename VoidPtr>
	struct Node COUNT_IT{
		using NodePtr=rebind_pointer<VoidPtr,Node>;
		using const_NodePtr=rebind_pointer<VoidPtr,const Node>;
		NodePtr next{};
		T data;
		template<typename... Args>
		Node(Args&&... args):data(static_cast<Args&&>(args)...){}
		Node(const Node&)=delete;
		Node(Node&&)=delete;

	};
	template<typename Types>
	class Node_iterator{
		public:
			using NodePtr=typename Types::NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=typename Types::reference;
		private:
			NodePtr ptr;
		public:
			Node_iterator(NodePtr ptr):ptr(ptr){}
			reference operator*(){
				return ptr->data;
			}
			pointer operator->(){
				return &(ptr->data);
			}
			Node_iterator& operator++(){
				ptr=ptr->next;
				return *this;
			}
			Node_iterator operator++(int){
				Node_iterator aux=*this;
				ptr=ptr->next;
				return *this;
			}
			bool operator==(const Node_iterator& other)const{
				return this->ptr==other.ptr;
			}
			bool operator!=(const Node_iterator& other)const{
				return this->ptr!=other.ptr;
			}
			template<typename> friend class const_Node_iterator;
	};
	template<typename Types>
	class const_Node_iterator{
		public:
			using NodePtr=typename Types::const_NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
		private:
			NodePtr ptr;
		public:
			const_Node_iterator(NodePtr ptr):ptr(ptr){}
			const_Node_iterator(Node_iterator<Types> nonconst_it):ptr(nonconst_it.ptr){}
			reference operator*(){
				return ptr->data;
			}
			pointer operator->(){
				return &(ptr->data);
			}
			const_Node_iterator& operator++(){
				ptr=ptr->next;
				return *this;
			}
			const_Node_iterator operator++(int){
				const_Node_iterator aux=*this;
				ptr=ptr->next;
				return aux;
			}
			bool operator==(const const_Node_iterator& other)const{
				return this->ptr==other.ptr;
			}
			bool operator!=(const const_Node_iterator& other)const{
				return this->ptr!=other.ptr;
			}
	};
	template<typename T,typename Alloc=allocator<T>>
	class Queue:EBCO<rebind_alloc<Alloc,Node<T,typename allocator_traits<Alloc>::void_pointer>>> COUNT_IT_{
		private:
			using Node=Node<T,typename allocator_traits<Alloc>::void_pointer>;
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
			NodePtr head{};
			NodePtr tail{};
			inline void delete_node(NodePtr node){
				AlNode_traits::destroy(this->get_val(),node);
				AlNode_traits::deallocate(this->get_val(),node,1);
			}
			template<typename... Args>
			inline NodePtr create_node(Args&&... args){
				NodePtr p=AlNode_traits::allocate(this->get_val(),1);
				AlNode_traits::construct(this->get_val(),p,static_cast<Args&&>(args)...);
				return p;
			}
			inline void reset(){
				head=nullptr;
				tail=nullptr;
			}
			template<typename... Args>
			inline void create_first_node(Args&&... args){
				this->head=this->create_node(static_cast<Args&&>(args)...);
				this->tail=this->head;
			}
			template<typename... Args>
			inline void link_back(Args&&... args){
				this->tail->next=this->create_node(static_cast<Args&&>(args)...);
				this->tail=this->tail->next;
			}
			inline void copy_elements(const Queue& other){
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
			inline void copy_elements(Queue&& other){
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
			inline void move_elements(Queue& other){
				this->head=other.head;
				this->tail=other.tail;
				other.reset();
			}
		public:
			Queue(){INCQUEUES}
			explicit Queue(const Alloc& al):EBCO<AlNode>(al){INCQUEUES}
			Queue(const Queue& other):EBCO<AlNode>(AlNode_traits::select_on_container_copy_construction(other.get_val())){INCQUEUES
				this->copy_elements(other);
			}
			Queue(const Queue& other,const Alloc& al):EBCO<AlNode>(al){INCQUEUES
				this->copy_elements(other);
			}
			Queue(Queue&& other):EBCO<AlNode>(static_cast<AlNode&&>(other.get_val())),head(other.head),tail(other.tail){INCQUEUES
				other.reset();
			}
			Queue(Queue&& other,const Alloc& al):EBCO<AlNode>(al){INCQUEUES
				if(this->get_val()==other.get_val()){
					head=other.head;
					tail=other.tail;
					other.reset();
				}
				else{
					if(other.head!=nullptr){
						other.tail=nullptr;
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
					}
				}
			}
			Queue& operator=(const Queue& other){
				this->clear();
				if constexpr(POCCA and not ALWAYS_EQ)
					this->get_val()=other.get_val();
				this->copy_elements(other);
				return *this;
			}
			Queue& operator=(Queue&& other){
				this->clear();
				if constexpr(not ALWAYS_EQ and not POCMA){
					if(this->get_val()!=other.get_val()){
						this->copy_elements(static_cast<Queue&&>(other));
						return *this;
					}
				}
				if constexpr(POCMA) this->get_val()=other.get_val();
				this->move_elements(other);
				return *this;
			}
			template<typename... Args>
			void emplace(Args&&... args){
				NodePtr new_node=AlNode_traits::allocate(this->get_val(),1);
				AlNode_traits::construct(this->get_val(),new_node,static_cast<Args&&>(args)...);
				if(this->head==nullptr)
					this->head=new_node;	
				else
					this->tail->next=new_node;
				this->tail=new_node;
			}
			void push(const_reference dat){
				NodePtr new_node=AlNode_traits::allocate(this->get_val(),1);
				AlNode_traits::construct(this->get_val(),new_node,dat);
				if(this->head==nullptr)
					this->head=new_node;	
				else
					this->tail->next=new_node;
				this->tail=new_node;	
			}
			void push(value_type&& dat){
				NodePtr new_node=AlNode_traits::allocate(this->get_val(),1);
				AlNode_traits::construct(this->get_val(),new_node,static_cast<value_type&&>(dat));
				if(this->head==nullptr)
					this->head=new_node;	
				else
					this->tail->next=new_node;
				this->tail=new_node;	
			}
			const_reference front()const{
				return this->head->data;
			}
			const_reference back()const{
				return this->tail->data;
			}
			reference front(){
				return this->head->data;
			}
			reference back(){
				return this->tail->data;
			}
			bool empty()const{
				return this->head==nullptr;
			}
			value_type pop(){
				NodePtr aux=this->head;
				value_type aux_data=static_cast<value_type&&>(this->head->data);
				this->head=this->head->next;
				if(this->head==nullptr) this->tail=nullptr;
				delete_node(aux);
				return aux_data;
			}
			void del(){
				NodePtr aux=this->head;
				this->head=this->head->next;
				if(this->head==nullptr) this->tail=nullptr;
				delete_node(aux);
			}
			void clear(){
				while(head!=nullptr){
					NodePtr aux=this->head;
					this->head=this->head->next;
					delete_node(aux);
				}
				tail=nullptr;
			}
			~Queue(){
				while(head!=nullptr){
					NodePtr aux=this->head;
					this->head=this->head->next;
					delete_node(aux);
				}
				DECQUEUES
			}
			iterator begin(){
				return iterator(head);
			}
			iterator last(){
				return iterator(tail);
			}
			iterator end(){
				return iterator(nullptr);
			}
			const_iterator begin()const{
				return const_iterator(head);
			}
			const_iterator last()const{
				return const_iterator(tail);
			}
			const_iterator end()const{
				return const_iterator(nullptr);
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
	class Stack:EBCO<rebind_alloc<Alloc,Node<T,typename allocator_traits<Alloc>::void_pointer>>> COUNT_IT_{
		private:
			using Node=Node<T,typename allocator_traits<Alloc>::void_pointer>;
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
			NodePtr head{};
			inline void delete_node(NodePtr node){
				AlNode_traits::destroy(this->get_val(),node);
				AlNode_traits::deallocate(this->get_val(),node,1);
			}
			template<typename... Args>
			inline NodePtr create_node(Args&&... args){
				NodePtr new_node=AlNode_traits::allocate(this->get_val(),1);
				AlNode_traits::construct(this->get_val(),new_node,static_cast<Args&&>(args)...);
				return new_node;
			}
			inline void copy_elements(const Stack& other){
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
			inline void copy_elements(Stack&& other){
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
			Stack(){INCSTACKS}
			explicit Stack(const Alloc& al):EBCO<AlNode>(al){INCSTACKS}
			Stack(const Stack& other):EBCO<AlNode>(AlNode_traits::select_on_container_copy_construction(other.get_val())){INCSTACKS
				this->copy_elements(other);
			}
			Stack(const Stack& other,const Alloc& al):EBCO<AlNode>(al){INCSTACKS
				this->copy_elements(other);
			}
			Stack(Stack&& other):EBCO<AlNode>(static_cast<AlNode&&>(other.get_val())),head(other.head){INCSTACKS
				other.head=nullptr;
			}
			Stack(Stack&& other,const Alloc& al):EBCO<AlNode>(al){INCSTACKS
				if(this->get_val()==other.get_val()){
					this->head=other.head;
					other.head=nullptr;
				}
				else
					this->copy_elements(static_cast<Stack&&>(other));
			}
			Stack& operator=(const Stack& other){
				this->clear();
				if constexpr(POCCA and not ALWAYS_EQ)
					this->get_val()=other.get_val();
				this->copy_elements(other);
				return *this;
			}
			Stack& operator=(Stack&& other){
				this->clear();
				if constexpr(not ALWAYS_EQ and not POCMA){
					if(this->get_val()!=other.get_val()){
						this->copy_elements(static_cast<Stack&&>(other));
						return *this;
					}
				}
				if constexpr(POCMA) this->get_val()=other.get_val();
				this->head=other.head;
				other.head=nullptr;
				return *this;
			}
			template<typename... Args>
			void emplace(Args&&... args){
				NodePtr new_node=create_node(static_cast<Args&&>(args)...);
				new_node->next=this->head;
				this->head=new_node;
			}
			void push(const_reference dat){
				NodePtr new_node=create_node(dat);
				new_node->next=this->head;
				this->head=new_node;
			}
			void push(value_type&& dat){
				NodePtr new_node=create_node(static_cast<value_type&&>(dat));
				new_node->next=this->head;
				this->head=new_node;
			}
			const_reference top()const{
				return this->head->data;
			}
			reference top(){
				return this->head->data;
			}
			bool empty()const{
				return this->head==nullptr;
			}
			value_type pop(){
				NodePtr aux=this->head;
				value_type aux_data=static_cast<value_type&&>(head->data);
				this->head=this->head->next;
				delete_node(aux);
				return aux_data;
			}
			void del(){
				NodePtr aux=this->head;
				this->head=this->head->next;
				delete_node(aux);
			}
			inline void clear(){
				while(head!=nullptr){
					NodePtr aux=head;
					head=head->next;
					delete_node(aux);
				}
			}
			~Stack(){
				clear();
				DECSTACKS
			}
			iterator begin(){
				return iterator(head);
			}
			iterator last(){
				NodePtr p=head;
				if(head!=nullptr){
					while(p->next!=nullptr){
						p=p->next;
					}
					return iterator(p);
				}
				else return iterator(nullptr);
			}
			iterator end(){
				return iterator(nullptr);
			}
			const_iterator begin()const{
				return const_iterator(head);
			}
			const_iterator last()const{
				NodePtr p=head;
				if(head!=nullptr){
					while(p->next!=nullptr){
						p=p->next;
					}
					return const_iterator(p);
				}
				else return const_iterator(nullptr);
			}
			const_iterator end()const{
				return const_iterator(nullptr);
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
}