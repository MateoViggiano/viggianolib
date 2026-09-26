#pragma once
namespace mpv{
    template<typename T,typename NodeAlloc>
    class HandleBase{
        public:
            using allocator_type=mpv::rebind_alloc<NodeAlloc,T>;
            using value_type=T;
        protected:
            using AlNode_traits=mpv::allocator_traits<NodeAlloc>;
            using NodePtr=typename AlNode_traits::pointer;
            NodePtr node=nullptr;
            union{
                nontrivial_dummy_t _{};
                NodeAlloc node_alloc;
            };
            void destroy()noexcept{
                if(node!=nullptr){
                    DESTROY(node_alloc,node);
                    AlNode_traits::deallocate(node_alloc,node,1);
                    node_alloc.~NodeAlloc();
                    node=nullptr;
                }
            }
            constexpr NodePtr drop()noexcept{
                NodePtr aux=node;
                node_alloc.~NodeAlloc();
                node=nullptr;
                return aux;
            }
            constexpr void reset()noexcept{
                node_alloc.~NodeAlloc();
                node=nullptr;
            }
        public:
            constexpr HandleBase()noexcept:_{}{}
            constexpr HandleBase(const NodePtr node/*node!=nullptr*/,const NodeAlloc& node_alloc)noexcept:node(node),node_alloc(node_alloc){}
            constexpr HandleBase(HandleBase&& other)noexcept:node(other.node){
                if(node){
                    ::new(&node_alloc) NodeAlloc(mpv::move(other.node_alloc));
                    other.reset();
                }
            }
            constexpr HandleBase& operator=(HandleBase&& other)noexcept{
                if(this==&other) return *this;
                else if(other.node==nullptr){
                    destroy();
                }                
                else if(node==nullptr){
                    ::new(&node_alloc) NodeAlloc(mpv::move(other.node_alloc));
                    node=other.drop();
                }
                else{
                    DESTROY(node_alloc,node);
                    AlNode_traits::deallocate(node_alloc,node,1);
                    pocma(node_alloc,other.node_alloc);
                    node=other.drop();
                }
                return *this;
            }
            constexpr allocator_type get_allocator()const noexcept{
                return allocator_type(node_alloc);
            }
            constexpr value_type& value()noexcept{
                return node->data;
            }
            constexpr const value_type& value()const noexcept{
                return node->data;
            }
            constexpr explicit operator bool()const noexcept{
                return node!=nullptr;
            }
            constexpr bool empty()const noexcept{
                return node==nullptr;
            }
            ~HandleBase(){
                destroy();
            }
            template<typename> friend class Tree;
    };
    template<typename K,typename Val,typename NodeAlloc>
    class NodeHandle:public HandleBase<Val,NodeAlloc>{
        public:
            using base=HandleBase<Val,NodeAlloc>;
            using typename base::NodePtr;
            using key_type=typename Val::key_type;
            using mapped_type=typename Val::mapped_type;
            constexpr NodeHandle()noexcept=default;
            constexpr NodeHandle(NodeHandle&&)noexcept=default;
            constexpr NodeHandle& operator=(NodeHandle&&)noexcept=default;
            constexpr NodeHandle(const NodePtr node,const NodeAlloc& node_alloc)noexcept:base(node,node_alloc){}

            constexpr const key_type& key()const noexcept{
                return this->node->data.key;
            }
            constexpr key_type& key()noexcept{
                return const_cast<key_type&>(this->node->data.key);
            }
            constexpr const mapped_type& mapped()const noexcept{
                return this->node->data.val;
            }
            constexpr mapped_type& mapped()noexcept{
                return this->node->data.val;
            }
    };
    template<typename K,typename NodeAlloc>
    class NodeHandle<K,K,NodeAlloc>:public HandleBase<K,NodeAlloc>{
        public:
            using base=HandleBase<K,NodeAlloc>;
            using typename base::NodePtr;
            using key_type=K;
            constexpr NodeHandle()noexcept=default;
            constexpr NodeHandle(NodeHandle&&)noexcept=default;
            constexpr NodeHandle& operator=(NodeHandle&&)noexcept=default;
            constexpr NodeHandle(const NodePtr node,const NodeAlloc& node_alloc)noexcept:base(node,node_alloc){}
    };
}