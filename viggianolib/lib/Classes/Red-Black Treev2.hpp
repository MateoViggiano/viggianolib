#pragma once
#include "Stacks_Queues.hpp"
namespace mpv{
    enum RedOrBlack:bool{RED,BLACK};
    template<typename T,typename VoidPtr>struct Tree_Node;
    template<typename T,typename VoidPtr>
    struct Base_Tree_Node{
        using BaseNodePtr=rebind_pointer<VoidPtr,Base_Tree_Node>;
        using NodePtr=rebind_pointer<VoidPtr,Tree_Node<T,VoidPtr>>;
        using const_NodePtr=rebind_pointer<VoidPtr,const Tree_Node<T,VoidPtr>>;
        NodePtr left=nullptr;
        NodePtr right=nullptr;
        NodePtr prev=nullptr;
        RedOrBlack color;
        Base_Tree_Node():color(BLACK){}
        Base_Tree_Node(RedOrBlack color):color(color){}
    };
    template<typename T,typename VoidPtr>
    struct Temp_Tree_Node:public Base_Tree_Node<T,VoidPtr>{
        Temp_Tree_Node():Base_Tree_Node<T,VoidPtr>(BLACK){}
        void link_to_right_of(typename Base_Tree_Node<T,VoidPtr>::NodePtr node){
            node->right=static_cast<typename Base_Tree_Node<T,VoidPtr>::NodePtr>(static_cast<typename Base_Tree_Node<T,VoidPtr>::BaseNodePtr>(this));
            this->prev=node;
        }
        ~Temp_Tree_Node(){
            if(this->prev!=nullptr){
                if(this->prev->right==static_cast<typename Base_Tree_Node<T,VoidPtr>::NodePtr>(static_cast<typename Base_Tree_Node<T,VoidPtr>::BaseNodePtr>(this)))
                    this->prev->right=nullptr;
                else
                    this->prev->left=nullptr;
            }   
        }
    };
    template<typename T,typename VoidPtr>
    struct Tree_Node:public Base_Tree_Node<T,VoidPtr>{
        using NodePtr=typename Base_Tree_Node<T,VoidPtr>::NodePtr;
        using const_NodePtr=typename Base_Tree_Node<T,VoidPtr>::const_NodePtr;
        T data;
        template<typename... Args>
        Tree_Node(Args&&... args):Base_Tree_Node<T,VoidPtr>(RED),data(static_cast<Args&&>(args)...){}
        Tree_Node(const Tree_Node&)=delete;
        Tree_Node& operator=(const Tree_Node&)=delete;
        inline NodePtr maximum(){
            NodePtr p=this;
            while(p->right)
                p=p->right;
            return p;
        }
        inline NodePtr minimum(){
            NodePtr p=this;
            while(p->left)
                p=p->left;
            return p;
        }
        inline const_NodePtr maximum()const{
            const_NodePtr p=this;
            while(p->right)
                p=p->right;
            return p;
        }
        inline const_NodePtr minimum()const{
            const_NodePtr p=this;
            while(p->left)
                p=p->left;
            return p;
        }
    };

    template<typename Types,typename Alloc>
    class StackBased_Tree_iterator COUNT_IT{
		public:
			using NodePtr=typename Types::NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=value_type&;
        private:
            NodePtr ptr;
            Stack<NodePtr,rebind_alloc<Alloc,NodePtr>> stack;
        public:
            StackBased_Tree_iterator(NodePtr p):ptr(p){
                if(ptr==nullptr)return;
                while(ptr->left!=nullptr){
                    stack.push(ptr);
                    ptr=ptr->left;
                }
            }
            StackBased_Tree_iterator(const StackBased_Tree_iterator& other):ptr(other.ptr),stack(other.stack){}
            StackBased_Tree_iterator(StackBased_Tree_iterator&& other):ptr(other.ptr),stack(static_cast<Stack<NodePtr,rebind_alloc<Alloc,NodePtr>>&&>(other.stack)){}
            StackBased_Tree_iterator& operator=(const StackBased_Tree_iterator& other){
                ptr=other.ptr;
                stack=other.stack;
                return *this;
            }
            StackBased_Tree_iterator& operator=(StackBased_Tree_iterator&& other){
                ptr=other.ptr;
                stack=static_cast<Stack<NodePtr,rebind_alloc<Alloc,NodePtr>>&&>(other.stack);
                return *this;
            }
            reference operator*(){
                return ptr->data;
            }
            pointer operator->(){
                return &(ptr->data);
            }
            StackBased_Tree_iterator& operator++(){
                if(ptr->right==nullptr and not stack.empty())
                    ptr=stack.pop();
                else if(ptr->right!=nullptr){
                    ptr=ptr->right;
                    while(ptr->left!=nullptr){
                        stack.push(ptr);
                        ptr=ptr->left;
                    }
                }
                else //ptr->right==nullptr and stack->empty()
                    ptr=nullptr;
                return *this;
            }
            StackBased_Tree_iterator operator++(int){
                StackBased_Tree_iterator aux=*this;
                if(ptr->right==nullptr and not stack.empty())
                    ptr=stack.pop();
                else if(ptr->right!=nullptr){
                    ptr=ptr->right;
                    while(ptr->left!=nullptr){
                        stack.push(ptr);
                        ptr=ptr->left;
                    }
                }
                else //ptr->right==nullptr and stack->empty()
                    ptr=nullptr;
                return aux;
            }
            bool operator==(const StackBased_Tree_iterator& other)const{
                return this->ptr==other.ptr;
            }
            bool operator!=(const StackBased_Tree_iterator& other)const{
                return this->ptr!=other.ptr;
            }
            ~StackBased_Tree_iterator(){}
    };
    template<typename NodePtr>
    inline NodePtr find_predecessor(NodePtr current){
        NodePtr p=current->left;
        while(p->right!=nullptr and p->right!=current){
            p=p->right;
        }
        return p;
    }
    template<typename Types>
    class Morris_Tree_iterator COUNT_IT{
		public:
			using NodePtr=typename Types::NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=value_type&;
        private:
            NodePtr ptr,current;
        public:
            Morris_Tree_iterator(NodePtr p):current(p){
                while(current!=nullptr){
                    if(current->left==nullptr){
                        ptr=current;
                        current=current->right;
                        return;
                    }
                    else{
                        NodePtr predecessor=find_predecessor(current);
                        if(predecessor->right==nullptr){
                            predecessor->right=current;
                            current=current->left;
                        }
                        else/*predecessor->right==current*/{
                            predecessor->right=nullptr;
                            ptr=current;
                            current=current->right;
                            return;
                        }
                    }                    
                }
                ptr=nullptr;
            }
            reference operator*(){
                return ptr->data;
            }
            pointer operator->(){
                return &(ptr->data);
            }
            Morris_Tree_iterator& operator++(){
                while(current!=nullptr){
                    if(current->left==nullptr){
                        ptr=current;
                        current=current->right;
                        return *this;
                    }
                    else{
                        NodePtr predecessor=find_predecessor(current);
                        if(predecessor->right==nullptr){
                            predecessor->right=current;
                            current=current->left;
                        }
                        else/*predecessor->right==current*/{
                            predecessor->right=nullptr;
                            ptr=current;
                            current=current->right;
                            return *this;
                        }
                    }                    
                }
                ptr=nullptr;
                return *this;
            }
            bool operator==(const Morris_Tree_iterator& other)const{
                return this->ptr==other.ptr;
            }
            bool operator!=(const Morris_Tree_iterator& other)const{
                return this->ptr!=other.ptr;
            }
            ~Morris_Tree_iterator(){}
    };
    template<typename Types>
    class Tree_iterator{
		public:
			using NodePtr=typename Types::NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=typename Types::reference;
        private:
            NodePtr ptr;
        public:
            Tree_iterator(NodePtr ptr):ptr(ptr ? ptr->minimum() : nullptr){}
            reference operator*(){
                return ptr->data;
            }
            pointer operator->(){
                return &(ptr->data);
            }
            Tree_iterator& operator++(){
                if(ptr->right!=nullptr) 
                    ptr=ptr->right->minimum();
                else if(ptr->prev==nullptr)
                    ptr=nullptr;
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr->prev!=nullptr and ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    if(ptr->prev==nullptr)
                        ptr=nullptr;
                    else
                        ptr=ptr->prev;
                }
                return *this;
            }
            Tree_iterator operator++(int){
                Tree_iterator aux=*this;
                if(ptr->right!=nullptr) 
                    ptr=ptr->right->minimum();
                else if(ptr->prev==nullptr)
                    ptr=nullptr;
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr->prev!=nullptr and ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    if(ptr->prev==nullptr)
                        ptr=nullptr;
                    else
                        ptr=ptr->prev;
                }
                return aux;
            }
            bool operator==(const Tree_iterator& other)const{
                return this->ptr==other.ptr;
            }
            bool operator!=(const Tree_iterator& other)const{
                return this->ptr!=other.ptr;
            }
            template<typename> friend class const_Tree_iterator;
    };
    template<typename Types>
    class const_Tree_iterator{
		public:
			using NodePtr=typename Types::const_NodePtr;
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
        private:
            NodePtr ptr;
        public:
            const_Tree_iterator(NodePtr ptr):ptr(ptr ? ptr->minimum() : nullptr){}
            const_Tree_iterator(Tree_iterator<Types> nonconst_it):ptr(nonconst_it.ptr){}
            reference operator*(){
                return ptr->data;
            }
            pointer operator->(){
                return &(ptr->data);
            }
            const_Tree_iterator& operator++(){
                if(ptr->right!=nullptr) 
                    ptr=ptr->right->minimum();
                else if(ptr->prev==nullptr)
                    ptr=nullptr;
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr->prev!=nullptr and ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    if(ptr->prev==nullptr)
                        ptr=nullptr;
                    else
                        ptr=ptr->prev;
                }
                return *this;
            }
            const_Tree_iterator operator++(int){
                const_Tree_iterator aux=*this;
                if(ptr->right!=nullptr) 
                    ptr=ptr->right->minimum();
                else if(ptr->prev==nullptr)
                    ptr=nullptr;
                else if(ptr==ptr->prev->left)
                    ptr=ptr->prev;
                else{
                    while(ptr->prev!=nullptr and ptr==ptr->prev->right)
                        ptr=ptr->prev;
                    if(ptr->prev==nullptr)
                        ptr=nullptr;
                    else
                        ptr=ptr->prev;
                }
                return aux;
            }
            bool operator==(const const_Tree_iterator& other)const{
                return this->ptr==other.ptr;
            }
            bool operator!=(const const_Tree_iterator& other)const{
                return this->ptr!=other.ptr;
            }
    };
    template<typename T,typename Alloc=allocator<T>,typename Cmp=less<T>>
    class Tree:EBCO<rebind_alloc<Alloc,Tree_Node<T,typename allocator_traits<Alloc>::void_pointer>>> COUNT_IT_{
#define root cp.V2
		private:
			using Node=Tree_Node<T,typename allocator_traits<Alloc>::void_pointer>;
            using TempNode=Temp_Tree_Node<T,typename allocator_traits<Alloc>::void_pointer>;
			using NodePtr=typename Node::NodePtr;
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
			using morris_iterator=Morris_Tree_iterator<Val_types>;
            using stack_iterator=StackBased_Tree_iterator<Val_types,Alloc>;
            using iterator=Tree_iterator<Val_types>;
            using const_iterator=const_Tree_iterator<Val_types>;
		private:
            CompressedPair<Cmp,NodePtr> cp;
            inline static bool is_red(NodePtr p){
                return p!=nullptr and p->color==RED;
            }
            inline static bool is_black(NodePtr p){
                return p==nullptr or p->color==BLACK;
            }
            inline bool cmp(const_reference a,const_reference b)const{
            	return cp.getV1()(a,b);
			}
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
            inline void clr(NodePtr node){
                if(node==nullptr)
                    return;
                else{
                    clr(node->left);
                    clr(node->right);
                    delete_node(node);
                }
            }
            template<typename Lambda>
            static void pre_order_helper(NodePtr node,Lambda& func){
                if(node==nullptr)
                    return;
                func(node->data);
                pre_order_helper(node->left,func);
                pre_order_helper(node->right,func);
            }
            template<typename Lambda>
            static void in_order_helper(NodePtr node,Lambda& func){
                if(node==nullptr)
                    return;
                in_order_helper(node->left,func);
                func(node->data);
                in_order_helper(node->right,func);
            }
            template<typename Lambda>
            static void post_order_helper(NodePtr node,Lambda& func){
                if(node==nullptr)
                    return;
                post_order_helper(node->left,func);
                post_order_helper(node->right,func);
                func(node->data);
            }
            inline void insert_fixup(NodePtr x){
                while(is_red(x->prev)){
                    if(x->prev==x->prev->prev->left){															//If the father is a left node
                        if(is_red(x->prev->prev->right)){				//case 1:X.uncle==RED
                            x->prev->color=BLACK;
                            x->prev->prev->right->color=BLACK;
                            x->prev->prev->color=RED;
                            x=x->prev->prev;
                        }
                        else/*(x->prev->prev->right->color==BLACK)*/{			//case 2 & 3:x.uncle==BLACK
                            if(x==x->prev->right){																//case 2:(triangle)
                                x=x->prev;
                                left_rotate(x);
                            }
                            x->prev->color=BLACK;																//case 3:(line)
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
                            if(x==x->prev->left){																//case 2:(triangle)
                                x=x->prev;
                                right_rotate(x);
                            }
                            x->prev->color=BLACK;																//case 3:(line)
                            x->prev->prev->color=RED;
                            left_rotate(x->prev->prev);
                        }
                    }
                }
                this->root->color=BLACK;																		//case 0:root==RED
            }
            inline void delete_fixup(NodePtr x){
                while(x!=root and x->color==BLACK){
                    if(x==x->prev->left){
                        NodePtr w=x->prev->right;
                        if(is_red(w)){
                            w->color=BLACK;
                            x->prev->color=RED;
                            left_rotate(x->prev);
                            w=x->prev->right;
                        }
                        if(is_black(w->left) and is_black(w->right)){
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
                        if(is_black(w->left) and is_black(w->right)){
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
            inline void right_rotate(NodePtr x){
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
                if(x->left!=nullptr)
                    x->left->prev=x;
                y->right=x;
                x->prev=y;
            }
            inline void left_rotate(NodePtr x){
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
                if(x->right!=nullptr)
                    x->right->prev=x;
                y->left=x;
                x->prev=y;
            }
            inline void transplant(NodePtr u,NodePtr v){//Desenlaza u con su padre y enlaza al padre con v
                if(u->prev==nullptr)
                    this->root=v;
                else if(u==u->prev->left)
                    u->prev->left=v;
                else
                    u->prev->right=v;
                if(v!=nullptr)
                    v->prev=u->prev;
            }
            struct CpyFunc{
                Tree& self;
                CpyFunc(Tree& ref):self(ref){}
                void operator()(reference v){
                    self.insert(v);
                }
            };
            struct MovFunc{
                Tree& self;
                MovFunc(Tree& ref):self(ref){}
                void operator()(reference v){
                    self.insert(static_cast<value_type&&>(v));
                }
            };
        public:
            Tree(){INCTREES}
            explicit Tree(const Cmp& comp,const Alloc& al=Alloc{}):EBCO<AlNode>(al),cp(arg1_tag{},comp){INCTREES}
            Tree(const Tree& other):EBCO<AlNode>(AlNode_traits::select_on_container_copy_construction(other.get_val())),cp(arg1_tag{},other.cp.getV1()){INCTREES
                CpyFunc f(*this);
                other.iterative_in_order(f);
            }
            Tree(const Tree& other,const Alloc& al):EBCO<AlNode>(al),cp(arg1_tag{},other.cp.getV1()){INCTREES
                CpyFunc f(*this);
                other.iterative_in_order(f);
            }
            Tree(Tree&& other):EBCO<AlNode>(static_cast<AlNode&&>(other.get_val())),cp(arg1_tag{},static_cast<Cmp&&>(other.cp.getV1()),other.root){INCTREES
                other.root=nullptr;
            }
            Tree(Tree&& other,const Alloc& al):EBCO<AlNode>(al),cp(arg1_tag{},static_cast<Cmp&&>(other.cp.getV1())){INCTREES
                if(this->get_val()==other.get_val()){
                    this->root=other.root;
                    other.root=nullptr;
                }
                else{
                    MovFunc f(*this);
                    other.iterative_in_order(f);
                    other.clear();
                }
            }
            Tree(std::initializer_list<T> initializerList){INCTREES
                for(const_reference v:initializerList)
                    this->insert(v);
            }
            Tree(std::initializer_list<T> initializerList,const Alloc& al):EBCO<AlNode>(al){INCTREES
                for(const_reference v:initializerList)
                    this->insert(v);
            }
            Tree& operator=(const Tree& other){
                this->clear();
                this->cp.getV1()=other.cp.getV1();
                if constexpr(POCCA) this->get_val()=other.get_val();
                CpyFunc f(*this);
                other.iterative_in_order(f);
                return *this;
            }
            Tree& operator=(Tree&& other){
                this->clear();
                this->cp.getV1()=static_cast<Cmp&&>(other.cp.getV1());
                if constexpr(not ALWAYS_EQ and not POCMA){
                    if(this->get_val()!=other.get_val()){
                        MovFunc f(*this);
                        other.iterative_in_order(f);
                        return *this;
                    }
                }
                if constexpr(POCMA) this->get_val()=other.get_val();
                this->root=other.root;
                other.root=nullptr;
                return *this;
            }
            pointer insert(const_reference data){
                NodePtr x;
                if(root==nullptr){
                    root=create_node(data);
                    x=root;
                }
                else{
                    x=root;
                    while(true){
                        if(cmp(data,x->data)){
                            if(x->left==nullptr){
                                x->left=create_node(data);
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else if(cmp(x->data,data)){
                            if(x->right==nullptr){
                                x->right=create_node(data);
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                        else return nullptr;
                    }
                }
                this->insert_fixup(x);
                return &x->data;
            }
            pointer insert(value_type&& data){
                NodePtr x;
                if(root==nullptr){
                    root=create_node(static_cast<value_type&&>(data));
                    x=root;
                }
                else{
                    x=root;
                    while(true){
                        if(cmp(data,x->data)){
                            if(x->left==nullptr){
                                x->left=create_node(static_cast<value_type&&>(data));
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else if(cmp(x->data,data)){
                            if(x->right==nullptr){
                                x->right=create_node(static_cast<value_type&&>(data));
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                        else return nullptr;
                    }
                }
                this->insert_fixup(x);
                return &x->data;
            }
            template<typename... Args>
            pointer emplace(Args&&... args){
                NodePtr x;
                T data(static_cast<Args&&>(args)...);
                if(root==nullptr){
                    root=create_node(static_cast<value_type&&>(data));
                    x=root;
                }
                else{
                    x=root;
                    while(true){
                        if(cmp(data,x->data)){
                            if(x->left==nullptr){
                                x->left=create_node(static_cast<value_type&&>(data));
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else if(cmp(x->data,data)){
                            if(x->right==nullptr){
                                x->right=create_node(static_cast<value_type&&>(data));
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                        else return nullptr;
                    }
                }
                this->insert_fixup(x);
                return &x->data;
            }
            // template<typename... Args>
            // bool emplace(Args&&... args){
            //     NodePtr x;
            //     T data(static_cast<Args&&>(args)...);
            //     if(root==nullptr){
            //         root=create_node(static_cast<value_type&&>(data));
            //         x=root;
            //     }
            //     else{
            //         x=root;
            //         while(true){
            //             if(cmp(data,x->data)){
            //                 if(x->left==nullptr){
            //                     x->left=create_node(static_cast<value_type&&>(data));
            //                     x->left->prev=x;
            //                     x=x->left;
            //                     break;							
            //                 }
            //                 else
            //                     x=x->left;
            //             }
            //             else if(cmp(x->data,data)){
            //                 if(x->right==nullptr){
            //                     x->right=create_node(static_cast<value_type&&>(data));
            //                     x->right->prev=x;
            //                     x=x->right;
            //                     break;							
            //                 }
            //                 else
            //                     x=x->right;
            //             }
            //             else return false;
            //         }
            //     }
            //     this->insert_fixup(x);
            //     return true;
            // }
            bool has(const_reference data)const{
                if(root==nullptr)
                    return false;
                NodePtr x=root;
                while(true){
                    if(cmp(data,x->data)){
                        if(x->left==nullptr)
                            return false;						
                        else
                            x=x->left;
                    }
                    else if(cmp(x->data,data)){
                        if(x->right==nullptr)
                            return false;							
                        else
                            x=x->right;
                    }
                    else return true;
                }
            }
            pointer get(const_reference data){
                if(root==nullptr)
                    return nullptr;
                NodePtr x=root;
                while(true){
                    if(cmp(data,x->data)){
                        if(x->left==nullptr)
                            return nullptr;						
                        else
                            x=x->left;
                    }
                    else if(cmp(x->data,data)){
                        if(x->right==nullptr)
                            return nullptr;							
                        else
                            x=x->right;
                    }
                    else return &x->data;
                }
            }
            const_pointer get(const_reference data)const{
                if(root==nullptr)
                    return nullptr;
                NodePtr x=root;
                while(true){
                    if(cmp(data,x->data)){
                        if(x->left==nullptr)
                            return nullptr;						
                        else
                            x=x->left;
                    }
                    else if(cmp(x->data,data)){
                        if(x->right==nullptr)
                            return nullptr;							
                        else
                            x=x->right;
                    }
                    else return &x->data;
                }
            }
            bool del(const_reference data){
                if(root==nullptr)
                    return false;
                NodePtr z=root;
                while(true){
                    if(cmp(data,z->data)){
                        if(z->left==nullptr)
                            return false;
                        else
                            z=z->left;
                    }
                    else if(cmp(z->data,data)){
                        if(z->right==nullptr)
                            return false;
                        else
                            z=z->right;
                    }
                    else break;
                }
                if(z==root and z->left==nullptr and z->right==nullptr){
                    delete_node(z);
                    root=nullptr;
                    return true;
                }
                NodePtr x;
                RedOrBlack original_color=z->color;
                TempNode tempnode;
                if(z->left==nullptr and z->right==nullptr){
                    tempnode.link_to_right_of(z);
                    x=z->right;
                    transplant(z,x);
                }
                else if(z->left==nullptr){
                    x=z->right;
                    transplant(z,x);
                }
                else if(z->right==nullptr){
                    x=z->left;
                    transplant(z,x);
                }
                else{
                    NodePtr y=z->right->minimum();
                    original_color=y->color;
                    if(y->right==nullptr){
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
                return true;
            }
			template<typename Lambda>
			bool any(Lambda&& func=Lambda{})const{
                for(const_reference val:*this)
                    if(func(val))return 1;
                return 0;
			}
			template<typename Lambda>
			void forEach(Lambda&& func=Lambda{}){
                iterative_in_order(func);
			}
			template<typename Lambda>
			auto map(Lambda&& func=Lambda{})const{
				Tree<decltype(func(root->data)),rebind_alloc<Alloc,decltype(func(root->data))>> new_tree;
                auto f=[&new_tree,&func](reference v){new_tree.insert(func(v));};
                this->iterative_in_order(f);
				return new_tree;
			}
			template<typename Lambda>
			Tree filter(Lambda&& func=Lambda{})const{
				Tree new_tree;
                auto f=[&new_tree,&func](reference v){if(func(v))new_tree.insert(v);};
                this->iterative_in_order(f);
				return new_tree;
			}
            template<typename Lambda>
            void iterative_in_order(Lambda&& func=Lambda{})const{
                if(root==nullptr) return;
                NodePtr current=root->minimum();
                while(true){
                    func(current->data);
                    if(current->right!=nullptr) 
                        current=current->right->minimum();
                    else if(current==root)
                        return;
                    else if(current==current->prev->left)
                        current=current->prev;
                    else{
                        while(current!=root and current==current->prev->right)
                            current=current->prev;
                        if(current==root)
                            return;
                        else
                            current=current->prev;
                    }
                }
            }
            template<typename Lambda>
            void morris_in_order(Lambda&& func=Lambda{})const{
                NodePtr current=root;
                while(current){
                    if(current->left==nullptr){
                        func(current->data);
                        current=current->right;
                    }
                    else{
                        NodePtr predecessor=find_predecessor(current);
                        if(predecessor->right==nullptr){
                            predecessor->right=current;
                            current=current->left;
                        }
                        else{/*predecessor->right==current*/
                            predecessor->right=nullptr;
                            func(current->data);
                            current=current->right;
                        }
                    }
                }
            }
            template<typename Lambda>
            void pre_order(Lambda&& func=Lambda{})const{
                pre_order_helper(root,func);
            }
            template<typename Lambda>
            void in_order(Lambda&& func=Lambda{})const{
                in_order_helper(root,func);
            }
            template<typename Lambda>
            void post_order(Lambda&& func=Lambda{})const{
                post_order_helper(root,func);
            }
            bool empty()const{
                return root==nullptr;
            }
            void clear(){
                clr(root);
                root=nullptr;
            }
            ~Tree(){DECTREES
                clr(root);
            }
            pointer get_root(){
                return root!=nullptr ? &root->data : nullptr;
            }
            const_iterator begin()const{
                return const_iterator(root);
            }
            const_iterator end()const{
                return const_iterator(nullptr);
            }
            iterator begin(){
                return iterator(root);
            }
            iterator end(){
                return iterator(nullptr);
            }
            template<typename Out,typename t,typename alloc,typename Comp> friend Out& operator<<(Out&,const Tree<t,alloc,Comp>&);
#undef root
    };
    template<typename Out,typename T,typename Alloc,typename Cmp>
    Out& operator<<(Out& stream,const Tree<T,Alloc,Cmp>& tree){
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