#pragma once

namespace mpv{
	template<typename T,typename VoidPtr> struct List_Node;
	template<typename T,typename VoidPtr>
	struct Base_List_Node{
		using BaseNodePtr=rebind_pointer<VoidPtr,Base_List_Node>;
		using NodePtr=rebind_pointer<VoidPtr,List_Node<T,VoidPtr>>;
		using const_NodePtr=rebind_pointer<VoidPtr,const List_Node<T,VoidPtr>>;
		NodePtr next;
		NodePtr prev;
		void link_right(NodePtr other){//links other to the right of this
			this->next->prev=other;
			other->next=this->next;
			this->next=other;
			other->prev=NodePtr(this);
		}
		void link_left(NodePtr other){//links other to the left of this
			this->prev->next=other;
			other->prev=this->prev;
			this->prev=other;
			other->next=NodePtr(this);
		}
		void unlink(){
			this->prev->next=this->next;
			this->next->prev=this->prev;
		}
		NodePtr address(){
			return NodePtr(this);
		}
		const_NodePtr address()const{
			return const_NodePtr(this);
		}
	};
	template<typename T,typename VoidPtr>
	struct List_Node:public Base_List_Node<T,VoidPtr> COUNT_IT_{
		using NodePtr=typename Base_List_Node<T,VoidPtr>::NodePtr;
		using const_NodePtr=typename Base_List_Node<T,VoidPtr>::const_NodePtr;
		T data;
		template<typename... Args>
		List_Node(Args&&... args):data(static_cast<Args&&>(args)...){INCLNODES}
		List_Node(const List_Node&)=delete;
		List_Node& operator=(const List_Node&)=delete;
#ifdef SHOWREMAININGOBJECTS
		~List_Node(){
			DECLNODES
			//REMAININGLNODES
		}
#endif
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
				return pointer_traits<pointer>::pointer_to(ptr->data);
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
				return pointer_traits<pointer>::pointer_to(ptr->data);
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
			using BaseNode=Base_List_Node<T,typename allocator_traits<Alloc>::void_pointer>;
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
		private:
#define HEAD null.next
#define TAIL null.prev
			size_type length=0;
			BaseNode null;
			template<typename... Args>
			inline NodePtr create_node(Args&&... args){
				NodePtr new_node=AlNode_traits::allocate(this->get_val(),1);
				/* AlNode_traits::construct */CONSTRUCT_VARARGS(this->get_val(),new_node,static_cast<Args&&>(args));
				new_node->next=new_node->prev=null.address();
				return new_node;
			}
			inline void delete_node(NodePtr node){
				/* AlNode_traits::destroy */DESTROY(this->get_val(),node);
				AlNode_traits::deallocate(this->get_val(),node,1);
			}
			inline void destroy_and_free(){
				NodePtr aux;
				TAIL=null.address();
				while(HEAD!=null.address()){
					aux=HEAD;
					HEAD=HEAD->next;
					/* AlNode_traits::destroy */DESTROY(this->get_val(),aux);
					AlNode_traits::deallocate(this->get_val(),aux,1);
				}//TAIL and HEAD en up pointing to null.address()
			}
			inline void reset(){
				HEAD=TAIL=null.address();
				length=0;
			}
			inline void set_null_sides(){
				this->HEAD->prev=this->TAIL->next=this->null.address();
			}
			inline void link_back(NodePtr node){
				TAIL->next=node;// node is allready pointing to null when created
				TAIL->next->prev=TAIL;
				TAIL=TAIL->next;
			}
			inline void link_front(NodePtr node){
				HEAD->prev=node;
				HEAD->prev->next=HEAD;
				HEAD=HEAD->prev;
			}
			inline void link_at(size_type pos,NodePtr node){
				NodePtr p=HEAD;
				for(size_type i=0;i<pos;i++) p=p->next;
				p->link_left(node);
			}
			NodePtr unlink_back(){
				NodePtr p=TAIL;
				p->unlink();
				return p;
			}
			NodePtr unlink_at(size_type pos){
				NodePtr p=HEAD;
				for(size_type i=0;i<pos;i++) p=p->next;
				p->unlink();
				return p;
			}
/*	Cada una de las funciones que siguen asume que recibe una lista valida en la que length coincide con 
	la actual cantidad de nodos  en la lista, sus nodos correctamente enlazados y apuntando a null 
	en caso de que ize sea 0. 
	Modifican la lista this, dejan length coincidiendo con la cantidad de nodos y en caso de que modifiquen la
	segunda lista(porque la primera lista se apropia de los elementos de la segunda), se encargan de que esta
	siga siendo valida, dejando length en 0 y haciendo que HEAD y TAIL apunten a other.null

*/
			inline void copy_elements(const List& other){
				NodePtr p=other.HEAD;
				while(p!=other.null.address()){
					this->link_back(create_node(p->data));
					p=p->next;			
				}
				this->length=other.length;
			}
			inline void copy_elements(List&& other){
				while(other.HEAD!=other.null.address()){
					this->link_back(create_node(static_cast<value_type&&>(other.HEAD->data)));
					NodePtr aux=other.HEAD;
					other.HEAD=other.HEAD->next;
					other.delete_node(aux);
				}
				this->length=other.length;
				other.TAIL=other.null.address();
				other.length=0;
			}
			inline void move_elements(List& other){
				this->length=other.length;
				if(other.length){
					this->HEAD=other.HEAD;
					this->TAIL=other.TAIL;
					this->set_null_sides();
					other.reset();					
				}
				else{
					this->HEAD=this->null.address();
					this->TAIL=this->null.address();
				}
			}
			inline void copy_elements_keeping_this_allocation(const List& other){
				if(this->length>=other.length){
					NodePtr i=this->HEAD, j=other.HEAD;
					while(j!=other.null.address()){
						this->TAIL=i;
						i->data=j->data;
						i=i->next;
						j=j->next;
					}
					this->TAIL=i->prev;
					TAIL->next=this->null.address();
					while(i!=this->null.address()){
						NodePtr aux=i;
						i=i->next;
						this->delete_node(aux);
					}
					this->length=other.length;
				}
				else{
					NodePtr i=this->HEAD, j=other.HEAD;
					while(i!=this->null.address()){
						i->data=j->data;
						i=i->next;
						j=j->next;
					}
					while(j!=other.null.address()){
						this->link_back(create_node(j->data));
						j=j->next;
					}
					this->length=other.length;
				}
			}
			inline void copy_elements_keeping_this_allocation(List&& other){
				if(this->length>=other.length){
					NodePtr i=this->HEAD, j=other.HEAD;
					while(j!=other.null.address()){
						this->TAIL=i;
						i->data=static_cast<value_type&&>(j->data);
						i=i->next;
						j=j->next;
					}
					this->TAIL=i->prev;
					TAIL->next=this->null.address();
					while(i!=this->null.address()){
						NodePtr aux=i;
						i=i->next;
						this->delete_node(aux);
					}
					this->length=other.length;
					other.clear();
				}
				else{
					NodePtr i=this->HEAD, j=other.HEAD;
					while(i!=this->null.address()){
						i->data=static_cast<value_type&&>(j->data);
						i=i->next;
						j=j->next;
					}
					while(j!=other.null.address()){
						this->link_back(create_node(static_cast<value_type&&>(j->data)));
						j=j->next;
					}
					this->length=other.length;
					other.clear();
				}
			}
			inline void add_back(const List& other){
				this->length+=other.length;
				NodePtr p=other.HEAD;
				while(p!=other.null.address()){
					this->link_back(create_node(p->data));
					p=p->next;
				}
			}
			inline void add_back(List&& other){
				this->length+=other.length;
				while(other.HEAD!=other.null.address()){
					this->link_back(create_node(static_cast<value_type&&>(other.HEAD->data)));
					NodePtr aux=other.HEAD;
					other.HEAD=other.HEAD->next;
					other.delete_node(aux);
				}
				other.reset();
			}
			inline void move_back(List& other){
				this->length+=other.length;
				this->TAIL->next=other.HEAD;
				other.HEAD->prev=this->TAIL;
				this->TAIL=other.TAIL;
				this->TAIL->next=this->null.address();
				other.reset();
			}
			inline void add_front(const List& other){
				this->length+=other.length;
				NodePtr p=other.TAIL;
				while(p!=other.null.address()){
					this->link_front(create_node(p->data));
					p=p->prev;
				}
			}
			inline void add_front(List&& other){
				this->length+=other.length;
				while(other.TAIL!=other.null.address()){
					this->link_front(create_node(static_cast<value_type&&>(other.TAIL->data)));
					NodePtr aux=other.TAIL;
					other.TAIL=other.TAIL->prev;
					other.delete_node(aux);
				}
				other.reset();
			}
			inline void move_front(List& other){
				this->length+=other.length;
				this->HEAD->prev=other.TAIL;
				other.TAIL->next=this->HEAD;
				this->HEAD=other.HEAD;
				this->HEAD->prev=this->null.address();
				other.reset();
			}
		 public:
			List(){
				HEAD=null.address();
				TAIL=null.address();
			}
			explicit List(const Alloc& al):EBCO<AlNode>(al){
				HEAD=null.address();
				TAIL=null.address();
			}
			List(const List& other):EBCO<AlNode>(AlNode_traits::select_on_container_copy_construction(other.get_val())),length(other.length){
				HEAD=null.address();
				TAIL=null.address();
				NodePtr p=other.HEAD;
				while(p!=other.null.address()){
					this->link_back(create_node(p->data));
					p=p->next;
				}
			}
			List(const List& other,const Alloc& al):EBCO<AlNode>(al),length(other.length){
				HEAD=null.address();
				TAIL=null.address();
				NodePtr p=other.HEAD;
				while(p!=other.null.address()){
					this->link_back(create_node(p->data));
					p=p->next;
				}
			}
			List(List&& other):EBCO<AlNode>(static_cast<AlNode&&>(other.get_val())),length(other.length){
				if(other.length){
					HEAD=other.HEAD;
					TAIL=other.TAIL;
					this->set_null_sides();
					other.reset();					
				}
				else{
					HEAD=null.address();
					TAIL=null.address();
				}
			}
			List(List&& other,const Alloc& al):EBCO<AlNode>(al),length(other.length){
				this->HEAD=this->null.address();
				this->TAIL=this->null.address();
				if(this->get_val()==other.get_val()){
					if(other.length){//			it will break if I don't check if other is empty
						this->HEAD=other.HEAD;
						this->TAIL=other.TAIL;
						this->set_null_sides();
						other.reset();						
					}
				}
				else{
					other.length=0;
					other.TAIL=other.null.address();
					while(other.HEAD!=other.null.address()){
						this->link_back(create_node(static_cast<value_type&&>(other.HEAD->data)));
						NodePtr aux=other.HEAD;
						other.HEAD=other.HEAD->next;
						other.delete_node(aux);
					}
				}
			}
			List(size_type length,const Alloc& al=Alloc()):EBCO<AlNode>(al),length(length){
				this->HEAD=this->null.address();
				this->TAIL=this->null.address();
				while(length--)
					this->link_back(create_node());
			}
			List(size_type length,const_reference fillwith,const Alloc& al=Alloc()):EBCO<AlNode>(al),length(length){
				this->HEAD=this->null.address();
				this->TAIL=this->null.address();
				while(length--)
					this->link_back(create_node(fillwith));
			}
			List(std::initializer_list<value_type> initializerList):length(initializerList.size()){
				this->HEAD=this->null.address();
				this->TAIL=this->null.address();
				const_pointer p=initializerList.begin();
				while(p!=initializerList.end())
					this->link_back(create_node(*(p++)));
			}
			List(std::initializer_list<value_type> initializerList,const Alloc& al):EBCO<AlNode>(al),length(initializerList.size()){
				this->HEAD=this->null.address();
				this->TAIL=this->null.address();
				const_pointer p=initializerList.begin();
				while(p!=initializerList.end())
					this->link_back(create_node(*(p++)));
			}
			template<typename Iterable,enable_if_t<!is_same_v<remove_cvref_t<Iterable>,List> && is_iterable_v<Iterable>>* = nullptr>
			List(Iterable&& lis){
				this->HEAD=this->null.address();
				this->TAIL=this->null.address();
				auto p=lis.begin();
				while(p!=lis.end()){
					length++;
					if constexpr(elements_are_movable_v<Iterable&&>)
						this->link_back(create_node(static_cast<typename Iterable::value_type&&>(*(p++))));
					else
						this->link_back(create_node(*(p++)));
				}
			}
			template<typename Iterable,enable_if_t<!is_same_v<remove_cvref_t<Iterable>,List> && is_iterable_v<Iterable>>* = nullptr>
			List(const Iterable& lis,const Alloc& al):EBCO<AlNode>(al){
				this->HEAD=this->null.address();
				this->TAIL=this->null.address();
				auto p=lis.begin();
				while(p!=lis.end()){
					length++;
					if constexpr(elements_are_movable_v<Iterable&&>)
						this->link_back(create_node(static_cast<typename Iterable::value_type&&>(*(p++))));
					else
						this->link_back(create_node(*(p++)));
				}
			}
			List& operator=(const List& other){										//Operador de copia(optimizado)
				if(this!=&other){
					if constexpr(!ALWAYS_EQ && POCCA){
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
				if(this==&other) return *this;//should be ub anyway
				if constexpr(!ALWAYS_EQ && !POCMA){
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
				if constexpr(!ALWAYS_EQ){
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
				if constexpr(!ALWAYS_EQ){
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
				if constexpr(!ALWAYS_EQ){
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
				while(p!=initializerList.end())
					this->link_back(create_node(*(p++)));
				this->length+=initializerList.size();
				return *this;
			}
			List operator*(size_type num)const{
				List new_list;
				for(size_type i=0;i<num;i++)
					new_list+=*this;
				return new_list;
			}
			List& operator*=(size_type num){
				if(length==0)
					return *this;
				else if(num==0){
					clear();
					return *this;
				}
				else{
					for(size_type i=1;i<num;i++){
						NodePtr p=HEAD;
						for(size_type j=0;j<length;j++){
							link_back(create_node(p->data));
							p=p->next;
						}
					}
					length*=num;
					return *this;					
				}
			}
			const_reference operator[](size_type index)const{
				NodePtr current=this->HEAD;
				for(size_type i=0;i<index;i++)
					current=current->next;
				return current->data;
			}
			reference operator[](size_type index){
				NodePtr current=this->HEAD;
				for(size_type i=0;i<index;i++)
					current=current->next;
				return current->data;
			}
			const_reference back()const{
				return TAIL->data;
			}
			reference back(){
				return TAIL->data;
			}
			bool operator==(const List& other)const{
				if(this->length!=other.length)
					return false;
				for(NodePtr i=this->HEAD,j=other.HEAD;i!=this->null.address();i=i->next,j=j->next){
					if(i->data!=j->data)
						return false;
				}
				return true;
			}
			bool operator!=(const List& other)const{
				if(this->length!=other.length)
					return true;
				for(NodePtr i=this->HEAD,j=other.HEAD;i!=this->null.address();i=i->next,j=j->next){
					if(i->data!=j->data)
						return true;
				}
				return false;
			}
			template<typename... Args>
			void emplace_back(Args&&... args){
				link_back(create_node(static_cast<Args&&>(args)...));
				length++;
			}
			void append(const_reference dat){
				link_back(create_node(dat));
				length++;
			}
			void append(value_type&& dat){
				link_back(create_node(static_cast<value_type&&>(dat)));
				length++;
			}
			template<typename... Args>
			void emplace(size_type index,Args&&... args){
				link_at(index,create_node(static_cast<Args&&>(args)...));
				length++;
			}
			void insert(size_type index,const_reference dat){
				link_at(index,create_node(dat));
				length++;
			}
			void insert(size_type index,value_type&& dat){
				link_at(index,create_node(dat));
				length++;
			}
			value_type pop(size_type index){
				length--;
				NodePtr p=unlink_at(index);
				value_type aux=static_cast<value_type&&>(p->data);
				delete_node(p);
				return aux;
			}
			void del(size_type index){
				length--;
				delete_node(unlink_at(index));
			}
			value_type pop_back(){
				length--;
				NodePtr p=unlink_back();
				value_type aux=static_cast<value_type&&>(p->data);
				delete_node(p);
				return aux;
			}
			void del_back(){
				length--;
				delete_node(unlink_back());
			}
		private:
			struct AuxList{
				BaseNode null;
				AuxList(){
					this->HEAD=this->null.address();
					this->TAIL=this->null.address();
				}
				AuxList(const List& list,AlNode& alloc){
					this->HEAD=this->null.address();
					this->TAIL=this->null.address();
					NodePtr p=list.HEAD;
					while(p!=list.null.address()){
						NodePtr new_node=AlNode_traits::allocate(alloc,1);
						CONSTRUCT(alloc,new_node,p->data);
						this->TAIL->link_right(new_node);
						p=p->next;
					}
				}
				AuxList(List&& list,AlNode& alloc){
					this->HEAD=this->null.address();
					this->TAIL=this->null.address();
					list.TAIL=list.null.address();
					while(list.HEAD!=list.null.address()){
						NodePtr node=AlNode_traits::allocate(alloc,1),aux=list.HEAD;
						CONSTRUCT(alloc,node,static_cast<value_type&&>(aux->data));
						this->TAIL->link_right(node);
						list.HEAD=list.HEAD->next;
						list.delete_node(aux);
					}
					list.TAIL=list.null.address();
					list.length=0;
				}
				void snip_off(List& list,size_type from,size_type to){
					if(from!=to){
						list.length-=to-from;
						this->HEAD=list.HEAD;
						this->TAIL=list.HEAD->prev;
						while(from--) this->HEAD=this->HEAD->next;
						while(to--) this->TAIL=this->TAIL->next;
						this->HEAD->prev->next=this->TAIL->next;
						this->TAIL->next->prev=this->HEAD->prev;
						this->HEAD->prev=this->null.address();
						this->TAIL->next=this->null.address();
					}
				}
			};

			void move_at(size_type pos,AuxList& other){//Need to increment this->length manually after calling this function
				NodePtr p=this->HEAD;
				while(pos--) p=p->next;
				p->prev->next=other.HEAD;
				other.HEAD->prev=p->prev;
				p->prev=other.TAIL;
				other.TAIL->next=p;
			}
			void move_at(size_type pos,List& other){//Need to increment this->length manually after calling this function
				NodePtr p=this->HEAD;
				while(pos--) p=p->next;
				p->prev->next=other.HEAD;
				other.HEAD->prev=p->prev;
				p->prev=other.TAIL;
				other.TAIL->next=p;
				other.reset();
			}
			void copy_range(const List& other,size_type from,size_type to){
				NodePtr start=other.HEAD, end=other.HEAD;
				this->length=to-from;
				while(from--) start=start->next;
				while(to--) end=end->next;
				while(start!=end){
					this->link_back(this->create_node(start->data));
					start=start->next;
				}
			}
			void move_range_nodebynode(List& other,size_type from,size_type to){
				AuxList auxlist;
				auxlist.snip_off(other,from,to);
				this->length=to-from;
				while(auxlist.HEAD!=auxlist.null.address()){
					this->link_back(this->create_node(static_cast<value_type&&>(auxlist.HEAD->data)));
					NodePtr aux=auxlist.HEAD;
					auxlist.HEAD=auxlist.HEAD->next;
					other.delete_node(aux);
				}
			}
			void move_range(List& other,size_type from,size_type to){
				AuxList auxlist;
				auxlist.snip_off(other,from,to);
				this->length=to-from;
				if(auxlist.HEAD!=auxlist.null.address()){
					this->HEAD=auxlist.HEAD;
					this->TAIL=auxlist.TAIL;
					this->HEAD->prev=this->null.address();
					this->TAIL->next=this->null.address();					
				}
			}
			void cut_range(List& other,size_type from,size_type to){
				AuxList auxlist;
				auxlist.snip_off(other,from,to);
				this->length=to-from;
				if(auxlist.HEAD!=auxlist.null.address()){
					this->HEAD=auxlist.HEAD;
					this->TAIL=auxlist.TAIL;
					this->HEAD->prev=this->null.address();
					this->TAIL->next=this->null.address();					
				}
			}
		public:
			List concat(size_type index,const List& other)const&{
				List new_list;
				new_list.copy_elements(*this);
				new_list.length+=other.length;
				AuxList auxlist(other,new_list.get_val());
				new_list.move_at(index,auxlist);
				return new_list;
			}
			List concat(size_type index,List&& other)const&{
				List new_list;
				new_list.copy_elements(*this);
				new_list.length+=other.length;
				if constexpr(!ALWAYS_EQ){
					if(new_list.get_val()!=other.get_val()){
						AuxList auxlist(static_cast<List&&>(other),new_list.get_val());
						new_list.move_at(index,auxlist);
						return new_list;
					}
				}
				new_list.move_at(index,other);
				return new_list;
			}
			List concat(size_type index,const List& other)&&{
				List new_list;
				if constexpr(!ALWAYS_EQ){
					if(new_list.get_val()!=this->get_val()){
						new_list.copy_elements(static_cast<List&&>(*this));
					}
					else new_list.move_elements(*this);
				}
				else new_list.move_elements(*this);

				new_list.length+=other.length;
				AuxList auxlist(other,new_list.get_val());
				new_list.move_at(index,auxlist);
				return new_list;
			}
			List concat(size_type index,List&& other)&&{
				List new_list;
				if constexpr(!ALWAYS_EQ){
					if(new_list.get_val()!=this->get_val()){
						new_list.copy_elements(static_cast<List&&>(*this));
					}
					else new_list.move_elements(*this);
				}
				else new_list.move_elements(*this);

				new_list.length+=other.length;
				if constexpr(!ALWAYS_EQ){
					if(new_list.get_val()!=other.get_val()){
						AuxList auxlist(static_cast<List&&>(other),new_list.get_val());
						new_list.move_at(index,auxlist);
						return new_list;
					}
				}
				new_list.move_at(index,other);
				return new_list;
			}
			void join(size_type index,const List& other){
				this->length+=other.length;
				AuxList auxlist(other,this->get_val());
				this->move_at(index,auxlist);
			}
			void join(size_type index,List&& other){
				this->length+=other.length;
				if constexpr(!ALWAYS_EQ){
					if(this->get_val()!=other.get_val()){
						AuxList auxlist(static_cast<List&&>(other),this->get_val());
						this->move_at(index,auxlist);
						return;
					}
				}
				this->move_at(index,other);
			}
			List sublist(size_type start, size_type end)const&{
				List new_list;
				new_list.copy_range(*this,start,end);
				return new_list;
			}
			List sublist(size_type start)const&{
				List new_list;
				new_list.copy_range(*this,start,this->length);
				return new_list;
			}
			List sublist(size_type start, size_type end)&&{
				List new_list;
				if constexpr(!ALWAYS_EQ){
					if(this->get_val()!=new_list.get_val()){
						new_list.move_range_nodebynode(*this,start,end);
					//	this->clear();//not necessary to clear this because using a variable after moving it is undefined behavior
						return new_list;
					}
				}
				new_list.move_range(*this,start,end);
				//this->clear();
				return new_list;
			}
			List sublist(size_type start)&&{
				List new_list;
				if constexpr(!ALWAYS_EQ){
					if(this->get_val()!=new_list.get_val()){
						new_list.move_range_nodebynode(*this,start,this->length);
					//	this->clear();
						return new_list;
					}
				}
				new_list.move_range(*this,start,this->length);
				//this->clear()
				return new_list;
			}
			void wipe(size_type start,size_type end){
				AuxList auxlist;
				auxlist.snip_off(*this,start,end);
				while(auxlist.HEAD!=auxlist.null.address()){
					NodePtr aux=auxlist.HEAD;
					auxlist.HEAD=auxlist.HEAD->next;
					delete_node(aux);
				}
			}
			void wipe(size_type start){
				this->length=start;
				NodePtr p=this->HEAD;
				while(start--) p=p->next;
				this->TAIL=p->prev;
				this->TAIL->next=this->null.address();
				while(p!=this->null.address()){
					NodePtr aux=p;
					p=p->next;
					delete_node(aux);
				}
			}
			List cut(size_type start,size_type end){
				List new_list;
				if constexpr(!ALWAYS_EQ){
					if(this->get_val()!=new_list.get_val()){
						new_list.move_range_nodebynode(*this,start,end);
						return new_list;
					}
				}
				new_list.move_range(*this,start,end);
				return new_list;
			}
			List cut(size_type start){
				List new_list;
				if constexpr(!ALWAYS_EQ){
					if(this->get_val()!=new_list.get_val()){
						new_list.move_range_nodebynode(*this,start,this->length);
						return new_list;
					}
				}
				new_list.move_range(*this,start,this->length);
				return new_list;
			}
			size_type size()const{
				#if defined(DEBUG) && (defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_))
				size_type node_count=0,node_count_reverse=0;
				for(NodePtr current=this->HEAD;current!=this->null.address();current=current->next){
					node_count++;
				}
				for(NodePtr current=this->TAIL;current!=this->null.address();current=current->prev){
					node_count_reverse++;
				}
				if(node_count!=node_count_reverse){
					std::cerr<<"\nNODE_COUNT AND NODE_COUNT_REVERSE DO NOT MATCH\a\n"
							<<"NODE_COUNT: "<<node_count<<"\nNODE_COUNT_REVERSE: "<<node_count_reverse<<std::endl;
					exit(-1);
				}
				if(node_count!=length){
					std::cerr<<"\nSIZE AND NODE_COUNT DO NOT MATCH\a\n"
							<<"LEN: "<<length<<"\nNODE_COUNT: "<<node_count<<std::endl;
					exit(-1);
				}
				#endif
				return length;
			}
			void resize(size_type new_size){
				while(this->length<new_size){
					link_back(create_node());
					length++;
				}
				while(this->length>new_size){
					delete_node(unlink_back());
					length--;
				}
			}
			template<typename D>
 			Optional<size_type> find(const D& data,size_type start,size_type end)const{	//retorna -1 si no se encontro el elemento
				if(start>=end) return {};
				NodePtr ptr_start=this->HEAD, ptr_end=this->HEAD;
				const size_type savedstart=start;
				while(start--) ptr_start=ptr_start->next;
				while(end--) ptr_end=ptr_end->next;
				for(size_type i=0;ptr_start!=ptr_end;ptr_start=ptr_start->next,i++){
					if(ptr_start->data==data)
						return i+savedstart;
				}
				return {};
			}
			template<typename D>
 			Optional<size_type> find(const D& data,size_type start=0)const{	//retorna -1 si no se encontro el elemento
				if(start>=length) return {};
				NodePtr ptr_start=this->HEAD;
				const size_type savedstart=start;
				while(start--) ptr_start=ptr_start->next;
				for(size_type i=0;ptr_start!=null.address();ptr_start=ptr_start->next,i++){
					if(ptr_start->data==data)
						return i+savedstart;
				}
				return {};
			}
			template<typename D>
			size_type count(const D& dat)const{
				size_type counter=0;
				NodePtr current=this->HEAD;
				while(current!=null.address()){
					if(current->data==dat)
						counter++;
					current=current->next;
				}
				return counter;
			}
			template<typename D>
			size_type remove_all(const D& dat){	//devuelve la cantidad removida
				NodePtr aux, current=this->HEAD;
				size_type found=0;
				while(current!=null.address()){
					if(current->data==dat){
						aux=current;
						current=current->next;
						aux->unlink();
						delete_node(aux);
						this->length--;
						found++;
					}
					else
						current=current->next;
				}
				return found;
			}
			template<typename D>
			size_type remove(const D& dat){	//devuelve la cantidad removida
				NodePtr current=this->HEAD;
				while(current!=null.address()){
					if(current->data==dat){
						current->unlink();
						delete_node(current);
						this->length--;
						return true;
					}
					else
						current=current->next;
				}
				return false;
			}
			template<typename D>
			bool has(const D& dat)const{
				for(NodePtr i=this->HEAD;i!=null.address();i=i->next){
					if(i->data==dat)
						return true;
				}
				return false;
			}
			bool empty()const{
				return length==0;
			}
			void sort(){
				value_type aux;
				for(NodePtr i=HEAD->next, j;i!=null.address();i=i->next){
					for(j=i,aux=static_cast<value_type&&>(i->data);j!=HEAD && aux<j->prev->data;j=j->prev){
						j->data=static_cast<value_type&&>(j->prev->data);
					}
					j->data=static_cast<value_type&&>(aux);
				}
			}
			void sort_reverse(){
				value_type aux;
				for(NodePtr i=HEAD->next, j;i!=null.address();i=i->next){
					for(j=i,aux=static_cast<value_type&&>(i->data);j!=HEAD && aux>j->prev->data;j=j->prev){
						j->data=static_cast<value_type&&>(j->prev->data);
					}
					j->data=static_cast<value_type&&>(aux);
				}
			}
			template<typename D>
			void sort(D(*func)(const_reference)){
				value_type aux;
				for(NodePtr i=HEAD->next, j;i!=null.address();i=i->next){
					for(j=i,aux=static_cast<value_type&&>(i->data);j!=HEAD && func(aux)<func(j->prev->data);j=j->prev){
						j->data=static_cast<value_type&&>(j->prev->data);
					}
					j->data=static_cast<value_type&&>(aux);
				}
			}
			template<typename Cmp>
			void sort(Cmp&& cmp=Cmp{}){
				value_type aux;
				for(NodePtr i=HEAD->next, j;i!=null.address();i=i->next){
					for(j=i,aux=static_cast<value_type&&>(i->data);j!=HEAD && cmp(aux,j->prev->data);j=j->prev){
						j->data=static_cast<value_type&&>(j->prev->data);
					}
					j->data=static_cast<value_type&&>(aux);
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
			template<typename Lambda>
			auto map(Lambda&& func=Lambda{})const{
				List<decltype(func(HEAD->data)),rebind_alloc<Alloc,decltype(func(HEAD->data))>> new_list;
				for(const_reference val:*this)
					new_list.append(func(val));
				return new_list;
			}
			template<typename Lambda>
			List filter(Lambda&& func=Lambda{})const{
				List new_list;
				for(const_reference val:*this)
					if(func(val)) new_list.append(val);
				return new_list;
			}
			void reverse(){
				value_type aux;
				for(NodePtr i=this->HEAD, j=this->TAIL;i!=j && i->prev!=j;i=i->next,j=j->prev){
					aux=static_cast<value_type&&>(i->data);
					i->data=static_cast<value_type&&>(j->data);
					j->data=static_cast<value_type&&>(aux);
				}
			}
			inline void clear(){
				NodePtr aux;
				length=0;
				TAIL=null.address();
				while(HEAD!=null.address()){
					aux=HEAD;
					HEAD=HEAD->next;
					delete_node(aux);
				}
			}
			~List(){
				NodePtr aux;
				while(HEAD!=null.address()){
					aux=HEAD;
					HEAD=HEAD->next;
					delete_node(aux);
				}
			}
			iterator begin(){
				return iterator(HEAD);
			}
			iterator end(){
				return iterator(null.address());
			}
			iterator rbegin(){
				return iterator(TAIL);
			}
			iterator rend(){
				return iterator(null.address());
			}
			const_iterator begin()const{
				return const_iterator(HEAD);
			}
			const_iterator end()const{
				return const_iterator(null.address());
			}
			const_iterator rbegin()const{
				return const_iterator(TAIL);
			}
			const_iterator rend()const{
				return const_iterator(null.address());
			}
			template<typename t,typename alloc>friend void debug(const List<t,alloc>&);
	};
	template<typename Out,typename T,typename Alloc>
	Out& reverse_print(Out& stream,const List<T,Alloc>& list){
		stream<<'[';
		typename List<T,Alloc>::const_iterator i=list.end();
		i--;
		for(;i!=list.rend();i--){
			stream<<*i;
			if(i!=list.begin())
				stream<<", ";		
		}
		return stream<<']';
	}
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
        std::cout<<"length: "<<list.size()<<std::endl
                 <<"head: "<<list.HEAD<<std::endl
                 <<"tail: "<<list.TAIL<<std::endl;
    }
#endif
}