#pragma once 
namespace mpv {
namespace ts{
	template<typename T, size_t N>
	class smart_ptr<T[N]> COUNT_IT{
	protected:
		CtrlBlock* ctrl_block;
		T* data;
		smart_ptr(CtrlBlock* control_block, T* dat) :ctrl_block(control_block), data(dat) {}
	public:
		T& operator*()const{
			return *data;
		}
		T* operator->()const{
			return data;
		}
		T& operator[](size_t n)const{
			return data[n];
		}
		bool operator==(const smart_ptr<T[N]>& other)const{
			return this->data==other.data;
		}
		bool operator!=(const smart_ptr<T[N]>& other)const{
			return this->data!=other.data;
		}
		bool operator<=(const smart_ptr<T[N]>& other)const{
			return this->data<=other.data;
		}
		bool operator>=(const smart_ptr<T[N]>& other)const{
			return this->data>=other.data;
		}
		bool operator<(const smart_ptr<T[N]>& other)const{
			return this->data<other.data;
		}
		bool operator>(const smart_ptr<T[N]>& other)const{
			return this->data>other.data;
		}
		using data_type=T;
		static constexpr size_t size=sizeof(T[N]);
		static constexpr size_t len=N;
	};
	template<typename T, size_t N>
	class sPtr<T[N]> :public smart_ptr<T[N]> {
		template<typename D> friend class sPtr;
		template<typename D> friend class wPtr;
		friend class sPtr<T[]>;
		friend class wPtr<T[]>;
	private:
		sPtr(CtrlBlock* ctrl_block, T* ptr) :smart_ptr<T[N]>(ctrl_block, ptr) {
			if (ctrl_block!=nullptr) ctrl_block->sharedCount++;
			INCSTRONGPOINTERS
		}
		void release_ownership() {
			if (this->ctrl_block != nullptr) {//	El puntero actual deja de apuntar a la memoria de la que era dueño, si esta no tiene mas dueños se libera
				if ((--this->ctrl_block->sharedCount) == 0) {	//Si sharedCount llega a 0 borra la memoria a la que apunta
					this->ctrl_block->release();
					if (--this->ctrl_block->weakCount == 0)	//Solo borra el contador de referencias si weakCount tambien es 0. En caso de que 
						delete this->ctrl_block;		//weakCount no sea 0, ctrl_block no se borrara y el destructor del wPtr que lo mantiene vivo se ocupara de esto
				}
			}
		}
	public:
		template<typename D>
		sPtr(const wPtr<D[N]>& weakPtr) :smart_ptr<T[N]>(weakPtr.ctrl_block, weakPtr.data) {	//construye un puntero fuerta en base a uno debil
			INCSTRONGPOINTERS
				if (weakPtr.ctrl_block == nullptr or weakPtr.ctrl_block->sharedCount == 0)
					this->ctrl_block = nullptr;
				else
					this->ctrl_block->sharedCount++;
		}
		sPtr() :smart_ptr<T[N]>(nullptr, nullptr) {
			INCSTRONGPOINTERS
		}
		sPtr(const sPtr<T[N]>& other) :smart_ptr<T[N]>(other.ctrl_block, other.data) {//Si no declaro este constructor el compilador lo declara de forma implicita y prefiere llamarlo antes que a la version template, lo que genera comportamiento indefinido.
			if (this->ctrl_block != nullptr) this->ctrl_block->sharedCount++;
			INCSTRONGPOINTERS
		}
		template<typename D>
		sPtr(const sPtr<D[N]>& other) : smart_ptr<T[N]>(other.ctrl_block, other.data) {
			if (this->ctrl_block != nullptr) this->ctrl_block->sharedCount++;
			INCSTRONGPOINTERS
		}
		sPtr(T* dat) : smart_ptr<T[N]>(dat != nullptr ? new Block_pointer<T[]>(dat) : nullptr, dat) {INCSTRONGPOINTERS}//si no defino esta funcion no puedo asignar el puntero a nullptr.
		template<typename D>
		sPtr(D* dat) : smart_ptr<T[N]>(dat != nullptr ? new Block_pointer<D[]>(dat) : nullptr, dat) {INCSTRONGPOINTERS}
		template<typename D>
		sPtr(sPtr<D[N]>&& other) : smart_ptr<T[N]>(other.ctrl_block, other.data) {
			other.data = nullptr;
			other.ctrl_block = nullptr;
			INCSTRONGPOINTERS
		}
		sPtr<T[N]>& operator=(const sPtr<T[N]>& other) {	//Con este pasa lo mismo que con el operador de copia, asi que tambien lo tengo que declarar
			if (other.ctrl_block != nullptr) other.ctrl_block->sharedCount++;
			this->release_ownership();
			this->data = other.data;
			this->ctrl_block = other.ctrl_block;
			return *this;
		}
		template<typename D>
		sPtr<T[N]>& operator=(const sPtr<D[N]>& other) {
			if (other.ctrl_block != nullptr) other.ctrl_block->sharedCount++;
			this->release_ownership();
			this->data = other.data;
			this->ctrl_block = other.ctrl_block;
			return *this;
		}
		template<typename D>
		sPtr<T[N]>& operator=(D* dat) {
			this->release_ownership();
			smart_ptr<T[N]>::data = dat;
			this->ctrl_block = dat != nullptr ? new Block_pointer<D[N]>(dat) : nullptr;
			return *this;
		}
		template<typename D>
		sPtr<T[N]>& operator=(sPtr<D[N]>&& other) {
			if (this == &other)
				return *this;
			this->release_ownership();
			this->data = other.data;
			this->ctrl_block = other.ctrl_block;
			other.data = nullptr;
			other.ctrl_block = nullptr;
			return *this;
		}
		size_t currentOwners()const {
			return this->ctrl_block == nullptr ? 0 : (size_t)this->ctrl_block->sharedCount;
		}
		~sPtr() {
			DECSTRONGPOINTERS
			//REMAININGSTRONGPOINTERS
			release_ownership();
		}
		template<typename t, typename D, size_t n>friend sPtr<t[n]> staticPtr_cast(const sPtr<D[n]>& other);
		template<typename t, typename D, size_t n>friend sPtr<t[n]> reinterpretPtr_cast(const sPtr<D[n]>& other);
		template<typename t, typename D, size_t n>friend sPtr<t[n]> constPtr_cast(const sPtr<D[n]>& other);
		template<typename t, typename D, size_t n>friend sPtr<t[n]> dynamicPtr_cast(const sPtr<D[n]>& other);
		template<typename t, typename ...Args>friend sPtr<t> make_sPtr(Args&& ...args);
	};
	template<typename T, size_t N>
	class wPtr<T[N]> :public smart_ptr<T[N]> {
		template<typename D> friend class sPtr;
		template<typename D> friend class wPtr;
		friend class sPtr<T[]>;
		friend class wPtr<T[]>;
	public:
		wPtr() :smart_ptr<T[N]>(nullptr) {
			INCWEAKPOINTERS
		}
		wPtr(const wPtr<T[N]>& other) :smart_ptr<T[N]>(other.ctrl_block, other.data) {
			if (this->ctrl_block != nullptr) this->ctrl_block->weakCount++;
			INCWEAKPOINTERS
		}
		template<typename D>
		wPtr(const wPtr<D[N]>& other) : smart_ptr<T[N]>(other.ctrl_block, other.data) {
			if (this->ctrl_block != nullptr) this->ctrl_block->weakCount++;
			INCWEAKPOINTERS
		}
		template<typename D>
		wPtr(const sPtr<D[N]>& other) : smart_ptr<T[N]>(other.ctrl_block, other.data) {
			if (this->ctrl_block != nullptr) this->ctrl_block->weakCount++;
			INCWEAKPOINTERS
		}
		wPtr<T[N]>& operator=(const wPtr<T[N]>& other) {
			if (other.ctrl_block != nullptr) other.ctrl_block->weakCount++;
			if (this->ctrl_block != nullptr) {
				if (--this->ctrl_block->weakCount == 0)
					delete this->ctrl_block;
			}
			this->ctrl_block = other.ctrl_block;
			return *this;
		}
		template<typename D>
		wPtr<T[N]>& operator=(const wPtr<D[N]>& other) {
			if (other.ctrl_block != nullptr) other.ctrl_block->weakCount++;
			if (this->ctrl_block != nullptr) {
				if (--this->ctrl_block->weakCount == 0)
					delete this->ctrl_block;
			}
			this->ctrl_block = other.ctrl_block;
			return *this;
		}
		template<typename D>
		wPtr<T[N]>& operator=(const sPtr<D[N]>& other) {
			if (other.ctrl_block != nullptr) other.ctrl_block->weakCount++;
			if (this->ctrl_block != nullptr) {
				if (--this->ctrl_block->weakCount == 0)
					delete this->ctrl_block;
			}
			this->ctrl_block = other.ctrl_block;
			return *this;
		}
		size_t currentOwners() {
			return this->ctrl_block == nullptr ? 0 : (size_t)this->ctrl_block->sharedCount;
		}
		bool alive() {
			return this->ctrl_block != nullptr and this->ctrl_block->sharedCount > 0;
		}
		~wPtr() {
			DECWEAKPOINTERS
				//REMAININGWEAKPOINTERS
				if (this->ctrl_block != nullptr) {
					if (--this->ctrl_block->weakCount == 0)
						delete this->ctrl_block;
				}
		}
	};
	template<typename T, typename D, size_t N>
	sPtr<T[N]> staticPtr_cast(const sPtr<D[N]>& other) {
		return sPtr<T[N]>(other.ctrl_block, static_cast<T*>(&*other));
	}
	template<typename T, typename D, size_t N>
	sPtr<T[N]> reinterpretPtr_cast(const sPtr<D[N]>& other) {
		return sPtr<T[N]>(other.ctrl_block, reinterpret_cast<T*>(&*other));
	}
	template<typename T, typename D, size_t N>
	sPtr<T[N]> constPtr_cast(const sPtr<D[N]>& other) {
		return sPtr<T[N]>(other.ctrl_block, const_cast<T*>(&*other));
	}
	template<typename T, typename D, size_t N>
	sPtr<T[N]> dynamicPtr_cast(const sPtr<D[N]>& other) {
		T* ptr = dynamic_cast<T*>(&*other);
		return ptr != nullptr ? sPtr<T[N]>(other.ctrl_block, ptr) : sPtr<T[N]>();
	}
	template<typename T,typename ...Args>
	sPtr<T> make_sPtr(Args&& ...args){
		sPtr<T> ptr;
		ptr.ctrl_block=new Block_object<T>((Args&&)args...);
		ptr.data = ((typename mpv::remove_arrayness<T>::type*)(((Block_object<T>*)ptr.ctrl_block)->obj));
		return ptr;
	}
}
}