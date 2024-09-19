#pragma once
namespace mpv{
	struct CtrlBlock COUNT_IT{
		size_t sharedCount;
		size_t weakCount;
		CtrlBlock():sharedCount(1),weakCount(1){
			INCCTRLBLOCKS
		}
		virtual void destroy_object()=0;
		virtual void delete_this()=0;
		~CtrlBlock(){
			DECCTRLBLOCKS
			//REMAININGCTRLBLOCKS
		}
		CtrlBlock(const CtrlBlock&)=delete;
		CtrlBlock& operator=(const CtrlBlock&)=delete;
	};
	template<typename T>
	struct Block_pointer final:CtrlBlock{
		remove_extent_t<T>* ptr;
		Block_pointer(remove_extent_t<T>* p):ptr(p){}
		void destroy_object()override{	if constexpr(is_no_size_array_v<T>) delete[] ptr;
										else 								delete ptr;  }
		void delete_this()override{ delete this; }
	};

	template<typename Tp,typename Del>
	struct Block_pointer_and_deleter final:CtrlBlock{
		CompressedPair<Del,Tp> compressed_pair;
		Block_pointer_and_deleter(Tp p,const Del& deleter):compressed_pair(arg1_tag{},deleter,p){}
		Block_pointer_and_deleter(Tp p,Del&& deleter):compressed_pair(arg1_tag{},static_cast<Del&&>(deleter),p){}
		void destroy_object()override{compressed_pair.getV1()(compressed_pair.V2);}
		void delete_this()override{ delete this; }
	};
	template<typename Tp,typename Del,typename Alloc>
	struct Block_pointer_deleter_and_allocator final:CtrlBlock{
		CompressedTriple<Del,Alloc,Tp> compressed_triple;
		Block_pointer_deleter_and_allocator(Tp p,const Del& deleter,Alloc&& alloc):compressed_triple(arg1and2_tag{},deleter,static_cast<Alloc&&>(alloc),p){}
		void destroy_object()override{ compressed_triple.getV1()(compressed_triple.getV3()); }
		void delete_this()override{
			using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_pointer_deleter_and_allocator>;
			BlockAlloc ctrlblock_alloc(compressed_triple.getV2());
			this->~Block_pointer_deleter_and_allocator();
			allocator_traits<BlockAlloc>::deallocate(ctrlblock_alloc,this,1);
		}
	};
	template<typename T>
	struct Block_object final:CtrlBlock{
		union{T obj;};
		template<typename... Args>
		constexpr Block_object(Args&&... args):obj(static_cast<Args&&>(args)...){}
		void destroy_object()override{if constexpr(is_class_v<T>) obj.~T();}
		void delete_this()override{ delete this; }
		~Block_object(){}
	};
	template<typename T,size_t N>
	struct Block_object<T[N]> final:CtrlBlock{
		union{T obj[N];};
		constexpr Block_object():obj{}{}
		constexpr Block_object(const T& init){
			for(size_t i=0;i<N;i++)
				new(obj+i) T(init);
		}
		void destroy_object()override{
			if constexpr(is_class_v<T>){
				for(size_t i=0;i<N;i++)
					obj[i].~T();
			}
        }
		void delete_this()override{ delete this; }
		~Block_object(){}
	};

	template<typename T>
	struct alignas(alignof(T)) Block_object<T[]> final:CtrlBlock{
		const size_t count;
		//T obj[];//Arreglo flexible
		constexpr Block_object(const size_t count):count(count){
			new(this+1) T[count]();
		}
		constexpr Block_object(const size_t count,const T& init):count(count){
			for(size_t i=0;i<count;i++)
				new(reinterpret_cast<T*>(this+1)+i) T(init);
		}
		void destroy_object()override{
			if constexpr(is_class_v<T>){
				for(size_t i=0;i<count;i++)
					(reinterpret_cast<T*>(this+1)+i)->~T();
			}
		}
		void delete_this()override{
			this->~Block_object();
			::operator delete(this);
		}
	};
	template<typename T,typename Alloc>
	struct Block_object_and_allocator final:CtrlBlock,EBCO<typename allocator_traits<Alloc>::template rebind_alloc<T>>{
		using TAlloc=typename allocator_traits<Alloc>::template rebind_alloc<T>;
		union{ T obj; };
		template<typename... Args>
		Block_object_and_allocator(const Alloc& alloc,Args&&... args):EBCO<TAlloc>(alloc){
			allocator_traits<TAlloc>::construct(this->get_val(),&obj,static_cast<Args&&>(args)...);
		}
		void destroy_object()override{
			allocator_traits<TAlloc>::destroy(this->get_val(),&obj);
		}
		void delete_this()override{
			using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator>;
			BlockAlloc ctrlblock_alloc(this->get_val());
			this->~Block_object_and_allocator();
			allocator_traits<BlockAlloc>::deallocate(ctrlblock_alloc,this,1);
		}
		~Block_object_and_allocator(){}
	};
	template<typename T,size_t N,typename Alloc>
	struct Block_object_and_allocator<T[N],Alloc> final:CtrlBlock,EBCO<typename allocator_traits<Alloc>::template rebind_alloc<T>>{
		using TAlloc=typename allocator_traits<Alloc>::template rebind_alloc<T>;
		union{ T obj[N]; };
		Block_object_and_allocator(const Alloc& alloc):EBCO<TAlloc>(alloc){
			for(size_t i=0;i<N;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),obj+i);
		}
		Block_object_and_allocator(const Alloc& alloc,const T& init):EBCO<TAlloc>(alloc){
			for(size_t i=0;i<N;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),obj+i,init);
		}
		void destroy_object()override{
			for(size_t i=0;i<N;i++)
				allocator_traits<TAlloc>::destroy(this->get_val(),obj+i);
		}
		void delete_this()override{
			using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator>;
			BlockAlloc ctrlblock_alloc(this->get_val());
			this->~Block_object_and_allocator();
			allocator_traits<BlockAlloc>::deallocate(ctrlblock_alloc,this,1);
		}
		~Block_object_and_allocator(){}
	};
	template<typename T,typename Alloc>
	struct alignas(alignof(T)) Block_object_and_allocator<T[],Alloc> final:CtrlBlock,EBCO<typename allocator_traits<Alloc>::template rebind_alloc<T>>{
		const size_t count;
		using TAlloc=typename allocator_traits<Alloc>::template rebind_alloc<T>;
		//T obj[];//Arreglo flexible
		Block_object_and_allocator(const size_t count,const Alloc& alloc):EBCO<TAlloc>(alloc),count(count){
			for(size_t i=0;i<count;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),reinterpret_cast<T*>(this+1)+i);
		}
		Block_object_and_allocator(const size_t count,const Alloc& alloc,const T& init):EBCO<TAlloc>(alloc),count(count){
			for(size_t i=0;i<count;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),reinterpret_cast<T*>(this+1)+i,init);
		}
		void destroy_object()override{
			for(size_t i=0;i<count;i++)
				allocator_traits<TAlloc>::destroy(this->get_val(),reinterpret_cast<T*>(this+1)+i);
		}
		void delete_this()override{
			using ByteAlloc=typename allocator_traits<Alloc>::template rebind_alloc<char>;
			ByteAlloc byte_allocator(this->get_val());
			this->~Block_object_and_allocator();
			allocator_traits<ByteAlloc>::deallocate(byte_allocator,reinterpret_cast<char*>(this),sizeof(Block_object_and_allocator)+count*sizeof(T));
		}
	};
	template<typename T> class wPtr;
	template<typename T>
	class sPtr:public PtrBase<T>{
		private:
			CtrlBlock* ctrl_block;
			sPtr(CtrlBlock* ctrl_block,remove_extent_t<T>* ptr):PtrBase<T>(ptr),ctrl_block(ctrl_block){// Solo las funciones de casteo pueden usar este constructor
				if(ctrl_block!=nullptr) ctrl_block->sharedCount++;
				INCSTRONGPOINTERS
			}
			void release(){
				if(this->ctrl_block!=nullptr){//	El puntero actual deja de apuntar a la memoria de la que era dueño, si esta no tiene mas dueños se libera
					if((--this->ctrl_block->sharedCount)==0){	//Si sharedCount llega a 0 borra la memoria a la que apunta
						this->ctrl_block->destroy_object();
						if(--this->ctrl_block->weakCount==0)	//Solo borra el contador de referencias si weakCount tambien es 0. En caso de que 
							this->ctrl_block->delete_this();		//weakCount no sea 0, ctrl_block no se borrara y el destructor del wPtr que lo mantiene vivo se ocupara de esto
					}
				}
			}
		public:
			using element_type=typename PtrBase<T>::element_type;
			template<typename U>
			sPtr(const wPtr<U>& weakPtr):PtrBase<T>(weakPtr.data),ctrl_block(weakPtr.ctrl_block){	//construye un puntero fuerte en base a uno debil
				INCSTRONGPOINTERS											
				if(weakPtr.ctrl_block==nullptr or weakPtr.ctrl_block->sharedCount==0){
					this->ctrl_block=nullptr;
					this->data=nullptr;
				}
				else
					this->ctrl_block->sharedCount++;
			}
			sPtr():PtrBase<T>(nullptr),ctrl_block(nullptr){INCSTRONGPOINTERS}
			explicit sPtr(decltype(nullptr)):PtrBase<T>(nullptr),ctrl_block(nullptr){INCSTRONGPOINTERS}
			template<typename U>
			explicit sPtr(U* dat):PtrBase<T>(dat),ctrl_block(dat!=nullptr ? new Block_pointer<remove_array_size_t<rebind_array_t<T,U>>>(dat) : nullptr){INCSTRONGPOINTERS}
			template<typename U,typename Del>
			sPtr(U* dat,Del&& deleter):PtrBase<T>(dat),ctrl_block(dat!=nullptr ? new Block_pointer_and_deleter<U*,Del>(dat,static_cast<Del&&>(deleter)) : nullptr){INCSTRONGPOINTERS}
			template<typename U,typename Del,typename Alloc>
			sPtr(U* dat,Del&& deleter,Alloc alloc):PtrBase<T>(dat),ctrl_block(nullptr){
				if(dat){
					typename allocator_traits<Alloc>::template rebind_alloc<Block_pointer_deleter_and_allocator<U*,Del,Alloc>> ctrlblock_alloc(alloc);
					ctrl_block=ctrlblock_alloc.allocate(1);
					::new(ctrl_block) Block_pointer_deleter_and_allocator<U*,Del,Alloc>(dat,static_cast<Del&&>(deleter),static_cast<Alloc&&>(alloc));
				}
			}
			template<typename U,typename Del>
			sPtr(uPtr<U,Del>&& unique):PtrBase<T>((typename uPtr<U,Del>::element_type*)unique.get()),
	ctrl_block(PtrBase<T>::data!=nullptr ? 
		new Block_pointer_and_deleter<typename uPtr<U,Del>::pointer,
										If_t<is_reference_v<Del>,
											reference_wrapper<remove_reference_t<Del>>//lo mismo que decltype(ref(unique.get_deleter()))
											,Del>>
				(unique.drop(),static_cast<Del&&>(unique.get_deleter())) 
		: nullptr){INCSTRONGPOINTERS}


			sPtr(const sPtr& other):PtrBase<T>(other.data),ctrl_block(other.ctrl_block){//Si no declaro este constructor el compilador lo declara de forma implicita y prefiere llamarlo antes instanciar un template, lo que genera comportamiento indefinido.
				if(this->ctrl_block!=nullptr) this->ctrl_block->sharedCount++;
				INCSTRONGPOINTERS
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			sPtr(const sPtr<U>& other):PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
				if(this->ctrl_block!=nullptr) this->ctrl_block->sharedCount++;
				INCSTRONGPOINTERS
			}
			sPtr(sPtr<T>&& other):PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
				other.data=nullptr;
				other.ctrl_block=nullptr;
				INCSTRONGPOINTERS
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			sPtr(sPtr<U>&& other):PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
				other.data=nullptr;
				other.ctrl_block=nullptr;
				INCSTRONGPOINTERS
			}
			template<typename U>
			sPtr(const sPtr<U>& other,remove_extent_t<T>* alias_ptr):PtrBase<T>(nullptr),ctrl_block(other.ctrl_block){
				if(this->ctrl_block!=nullptr){
					this->ctrl_block->sharedCount++;
					this->data=alias_ptr;
				}
				INCSTRONGPOINTERS
			}
			template<typename U>
			sPtr(sPtr<U>&& other,remove_extent_t<T>* alias_ptr):PtrBase<T>(other.ctrl_block!=nullptr ? alias_ptr : nullptr),ctrl_block(other.ctrl_block){
				other.ctrl_block=nullptr;
				other.data=nullptr;
				INCSTRONGPOINTERS
			}
			sPtr& operator=(decltype(nullptr)){
				this->release();
				PtrBase<T>::data=nullptr;
				this->ctrl_block=nullptr;
				return *this;
			}
			template<typename U>
			sPtr& operator=(U* dat){
				this->release();
				PtrBase<T>::data=dat;
				this->ctrl_block=dat!=nullptr ? new Block_pointer<remove_array_size_t<rebind_array_t<T,U>>>(dat) : nullptr;
				return *this;
			}
			template<typename U>
			sPtr& reasign(U* dat){
				this->release();
				PtrBase<T>::data=dat;
				this->ctrl_block=dat!=nullptr ? new Block_pointer<remove_array_size_t<rebind_array_t<T,U>>>(dat) : nullptr;
				return *this;
			}
			template<typename U,typename Del>
			sPtr& reasign(U* dat,Del&& deleter){
				this->release();
				PtrBase<T>::data=dat;
				this->ctrl_block=dat!=nullptr ? new Block_pointer_and_deleter<U*,Del>(dat,static_cast<Del&&>(deleter)) : nullptr;
				return *this;
			}
			template<typename U,typename Del,typename Alloc>
			sPtr& reasign(U* dat,Del&& deleter,Alloc alloc){
				this->release();
				PtrBase<T>::data=dat;
				if(dat){
					typename allocator_traits<Alloc>::template rebind_alloc<Block_pointer_deleter_and_allocator<T,Del,Alloc>> ctrlblock_alloc(alloc);
					ctrl_block=ctrlblock_alloc.allocate(1);
					new(ctrl_block) Block_pointer_deleter_and_allocator<U*,Del,Alloc>(dat,static_cast<Del&&>(deleter),static_cast<Alloc&&>(alloc));
				}
				else ctrl_block=nullptr;
				return *this;
			}
			template<typename U,typename Del,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			sPtr& operator=(uPtr<U,Del>&& unique){
				this->release();
				this->data=(typename uPtr<U,Del>::element_type*)unique.get();
				this->ctrl_block=this->data!=nullptr ? new Block_pointer_and_deleter<typename uPtr<U,Del>::pointer,
																	If_t<is_reference_v<Del>,
																		//decltype(ref(unique.get_deleter()))
																		reference_wrapper<remove_reference_t<Del>>
																		,Del>> (unique.drop(),static_cast<Del&&>(unique.get_deleter())) 
													 : nullptr;
				PtrBase<T>::data=unique.get();
				return *this;
			}
			sPtr& operator=(const sPtr& other){	//Con este pasa lo mismo que con el constructor de copia, asi que tambien lo tengo que declarar
				if(other.ctrl_block!=nullptr) other.ctrl_block->sharedCount++;				
				this->release();
				this->data=other.data;
				this->ctrl_block=other.ctrl_block;
				return *this;
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			sPtr& operator=(const sPtr<U>& other){
				if(other.ctrl_block!=nullptr) other.ctrl_block->sharedCount++;
				this->release();
				this->data=other.data;
				this->ctrl_block=other.ctrl_block;
				return *this;
			}
			sPtr& operator=(sPtr&& other){
				if(this==&other) return other;
				this->release();// No es necesario comprobar si es el mismo ctrl_block que el de other porq si se esta moviendo un sPtr a otro
				this->data=other.data;// y ninguno apunta a null, eso significa que sharedCount es mas de 2.
				this->ctrl_block=other.ctrl_block;
				other.data=nullptr;
				other.ctrl_block=nullptr;
				return *this;
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			sPtr& operator=(sPtr<U>&& other){
				this->release();
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
				release();
			}
			template<typename u> friend class sPtr;
			template<typename u> friend class wPtr;
			template<typename t,typename u> friend enable_if_t<have_same_extent_v<t,u>,sPtr<t>> static_ptr_cast(const sPtr<u>&);
			template<typename t,typename u> friend enable_if_t<have_same_extent_v<t,u>,sPtr<t>> reinterpret_ptr_cast(const sPtr<u>&);
			template<typename t,typename u> friend enable_if_t<have_same_extent_v<t,u>,sPtr<t>> const_ptr_cast(const sPtr<u>&);
			template<typename t,typename u> friend enable_if_t<have_same_extent_v<t,u>,sPtr<t>> dynamic_ptr_cast(const sPtr<u>&);
			template<typename t,typename u> friend enable_if_t<have_same_extent_v<t,u>,sPtr<t>> static_ptr_cast(sPtr<u>&&);
			template<typename t,typename u> friend enable_if_t<have_same_extent_v<t,u>,sPtr<t>> reinterpret_ptr_cast(sPtr<u>&&);
			template<typename t,typename u> friend enable_if_t<have_same_extent_v<t,u>,sPtr<t>> const_ptr_cast(sPtr<u>&&);
			template<typename t,typename u> friend enable_if_t<have_same_extent_v<t,u>,sPtr<t>> dynamic_ptr_cast(sPtr<u>&&);
			template<typename t,typename ...Args> friend constexpr enable_if_t<not is_array_v<t>,sPtr<t>> make_sPtr(Args&&...);
			template<typename t> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> make_sPtr();
			template<typename t> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> make_sPtr(const remove_extent_t<t>&);
			template<typename t> friend constexpr enable_if_t<is_no_size_array_v<t>,sPtr<t>> make_sPtr(size_t);
			template<typename t> friend constexpr enable_if_t<is_no_size_array_v<t>,sPtr<t>> make_sPtr(size_t,const remove_extent_t<t>&);
			template<typename t,typename Alloc,typename... Args> friend constexpr enable_if_t<not is_array_v<t>,sPtr<t>> alloc_sPtr(const Alloc&,Args&&...);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> alloc_sPtr(const Alloc&);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> alloc_sPtr(const Alloc&,const remove_extent_t<t>&);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_no_size_array_v<t>,sPtr<t>> alloc_sPtr(size_t,const Alloc&);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_no_size_array_v<t>,sPtr<t>> alloc_sPtr(size_t,const Alloc&,const remove_extent_t<t>&);
	};
	template<typename T,typename U>
	enable_if_t<have_same_extent_v<T,U>,sPtr<T>> static_ptr_cast(const sPtr<U>& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		return sPtr<T>(other.ctrl_block,static_cast<remove_extent_t<T>*>(&*other));
	}
	template<typename T,typename U>
	enable_if_t<have_same_extent_v<T,U>,sPtr<T>> reinterpret_ptr_cast(const sPtr<U>& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		return sPtr<T>(other.ctrl_block,reinterpret_cast<remove_extent_t<T>*>(&*other));
	}
	template<typename T,typename U>
	enable_if_t<have_same_extent_v<T,U>,sPtr<T>> const_ptr_cast(const sPtr<U>& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		return sPtr<T>(other.ctrl_block,const_cast<remove_extent_t<T>*>(&*other));
	}
	template<typename T,typename U>
	enable_if_t<have_same_extent_v<T,U>,sPtr<T>> dynamic_ptr_cast(const sPtr<U>& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		remove_extent_t<T>* ptr=dynamic_cast<remove_extent_t<T>*>(&*other);
		return ptr!=nullptr? sPtr<T>(other.ctrl_block,ptr) : sPtr<T>();
	}
	template<typename T,typename U>
	enable_if_t<have_same_extent_v<T,U>,sPtr<T>> static_ptr_cast(sPtr<U>&& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		sPtr<T> new_ptr;
		new_ptr.data=static_cast<remove_extent_t<T>*>(other.data);
		new_ptr.ctrl_block=other.ctrl_block;
		other.ctrl_block=nullptr;
		other.data=nullptr;
		return new_ptr;
	}
	template<typename T,typename U>
	enable_if_t<have_same_extent_v<T,U>,sPtr<T>> reinterpret_ptr_cast(sPtr<U>&& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		sPtr<T> new_ptr;
		new_ptr.data=reinterpret_cast<remove_extent_t<T>*>(other.data);
		new_ptr.ctrl_block=other.ctrl_block;
		other.ctrl_block=nullptr;
		other.data=nullptr;
		return new_ptr;
	}
	template<typename T,typename U>
	enable_if_t<have_same_extent_v<T,U>,sPtr<T>> const_ptr_cast(sPtr<U>&& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		sPtr<T> new_ptr;
		new_ptr.data=const_cast<remove_extent_t<T>*>(other.data);
		new_ptr.ctrl_block=other.ctrl_block;
		other.ctrl_block=nullptr;
		other.data=nullptr;
		return new_ptr;
	}
	template<typename T,typename U>
	enable_if_t<have_same_extent_v<T,U>,sPtr<T>> dynamic_ptr_cast(sPtr<U>&& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		sPtr<T> new_ptr;
		new_ptr.data=dynamic_cast<remove_extent_t<T>*>(other.data);
		if(new_ptr.data==nullptr)
			other.release();
		else
			new_ptr.ctrl_block=other.ctrl_block;
		other.ctrl_block=nullptr;
		other.data=nullptr;
		return new_ptr;
	}

	template<typename T,typename... Args>
	constexpr enable_if_t<!is_array_v<T>,sPtr<T>> make_sPtr(Args&& ...args){
		sPtr<T> ptr;
		ptr.ctrl_block=new Block_object<T>((Args&&)args...);
		ptr.data = &static_cast<Block_object<T>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> make_sPtr(){
		sPtr<T> ptr;
		ptr.ctrl_block=new Block_object<T>();
		ptr.data=static_cast<Block_object<T>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> make_sPtr(const remove_extent_t<T>& init){
		sPtr<T> ptr;
		ptr.ctrl_block=new Block_object<T>(init);
		ptr.data=static_cast<Block_object<T>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T>
	constexpr enable_if_t<is_no_size_array_v<T>,sPtr<T>> make_sPtr(size_t n){
		sPtr<T> ptr;
		ptr.ctrl_block=(CtrlBlock*)::operator new(sizeof(Block_object<T>)+sizeof(remove_extent_t<T>)*n);
		new(ptr.ctrl_block) Block_object<T>(n);
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object<T>));
		return ptr;
	}
	template<typename T>
	constexpr enable_if_t<is_no_size_array_v<T>,sPtr<T>> make_sPtr(size_t n,const remove_extent_t<T>& init){
		sPtr<T> ptr;
		ptr.ctrl_block=(CtrlBlock*)::operator new(sizeof(Block_object<T>)+sizeof(remove_extent_t<T>)*n);
		new(ptr.ctrl_block) Block_object<T>(n,init);
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object<T>));
		return ptr;
	}
	template<typename T,typename Alloc,typename... Args>
	constexpr enable_if_t<!is_array_v<T>,sPtr<T>> alloc_sPtr(const Alloc& alloc,Args&&... args){
		sPtr<T> ptr;
		using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator<T,Alloc>>;
		BlockAlloc block_allocator(alloc);
		ptr.ctrl_block=block_allocator.allocate(1);
		new(ptr.ctrl_block) Block_object_and_allocator<T,Alloc>(alloc,static_cast<Args&&>(args)...);
		ptr.data=&static_cast<Block_object_and_allocator<T,Alloc>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> alloc_sPtr(const Alloc& alloc){
		sPtr<T> ptr;
		using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator<T,Alloc>>;
		BlockAlloc block_allocator(alloc);
		ptr.ctrl_block=block_allocator.allocate(1);
		new(ptr.ctrl_block) Block_object_and_allocator<T,Alloc>(alloc);
		ptr.data=static_cast<Block_object_and_allocator<T,Alloc>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> alloc_sPtr(const Alloc& alloc,const remove_extent_t<T>& init){
		sPtr<T> ptr;
		using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator<T,Alloc>>;
		BlockAlloc block_allocator(alloc);
		ptr.ctrl_block=block_allocator.allocate(1);
		new(ptr.ctrl_block) Block_object_and_allocator<T,Alloc>(alloc,init);
		ptr.data=static_cast<Block_object_and_allocator<T,Alloc>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_no_size_array_v<T>,sPtr<T>> alloc_sPtr(size_t n,const Alloc& alloc){
		sPtr<T> ptr;
		typename allocator_traits<Alloc>::template rebind_alloc<char> byte_allocator(alloc);
		ptr.ctrl_block=reinterpret_cast<CtrlBlock*>(byte_allocator.allocate(sizeof(Block_object_and_allocator<T,Alloc>)+n*sizeof(remove_extent_t<T>)));
		new(ptr.ctrl_block) Block_object_and_allocator<T,Alloc>(n,alloc);
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object_and_allocator<T,Alloc>));
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_no_size_array_v<T>,sPtr<T>> alloc_sPtr(size_t n,const Alloc& alloc,const remove_extent_t<T>& init){
		sPtr<T> ptr;
		typename allocator_traits<Alloc>::template rebind_alloc<char> byte_allocator(alloc);
		ptr.ctrl_block=reinterpret_cast<CtrlBlock*>(byte_allocator.allocate(sizeof(Block_object_and_allocator<T,Alloc>)+n*sizeof(remove_extent_t<T>)));
		new(ptr.ctrl_block) Block_object_and_allocator<T,Alloc>(n,alloc,init);
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object_and_allocator<T,Alloc>));
		return ptr;
	}
}