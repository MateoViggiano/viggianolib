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
            mpv::Optional<NodeAlloc> node_alloc;
            void destroy()noexcept{
                if(node!=nullptr){
                    DESTROY(*node_alloc,node);
                    AlNode_traits::deallocate(*node_alloc,node,1);
                }
            }
            constexpr NodePtr drop()noexcept{
                NodePtr aux=node;
                node_alloc.reset();
                node=nullptr;
                return aux;
            }
        public:
            constexpr HandleBase()noexcept=default;
            HandleBase(const NodePtr node,const NodeAlloc& node_alloc)noexcept:node(node),node_alloc(node_alloc){}
            HandleBase(HandleBase&& other)noexcept:node(other.node),node_alloc(mpv::move(other.node_alloc)){
                other.node=nullptr;
                other.node_alloc.reset();
            }
            HandleBase& operator=(HandleBase&& other)noexcept{
                if(this==&other) return *this;
                destroy();
                node=other.node;
                if constexpr(is_move_assignable_v<NodeAlloc>){
                    if(AlNode_traits::propagate_on_container_move_assignment::value || !node_alloc.has_value()){
                        node_alloc=mpv::move(other.node_alloc);
                    }
                }
                other.node=nullptr;
                other.node_alloc.reset();
                return *this;
            }
            constexpr allocator_type get_allocator()const noexcept{
                return allocator_type(*node_alloc);
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
            NodeHandle(NodeHandle&&)noexcept=default;
            NodeHandle& operator=(NodeHandle&&)noexcept=default;
            NodeHandle(const NodePtr node,const NodeAlloc& node_alloc)noexcept:base(node,node_alloc){}

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
            NodeHandle(NodeHandle&&)noexcept=default;
            NodeHandle& operator=(NodeHandle&&)noexcept=default;
            NodeHandle(const NodePtr node,const NodeAlloc& node_alloc)noexcept:base(node,node_alloc){}
    };
}