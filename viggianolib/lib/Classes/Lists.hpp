#pragma once
#define APPEND(DATA)  	NodePtr new_node=AlNode_traits::allocate(this->get_val(),1);\
						AlNode_traits::construct(this->get_val(),new_node,DATA);\
						if(this->head==nullptr){\
							this->head=new_node;\
							this->tail=new_node;\
						}else{\
							this->tail->next=new_node;\
							new_node->prev=this->tail;\
							this->tail=new_node;\
						}

#define POP_BACK		NodePtr last=tail;\
						if(tail==head){\
							head=nullptr;\
							tail=nullptr;\
						}\
						else{\
							tail=tail->prev;\
							tail->next=nullptr;\
						}\
						AlNode_traits::destroy(this->get_val(),last);\
						AlNode_traits::deallocate(this->get_val(),last,1);

namespace mpv{
	template<typename T,typename VoidPtr>
	struct List_Node COUNT_IT{
		using NodePtr=rebind_pointer<VoidPtr,List_Node>;
		using const_NodePtr=rebind_pointer<VoidPtr,const List_Node>;
		NodePtr next=nullptr;
		NodePtr prev=nullptr;
		T data;
		template<typename... Args>
		List_Node(Args&&... args):data(static_cast<Args&&>(args)...){INCLNODES}
		List_Node(const List_Node&)=delete;
		List_Node& operator=(const List_Node&)=delete;
		~List_Node(){
			DECLNODES
			//REMAININGLNODES
		}
	};
	template<typename Types>
	class List_iterator COUNT_IT{
		public:
			using NodePtr=typename Types::NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=value_type&;
		private:
			NodePtr ptr;
		public:
			List_iterator(NodePtr ptr):ptr(ptr){}
			reference operator*(){
				return ptr->data;
			}
			pointer operator->(){
				return &(ptr->data);
			}
			List_iterator& operator++(){
				ptr=ptr->next;
				return *this;
			}
			List_iterator operator++(int){
				List_iterator aux=*this;
				ptr=ptr->next;
				return aux;
			}
			List_iterator& operator--(){
				ptr=ptr->prev;
				return *this;
			}
			List_iterator operator--(int){
				List_iterator aux=*this;
				ptr=ptr->prev;
				return aux;
			}
			bool operator==(const List_iterator& other)const{
				return this->ptr==other.ptr;
			}
			bool operator!=(const List_iterator& other)const{
				return this->ptr!=other.ptr;
			}
			template<typename> friend class const_List_iterator;
	};
	template<typename Types>
	class const_List_iterator COUNT_IT{
		public:
			using NodePtr=typename Types::const_NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
		private:
			NodePtr ptr;
		public:
			const_List_iterator(NodePtr ptr):ptr(ptr){}
			const_List_iterator(List_iterator<Types> nonconst_it):ptr(nonconst_it.ptr){}
			reference operator*(){
				return ptr->data;
			}
			pointer operator->(){
				return &(ptr->data);
			}
			const_List_iterator& operator++(){
				ptr=ptr->next;
				return *this;
			}
			const_List_iterator operator++(int){
				const_List_iterator aux=*this;
				ptr=ptr->next;
				return aux;
			}
			const_List_iterator& operator--(){
				ptr=ptr->prev;
				return *this;
			}
			const_List_iterator operator--(int){
				const_List_iterator aux=*this;
				ptr=ptr->prev;
				return aux;
			}
			bool operator==(const const_List_iterator& other)const{
				return this->ptr==other.ptr;
			}
			bool operator!=(const const_List_iterator& other)const{
				return this->ptr!=other.ptr;
			}
	};
	template<typename T,typename Alloc=allocator<T>>
	class List:EBCO<rebind_alloc<Alloc,List_Node<T,typename allocator_traits<Alloc>::void_pointer>>> COUNT_IT_{
		private:
			using Node=List_Node<T,typename allocator_traits<Alloc>::void_pointer>;
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
			using iterator=List_iterator<Val_types>;
			using const_iterator=const_List_iterator<Val_types>;
		private:
			NodePtr head{};
			NodePtr tail{};
			size_type size{};
			template<typename... Args>
			inline NodePtr create_node(Args&&... args){
				NodePtr new_node=AlNode_traits::allocate(this->get_val(),1);
				AlNode_traits::construct(this->get_val(),new_node,static_cast<Args&&>(args)...);
				return new_node;
			}
			inline void delete_node(NodePtr node){
				AlNode_traits::destroy(this->get_val(),node);
				AlNode_traits::deallocate(this->get_val(),node,1);
			}
			inline void destroy_and_free(){
				NodePtr aux;
				tail=nullptr;
				while(head!=nullptr){
					aux=head;
					head=head->next;
					AlNode_traits::destroy(this->get_val(),aux);
					AlNode_traits::deallocate(this->get_val(),aux,1);
				}
			}
			inline void reset(){
				head=nullptr;
				tail=nullptr;
				size=0;
			}
			template<typename... Args>
			inline void create_first_node(Args&&... args){
				this->head=this->create_node(static_cast<Args&&>(args)...);
				this->tail=this->head;
			}
			template<typename... Args>
			inline void link_back(Args&&... args){
				this->tail->next=this->create_node(static_cast<Args&&>(args)...);
				this->tail->next->prev=this->tail;
				this->tail=this->tail->next;
			}
			template<typename... Args>
			inline void link_front(Args&&... args){
				this->head->prev=this->create_node(static_cast<Args&&>(args)...);
				this->head->prev->next=this->head;
				this->head=this->head->prev;
			}
			inline void copy_elements(const List& other){
				NodePtr p=other.head;
				if(p!=nullptr){
					this->create_first_node(p->data);
					p=p->next;
					while(p!=nullptr){
						this->link_back(p->data);
						p=p->next;
					}					
				}
				this->size=other.size;
			}
			inline void copy_elements(List&& other){
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
				}
				this->size=other.size;
				other.reset();
			}
			inline void move_elements(List& other){
				this->head=other.head;
				this->tail=other.tail;
				this->size=other.size;
				other.reset();
			}
			inline void copy_elements_keeping_this_allocation(const List& other){
				if(other.size==0 or this->size==0){
					this->destroy_and_free();
					this->copy_elements(other);
				}
				else if(this->size>=other.size){
					NodePtr i=this->head, j=other.head;
					while(j!=nullptr){
						this->tail=i;
						i->data=j->data;
						i=i->next;
						j=j->next;
					}
					tail->next=nullptr;
					while(i!=nullptr){
						NodePtr aux=i;
						i=i->next;
						this->delete_node(aux);
					}
					this->size=other.size;
				}
				else{
					NodePtr i=this->head, j=other.head;
					while(i!=nullptr){
						i->data=j->data;
						i=i->next;
						j=j->next;
					}
					while(j!=nullptr){
						this->link_back(j->data);
						j=j->next;
					}
					this->size=other.size;
				}
			}
			inline void copy_elements_keeping_this_allocation(List&& other){
				if(other.size==0 or this->size==0){
					this->destroy_and_free();
					this->copy_elements(static_cast<List&&>(other));
				}
				else if(this->size>=other.size){
					NodePtr i=this->head, j=other.head;
					while(j!=nullptr){
						this->tail=i;
						i->data=static_cast<value_type&&>(j->data);
						i=i->next;
						j=j->next;
					}
					tail->next=nullptr;
					while(i!=nullptr){
						NodePtr aux=i;
						i=i->next;
						this->delete_node(aux);
					}
					this->size=other.size;
					other.clear();
				}
				else{
					NodePtr i=this->head, j=other.head;
					while(i!=nullptr){
						i->data=static_cast<value_type&&>(j->data);
						i=i->next;
						j=j->next;
					}
					while(j!=nullptr){
						this->link_back(static_cast<value_type&&>(j->data));
						j=j->next;
					}
					this->size=other.size;
					other.clear();
				}
			}
			inline void add_back(const List& other){
				this->size+=other.size;
				NodePtr p=other.head;
				if(p!=nullptr){
					if(this->tail==nullptr){
						this->create_first_node(p->data);
						p=p->next;						
					}
					while(p!=nullptr){
						this->link_back(p->data);
						p=p->next;
					}
				}
			}
			inline void add_back(List&& other){
				this->size+=other.size;
				if(other.head!=nullptr){
					if(this->tail==nullptr){
						this->create_first_node(static_cast<value_type&&>(other.head->data));
						NodePtr aux=other.head;
						other.head=other.head->next;
						other.delete_node(aux);
					}
					while(other.head!=nullptr){
						this->link_back(static_cast<value_type&&>(other.head->data));
						NodePtr aux=other.head;
						other.head=other.head->next;
						other.delete_node(aux);
					}
					other.reset();
				}
			}
			inline void move_back(List& other){
				if(other.head==nullptr) return;
				this->size+=other.size;
				if(this->head==nullptr){
					this->head=other.head;
					this->tail=other.tail;
				}
				else{
					this->tail->next=other.head;
					other.head->prev=this->tail;
					this->tail=other.tail;
				}
				other.reset();
			}
			inline void add_front(const List& other){
				this->size+=other.size;
				NodePtr p=other.tail;
				if(p!=nullptr){
					if(this->head==nullptr){
						this->create_first_node(p->data);
						p=p->prev;						
					}
					while(p!=nullptr){
						this->link_front(p->data);
						p=p->prev;
					}
				}
			}
			inline void add_front(List&& other){
				this->size+=other.size;
				if(other.tail!=nullptr){
					if(this->head==nullptr){
						this->create_first_node(static_cast<value_type&&>(other.tail->data));
						NodePtr aux=other.tail;
						other.tail=other.tail->prev;
						other.delete_node(aux);
					}
					while(other.tail!=nullptr){
						this->link_front(static_cast<value_type&&>(other.tail->data));
						NodePtr aux=other.tail;
						other.tail=other.tail->prev;
						other.delete_node(aux);
					}
					other.reset();
				}
			}
			inline void move_front(List& other){
				if(other.head==nullptr) return;
				this->size+=other.size;
				if(this->head==nullptr){
					this->head=other.head;
					this->tail=other.tail;
				}
				else{
					this->head->prev=other.tail;
					other.tail->next=this->head;
					this->head=other.head;
				}
				other.reset();
			}
		public:
			List(){INCLISTS}
			explicit List(const Alloc& al):EBCO<AlNode>(al){INCLISTS}
			List(const List& other):EBCO<AlNode>(AlNode_traits::select_on_container_copy_construction(other.get_val())),size(other.size){INCLISTS
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
			List(const List& other,const Alloc& al):EBCO<AlNode>(al),size(other.size){INCLISTS
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
			List(List&& other):EBCO<AlNode>(static_cast<AlNode&&>(other.get_val())),head(other.head),tail(other.tail),size(other.size){INCLISTS
				other.reset();
			}
			List(List&& other,const Alloc& al):EBCO<AlNode>(al),size(other.size){INCLISTS
				if(this->get_val()==other.get_val()){
					this->head=other.head;
					this->tail=other.tail;
					other.reset();
				}
				else{
					if(other.head!=nullptr){
						other.size=0;
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
			List(size_type size,const Alloc& al=Alloc()):EBCO<AlNode>(al),size(size){INCLISTS
				if(size--){
					this->create_first_node(value_type());
					while(size--)
						this->link_back(value_type());
				}
			}
			List(size_type size,const_reference fillwith,const Alloc& al=Alloc()):EBCO<AlNode>(al),head(nullptr),tail(nullptr),size(size){INCLISTS
				if(size--){
					this->create_first_node(fillwith);
					while(size--)
						this->link_back(fillwith);
				}
			}
			List(std::initializer_list<value_type> initializerList):head(nullptr),tail(nullptr),size(initializerList.size()){INCLISTS
				const_pointer p=initializerList.begin();
				if(p!=initializerList.end()){
					this->create_first_node(*(p++));
					while(p!=initializerList.end())
						this->link_back(*(p++));
				}
			}
			List(std::initializer_list<value_type> initializerList,const Alloc& al):EBCO<AlNode>(al),head(nullptr),tail(nullptr),size(initializerList.size()){INCLISTS
				const_pointer p=initializerList.begin();
				if(p!=initializerList.end()){
					this->create_first_node(*(p++));
					while(p!=initializerList.end())
						this->link_back(*(p++));
				}
			}
			template<typename Iterable,enable_if_t<is_iterable_v<Iterable>>* = nullptr>
			List(const Iterable& lis){
				auto p=lis.begin();
				if(p!=lis.end()){
					size=1;
					this->create_first_node(*(p++));
					while(p!=lis.end()){
						size++;
						this->link_back(*(p++));
					}
				}
			}
			template<typename Iterable,enable_if_t<is_iterable_v<Iterable>>* = nullptr>
			List(const Iterable& lis,const Alloc& al):EBCO<AlNode>(al){
				auto p=lis.begin();
				if(p!=lis.end()){
					size=1;
					this->create_first_node(*(p++));
					while(p!=lis.end()){
						size++;
						this->link_back(*(p++));
					}
				}
			}
			List& operator=(const List& other){										//Operador de copia(optimizado)
				if(this!=&other){
					if constexpr(not ALWAYS_EQ and POCCA){
						if(this->get_val()!=other.get_val()){
							this->destroy_and_free();
							this->get_val()=other.get_val();
							this->copy_elements(other);
							return *this;
						}
					}
					this->copy_elements_keeping_this_allocation(other);
				}
				return *this;
			}
			List& operator=(List&& other){												//Operador de movimiento
				if(this==&other) return *this;
				if constexpr(not ALWAYS_EQ and not POCMA){
					if(this->get_val()!=other.get_val()){
						this->copy_elements_keeping_this_allocation(static_cast<List&&>(other));
						return *this;
					}
				}
				this->destroy_and_free();
				if constexpr(POCMA) this->get_val()=static_cast<AlNode&&>(other.get_val());
				this->move_elements(other);
				return *this;
			}
			List operator+(const List& other)const&{
				List new_list;
				new_list.copy_elements(*this);
				new_list.add_back(other);
				return new_list;
			}
			List operator+(List&& other)const&{
				List new_list;
				new_list.copy_elements(*this);
				if constexpr(not ALWAYS_EQ){
					if(new_list.get_val()==other.get_val())
						new_list.move_back(other);
					else
						new_list.add_back(static_cast<List&&>(other));
				}
				else
					new_list.move_back(other);
				return new_list;
			}
			List operator+(const List& other)&&{
				List new_list;
				new_list.copy_elements(other);
				if constexpr(not ALWAYS_EQ){
					if(new_list.get_val()==this->get_val())
						new_list.move_front(*this);
					else
						new_list.add_front(static_cast<List&&>(*this));
				}
				else
					new_list.move_front(*this);
				return new_list;
			}
			List operator+(List&& other)&&{
				List new_list;
				if constexpr(not ALWAYS_EQ){
					if(new_list.get_val()==this->get_val())
						new_list.move_elements(*this);
					else
						new_list.copy_elements(static_cast<List&&>(*this));
					if(new_list.get_val()==other.get_val())
						new_list.move_back(other);
					else
						new_list.add_back(static_cast<List&&>(other));
				}
				else{
					new_list.move_elements(*this);
					new_list.move_back(other);
				}
				return new_list;
			}
			List& operator+=(const List& other){
				this->add_back(other);
				return *this;
			}
			List& operator+=(List&& other){
				if constexpr(ALWAYS_EQ){
					this->move_back(other);
				}
				else{
					if(this->get_val()==other.get_val()){
						this->add_back(static_cast<List&&>(other));
					}
				}
				return *this;
			}
			List& operator+=(std::initializer_list<value_type> initializerList){
				const_pointer p=initializerList.begin();
				if(p!=initializerList.end()){
					if(this->head==nullptr)
						this->create_first_node(*(p++));
					while(p!=initializerList.end())
						this->link_back(*(p++));
				}
				this->size+=initializerList.size();
				return *this;
			}
			List operator*(size_type num)const{
				List new_list;
				for(size_type i=0;i<num;i++)
					new_list+=*this;
				return new_list;
			}
			List& operator*=(size_type num){
				if(size==0)
					return *this;
				else if(num==0){
					clear();
					return *this;
				}
				for(size_type i=1;i<num;i++){
					NodePtr p=head;
					for(size_type j=0;j<size;j++){
						link_back(p->data);
						p=p->next;
					}
				}
				size*=num;
				return *this;
			}
			const_reference operator[](size_type index)const{
				NodePtr current=this->head;
				for(size_type i=0;i<index;i++)
					current=current->next;
				return current->data;
			}
			reference operator[](size_type index){
				NodePtr current=this->head;
				for(size_type i=0;i<index;i++)
					current=current->next;
				return current->data;
			}
			const_reference back()const{
				return tail->data;
			}
			reference back(){
				return tail->data;
			}
			bool operator==(const List& other)const{
				if(this->size!=other.size)
					return false;
				for(NodePtr i=this->head,j=other.head;i!=nullptr;i=i->next,j=j->next){
					if(i->data!=j->data)
						return false;
				}
				return true;
			}
			bool operator!=(const List& other)const{
				if(this->size!=other.size)
					return true;
				for(NodePtr i=this->head,j=other.head;i!=nullptr;i=i->next,j=j->next){
					if(i->data!=j->data)
						return true;
				}
				return false;
			}
			template<typename... Args>
			void emplace_back(Args&&... args){
				APPEND(args...)
				this->size++;
			}
			void append(const_reference dat){
				APPEND(dat)
				this->size++;
			}
			void append(value_type&& dat){
				APPEND(static_cast<value_type&&>(dat))
				this->size++;
			}
			template<typename... Args>
			void emplace(size_type index,Args&&... args){
				NodePtr new_node=create_node(static_cast<Args&&>(args)...);
				if(this->head==nullptr){
					this->head=new_node;
					this->tail=new_node;
				}
				else{
					NodePtr current=this->head;
					for(size_type i=0;i<index;i++)
						current=current->next;
					if(current==this->head){
						this->head->prev=new_node;
						new_node->next=this->head;
						this->head=new_node;
					}						
					else if(current==nullptr){
						this->tail->next=new_node;
						new_node->prev=this->tail;
						this->tail=new_node;
					}
					else{
						new_node->next=current;
						new_node->prev=current->prev;
						new_node->prev->next=new_node;
						new_node->next->prev=new_node;
					}
				}
				this->size++;
			}
			void insert(size_type index,const_reference dat){
				NodePtr new_node=create_node(dat);
				if(this->head==nullptr){
					this->head=new_node;
					this->tail=new_node;
				}
				else{
					NodePtr current=this->head;
					for(size_type i=0;i<index;i++)
						current=current->next;
					if(current==this->head){
						this->head->prev=new_node;
						new_node->next=this->head;
						this->head=new_node;
					}						
					else if(current==nullptr){
						this->tail->next=new_node;
						new_node->prev=this->tail;
						this->tail=new_node;
					}
					else{
						new_node->next=current;
						new_node->prev=current->prev;
						new_node->prev->next=new_node;
						new_node->next->prev=new_node;
					}
				}
				this->size++;
			}
			void insert(size_type index,value_type&& dat){
				NodePtr new_node=create_node(static_cast<value_type&&>(dat));
				if(this->head==nullptr){
					this->head=new_node;
					this->tail=new_node;
				}
				else{
					NodePtr current=this->head;
					for(size_type i=0;i<index;i++)
						current=current->next;
					if(current==this->head){
						this->head->prev=new_node;
						new_node->next=this->head;
						this->head=new_node;
					}						
					else if(current==nullptr){
						this->tail->next=new_node;
						new_node->prev=this->tail;
						this->tail=new_node;
					}
					else{
						new_node->next=current;
						new_node->prev=current->prev;
						new_node->prev->next=new_node;
						new_node->next->prev=new_node;
					}
				}
				this->size++;
			}
			value_type pop(size_type index){
				NodePtr current=this->head;
				for(size_type i=0;i<index;i++)
					current=current->next;
				value_type aux=static_cast<value_type&&>(current->data);
				if(current==this->tail and current==this->head){
					this->head=nullptr;
					this->tail=nullptr;
				}
				else if(current==this->tail){
					this->tail=current->prev;
					this->tail->next=nullptr;
				}
				else if(current==this->head){
					this->head=current->next;
					this->head->prev=nullptr;
				}
				else{
					current->prev->next=current->next;
					current->next->prev=current->prev;				
				}
				delete_node(current);
				this->size--;
				return aux;
			}
			void del(size_type index){
				NodePtr current=this->head;
				for(size_type i=0;i<index;i++)
					current=current->next;
				if(current==this->tail and current==this->head){
					this->head=nullptr;
					this->tail=nullptr;
				}
				else if(current==this->tail){
					this->tail=current->prev;
					this->tail->next=nullptr;
				}
				else if(current==this->head){
					this->head=current->next;
					this->head->prev=nullptr;
				}
				else{
					current->prev->next=current->next;
					current->next->prev=current->prev;				
				}
				delete_node(current);
				this->size--;
			}
			value_type pop_back(){
				value_type aux=tail->data;
				POP_BACK
				size--;
				return aux;
			}
			void del_back(){
				POP_BACK
				size--;
			}
		private:
			struct AuxList{
				NodePtr head{};
				NodePtr tail{};
				AuxList(const List& list,AlNode& alloc){
					NodePtr p=list.head;
					if(p!=nullptr){
						this->head=AlNode_traits::allocate(alloc,1);
						AlNode_traits::construct(alloc,this->head,p->data);
						this->tail=this->head;
						p=p->next;
						while(p!=nullptr){
							this->tail->next=AlNode_traits::allocate(alloc,1);
							AlNode_traits::construct(alloc,this->tail->next,p->data);
							this->tail->next->prev=this->tail;
							this->tail=this->tail->next;
							p=p->next;
						}
					}
				}
				AuxList(List&& list,AlNode& alloc){
					if(list.head!=nullptr){
						this->head=AlNode_traits::allocate(alloc,1);
						AlNode_traits::construct(alloc,this->head,list.head->data);
						this->tail=this->head;
						NodePtr aux=list.head;
						list.head=list.head->next;
						list.delete_node(aux);
						while(list.head!=nullptr){
							this->tail->next=AlNode_traits::allocate(alloc,1);
							AlNode_traits::construct(alloc,this->tail->next,list.head->data);
							this->tail->next->prev=this->tail;
							this->tail=this->tail->next;
							aux=list.head;
							list.head=list.head->next;
							list.delete_node(aux);
						}
					}
					list.reset();
				}
			};
			inline void link_with_head(AuxList& auxlist){
				auxlist.tail->next=this->head;
				this->head->prev=auxlist.tail;
				this->head=auxlist.head;
			}
			inline void link_with_tail(AuxList& auxlist){
				auxlist.head->prev=this->tail;
				this->tail->next=auxlist.head;
				this->tail=auxlist.tail;
			}
			inline void cut_alloc_helper(NodePtr start,AlNode& dealloc){
				if(start!=nullptr){
					this->create_first_node(static_cast<value_type&&>(start->data));
					NodePtr aux=start;
					start=start->next;
					AlNode_traits::destroy(dealloc,aux);
					AlNode_traits::deallocate(dealloc,aux,1);
					while(start!=nullptr){
						this->link_back(static_cast<value_type&&>(start->data));
						aux=start;
						start=start->next;
						AlNode_traits::destroy(dealloc,aux);
						AlNode_traits::deallocate(dealloc,aux,1);
					}
				}
			}
		public:
			List concat(size_type index,const List& other)const{
				if(other.head==nullptr) return *this;
				List new_list;
				new_list.copy_elements(*this);
				if(index==0)
					new_list.add_front(other);
				else if(index==this->size)
					new_list.add_back(other);
				else{
					AuxList aux(other,new_list.get_val());
					new_list.size+=other.size;
					NodePtr ptr=new_list.head;
					while(index--)
						ptr=ptr->next;
					ptr->prev->next=aux.head;
					aux.head->prev=ptr->prev;
					aux.tail->next=ptr;
					ptr->prev=aux.tail;
				}
				return new_list;
			}
			List concat(size_type index,List&& other)const{
				if(other.head==nullptr) return *this;
				List new_list;
				new_list.copy_elements(*this);
				if constexpr(not ALWAYS_EQ){
					if(new_list.get_val()!=other.get_val()){
						if(index==0)
							new_list.add_front(static_cast<List&&>(other));
						else if(index==this->size)
							new_list.add_back(static_cast<List&&>(other));
						else{
							AuxList aux(static_cast<List&&>(other),new_list.get_val());
							new_list.size+=other.size;
							NodePtr ptr=new_list.head;
							while(index--)
								ptr=ptr->next;
							ptr->prev->next=aux.head;
							aux.head->prev=ptr->prev;
							aux.tail->next=ptr;
							ptr->prev=aux.tail;
						}
						return new_list;
					}
				}
				if(index==0)
					new_list.move_front(other);
				else if(index==this->size)
					new_list.move_back(other);
				else{
					new_list.size+=other.size;
					NodePtr ptr=new_list.head;
					while(index--)
						ptr=ptr->next;
					ptr->prev->next=other.head;
					other.head->prev=ptr->prev;
					other.tail->next=ptr;
					ptr->prev=other.tail;
					other.reset();
				}
				return new_list;
			}
			void join(size_type index,const List& other){ 
				if(other.head==nullptr) return;
				if(&other==this){					// this may be equeal to &other
					AuxList aux(other,this->get_val());
					if(index==0)
						this->link_with_head(aux);
					else if(index==this->size)
						this->link_with_tail(aux);
					else{
						NodePtr ptr=this->head;
						while(index--)
							ptr=ptr->next;
						ptr->prev->next=aux.head;
						aux.head->prev=ptr->prev;
						aux.tail->next=ptr;
						ptr->prev=aux.tail;
					}
					this->size*=2;
				}
				else{
					if(index==0)
						this->add_front(other);
					else if(index==this->size)
						this->add_back(other);
					else{
						AuxList aux(other,this->get_val());
						this->size+=other.size;
						NodePtr ptr=this->head;
						while(index--)
							ptr=ptr->next;
						ptr->prev->next=aux.head;
						aux.head->prev=ptr->prev;
						aux.tail->next=ptr;
						ptr->prev=aux.tail;
					}					
				}
			}
			void join(size_type index,List&& other){
				if(other.head==nullptr) return;
				if constexpr(not ALWAYS_EQ){
					if(this->get_val()!=other.get_val()){
						if(index==0)
							this->add_front(static_cast<List&&>(other));
						else if(index==this->size)
							this->add_back(static_cast<List&&>(other));
						else{
							AuxList aux(static_cast<List&&>(other),this->get_val());
							this->size+=other.size;
							NodePtr ptr=this->head;
							while(index--)
								ptr=ptr->next;
							ptr->prev->next=aux.head;
							aux.head->prev=ptr->prev;
							aux.tail->next=ptr;
							ptr->prev=aux.tail;
						}
						return;
					}
				}
				if(index==0)
					this->move_front(other);
				else if(index==this->size)
					this->move_back(other);
				else{
					this->size+=other.size;
					NodePtr ptr=this->head;
					while(index--)
						ptr=ptr->next;
					ptr->prev->next=other.head;
					other.head->prev=ptr->prev;
					other.tail->next=ptr;
					ptr->prev=other.tail;
					other.reset();
				}
			}
			List sublist(size_type start, size_type end)const{
				List new_list;
				new_list.size=end-start;
				NodePtr ptr_start=this->head, ptr_end=this->head;
				for(size_type i=0;i<start;i++)
					ptr_start=ptr_start->next;
				for(size_type i=0;i<end;i++)
					ptr_end=ptr_end->next;
				if(ptr_start!=ptr_end){
					new_list.create_first_node(ptr_start->data);
					ptr_start=ptr_start->next;
					while(ptr_start!=ptr_end){
						new_list.link_back(ptr_start->data);
						ptr_start=ptr_start->next;
					}
				}
				return new_list;
			}
			List sublist(size_type start)const{
				List new_list;
				new_list.size=size-start;
				NodePtr ptr_start=this->head;
				for(size_type i=0;i<start;i++)
					ptr_start=ptr_start->next;
				if(ptr_start!=nullptr){
					new_list.create_first_node(ptr_start->data);
					ptr_start=ptr_start->next;
					while(ptr_start!=nullptr){
						new_list.link_back(ptr_start->data);
						ptr_start=ptr_start->next;
					}
				}
				return new_list;
			}
			void wipe(size_type start, size_type end){
				if(start==end) return;
				NodePtr ptr_start=this->head, ptr_end=this->head;
				for(size_type i=0;i<start;i++)
					ptr_start=ptr_start->next;
				for(size_type i=0;i<end;i++)
					ptr_end=ptr_end->next;
				size-=end-start;
				NodePtr start_prev=ptr_start->prev;
				while(ptr_start!=ptr_end){
					NodePtr aux=ptr_start;
					ptr_start=ptr_start->next;
					delete_node(aux);
				}
				if(start_prev and ptr_end){
					start_prev->next=ptr_end;
					ptr_end->prev=start_prev;
				}
				else if(start_prev){
					tail=start_prev;
					start_prev->next=nullptr;
				}
				else if(ptr_end){
					head=ptr_end;
					ptr_end->prev=nullptr;
				}
				else{
					head=nullptr;
					tail=nullptr;
				}
			}
			void wipe(size_type start){
				if(start==size) return;
				NodePtr ptr_start=this->head;
				for(size_type i=0;i<start;i++)
					ptr_start=ptr_start->next;
				size=start;
				NodePtr start_prev=ptr_start->prev;
				while(ptr_start!=nullptr){
					NodePtr aux=ptr_start;
					ptr_start=ptr_start->next;
					delete_node(aux);
				}
				if(start_prev){
					tail=start_prev;
					start_prev->next=nullptr;
				}
				else{
					head=nullptr;
					tail=nullptr;
				}
			}
			List cut(size_type start,size_type end){
				List new_list;
				if(start==end) return new_list;//si this esta vacio y start y end son distintos de 0 es comportamiento indefinido an igual que si start<end o alguno se va del rango de la lista
				NodePtr ptr_start=this->head, ptr_end=this->head;
				for(size_type i=0;i<start;i++)
					ptr_start=ptr_start->next;
				for(size_type i=0;i<end;i++)
					ptr_end=ptr_end->next;
				new_list.size=end-start;
				this->size-=new_list.size;
				NodePtr start_prev=ptr_start->prev;

				if constexpr(not ALWAYS_EQ){
					if(new_list.get_val()!=this->get_val()){
						ptr_start->prev=nullptr;
						if(start_prev and ptr_end){
							ptr_end->prev->next=nullptr;
							start_prev->next=ptr_end;
							ptr_end->prev=start_prev;
						}
						else if(start_prev){
							this->tail=start_prev;
							start_prev->next=nullptr;
						}
						else if(ptr_end){
							ptr_end->prev->next=nullptr;
							this->head=ptr_end;
							ptr_end->prev=nullptr;
						}
						else{
							this->head=nullptr;
							this->tail=nullptr;
						}
						new_list.cut_alloc_helper(ptr_start,this->get_val());
						return new_list;
					}
				}
				new_list.head=ptr_start;
				ptr_start->prev=nullptr;
				if(start_prev and ptr_end){//no corta ni el inicio ni el final de la lista
					ptr_end->prev->next=nullptr;
					new_list.tail=ptr_end->prev;
					start_prev->next=ptr_end;
					ptr_end->prev=start_prev;
				}
				else if(start_prev){//corta hasta el final de la lista
					new_list.tail=this->tail;
					this->tail=start_prev;
					start_prev->next=nullptr;
				}
				else if(ptr_end){//corta desde el inicio de la lista
					ptr_end->prev->next=nullptr;
					new_list.tail=ptr_end->prev;
					this->head=ptr_end;
					ptr_end->prev=nullptr;
				}
				else{//corta lista entera
					new_list.tail=this->tail;
					this->head=nullptr;
					this->tail=nullptr;
				}
				return new_list;
			}
			List cut(size_type start){
				List new_list;
				if(start==size) return new_list;
				NodePtr ptr_start=this->head;
				for(size_type i=0;i<start;i++)
					ptr_start=ptr_start->next;
				new_list.size=this->size-start;
				this->size=start;
				NodePtr start_prev=ptr_start->prev;

				if constexpr(not ALWAYS_EQ){
					if(new_list.get_val()!=this->get_val()){
						ptr_start->prev=nullptr;
						if(start_prev){
							this->tail=start_prev;
							start_prev->next=nullptr;
						}
						else{
							this->head=nullptr;
							this->tail=nullptr;
						}
						new_list.cut_alloc_helper(ptr_start,this->get_val());
						return new_list;
					}
				}
				new_list.head=ptr_start;
				ptr_start->prev=nullptr;
				new_list.tail=this->tail;
				if(start_prev){
					this->tail=start_prev;
					start_prev->next=nullptr;
				}
				else{
					this->head=nullptr;
					this->tail=nullptr;
				}
				return new_list;
			}
			size_type len()const{
				#if defined(_DEBUG) && (defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_))
				size_type node_count=0;
				for(NodePtr current=this->head;current!=nullptr;node_count++){
					current=current->next;
				}
				if(node_count!=size){
					std::cerr<<"\nSIZE AND NODE_COUNT DO NOT MATCH\n"
							<<"LEN: "<<size<<"\nNODE_COUNT: "<<node_count<<std::endl;
					exit(-1);
				}
				#endif
				return size;
			}
			void resize(size_type new_size){
				if(this->size<new_size){
					if(this->head==nullptr){
						this->create_first_node(value_type());
						this->size++;						
					}
					while(this->size<new_size){
						this->link_back(value_type());
						this->size++;
					}
				}
				else{
					while(this->size>new_size){
						POP_BACK
						this->size--;
					}					
				}
			}
			template<typename D>
 			Optional<size_type> find(const D& data,size_type start,size_type end)const{	//retorna -1 si no se encontro el elemento
				if(start>=end)
					return {};
				NodePtr ptr_start=this->head, ptr_end=this->head;
				for(size_type i=0;i<start;i++)
					ptr_start=ptr_start->next;
				for(size_type i=0;i<end;i++)
					ptr_end=ptr_end->next;
				for(size_type i=0;ptr_start!=ptr_end;ptr_start=ptr_start->next,i++){
					if(ptr_start->data==data)
						return i+start;
				}
				return {};
			}
			template<typename D>
 			Optional<size_type> find(const D& data,size_type start=0)const{	//retorna -1 si no se encontro el elemento
				if(start>=size)
					return {};
				NodePtr ptr_start=this->head;
				for(size_type i=0;i<start;i++)
					ptr_start=ptr_start->next;
				for(size_type i=0;ptr_start!=nullptr;ptr_start=ptr_start->next,i++){
					if(ptr_start->data==data)
						return i+start;
				}
				return {};
			}
			template<typename D>
			size_type count(const D& dat)const{
				size_type counter=0;
				NodePtr current=this->head;
				while(current!=nullptr){
					if(current->data==dat)
						counter++;
					current=current->next;
				}
				return counter;
			}
			template<typename D>
			size_type remove_all(const D& dat){	//devuelve la cantidad removida
				NodePtr aux, current=this->head;
				size_type found=0;
				while(current!=nullptr){
					if(current->data==dat){
						if(current==this->tail and current==this->head){
							delete_node(current);
							this->head=nullptr;
							this->tail=nullptr;
							aux=nullptr;
						}
						else if(current==this->tail){
							this->tail=current->prev;
							this->tail->next=nullptr;
							delete_node(current);
							aux=nullptr;
						}
						else if(current==this->head){
							aux=this->head->next;
							this->head=current->next;
							this->head->prev=nullptr;
							delete_node(current);
						}
						else{
							aux=current->next;
							current->prev->next=current->next;
							current->next->prev=current->prev;
							delete_node(current);					
						}
						this->size--;
						current=aux;
						found++;
					}
					else
						current=current->next;
				}
				return found;
			}
			template<typename D>
			size_type remove(const D& dat,bool all=false){	//devuelve la cantidad removida
				NodePtr current=this->head;
				while(current!=nullptr){
					if(current->data==dat){
						if(current==this->tail and current==this->head){
							delete_node(current);
							this->head=nullptr;
							this->tail=nullptr;
						}
						else if(current==this->tail){
							this->tail=current->prev;
							this->tail->next=nullptr;
							delete_node(current);
						}
						else if(current==this->head){
							this->head=current->next;
							this->head->prev=nullptr;
							delete_node(current);
						}
						else{
							current->prev->next=current->next;
							current->next->prev=current->prev;
							delete_node(current);					
						}
						this->size--;
						return true;
					}
					else
						current=current->next;
				}
				return false;
			}
			template<typename D>
			bool has(const D& dat)const{
				for(NodePtr i=this->head;i!=nullptr;i=i->next){
					if(i->data==dat)
						return true;
				}
				return false;
			}
			bool empty()const{
				return this->head==nullptr;
			}
			void sort(){
				if(head==nullptr) return;
				value_type aux;
				for(NodePtr i=head->next, j;i!=nullptr;i=i->next){
					for(j=i,aux=move(i->data);j!=head and aux<j->prev->data;j=j->prev){
						j->data=move(j->prev->data);
					}
					j->data=move(aux);
				}
			}
			void sort_reverse(){
				if(head==nullptr) return;
				value_type aux;
				for(NodePtr i=head->next, j;i!=nullptr;i=i->next){
					for(j=i,aux=move(i->data);j!=head and aux>j->prev->data;j=j->prev){
						j->data=move(j->prev->data);
					}
					j->data=move(aux);
				}
			}
			template<typename D>
			void sort(D(*func)(const_reference)){
				if(head==nullptr) return;
				value_type aux;
				for(NodePtr i=head->next, j;i!=nullptr;i=i->next){
					for(j=i,aux=(value_type&&)i->data;j!=head and func(aux)<func(j->prev->data);j=j->prev){
						j->data=(value_type&&)j->prev->data;
					}
					j->data=move(aux);
				}
			}
			template<typename Cmp>
			void sort(Cmp&& cmp=Cmp{}){
				if(head==nullptr) return;
				value_type aux;
				for(NodePtr i=head->next, j;i!=nullptr;i=i->next){
					for(j=i,aux=move(i->data);j!=head and cmp(aux,j->prev->data);j=j->prev){
						j->data=move(j->prev->data);
					}
					j->data=move(aux);
				}
			}
			template<typename Lambda>
			bool any(Lambda&& func=Lambda{})const{
                for(const_reference val:*this)
                    if(func(val))return 1;
                return 0;
			}
			template<typename Lambda>
			void forEach(Lambda&& func=Lambda{}){
				for(reference val:*this)
					func(val);
			}
			template<typename,typename> friend class List;
			template<typename Lambda>
			auto map(Lambda&& func=Lambda{})const{
				List<decltype(func(head->data)),rebind_alloc<Alloc,decltype(func(head->data))>> new_list;
				NodePtr p=this->head;
				if(this->head!=nullptr){
					new_list.create_first_node(func(p->data));
					p=p->next;
					while(p!=nullptr){
						new_list.link_back(func(p->data));
						p=p->next;
					}
					new_list.size=this->size;
				}
				return new_list;
			}
			template<typename Lambda>
			List filter(Lambda&& func=Lambda{})const{
				List new_list;
				NodePtr p=this->head;
				if(this->head!=nullptr){
					while(new_list.head==nullptr and p!=nullptr){
						if(func(p->data)){
							new_list.create_first_node(p->data);
							new_list.size++;
						}
						p=p->next;
					}
					while(p!=nullptr){
						if(func(p->data)){
							new_list.link_back(p->data);
							new_list.size++;
						}
						p=p->next;
					}
				}
				return new_list;
			}
			void reverse(){
				value_type aux;
				for(NodePtr i=this->head, j=this->tail;i!=j and i->prev!=j;i=i->next,j=j->prev){
					aux=move(i->data);
					i->data=move(j->data);
					j->data=move(aux);
				}
			}
			inline void clear(){
				NodePtr aux;
				size=0;
				tail=nullptr;
				while(head!=nullptr){
					aux=head;
					head=head->next;
					delete_node(aux);
				}
			}
			~List(){
				NodePtr aux;
				while(head!=nullptr){
					aux=head;
					head=head->next;
					delete_node(aux);
				}
				DECLISTS
			}
			iterator begin(){
				return iterator(head);
			}
			iterator end(){
				return iterator(nullptr);
			}
			iterator rbegin(){
				return iterator(tail);
			}
			iterator rend(){
				return iterator(nullptr);
			}
			const_iterator begin()const{
				return const_iterator(head);
			}
			const_iterator end()const{
				return const_iterator(nullptr);
			}
			const_iterator rbegin()const{
				return const_iterator(tail);
			}
			const_iterator rend()const{
				return const_iterator(nullptr);
			}
			template<typename t,typename alloc>friend void debug(const List<t,alloc>&);
	};
	template<typename Out,typename T,typename Alloc>
	Out& operator<<(Out& stream,const List<T,Alloc>& list){
		stream<<'[';
		for(typename List<T,Alloc>::const_iterator i=list.begin();i!=list.end();i++){
			stream<<*i;
			if(i!=list.rbegin())
				stream<<", ";		
		}
		return stream<<']';
	}
#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
    template<typename T,typename Alloc>
    void debug(const List<T,Alloc>& list){
        std::cout<<"size: "<<list.len()<<std::endl
                 <<"head: "<<list.head<<std::endl
                 <<"tail: "<<list.tail<<std::endl;
    }
#endif
}