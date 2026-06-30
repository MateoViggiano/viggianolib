#pragma once
namespace mpv{
    enum RedOrBlack:bool{RED,BLACK};
    template<typename T,typename VoidPtr>struct Tree_Node;
    template<typename T,typename VoidPtr>
    struct Base_Tree_Node{
        using BaseNodePtr=rebind_pointer<VoidPtr,Base_Tree_Node>;
        using NodePtr=rebind_pointer<VoidPtr,Tree_Node<T,VoidPtr>>;
        using const_NodePtr=rebind_pointer<VoidPtr,const Tree_Node<T,VoidPtr>>;
        NodePtr prev=nullptr;
        NodePtr left=nullptr;
        NodePtr right=nullptr;
        RedOrBlack color=BLACK;
        constexpr NodePtr address()noexcept{
            return NodePtr(reinterpret_cast<Tree_Node<T,VoidPtr>*>(this));
        }
        constexpr const_NodePtr address()const noexcept{
            return const_NodePtr(reinterpret_cast<const Tree_Node<T,VoidPtr>*>(this));
        }
        constexpr Base_Tree_Node()noexcept=default;
        Base_Tree_Node(const Base_Tree_Node&)=delete;
        Base_Tree_Node& operator=(const Base_Tree_Node&)=delete;
        constexpr Base_Tree_Node(RedOrBlack color)noexcept:color(color){}
    };
    template<typename T,typename VoidPtr>
    struct Temp_Tree_Node:public Base_Tree_Node<T,VoidPtr>{
        using NodePtr=typename Base_Tree_Node<T,VoidPtr>::NodePtr;
        NodePtr replace_with=nullptr;
        constexpr Temp_Tree_Node()noexcept:Base_Tree_Node<T,VoidPtr>(BLACK){}
        constexpr void link_to_right_of(NodePtr node)noexcept{
            replace_with=node->right;// node->right may be the sentinel(node would be right_most in this case) so we need to link it back 
            node->right=static_cast<NodePtr>(static_cast<typename Base_Tree_Node<T,VoidPtr>::BaseNodePtr>(this));
            this->prev=node;
        }
        constexpr void link_to_left_of(NodePtr node)noexcept{
            replace_with=node->left;// node->left may be the sentinel(node would be left_most in this case) so we need to link it back 
            node->left=static_cast<NodePtr>(static_cast<typename Base_Tree_Node<T,VoidPtr>::BaseNodePtr>(this));
            this->prev=node;
        }
        ~Temp_Tree_Node()noexcept{
            if(this->prev!=nullptr){
                if(this->prev->right==static_cast<typename Base_Tree_Node<T,VoidPtr>::NodePtr>(static_cast<typename Base_Tree_Node<T,VoidPtr>::BaseNodePtr>(this)))
                    this->prev->right=replace_with;
                else
                    this->prev->left=replace_with;
            }   
        }
    };
    template<typename T,typename VoidPtr>
    struct Tree_Node:public Base_Tree_Node<T,VoidPtr>{
        using NodePtr=typename Base_Tree_Node<T,VoidPtr>::NodePtr;
        using const_NodePtr=typename Base_Tree_Node<T,VoidPtr>::const_NodePtr;
        T data;
        template<typename... Args>
        constexpr Tree_Node(Args&&... args)noexcept(is_nothrow_constructible_v<T,Args...>):Base_Tree_Node<T,VoidPtr>(RED),data(static_cast<Args&&>(args)...){}
        Tree_Node(const Tree_Node&)=delete;
        Tree_Node& operator=(const Tree_Node&)=delete;
        constexpr NodePtr maximum()noexcept{
            NodePtr p=this;
            while(p->right)
                p=p->right;
            return p;
        }
        constexpr NodePtr minimum()noexcept{
            NodePtr p=this;
            while(p->left)
                p=p->left;
            return p;
        }
        constexpr const_NodePtr maximum()const noexcept{
            const_NodePtr p=this;
            while(p->right)
                p=p->right;
            return p;
        }
        constexpr const_NodePtr minimum()const noexcept{
            const_NodePtr p=this;
            while(p->left)
                p=p->left;
            return p;
        }
    };
    template<typename Types>
    class Tree_iterator{//in order
		public:
            using iterator_category=bidirectional_iterator_tag;
			using NodePtr=typename Types::NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=typename Types::reference;
        private:
            NodePtr ptr;
        public:
            constexpr Tree_iterator(NodePtr ptr)noexcept:ptr(ptr){}
            constexpr reference operator*()const noexcept{
                return ptr->data;
            }
            constexpr pointer operator->()const noexcept{
                return pointer_traits<pointer>::pointer_to(ptr->data);
            }
            constexpr Tree_iterator& operator++()noexcept{
                if(ptr->right!=nullptr){
                    if(ptr==ptr->right->left) ptr=ptr->right;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->right->minimum();
                }
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return *this;
            }
            constexpr Tree_iterator operator++(int)noexcept{
                Tree_iterator aux=*this;
                if(ptr->right!=nullptr){
                    if(ptr==ptr->right->left) ptr=ptr->right;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->right->minimum();
                }
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return aux;
            }
            constexpr Tree_iterator& operator--()noexcept{
                if(ptr->left!=nullptr){
                    if(ptr==ptr->left->right) ptr=ptr->left;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->left->maximum();
                }
                else if(ptr==ptr->prev->right)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return *this;
            }
            constexpr Tree_iterator operator--(int)noexcept{
                Tree_iterator aux=*this;
                if(ptr->left!=nullptr){
                    if(ptr==ptr->left->right) ptr=ptr->left;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->left->maximum();
                }
                else if(ptr==ptr->prev->right)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return aux;
            }
            constexpr bool operator==(const Tree_iterator& other)const noexcept{
                return this->ptr==other.ptr;
            }
            constexpr bool operator!=(const Tree_iterator& other)const noexcept{
                return this->ptr!=other.ptr;
            }
            template<typename> friend class const_Tree_iterator;
            template<typename> friend class Tree;
    };
    template<typename Types>
    class const_Tree_iterator{//in order
		public:
            using iterator_category=bidirectional_iterator_tag;
			using NodePtr=typename Types::const_NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
        private:
            NodePtr ptr;
        public:
            constexpr const_Tree_iterator(NodePtr ptr)noexcept:ptr(ptr){}
            constexpr const_Tree_iterator(Tree_iterator<Types> nonconst_it)noexcept:ptr(nonconst_it.ptr){}
            constexpr reference operator*()const noexcept{
                return ptr->data;
            }
            constexpr pointer operator->()const noexcept{
                return pointer_traits<pointer>::pointer_to(ptr->data);
            }
            constexpr const_Tree_iterator& operator++()noexcept{
                if(ptr->right!=nullptr){
                    if(ptr==ptr->right->left) ptr=ptr->right;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->right->minimum();
                }
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return *this;
            }
            constexpr const_Tree_iterator operator++(int)noexcept{
                const_Tree_iterator aux=*this;
                if(ptr->right!=nullptr){
                    if(ptr==ptr->right->left) ptr=ptr->right;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->right->minimum();
                }
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return aux;
            }
            constexpr const_Tree_iterator& operator--()noexcept{
                if(ptr->left!=nullptr){
                    if(ptr==ptr->left->right) ptr=ptr->left;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->left->maximum();
                }
                else if(ptr==ptr->prev->right)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return *this;
            }
            constexpr const_Tree_iterator operator--(int)noexcept{
                const_Tree_iterator aux=*this;
                if(ptr->left!=nullptr){
                    if(ptr==ptr->left->right) ptr=ptr->left;//Esto maneja los casos en los que estoy parado en end() o en el anterior a end()
                    else ptr=ptr->left->maximum();
                }
                else if(ptr==ptr->prev->right)
                    ptr=ptr->prev;
                else{
                    while(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    ptr=ptr->prev;
                }
                return aux;
            }
            constexpr bool operator==(const const_Tree_iterator& other)const noexcept{
                return this->ptr==other.ptr;
            }
            constexpr bool operator!=(const const_Tree_iterator& other)const noexcept{
                return this->ptr!=other.ptr;
            }
            template<typename> friend class const_Tree_iterator;
            template<typename> friend class Tree;
    };
    template<typename Traits>
    class Tree COUNT_IT{
        public:
            using value_type=typename Traits::value_type;
            using key_type=typename Traits::key_type;
            using allocator_type=typename Traits::allocator_type;
            using key_compare=typename Traits::key_compare;
            using value_compare=typename Traits::value_compare;
		protected:
            using Alloc=allocator_type;
			using Node=Tree_Node<value_type,typename allocator_traits<Alloc>::void_pointer>;
            using TempNode=Temp_Tree_Node<value_type,typename allocator_traits<Alloc>::void_pointer>;
            using BaseNode=Base_Tree_Node<value_type,typename allocator_traits<Alloc>::void_pointer>;
			using NodePtr=typename Node::NodePtr;
            using const_NodePtr=typename Node::const_NodePtr;
			using AlTy=rebind_alloc<Alloc,value_type>;
			using AlTy_traits=allocator_traits<AlTy>;
			using AlNode=rebind_alloc<Alloc,Node>;
			using AlNode_traits=allocator_traits<AlNode>;
			struct Val_types{
				using value_type=typename Traits::value_type;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=value_type&;
				using const_reference=const value_type&;
				using NodePtr=typename Node::NodePtr;
                using const_NodePtr=typename Node::const_NodePtr;
			};
            static constexpr bool allow_repeated=Traits::allow_repeated;
            static constexpr bool POCCA=AlTy_traits::propagate_on_container_copy_assignment::value;
            static constexpr bool POCMA=AlTy_traits::propagate_on_container_move_assignment::value;
            static constexpr bool POCS=AlTy_traits::propagate_on_container_swap::value;
            static constexpr bool ALWAYS_EQ=AlTy_traits::is_always_equal::value;
		public:
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
            using const_iterator=const_Tree_iterator<Val_types>;
            using iterator=If_t<is_same_v<key_type,value_type>,const_iterator,Tree_iterator<Val_types>>;
            
		protected:
            CompressedTriple<key_compare,AlNode,BaseNode> ct;
            size_type length=0;
#define alloc       ct.getV2()
#define root        ct.getV3().prev
#define left_most   ct.getV3().right
#define right_most  ct.getV3().left
#define sent        ct.getV3().address()
            constexpr static bool is_red(NodePtr p)noexcept{
                return p!=nullptr && p->color==RED;
            }
            constexpr static bool is_black(NodePtr p)noexcept{
                return p==nullptr || p->color==BLACK;
            }
            constexpr bool is_null(NodePtr p)const{
                return p==nullptr || p==sent;
            }
            constexpr void reset()noexcept{
                length=0;
                root=left_most=right_most=sent;
            }
            constexpr bool cmp(const key_type& a,const key_type& b)const{
            	return ct.getV1()(a,b);
			}
			template<typename... Args>
			constexpr NodePtr create_node(Args&&... args){
                AllocConstructPtr guard(alloc);
                guard.allocate();
				CONSTRUCT_VARARGS(this->alloc,guard.ptr,static_cast<Args&&>(args));
				return guard.release();
			}
			constexpr void delete_node(NodePtr node)noexcept{
				DESTROY(this->alloc,node);
				AlNode_traits::deallocate(this->alloc,node,1);
			}
            constexpr void clr(NodePtr node)noexcept{
                if(is_null(node)) return;
                else{
                    clr(node->left);
                    clr(node->right);
                    delete_node(node);
                }
            }
            template<typename Lambda>
            constexpr void pre_order_helper(NodePtr node,Lambda& func){
                if(is_null(node)) return;
                func(node->data);
                pre_order_helper(node->left,func);
                pre_order_helper(node->right,func);
            }
            template<typename Lambda>
            constexpr void in_order_helper(NodePtr node,Lambda& func){
                if(is_null(node)) return;
                in_order_helper(node->left,func);
                func(node->data);
                in_order_helper(node->right,func);
            }
            template<typename Lambda>
            constexpr void post_order_helper(NodePtr node,Lambda& func){
                if(is_null(node)) return;
                post_order_helper(node->left,func);
                post_order_helper(node->right,func);
                func(node->data);
            }
            constexpr void insert_fixup(NodePtr x)noexcept{
                while(is_red(x->prev)){
                    if(x->prev==x->prev->prev->left){															//If the father is a left node
                        if(is_red(x->prev->prev->right)){			//case 1:X.uncle==RED
                            x->prev->color=BLACK;
                            x->prev->prev->right->color=BLACK;
                            x->prev->prev->color=RED;
                            x=x->prev->prev;
                        }
                        else/*(x->prev->prev->right->color==BLACK)*/{//case 2 & 3:x.uncle==BLACK
                            if(x==x->prev->right){							//case 2:(triangle)
                                x=x->prev;
                                left_rotate(x);
                            }
                            x->prev->color=BLACK;							//case 3:(line)
                            x->prev->prev->color=RED;
                            right_rotate(x->prev->prev);
                        }
                    }
                    else{																						//If the father is a right node
                        if(is_red(x->prev->prev->left)){					//case 1:X.uncle==RED
                            x->prev->color=BLACK;
                            x->prev->prev->left->color=BLACK;
                            x->prev->prev->color=RED;
                            x=x->prev->prev;
                        }
                        else/*(x->prev->prev->left->color==BLACK)*/{			//case 2 & 3:x.uncle==BLACK
                            if(x==x->prev->left){							//case 2:(triangle)
                                x=x->prev;
                                right_rotate(x);    // Case 2 always leads to case 3
                            }
                            x->prev->color=BLACK;							//case 3:(line)
                            x->prev->prev->color=RED;
                            left_rotate(x->prev->prev);
                        }
                    }
                }
                this->root->color=BLACK;									//case 0:root==RED
            }
            constexpr void delete_fixup(NodePtr x)noexcept{
                while(x!=root && x->color==BLACK){
                    if(x==x->prev->left){
                        NodePtr w=x->prev->right;
                        if(is_red(w)){
                            w->color=BLACK;
                            x->prev->color=RED;
                            left_rotate(x->prev);
                            w=x->prev->right;
                        }
                        if(is_black(w->left) && is_black(w->right)){
                            w->color=RED;
                            x=x->prev;
                        }
                        else{
                            if(is_black(w->right)){
                                w->left->color=BLACK;
                                w->color=RED;
                                right_rotate(w);
                                w=x->prev->right;
                            }
                            w->color=x->prev->color;
                            x->prev->color=BLACK;
                            w->right->color=BLACK;
                            left_rotate(x->prev);
                            x=root;
                        }
                    }
                    else{
                        NodePtr w=x->prev->left;
                        if(is_red(w)){
                            w->color=BLACK;
                            x->prev->color=RED;
                            right_rotate(x->prev);
                            w=x->prev->left;
                        }
                        if(is_black(w->left) && is_black(w->right)){
                            w->color=RED;
                            x=x->prev;
                        }
                        else{
                            if(is_black(w->left)){
                                w->right->color=BLACK;
                                w->color=RED;
                                left_rotate(w);
                                w=x->prev->left;
                            }
                            w->color=x->prev->color;
                            x->prev->color=BLACK;
                            w->left->color=BLACK;
                            right_rotate(x->prev);
                            x=root;
                        }
                    }
                }
                x->color=BLACK;
            }
            constexpr void right_rotate(NodePtr x)noexcept{
                NodePtr y=x->left;
                if(x->prev==nullptr)
                    this->root=y;
                else{
                    if(x==x->prev->right)
                        x->prev->right=y;
                    else/*(x==x->prev->left)*/
                        x->prev->left=y;
                }
                y->prev=x->prev;
                x->left=y->right;
                //assert(x->left!=sent);
                if(x->left!=nullptr)
                    x->left->prev=x;
                y->right=x;
                x->prev=y;
            }
            constexpr void left_rotate(NodePtr x)noexcept{
                NodePtr y=x->right;
                if(x->prev==nullptr)
                    this->root=y;	
                else{
                    if(x==x->prev->left)
                        x->prev->left=y;
                    else/*(x==x->prev->right)*/
                        x->prev->right=y;
                }
                y->prev=x->prev;
                x->right=y->left;
                //assert(x->right!=sent);
                if(x->right!=nullptr)
                    x->right->prev=x;
                y->left=x;
                x->prev=y;
            }
            constexpr void transplant(NodePtr u,NodePtr v)noexcept{//Desenlaza u con su padre y enlaza al padre con v
                if(u->prev==nullptr)
                    this->root=v;
                else if(u==u->prev->left)
                    u->prev->left=v;
                else
                    u->prev->right=v;
                if(v!=nullptr)
                    v->prev=u->prev;
            }
            template<typename Lambda>
            constexpr void iterative_in_order(Lambda&& func=Lambda{}){
                if(root==sent) return;
                NodePtr ptr=left_most;
                while(true){
                    func(ptr->data);
                    if(ptr->right!=nullptr){
                        if(ptr==ptr->right->left) return;
                        else ptr=ptr->right->minimum();
                    }
                    else if(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    else{
                        while(ptr==ptr->prev->right)
                            ptr=ptr->prev;
                        ptr=ptr->prev;
                    }
                }
            }
            constexpr void iterative_deletion()noexcept{
                if(root==sent) return;
                NodePtr ptr=left_most,aux;
                while(true){
                    if(ptr->right!=nullptr){
                        if(ptr->right==sent){
                            while(ptr!=root){
                                aux=ptr;
                                ptr=ptr->prev;
                                delete_node(aux);
                            }
                            delete_node(ptr);
                            return;
                        }
                        else{
                            ptr=ptr->right->minimum();
                        } 
                    }
                    else if(ptr==ptr->prev->left){
                        aux=ptr;
                        ptr=ptr->prev;
                        delete_node(aux);
                    }
                    else{
                        while(ptr==ptr->prev->right){
                            aux=ptr;
                            ptr=ptr->prev;
                            delete_node(aux);
                        }
                        aux=ptr;
                        ptr=ptr->prev;
                        delete_node(aux);
                    }
                }
            }
            template<typename Lambda>
            constexpr void iterative_in_order(Lambda&& func=Lambda{})const{
                if(root==sent) return;
                const_NodePtr ptr=left_most;
                while(true){
                    func(ptr->data);
                    if(ptr->right!=nullptr){
                        if(ptr==ptr->right->left) return;
                        else ptr=ptr->right->minimum();
                    }
                    else if(ptr==ptr->prev->left)
                        ptr=ptr->prev;
                    else{
                        while(ptr==ptr->prev->right)
                            ptr=ptr->prev;
                        ptr=ptr->prev;
                    }
                }
            }
            struct CpyFunc{
                Tree& self;
                constexpr CpyFunc(Tree& ref)noexcept:self(ref){}
                constexpr void operator()(const value_type& v){
                    self.insert(v);
                }
            };
            struct MovFunc{
                Tree& self;
                constexpr MovFunc(Tree& ref)noexcept:self(ref){}
                constexpr void operator()(value_type& v){
                    self.insert(static_cast<value_type&&>(v));
                }
            };
            template<typename val_type>
            constexpr iterator insert_without_repeated(val_type&& val){//universal reference of value_type
                NodePtr x;
                value_compare vcmp(ct.getV1());
                if(root==sent){
                    x=root=left_most=right_most=create_node(static_cast<val_type&&>(val));
                    x->right=x->left=sent;
                }
                else if(!vcmp(val,right_most->data)){//val>=right_most->data
                    if(vcmp(right_most->data,val)){//val>right_most->data
                        right_most->right=create_node(static_cast<val_type&&>(val));
                        right_most->right->prev=right_most;
                        right_most=right_most->right;
                        right_most->right=sent;
                        x=right_most;
                    }
                    else/*val==right_most->data*/ return iterator(right_most);
                }
                else if(!vcmp(left_most->data,val)){//val<=left_most->data
                    if(vcmp(val,left_most->data)){//val<left_most->data
                        left_most->left=create_node(static_cast<val_type&&>(val));
                        left_most->left->prev=left_most;
                        left_most=left_most->left;
                        left_most->left=sent;
                        x=left_most;
                    }
                    else/*val==left_most->data*/ return iterator(left_most);
                }
                else{
                    x=root;
                    while(true){
                        if(vcmp(val,x->data)){
                            if(x->left==nullptr){
                                x->left=create_node(static_cast<val_type&&>(val));
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else if(vcmp(x->data,val)){
                            if(x->right==nullptr){
                                x->right=create_node(static_cast<val_type&&>(val));
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                        else return iterator(x);
                    }
                }
                length++;
                this->insert_fixup(x);
                return iterator(x);
            }
            template<typename val_type>
            constexpr iterator insert_with_repeated(val_type&& val){//universal reference of value_type
                NodePtr x;
                value_compare vcmp(ct.getV1());
                if(root==sent){
                    x=root=left_most=right_most=create_node(static_cast<val_type&&>(val));
                    x->right=x->left=sent;
                }
                else if(!vcmp(val,right_most->data)){//val>=right_most->data
                    right_most->right=create_node(static_cast<val_type&&>(val));
                    right_most->right->prev=right_most;
                    right_most=right_most->right;
                    right_most->right=sent;
                    x=right_most;
                }
                else if(vcmp(val,left_most->data)){//val<left_most->data
                    left_most->left=create_node(static_cast<val_type&&>(val));
                    left_most->left->prev=left_most;
                    left_most=left_most->left;
                    left_most->left=sent;
                    x=left_most;
                }
                else{
                    x=root;
                    while(true){
                        if(vcmp(val,x->data)){
                            if(x->left==nullptr){
                                x->left=create_node(static_cast<val_type&&>(val));
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else{
                            if(x->right==nullptr){
                                x->right=create_node(static_cast<val_type&&>(val));
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                    }
                }
                length++;
                this->insert_fixup(x);
                return iterator(x);
            }
        public:
            constexpr Tree()noexcept(is_nothrow_default_constructible_v<AlTy> && is_nothrow_default_constructible_v<key_compare>){root=left_most=right_most=sent;}
            constexpr explicit Tree(const Alloc& al)noexcept(is_nothrow_default_constructible_v<key_compare>):ct(arg2_tag{},al){root=left_most=right_most=sent;}
            constexpr explicit Tree(const key_compare& comp,const Alloc& al=Alloc{})noexcept:ct(arg1and2_tag{},comp,al){root=left_most=right_most=sent;}
            template<typename It, enable_if_t<is_iterator_v<It>,int> = 0>
            constexpr Tree(It first,It last,const key_compare& comp=key_compare{},const Alloc& al=Alloc{}):ct(arg1and2_tag{},comp,al){
                root=left_most=right_most=sent;
                while(first!=last) insert(*first++);
            }
            constexpr Tree(std::initializer_list<value_type> ilist,const key_compare& comp=key_compare{},const Alloc& al=Alloc{}):ct(arg1and2_tag{},comp,al){
                root=left_most=right_most=sent;
                for(const_reference v:ilist) insert(v);
            }
            constexpr Tree(const Tree& other):ct(arg1and2_tag{},other.ct.getV1(),AlNode_traits::select_on_container_copy_construction(other.alloc)){
                root=left_most=right_most=sent;
                other.iterative_in_order(CpyFunc(*this));
            }
            constexpr Tree(const Tree& other,const Alloc& al):ct(arg1and2_tag{},other.ct.getV1(),al){
                root=left_most=right_most=sent;
                other.iterative_in_order(CpyFunc(*this));
            }
            constexpr Tree(Tree&& other)noexcept:ct(arg1and2_tag{},static_cast<key_compare&&>(other.ct.getV1()),static_cast<AlNode&&>(other.alloc)),length(other.length){
                if(other.root==other.sent) root=left_most=right_most=sent;
                else{
                    root=other.root;
                    left_most=other.left_most;
                    right_most=other.right_most;
                    left_most->left=right_most->right=sent;
                    other.reset();
                }
            }
            constexpr Tree(Tree&& other,const Alloc& al)noexcept(ALWAYS_EQ):ct(arg1and2_tag{},static_cast<key_compare&&>(other.ct.getV1()),al){
                if(this->alloc==other.alloc){
                    length=other.length;
                    if(other.root==other.sent) root=left_most=right_most=sent;
                    else{
                        root=other.root;
                        left_most=other.left_most;
                        right_most=other.right_most;
                        left_most->left=right_most->right=sent;
                        other.reset();
                    }                    
                }
                else{
                    root=left_most=right_most=sent;
                    other.iterative_in_order(MovFunc(*this));
                    other.clear();
                }
            }
            constexpr Tree& operator=(const Tree& other){
                if(this==&other) return *this;
                this->clear();
                this->ct.getV1()=other.ct.getV1();
                pocca(this->alloc,other.alloc);
                other.iterative_in_order(CpyFunc(*this));
                return *this;
            }
            constexpr Tree& operator=(Tree&& other)noexcept(ALWAYS_EQ || POCMA){
                if(this==&other) return *this;
                this->clear();
                this->ct.getV1()=static_cast<key_compare&&>(other.ct.getV1());
                if constexpr(!ALWAYS_EQ && !POCMA){
                    if(this->alloc!=other.alloc){
                        other.iterative_in_order(MovFunc(*this));
                        other.clear();
                        return *this;
                    }
                }
                pocma(this->alloc,other.alloc);
                length=other.length;
                if(other.root!=other.sent){
                    root=other.root;
                    left_most=other.left_most;
                    right_most=other.right_most;
                    left_most->left=right_most->right=sent;
                    other.reset();
                }
                return *this;
            }
            constexpr iterator insert(const value_type& val){
                if constexpr(allow_repeated) return insert_with_repeated(val);
                else return insert_without_repeated(val);
            }
            constexpr iterator insert(value_type&& val){
                if constexpr(allow_repeated) return insert_with_repeated(static_cast<value_type&&>(val));
                else return insert_without_repeated(static_cast<value_type&&>(val));
            }
            template<typename It>
            constexpr enable_if_t<is_iterator_v<It>> insert(It first,It last){
                while(first!=last)
                    insert(*first++);
            }
            template<typename... Args>
            constexpr iterator emplace(Args&&... args){
                value_type val(static_cast<Args&&>(args)...);
                if constexpr(allow_repeated) return insert_with_repeated(static_cast<value_type&&>(val));
                else return insert_without_repeated(static_cast<value_type&&>(val));
            }
            constexpr bool del(const_iterator pos)noexcept{//pos should be an iterator to an element of this or the end() iterator.
                if(pos==end()) return false;
                NodePtr z=const_cast<NodePtr>(pos.ptr);//the node that will be deleted
                if(z==root && z->left==sent && z->right==sent){
                    delete_node(z);
                    reset();
                    return true;
                }
                NodePtr x;//the node that repaces z
                RedOrBlack original_color=z->color;
                TempNode tempnode;
                if(is_null(z->left) && is_null(z->right)){
                    if(z->right==sent){
                        right_most=z->prev;
                        tempnode.link_to_right_of(z);
                        x=z->right;
                    }
                    else if(z->left==sent){
                        left_most=z->prev;
                        tempnode.link_to_left_of(z);
                        x=z->left;
                    }
                    else{
                        tempnode.link_to_left_of(z);
                        x=z->left;
                    }
                    transplant(z,x);
                }
                else if(is_null(z->left)){
                    x=z->right;
                    transplant(z,x);
                    if(left_most==z){
                        left_most=x;
                        x->left=sent;
                    }
                }
                else if(is_null(z->right)){
                    x=z->left;
                    transplant(z,x);
                    if(right_most==z){
                        right_most=x;
                        x->right=sent;
                    }
                }
                else{// In this case, z isn't right_most or left_most
                    NodePtr y=z->right->minimum();
                    original_color=y->color;
                    if(is_null(y->right)){
                        tempnode.link_to_right_of(y);
                    }
                    x=y->right;
                    if(y->prev!=z){
                        transplant(y, x);
                        y->right=z->right;
                        y->right->prev=y;
                    }
                    transplant(z,y);
                    y->left=z->left;
                    y->left->prev=y;
                    y->color=z->color;
                }
                delete_node(z);
                if(original_color==BLACK)
                    delete_fixup(x);
                length--;
                return true;
            }
            constexpr Optional<value_type> pop(const_iterator pos)noexcept(is_nothrow_move_constructible_v<value_type>){
                if(pos==end()) return {};
                NodePtr z=const_cast<NodePtr>(pos.ptr);//the node that will be deleted
                if(z==root && z->left==sent && z->right==sent){
                    value_type ret=static_cast<value_type&&>(z->data);
                    delete_node(z);
                    reset();
                    return ret;
                }
                NodePtr x;//the node that repaces z
                RedOrBlack original_color=z->color;
                TempNode tempnode;
                if(is_null(z->left) && is_null(z->right)){
                    if(z->right==sent){
                        right_most=z->prev;
                        tempnode.link_to_right_of(z);
                        x=z->right;
                    }
                    else if(z->left==sent){
                        left_most=z->prev;
                        tempnode.link_to_left_of(z);
                        x=z->left;
                    }
                    else{
                        tempnode.link_to_left_of(z);
                        x=z->left;
                    }
                    transplant(z,x);
                }
                else if(is_null(z->left)){
                    x=z->right;
                    transplant(z,x);
                    if(left_most==z){
                        left_most=x;
                        x->left=sent;
                    }
                }
                else if(is_null(z->right)){
                    x=z->left;
                    transplant(z,x);
                    if(right_most==z){
                        right_most=x;
                        x->right=sent;
                    }
                }
                else{// In this case, z isn't right_most or left_most
                    NodePtr y=z->right->minimum();
                    original_color=y->color;
                    if(is_null(y->right)){
                        tempnode.link_to_right_of(y);
                    }
                    x=y->right;
                    if(y->prev!=z){
                        transplant(y, x);
                        y->right=z->right;
                        y->right->prev=y;
                    }
                    transplant(z,y);
                    y->left=z->left;
                    y->left->prev=y;
                    y->color=z->color;                    
                }
                value_type ret=static_cast<value_type&&>(z->data);
                delete_node(z);
                if(original_color==BLACK)
                    delete_fixup(x);
                length--;
                return ret;
            }
            constexpr bool del_elem(const key_type& val){
                return del(find(val));
            }
            constexpr Optional<value_type> pop_elem(const key_type& val){
                return pop(find(val));
            }
            constexpr iterator erase(const_iterator first,const_iterator last)noexcept{
                while(first!=last){
                    iterator aux(first.ptr);
                    ++first;
                    del(aux);
                }
                return last;
            }
            constexpr bool contains(const key_type& val)const{
                if(root==sent || cmp(Traits::getKey(right_most->data),val) || cmp(val,Traits::getKey(left_most->data))) return false;
                else if((!cmp(val,Traits::getKey(right_most->data)) && !cmp(Traits::getKey(right_most->data),val)) || (!cmp(val,Traits::getKey(left_most->data)) && !cmp(Traits::getKey(left_most->data),val)))/*val==right_most->data || val==left_most->data*/ return true;
                NodePtr x=root;
                while(true){
                    if(cmp(val,Traits::getKey(x->data))){
                        if(x->left==nullptr)
                            return false;						
                        else
                            x=x->left;
                    }
                    else if(cmp(Traits::getKey(x->data),val)){
                        if(x->right==nullptr)
                            return false;							
                        else
                            x=x->right;
                    }
                    else return true;
                }
            }
            constexpr iterator find(const key_type& val){
                if(root==sent || cmp(Traits::getKey(right_most->data),val) || cmp(val,Traits::getKey(left_most->data))) return end();
                if constexpr(!allow_repeated){
                    if(!cmp(val,Traits::getKey(right_most->data)) && !cmp(Traits::getKey(right_most->data),val))/*val==right_most->data*/ return iterator(right_most);
                    else if(!cmp(val,Traits::getKey(left_most->data)) && !cmp(Traits::getKey(left_most->data),val))/*val==left_most->data*/ return iterator(left_most);
                }
                NodePtr x=root;
/*  Aclaracion
    if(x->left==nullptr) y su contraparte con right estan bien asi.
    No hace falta hacer is_null(x->left) ni is_null(x->right) porque el if de la primera linea checkea si val es mayor al
    elemento mas grande o menor al elemento mas chico y deviuelve this->end() si alguna es verdadera, por lo que nunca se 
    va a necesitar leer left_most->left ni right_mosat->right, que son los unicos nodos nulos que no apuntan a nullptr sino 
    que apuntan al nodo sentinela
*/
                while(true){
                    if(cmp(val,Traits::getKey(x->data))){
                        if(x->left==nullptr)
                            return end();
                        else
                            x=x->left;
                    }
                    else if(cmp(Traits::getKey(x->data),val)){
                        if(x->right==nullptr)
                            return end();							
                        else
                            x=x->right;
                    }
                    else return iterator(x);
                }
            }
            constexpr const_iterator find(const key_type& val)const{
                if(root==sent || cmp(Traits::getKey(right_most->data),val) || cmp(val,Traits::getKey(left_most->data))) return end();
                if constexpr(!allow_repeated){
                    if(!cmp(val,Traits::getKey(right_most->data)) && !cmp(Traits::getKey(right_most->data),val))/*val==right_most->data*/ return iterator(right_most);
                    else if(!cmp(val,Traits::getKey(left_most->data)) && !cmp(Traits::getKey(left_most->data),val))/*val==left_most->data*/ return iterator(left_most);
                }
                NodePtr x=root;
                while(true){
                    if(cmp(val,Traits::getKey(x->data))){
                        if(x->left==nullptr)
                            return end();
                        else
                            x=x->left;
                    }
                    else if(cmp(Traits::getKey(x->data),val)){
                        if(x->right==nullptr)
                            return end();							
                        else
                            x=x->right;
                    }
                    else return const_iterator(x);
                }
            }
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const{
                for(const_reference x:*this)
                    if(func(x))return 1;
                return 0;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{}){
                if constexpr(is_same_v<key_type,value_type>) const_cast<const Tree*>(this)->iterative_in_order(func);
                else iterative_in_order(func);
			}            
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{})const{
                iterative_in_order(func);
			}
			template<typename Lambda>
			constexpr Tree filter(Lambda&& func=Lambda{})const{
				Tree new_tree(ct.getV1(),AlNode_traits::select_on_container_copy_construction(alloc));
                auto f=[&new_tree,&func](const value_type& v){if(func(v))new_tree.insert(v);};
                this->iterative_in_order(f);
				return new_tree;
			}
            template<typename Lambda>
            constexpr void pre_order(Lambda&& func=Lambda{})const{
                pre_order_helper(root,func);
            }
            template<typename Lambda>
            constexpr void in_order(Lambda&& func=Lambda{})const{
                in_order_helper(root,func);
            }
            template<typename Lambda>
            constexpr void post_order(Lambda&& func=Lambda{})const{
                post_order_helper(root,func);
            }
			constexpr bool operator==(const Tree& other)const{
				if(this->length!=other.length) return false;
				else return equal(this->begin(),this->end(),other.begin());
			}
			constexpr bool operator!=(const Tree& other)const{
				if(this->length!=other.length) return true;
				else return !equal(this->begin(),this->end(),other.begin());
			}
            constexpr bool operator<(const Tree& other)const{
                return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool operator>(const Tree& other)const{
                return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator<=(const Tree& other)const{
                return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator>=(const Tree& other)const{
                return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool empty()const noexcept{
                return length==0;
            }
            constexpr iterator get_root()noexcept{
                return iterator(root);
            }
            constexpr const_iterator get_root()const noexcept{
                return const_iterator(root);
            }
            constexpr iterator get_min()noexcept{
                return iterator(left_most);
            }
            constexpr const_iterator get_min()const noexcept{
                return const_iterator(left_most);
            }
            constexpr iterator get_max()noexcept{
                return iterator(right_most);
            }
            constexpr const_iterator get_max()const noexcept{
                return const_iterator(right_most);
            }
            constexpr allocator_type get_allocator()const noexcept{
                return this->alloc;
            }
            constexpr size_type get_size()const{
                size_t len=0;
                iterative_in_order([&len](const value_type&){++len;});
                return len;
            }
            constexpr size_type size()const noexcept{
#if defined(DEBUG) && (defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_))
                if(length!=get_size()){
                    std::cerr<<"\nSIZE AND NODE_COUNT DO NOT MATCH\a\n"
                            <<"LEN: "<<length<<"\nNODE_COUNT: "<<get_size()<<std::endl;
                    exit(-1);
                }
#endif
                return length;
            }
            constexpr void clear()noexcept{
                iterative_deletion();
                reset();
            }
            ~Tree()noexcept{
                iterative_deletion();
            }
            constexpr const_iterator begin()const noexcept{
                return const_iterator(left_most);
            }
            constexpr const_iterator end()const noexcept{
                return const_iterator(sent);
            }
            constexpr iterator begin()noexcept{
                return iterator(left_most);
            }
            constexpr iterator end()noexcept{
                return iterator(sent);
            }
            template<typename Out,typename Traits_> friend Out& operator<<(Out&,const Tree<Traits_>&);
#undef root
#undef alloc
#undef left_most
#undef right_most
#undef sent
    };
    template<typename Out,typename Traits>
    Out& operator<<(Out& stream,const Tree<Traits>& tree){
        stream<<'[';
        auto it(tree.begin());
        auto it_end(tree.end());
        while(it!=it_end){
            stream<<*it;
            if(++it!=it_end)
                stream<<", ";
        }
        return stream<<']';
    }
}