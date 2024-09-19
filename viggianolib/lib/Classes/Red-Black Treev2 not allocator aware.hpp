#pragma once
#include "Stacks_Queues.hpp"
#define RED 1
#define BLACK 0
namespace mpv{
    template<typename T,typename Cmp=less<T>>
    class Tree COUNT_IT{
        private:
            struct Node;
            struct BaseNode COUNT_IT{
                Node* left = nullptr;
                Node* right = nullptr;
                Node* prev = nullptr;
                bool color;
                BaseNode():color(BLACK){}
                BaseNode(bool color):color(color){}
            };
            struct Node:public BaseNode{
                T data;
                Node(const T& dat,bool color=RED):BaseNode(color),data(dat){}
                Node(T&& dat,bool color=RED):BaseNode(color),data((T&&)dat){}
            };
            Node* root;
            BaseNode TNULL=BLACK;
            Cmp cmp;
            void clr(Node* node){
                if(node==(Node*)&TNULL)
                    return;
                else{
                    clr(node->left);
                    clr(node->right);
                    delete node;
                }
            }
#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
            void pre_order(Node* node){
                if(node==(Node*)&TNULL)
                    return;
                std::cout<<node->data<<',';
                pre_order(node->left);
                pre_order(node->right);
            }
            void in_order(Node* node){
                if(node==(Node*)&TNULL)
                    return;
                in_order(node->left);
                std::cout<<node->data<<',';
                in_order(node->right);
            }
            void post_order(Node* node){
                if(node==(Node*)&TNULL)
                    return;
                post_order(node->left);
                post_order(node->right);
                std::cout<<node->data<<',';
            }
#endif
            void insert_fixup(Node* x){
                while(x->prev!=nullptr and x->prev->color==RED){
                    if(x->prev==x->prev->prev->left){															//If the father is a left node
                        if(x->prev->prev->right->color==RED){				//case 1:X.uncle==RED
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
                        if(x->prev->prev->left->color==RED){					//case 1:X.uncle==RED
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
            void delete_fixup(Node* x){
                while(x!=root and x->color==BLACK){
                    if(x==x->prev->left){
                        Node* w=x->prev->right;
                        if(w->color==RED){
                            w->color=BLACK;
                            x->prev->color=RED;
                            left_rotate(x->prev);
                            w=x->prev->right;
                        }
                        if(w->right->color==BLACK and w->left->color==BLACK){
                            w->color=RED;
                            x=x->prev;
                        }
                        else{
                            if(w->left->color==RED and w->right->color==BLACK){
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
                        Node* w=x->prev->left;
                        if(w->color==RED){
                            w->color=BLACK;
                            x->prev->color=RED;
                            right_rotate(x->prev);
                            w=x->prev->left;
                        }
                        if(w->left->color==BLACK and w->left->color==BLACK){
                            w->color=RED;
                            x=x->prev;
                        }
                        else{
                            if(w->right->color==RED and w->left->color==BLACK){
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
            void right_rotate(Node* x){
                Node* y=x->left;
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
                if(y->right!=(Node*)&TNULL)
                    y->right->prev=x;
                y->right=x;
                x->prev=y;
            }
            void left_rotate(Node* x){
                Node* y=x->right;
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
                if(y->left!=(Node*)&TNULL)
                    y->left->prev=x;
                y->left=x;
                x->prev=y;
            }
            Node* minimum(Node* x){
                while(x->left!=(Node*)&TNULL)
                    x=x->left;
                return x;
            }
            void transplant(Node* u,Node* v){//Desenlaza u con su padre y enlaza al padre con v
                if(u->prev==nullptr)
                    this->root=v;
                else{
                    if(u==u->prev->left)
                        u->prev->left=v;
                    else
                        u->prev->right=v;
                }
                v->prev=u->prev;
            }
        public:
            Tree():root((Node*)&TNULL){INCTREES
            }
            Tree(const Tree& other):root((Node*)&TNULL){INCTREES
                for(const T& val:other)
                    this->insert(val);
            }
            Tree(Tree&& other):root(other.root!=(Node*)&other.TNULL ? other.root : (Node*)&TNULL){INCTREES
                other.root=(Node*)&other.TNULL;
            }
            Tree(std::initializer_list<T> initializerList):root((Node*)&TNULL){INCTREES
                for(const T& v:initializerList)
                    this->insert((T&&)v);
            }
            bool insert(const T& data){
                Node* x;
                if(root==(Node*)&TNULL){
                    root=new Node(data);
                    root->right=(Node*)&TNULL;
                    root->left=(Node*)&TNULL;
                    x=root;
                }
                else{
                    x=root;
                    while(true){
                        if(cmp(data,x->data)){
                            if(x->left==(Node*)&TNULL){
                                x->left=new Node(data);
                                x->left->right=(Node*)&TNULL;
                                x->left->left=(Node*)&TNULL;
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else if(cmp(x->data,data)){
                            if(x->right==(Node*)&TNULL){
                                x->right=new Node(data);
                                x->right->right=(Node*)&TNULL;
                                x->right->left=(Node*)&TNULL;
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                        else return false;
                    }
                }
                this->insert_fixup(x);
                return true;
            }
            bool insert(T&& data){
                Node* x;
                if(root==(Node*)&TNULL){
                    root=new Node((T&&)data);
                    root->right=(Node*)&TNULL;
                    root->left=(Node*)&TNULL;
                    x=root;
                }
                else{
                    x=root;
                    while(true){
                        if(cmp(data,x->data)){
                            if(x->left==(Node*)&TNULL){
                                x->left=new Node((T&&)data);
                                x->left->right=(Node*)&TNULL;
                                x->left->left=(Node*)&TNULL;
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else if(cmp(x->data,data)){
                            if(x->right==(Node*)&TNULL){
                                x->right=new Node((T&&)data);
                                x->right->right=(Node*)&TNULL;
                                x->right->left=(Node*)&TNULL;
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                        else return false;
                    }
                }
                this->insert_fixup(x);
                return true;
            }
            template<typename... Args>
            bool insert(Args&&... args){
                Node* x;
                T data(static_cast<Args&&>(args)...);
                if(root==(Node*)&TNULL){
                    root=new Node((T&&)data);
                    root->right=(Node*)&TNULL;
                    root->left=(Node*)&TNULL;
                    x=root;
                }
                else{
                    x=root;
                    while(true){
                        if(cmp(data,x->data)){
                            if(x->left==(Node*)&TNULL){
                                x->left=new Node((T&&)data);
                                x->left->right=(Node*)&TNULL;
                                x->left->left=(Node*)&TNULL;
                                x->left->prev=x;
                                x=x->left;
                                break;							
                            }
                            else
                                x=x->left;
                        }
                        else if(cmp(x->data,data)){
                            if(x->right==(Node*)&TNULL){
                                x->right=new Node((T&&)data);
                                x->right->right=(Node*)&TNULL;
                                x->right->left=(Node*)&TNULL;
                                x->right->prev=x;
                                x=x->right;
                                break;							
                            }
                            else
                                x=x->right;
                        }
                        else return false;
                    }
                }
                this->insert_fixup(x);
                return true;
            }
            bool has(const T& data){
                if(root==(Node*)&TNULL)
                    return false;
                Node* x=root;
                while(true){
                    if(cmp(data,x->data)){
                        if(x->left==(Node*)&TNULL)
                            return false;						
                        else
                            x=x->left;
                    }
                    else if(cmp(x->data,data)){
                        if(x->right==(Node*)&TNULL)
                            return false;							
                        else
                            x=x->right;
                    }
                    else return true;
                }
            }
            T* get(const T& data){
                if(root==(Node*)&TNULL)
                    return nullptr;
                Node* x=root;
                while(true){
                    if(cmp(data,x->data)){
                        if(x->left==(Node*)&TNULL)
                            return nullptr;						
                        else
                            x=x->left;
                    }
                    else if(cmp(x->data,data)){
                        if(x->right==(Node*)&TNULL)
                            return nullptr;							
                        else
                            x=x->right;
                    }
                    else return &x->data;
                }
            }
            bool del(const T& data){
                if(root==(Node*)&TNULL)
                    return false;
                Node* z=root;
                while(true){
                    if(cmp(data,z->data)){
                        if(z->left==(Node*)&TNULL)
                            return false;
                        else
                            z=z->left;
                    }
                    else if(cmp(z->data,data)){
                        if(z->right==(Node*)&TNULL)
                            return false;
                        else
                            z=z->right;
                    }
                    else break;
                }
                Node* y,* x;
                bool z_original_color=z->color;
                if(z->left==(Node*)&TNULL){
                    x=z->right;
                    transplant(z,z->right);
                }
                else if(z->right==(Node*)&TNULL){
                    x=z->left;
                    transplant(z,z->left);
                }
                else{
                    y=minimum(z->right);
                    z_original_color=y->color;
                    x=y->right;
                    if(y->prev==z) 
                        x->prev=y;
                    else{
                        transplant(y, y->right);
                        y->right=z->right;
                        y->right->prev=y;
                    }
                    transplant(z,y);
                    y->left=z->left;
                    y->left->prev=y;
                    y->color=z->color;
                }
                delete z;
                if (z_original_color==BLACK)
                    delete_fixup(x);
                return true;
            }
			template<typename Lambda>
			bool any(Lambda&& func){
                for(T& val:*this)
                    if(func(val))return 1;
                return 0;
			}
			template<typename Lambda>
			void forEach(Lambda&& func){
				for(T& val:*this)
					func(val);
			}
			template<typename Lambda>
			auto map(Lambda&& func){
				Tree<decltype(func(root->data))> new_tree;
				for(T& val:*this)
					new_tree.insert(func(val));
				return new_tree;
			}
			template<typename Lambda>
			Tree filter(Lambda&& func){
				Tree new_tree;
				for(T& val:*this){
					if(func(val))new_tree.insert(val);
				}
				return new_tree;
			}
#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
            void preOrder(){
                std::cout<<'[';
                pre_order(root);
                std::cout<<']';
            }
            void inOrder(){
                std::cout<<'[';
                in_order(root);
                std::cout<<']';
            }
            void postOrder(){
                std::cout<<'[';
                post_order(root);
                std::cout<<']';
            }
#endif
            void clear(){
                clr(root);
                root=(Node*)&TNULL;
            }
            ~Tree(){DECTREES
                clr(root);
                REMAININGTREES
            }
            T* get_root(){
                return root!=nullptr ? &root->data : nullptr;
            }
            class iterator COUNT_IT{
                private:
                    Node* ptr;
                    Stack<Node*> stack;
                public:
                    iterator(Node* p):ptr(p){
                    	if(ptr==nullptr or ptr->right==nullptr){
                            ptr=nullptr;
                            return;
                        }
                        while(ptr->left->left!=nullptr){
                        //while(ptr->left!)
                            stack.push(ptr);
                            ptr=ptr->left;
                        }
                    }
                    iterator(const iterator&)=delete;
                    iterator(iterator&& other):ptr(other.ptr),stack((Stack<Node*>&&)other.stack){}
                    T& operator*(){
                        return ptr->data;
                    }
                    T* operator->(){
                        return &(ptr->data);
                    }
                    iterator& operator++(){
                        if(ptr->right->right==nullptr and not stack.empty())
                            ptr=stack.pop();
                        else if(ptr->right->right!=nullptr){
                            ptr=ptr->right;
                            while(ptr->left->left!=nullptr){
                                stack.push(ptr);
                                ptr=ptr->left;
                            }
                        }
                        else //ptr->right->right==nullptr and stack->empty()
                            ptr=nullptr;
                        return *this;
                    }
                    iterator operator++(int){
                        iterator aux=*this;
                        if(ptr->right->right==nullptr and not stack.empty())
                            ptr=stack.pop();
                        else if(ptr->right->right!=nullptr){
                            ptr=ptr->right;
                            while(ptr->left->left!=nullptr){
                                stack.push(ptr);
                                ptr=ptr->left;
                            }
                        }
                        else //ptr->right->right==nullptr and stack->empty()
                            ptr=nullptr;
                        return aux;
                    }
                    bool operator==(const iterator& other)const{
                        return this->ptr==other.ptr;
                    }
                    bool operator!=(const iterator& other)const{
                        return this->ptr!=other.ptr;
                    }
                    ~iterator(){}
            };
            iterator begin()const{
                return iterator(root);
            }
            iterator end()const{
                return iterator(nullptr);
            }
    };
#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
    template<typename T>
    std::ostream& operator<<(std::ostream& stream,const Tree<T>& tree){
        stream<<'[';
        typename Tree<T>::iterator it=tree.begin();
        typename Tree<T>::iterator it_end=tree.end();
        while(it!=it_end){
            stream<<*it;
            if(++it!=it_end)
                stream<<", ";
        }
        return stream<<']';
    }
#endif
}