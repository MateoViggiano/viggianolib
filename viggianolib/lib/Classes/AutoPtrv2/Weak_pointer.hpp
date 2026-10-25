#pragma once
namespace mpv{
    template<typename T>
    class wPtr:private PtrBase<T>{
        template<typename> friend class sPtr;
        template<typename> friend class wPtr;
        private:
            CtrlBlock* ctrl_block=nullptr;
            constexpr void release()noexcept{
                if(ctrl_block!=nullptr){
                    if(ctrl_block->DECWEAK==1){
                        THREADFENCEACQ;
                        ctrl_block->delete_this();
                    }
                }
            }
        public:
            constexpr wPtr()noexcept=default;
            constexpr wPtr(const wPtr& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(this->ctrl_block!=nullptr) this->ctrl_block->INCWEAK;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
            constexpr wPtr(const wPtr<U>& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(this->ctrl_block!=nullptr) this->ctrl_block->INCWEAK;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
            constexpr wPtr(wPtr<U>&& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                other.data=nullptr;
                other.ctrl_block=nullptr;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
            constexpr wPtr(const sPtr<U>& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(this->ctrl_block!=nullptr) this->ctrl_block->INCWEAK;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
            constexpr wPtr(sPtr<U>&& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(other.ctrl_block!=nullptr){
                    other.ctrl_block->INCWEAK;
                    if(other.ctrl_block->DECSHARED==1){// Despues de esta linea, todos los otros smart ptrs reconocen este ctrl_block como muerto, por lo que sharedCount no va a volver a cambiar. Entonces dentro de este if se puede destruir tranquilamente el objeto del ctrl_block.
                        THREADFENCEACQ;
                        other.ctrl_block->destroy_object();//  Aca es donde realmente muere
                        other.ctrl_block->DECWEAK;
                    }
                }
                other.data=nullptr;
                other.ctrl_block=nullptr;
            }
            constexpr wPtr& operator=(const wPtr& other)noexcept{
                if(other.ctrl_block!=nullptr) other.ctrl_block->INCWEAK;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                return *this;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
            constexpr wPtr& operator=(const wPtr<U>& other)noexcept{
                if(other.ctrl_block!=nullptr) other.ctrl_block->INCWEAK;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                return *this;
            }
            constexpr wPtr& operator=(wPtr&& other)noexcept{
                if(this==&other) return other;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                other.data=nullptr;
                other.ctrl_block=nullptr;
                return *this;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
            constexpr wPtr& operator=(wPtr<U>&& other)noexcept{
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                other.data=nullptr;
                other.ctrl_block=nullptr;
                return *this;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
            constexpr wPtr& operator=(const sPtr<U>& other)noexcept{
                if(other.ctrl_block!=nullptr) other.ctrl_block->INCWEAK;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                return *this;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
            constexpr wPtr& operator=(sPtr<U>&& other)noexcept{
                if(other.ctrl_block!=nullptr){
                    other.ctrl_block->INCWEAK;
                    if(other.ctrl_block->DECSHARED==1){
                        THREADFENCEACQ;
                        other.ctrl_block->destroy_object();
                        other.ctrl_block->DECWEAK;
                    }
                }
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                other.data=nullptr;
                other.ctrl_block=nullptr;
                return *this;
            }
            constexpr void reset()noexcept{
                release();
                this->data=nullptr;
                this->ctrl_block=nullptr;
                
            }
            constexpr size_t currentOwners()noexcept{ // returning>0 doesn't mean it won't expire after the call. Only a sPtr promotion can ensure that
                return ctrl_block==nullptr ? 0 : ctrl_block->LOADSHARED;
            }
            constexpr bool expired()noexcept{ // returning false doesn't mean it won't expire after the call. Only a sPtr promotion can ensure that
                return ctrl_block==nullptr ? true : !ctrl_block->LOADSHARED;
            }
            ~wPtr(){
                release();
            }
    };
}