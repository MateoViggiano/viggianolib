#pragma once
namespace mpv{
    template<typename T>
    class wPtr:public PtrBase<T>{
        private:
            CtrlBlock* ctrl_block;
            void release(){
                if(ctrl_block!=nullptr){
                    if(--ctrl_block->weakCount==0){
                        ctrl_block->delete_this();
                    }
                }
            }
        public:
            wPtr():PtrBase<T>(nullptr),ctrl_block(nullptr){}
            wPtr(const wPtr& other):PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(this->ctrl_block!=nullptr) this->ctrl_block->weakCount++;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
            wPtr(const wPtr<U>& other):PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(this->ctrl_block!=nullptr) this->ctrl_block->weakCount++;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U>,int> = 0>
            wPtr(wPtr<U>&& other):PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                other.data=nullptr;
                other.ctrl_block=nullptr;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U>,int> = 0>
            wPtr(const sPtr<U>& other):PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(this->ctrl_block!=nullptr) this->ctrl_block->weakCount++;
            }
            template<typename U,enable_if_t<have_same_extent_v<T,U>,int> = 0>
            wPtr(sPtr<U>&& other):PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
                if(other.ctrl_block!=nullptr){
                    other.ctrl_block->weakCount++;
                    if(--other.ctrl_block->sharedCount==0){
                        other.ctrl_block->destroy_object();
                        other.ctrl_block->weakCount--;
                    }
                }
                other.data=nullptr;
                other.ctrl_block=nullptr;
            }
            wPtr& operator=(const wPtr& other){
                if(this==&other) return other;
                if(other.ctrl_block!=nullptr) other.ctrl_block->weakCount++;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                return *this;
            }
            template<typename U>
            wPtr& operator=(const wPtr<U>& other){
                if(other.ctrl_block!=nullptr) other.ctrl_block->weakCount++;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                return *this;
            }
            wPtr& operator=(wPtr&& other){
                if(this==&other) return other;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                other.data=nullptr;
                other.ctrl_block=nullptr;
                return *this;
            }
            template<typename U>
            wPtr& operator=(wPtr<U>&& other){
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                other.data=nullptr;
                other.ctrl_block=nullptr;
                return *this;
            }
            template<typename U>
            wPtr& operator=(const sPtr<U>& other){
                if(other.ctrl_block!=nullptr) other.ctrl_block->weakCount++;
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                return *this;
            }
            template<typename U>
            wPtr& operator=(sPtr<U>&& other){
                if(other.ctrl_block!=nullptr){
                    other.ctrl_block->weakCount++;
                    if(--other.ctrl_block->sharedCount==0){
                        other.ctrl_block->destroy_object();
                        other.ctrl_block->weakCount--;
                    }
                }
                this->release();
                this->ctrl_block=other.ctrl_block;
                this->data=other.data;
                other.data=nullptr;
                other.ctrl_block=nullptr;
                return *this;
            }
            size_t currentOwners(){
                return ctrl_block==nullptr ? 0 : ctrl_block->sharedCount;
            }
            bool expired(){
                return ctrl_block==nullptr ? true : !ctrl_block->sharedCount;
            }
            ~wPtr(){
                release();
            }
    };
}