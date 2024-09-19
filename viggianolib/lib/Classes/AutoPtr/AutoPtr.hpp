#pragma once
namespace mpv{
	template<typename T>class sPtr;
	template<typename T>class wPtr;
	template<typename T>class uPtr;
	struct CtrlBlock COUNT_IT{
		size_t sharedCount;
		size_t weakCount;
		CtrlBlock():sharedCount(1),weakCount(1){
			INCCTRLBLOCKS
		}
		virtual void release()=0;
		virtual ~CtrlBlock(){
			DECCTRLBLOCKS
			//REMAININGCTRLBLOCKS
		}
	};
	template<typename T>
	struct Block_pointer:public CtrlBlock{
		T* ptr;
		Block_pointer(T* p):ptr(p){}
		void release()override{delete ptr;}
		~Block_pointer()override{}
	};
	template<typename T>
	struct Block_object:public CtrlBlock{
		char obj[sizeof(T)];
		template<typename... Args>
		Block_object(Args&&... args){new(obj)T(static_cast<Args&&>(args)...);}
		void release()override{(*((T*)obj)).~T();}
		~Block_object()override{}
	};
	template<typename T,size_t N>
	struct Block_object<T[N]>:public CtrlBlock{
		char obj[sizeof(T)*N];
		template<typename... Args>
		Block_object(Args&&... args){new(obj)T[N]{static_cast<Args&&>(args)...};}
		void release()override{
            for(size_t i=0;i<N;i++)
                (*(((T*)obj)+i)).~T();
        }
		~Block_object()override{}
	};
	template<typename T>
	struct Block_pointer<T[]>:public CtrlBlock{
		T* ptr;
		Block_pointer(T* p):ptr(p){}
		void release()override{delete[] ptr;}
		~Block_pointer()override{}
	};
	template<typename T>
	class smart_ptr COUNT_IT /*abstract*/{
		protected:
			CtrlBlock* ctrl_block;
			T* data;
			smart_ptr(CtrlBlock* control_block,T* dat):ctrl_block(control_block),data(dat){}
		public:
			T& operator*()const{
				return *data;
			}
			T* operator->()const{
				return data;
			}
			bool operator==(const smart_ptr<T>& other)const{
				return this->data==other.data;
			}
			bool operator!=(const smart_ptr<T>& other)const{
				return this->data!=other.data;
			}
			bool operator<=(const smart_ptr<T>& other)const{
				return this->data<=other.data;
			}
			bool operator>=(const smart_ptr<T>& other)const{
				return this->data>=other.data;
			}
			bool operator<(const smart_ptr<T>& other)const{
				return this->data<other.data;
			}
			bool operator>(const smart_ptr<T>& other)const{
				return this->data>other.data;
			}
			using data_type=T;
	};
	template<typename T>
	class sPtr:public smart_ptr<T>{
		template<typename D> friend class sPtr;
		template<typename D> friend class wPtr;
		private:
			sPtr(CtrlBlock* ctrl_block,T* ptr):smart_ptr<T>(ctrl_block,ptr){// Solo las funciones de casteo pueden usar este constructor
				if(ctrl_block!=nullptr) ctrl_block->sharedCount++;
				INCSTRONGPOINTERS
			}
			void release_ownership(){
				if(this->ctrl_block!=nullptr){//	El puntero actual deja de apuntar a la memoria de la que era dueño, si esta no tiene mas dueños se libera
					if((--this->ctrl_block->sharedCount)==0){	//Si sharedCount llega a 0 borra la memoria a la que apunta
						this->ctrl_block->release();
						if(--this->ctrl_block->weakCount==0)	//Solo borra el contador de referencias si weakCount tambien es 0. En caso de que 
							delete this->ctrl_block;		//weakCount no sea 0, ctrl_block no se borrara y el destructor del wPtr que lo mantiene vivo se ocupara de esto
					}
				}
			}
		public:
			template<typename D>
			sPtr(const wPtr<D>& weakPtr):smart_ptr<T>(weakPtr.ctrl_block,weakPtr.data){	//construye un puntero fuerta en base a uno debil
				INCSTRONGPOINTERS											
				if(weakPtr.ctrl_block==nullptr or weakPtr.ctrl_block->sharedCount==0)
					this->ctrl_block=nullptr;
				else
					this->ctrl_block->sharedCount++;
			}
			sPtr():smart_ptr<T>(nullptr,nullptr){
				INCSTRONGPOINTERS
			}
			sPtr(const sPtr& other):smart_ptr<T>(other.ctrl_block,other.data){//Si no declaro este constructor el compilador lo declara de forma implicita y prefiere llamarlo antes que a la version template, lo que genera comportamiento indefinido.
				if(this->ctrl_block!=nullptr) this->ctrl_block->sharedCount++;
				INCSTRONGPOINTERS
			}
			template<typename D>
			sPtr(const sPtr<D>& other):smart_ptr<T>(other.ctrl_block,other.data){
				if(this->ctrl_block!=nullptr) this->ctrl_block->sharedCount++;
				INCSTRONGPOINTERS
			}
			sPtr(T* dat):smart_ptr<T>(dat!=nullptr ? new Block_pointer<T>(dat) : nullptr,dat){INCSTRONGPOINTERS}//si no defino esta funcion no puedo asignar el puntero a nullptr.
			template<typename D>
			sPtr(D* dat):smart_ptr<T>(dat!=nullptr ? new Block_pointer<D>(dat) : nullptr,dat){INCSTRONGPOINTERS}
			template<typename D>
			sPtr(sPtr<D>&& other):smart_ptr<T>(other.ctrl_block,other.data){
				other.data=nullptr;
				other.ctrl_block=nullptr;
				INCSTRONGPOINTERS
			}
			sPtr& operator=(const sPtr& other){	//Con este pasa lo mismo que con el constructor de copia, asi que tambien lo tengo que declarar
				if(other.ctrl_block!=nullptr) other.ctrl_block->sharedCount++;				
				this->release_ownership();
				this->data=other.data;
				this->ctrl_block=other.ctrl_block;
				return *this;
			}
			template<typename D>
			sPtr& operator=(const sPtr<D>& other){
				if(other.ctrl_block!=nullptr) other.ctrl_block->sharedCount++;
				this->release_ownership();
				this->data=other.data;
				this->ctrl_block=other.ctrl_block;
				return *this;
			}
			template<typename D>
			sPtr& operator=(D* dat){
				this->release_ownership();
				smart_ptr<T>::data=dat;
				this->ctrl_block=dat!=nullptr ? new Block_pointer<D>(dat) : nullptr;
				return *this;
			}
			template<typename D>
			sPtr& operator=(sPtr<D>&& other){
				if(this==&other)
					return *this;
				this->release_ownership();
				this->data=other.data;
				this->ctrl_block=other.ctrl_block;
				other.data=nullptr;
				other.ctrl_block=nullptr;
				return *this;
			}
			size_t currentOwners()const{
				return this->ctrl_block==nullptr? 0 : this->ctrl_block->sharedCount;
			}
			~sPtr(){
				DECSTRONGPOINTERS
				//REMAININGSTRONGPOINTERS
				release_ownership();
			}
			template<typename D>sPtr(const sPtr<D[]>& other)=delete;
			template<typename D,size_t N>sPtr(const sPtr<D[N]>& other)=delete;
			template<typename D>sPtr(const wPtr<D[]>& other)=delete;
			template<typename D,size_t N>sPtr(const wPtr<D[N]>& other)=delete;
			template<typename D>sPtr& operator=(const sPtr<D[]>& other)=delete;
			template<typename D,size_t N>sPtr& operator=(const sPtr<D[N]>& other)=delete;
			template<typename D>sPtr& operator=(const wPtr<D[]>& other)=delete;
			template<typename D,size_t N>sPtr& operator=(const wPtr<D[N]>& other)=delete;
			template<typename D>sPtr(sPtr<D[]>&& other)=delete;
			template<typename D,size_t N>sPtr(sPtr<D[N]>&& other)=delete;
			template<typename D>sPtr(wPtr<D[]>&& other)=delete;
			template<typename D,size_t N>sPtr(wPtr<D[N]>&& other)=delete;
			template<typename D>sPtr& operator=(sPtr<D[]>&& other)=delete;
			template<typename D,size_t N>sPtr& operator=(sPtr<D[N]>&& other)=delete;
			template<typename D>sPtr& operator=(wPtr<D[]>&& other)=delete;
			template<typename D,size_t N>sPtr& operator=(wPtr<D[N]>&& other)=delete;

			template<typename A,typename ...Args>friend sPtr<A> make_sPtr(Args&& ...args);
			template<typename t,typename D>friend sPtr<t> staticPtr_cast(const sPtr<D>& other);
			template<typename t,typename D>friend sPtr<t> reinterpretPtr_cast(const sPtr<D>& other);
			template<typename t,typename D>friend sPtr<t> constPtr_cast(const sPtr<D>& other);
			template<typename t,typename D>friend sPtr<t> dynamicPtr_cast(const sPtr<D>& other);
	};

	template<typename T>
 	class wPtr:public smart_ptr<T>{
 		friend class sPtr<T>;
 		public:
 			wPtr():smart_ptr<T>(nullptr){
 				INCWEAKPOINTERS
 			}
 			wPtr(const wPtr<T>& other):smart_ptr<T>(other.ctrl_block,other.data){
 				if(this->ctrl_block!=nullptr) this->ctrl_block->weakCount++;
 				INCWEAKPOINTERS
 			}
			template<typename D>
			wPtr(const wPtr<D>& other):smart_ptr<T>(other.ctrl_block,other.data){
 				if(this->ctrl_block!=nullptr) this->ctrl_block->weakCount++;
 				INCWEAKPOINTERS
 			}
			template<typename D>
 			wPtr(const sPtr<D>& other):smart_ptr<T>(other.ctrl_block,other.data){
 				if(this->ctrl_block!=nullptr) this->ctrl_block->weakCount++;
 				INCWEAKPOINTERS
 			}
 			wPtr& operator=(const wPtr<T>& other){
 				if(other.ctrl_block!=nullptr) other.ctrl_block->weakCount++;
 				if(this->ctrl_block!=nullptr){
 					if(--this->ctrl_block->weakCount==0)
 						delete this->ctrl_block;
 				}
 				this->ctrl_block=other.ctrl_block;
				return *this;
 			}
			template<typename D>
 			wPtr& operator=(const wPtr<D>& other){
				if(other.ctrl_block!=nullptr) other.ctrl_block->weakCount++;
 				if(this->ctrl_block!=nullptr){
 					if(--this->ctrl_block->weakCount==0)
 						delete this->ctrl_block;
 				}
 				this->ctrl_block=other.ctrl_block;
				return *this;
 			}
			template<typename D>
 			wPtr& operator=(const sPtr<D>& other){
 				if(other.ctrl_block!=nullptr) other.ctrl_block->weakCount++;
 				if(this->ctrl_block!=nullptr){
 					if(--this->ctrl_block->weakCount==0)
 						delete this->ctrl_block;
 				}
 				this->ctrl_block=other.ctrl_block;
				return *this;
 			}
 			size_t currentOwners(){
 				return this->ctrl_block==nullptr? 0 : this->ctrl_block->sharedCount;
 			}
 			bool alive(){
				return this->ctrl_block!=nullptr and this->ctrl_block->sharedCount>0;
			}
 			~wPtr(){
 				DECWEAKPOINTERS
 				//REMAININGWEAKPOINTERS
 				if(this->ctrl_block!=nullptr){
 					if(--this->ctrl_block->weakCount==0)
 						delete this->ctrl_block;
 				}
 			}
			template<typename D>wPtr(const sPtr<D[]>& other)=delete;
			template<typename D,size_t N>wPtr(const sPtr<D[N]>& other)=delete;
			template<typename D>wPtr(const wPtr<D[]>& other)=delete;
			template<typename D,size_t N>wPtr(const wPtr<D[N]>& other)=delete;
			template<typename D>wPtr& operator=(const sPtr<D[]>& other)=delete;
			template<typename D,size_t N>wPtr& operator=(const sPtr<D[N]>& other)=delete;
			template<typename D>wPtr& operator=(const wPtr<D[]>& other)=delete;
			template<typename D,size_t N>wPtr& operator=(const wPtr<D[N]>& other)=delete;
			template<typename D>wPtr(sPtr<D[]>&& other)=delete;
			template<typename D,size_t N>wPtr(sPtr<D[N]>&& other)=delete;
			template<typename D>wPtr(wPtr<D[]>&& other)=delete;
			template<typename D,size_t N>wPtr(wPtr<D[N]>&& other)=delete;
			template<typename D>wPtr& operator=(sPtr<D[]>&& other)=delete;
			template<typename D,size_t N>wPtr& operator=(sPtr<D[N]>&& other)=delete;
			template<typename D>wPtr& operator=(wPtr<D[]>&& other)=delete;
			template<typename D,size_t N>wPtr& operator=(wPtr<D[N]>&& other)=delete;
 	};
	template<typename T,typename D>
	sPtr<T> staticPtr_cast(const sPtr<D>& other){
		return sPtr<T>(other.ctrl_block,static_cast<T*>(&*other));
	}
	template<typename T,typename D>
	sPtr<T> reinterpretPtr_cast(const sPtr<D>& other){
		return sPtr<T>(other.ctrl_block,reinterpret_cast<T*>(&*other));
	}
	template<typename T,typename D>
	sPtr<T> constPtr_cast(const sPtr<D>& other){
		return sPtr<T>(other.ctrl_block,const_cast<T*>(&*other));
	}
	template<typename T,typename D>
	sPtr<T> dynamicPtr_cast(const sPtr<D>& other){
		T* ptr=dynamic_cast<T*>(&*other);
		return ptr!=nullptr? sPtr<T>(other.ctrl_block,ptr) : sPtr<T>();
	}
}

