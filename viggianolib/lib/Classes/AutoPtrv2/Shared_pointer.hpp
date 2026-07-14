#pragma once
namespace mpv{
	struct CtrlBlock{
#ifdef USE_ATOMIC
		std::atomic<size_t> sharedCount;
		std::atomic<size_t> weakCount;
		// size_t sharedCount;
		// size_t weakCount;		
#else
		size_t sharedCount;
		size_t weakCount;
#endif
		constexpr CtrlBlock():sharedCount(1),weakCount(1){
			//INCCTRLBLOCKS
		}
		virtual void destroy_object()noexcept=0;
		virtual void delete_this()noexcept=0;
		~CtrlBlock()noexcept{
			//DECCTRLBLOCKS
			//REMAININGCTRLBLOCKS
		}
		CtrlBlock(const CtrlBlock&)=delete;
		CtrlBlock& operator=(const CtrlBlock&)=delete;
	};
	template<typename T>
	struct Block_pointer final:CtrlBlock{
		remove_extent_t<T>* ptr;
		constexpr Block_pointer(remove_extent_t<T>* p):ptr(p){}
		void destroy_object()noexcept override{	if constexpr(is_no_size_array_v<T>) delete[] ptr;
										else 								delete ptr;  }
		void delete_this()noexcept override{ delete this; }
	};

	template<typename Tp,typename Del>
	struct Block_pointer_and_deleter final:CtrlBlock{
		CompressedPair<Del,Tp> compressed_pair;
		constexpr Block_pointer_and_deleter(Tp p,const Del& deleter):compressed_pair(arg1_tag{},deleter,p){}
		constexpr Block_pointer_and_deleter(Tp p,Del&& deleter):compressed_pair(arg1_tag{},static_cast<Del&&>(deleter),p){}
		void destroy_object()noexcept override{compressed_pair.getV1()(compressed_pair.V2);}
		void delete_this()noexcept override{ delete this; }
	};
	template<typename Tp,typename Del,typename Alloc>
	struct Block_pointer_deleter_and_allocator final:CtrlBlock{
		CompressedTriple<Del,Alloc,Tp> compressed_triple;
		constexpr Block_pointer_deleter_and_allocator(Tp p,const Del& deleter,const Alloc& alloc):compressed_triple(arg1and2_tag{},deleter,alloc,p){}
		void destroy_object()noexcept override{ compressed_triple.getV1()(compressed_triple.getV3()); }
		void delete_this()noexcept override{
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
		void destroy_object()noexcept override{if constexpr(is_class_v<T>) obj.~T();}
		void delete_this()noexcept override{ delete this; }
		~Block_object()noexcept{}
	};
	template<typename T,size_t N>
	struct Block_object<T[N]> final:CtrlBlock{
		union{T obj[N];};
		constexpr Block_object():obj{}{}
		constexpr Block_object(const T& init){
			for(size_t i=0;i<N;i++)
				new(obj+i) T(init);
		}
		void destroy_object()noexcept override{
			if constexpr(is_class_v<T>){
				for(size_t i=0;i<N;i++)
					obj[i].~T();
			}
        }
		void delete_this()noexcept override{ delete this; }
		~Block_object()noexcept{}
	};

	template<typename T>
	struct Block_object<T[]> final:CtrlBlock{
		const size_t count;
		//T obj[];//Arreglo flexible
		constexpr Block_object(const size_t count):count(count){
			new(this+1) T[count]();
		}
		constexpr Block_object(const size_t count,const T& init):count(count){
			for(size_t i=0;i<count;i++)
				new(reinterpret_cast<T*>(this+1)+i) T(init);
		}
		void destroy_object()noexcept override{
			if constexpr(is_class_v<T>){
				for(size_t i=0;i<count;i++)
					(reinterpret_cast<T*>(this+1)+i)->~T();
			}
		}
		void delete_this()noexcept override{
			this->~Block_object();
			::operator delete(this);
		}
	};
	template<typename T,typename Alloc>
	struct Block_object_and_allocator final:CtrlBlock,EBCO<typename allocator_traits<Alloc>::template rebind_alloc<T>>{
		using TAlloc=typename allocator_traits<Alloc>::template rebind_alloc<T>;
		union{ T obj; };
		template<typename... Args>
		constexpr Block_object_and_allocator(const Alloc& alloc,Args&&... args):EBCO<TAlloc>(alloc){
			allocator_traits<TAlloc>::construct(this->get_val(),&obj,static_cast<Args&&>(args)...);
		}
		void destroy_object()noexcept override{
			allocator_traits<TAlloc>::destroy(this->get_val(),&obj);
		}
		void delete_this()noexcept override{
			using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator>;
			BlockAlloc ctrlblock_alloc(this->get_val());
			this->~Block_object_and_allocator();
			allocator_traits<BlockAlloc>::deallocate(ctrlblock_alloc,this,1);
		}
		~Block_object_and_allocator()noexcept{}
	};
	template<typename T,size_t N,typename Alloc>
	struct Block_object_and_allocator<T[N],Alloc> final:CtrlBlock,EBCO<typename allocator_traits<Alloc>::template rebind_alloc<T>>{
		using TAlloc=typename allocator_traits<Alloc>::template rebind_alloc<T>;
		union{ T obj[N]; };
		constexpr Block_object_and_allocator(const Alloc& alloc):EBCO<TAlloc>(alloc){
			for(size_t i=0;i<N;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),obj+i);
		}
		constexpr Block_object_and_allocator(const Alloc& alloc,const T& init):EBCO<TAlloc>(alloc){
			for(size_t i=0;i<N;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),obj+i,init);
		}
		void destroy_object()noexcept override{
			for(size_t i=0;i<N;i++)
				allocator_traits<TAlloc>::destroy(this->get_val(),obj+i);
		}
		void delete_this()noexcept override{
			using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator>;
			BlockAlloc ctrlblock_alloc(this->get_val());
			this->~Block_object_and_allocator();
			allocator_traits<BlockAlloc>::deallocate(ctrlblock_alloc,this,1);
		}
		~Block_object_and_allocator()noexcept{}
	};
	template<typename T,typename Alloc>
	struct Block_object_and_allocator<T[],Alloc> final:CtrlBlock,EBCO<typename allocator_traits<Alloc>::template rebind_alloc<T>>{
		const size_t count;
		using TAlloc=typename allocator_traits<Alloc>::template rebind_alloc<T>;
		//T obj[];//Arreglo flexible
		constexpr Block_object_and_allocator(const size_t count,const Alloc& alloc):EBCO<TAlloc>(alloc),count(count){
			for(size_t i=0;i<count;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),reinterpret_cast<T*>(this+1)+i);
		}
		constexpr Block_object_and_allocator(const size_t count,const Alloc& alloc,const T& init):EBCO<TAlloc>(alloc),count(count){
			for(size_t i=0;i<count;i++)
				allocator_traits<TAlloc>::construct(this->get_val(),reinterpret_cast<T*>(this+1)+i,init);
		}
		void destroy_object()noexcept override{
			for(size_t i=0;i<count;i++)
				allocator_traits<TAlloc>::destroy(this->get_val(),reinterpret_cast<T*>(this+1)+i);
		}
		void delete_this()noexcept override{
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
			CtrlBlock* ctrl_block=nullptr;
			template<typename U>
			constexpr void setp(U* const ptr){
				TemporaryOwner<remove_array_size_t<rebind_array_t<T,U>>> temp_owner(ptr);
				ctrl_block=new Block_pointer<remove_array_size_t<rebind_array_t<T,U>>>(ptr);
				this->data=ptr;
				temp_owner.ptr=nullptr;
			}
			template<typename Up,typename Del>
			constexpr void setpd(const Up ptr,Del dt){
				TemporaryOwnerDel<Up,Del> temp_owner(ptr,dt);
				ctrl_block=new Block_pointer_and_deleter<Up,Del>(ptr,static_cast<Del&&>(dt));
				this->data=unfancy(ptr);
				temp_owner.call_deleter=false;
			}
			template<typename Up,typename Del,typename Alloc>
			constexpr void setpda(const Up ptr,Del dt,Alloc alloc){
				using CtrlBlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_pointer_deleter_and_allocator<Up,Del,Alloc>>;
				TemporaryOwnerDel<Up,Del> temp_owner(ptr,dt);
				CtrlBlockAlloc ctrlblock_alloc(alloc);
				AllocConstructPtr allocation_temp_owner(ctrlblock_alloc);
				allocation_temp_owner.allocate();
				new(unfancy(allocation_temp_owner.ptr)) Block_pointer_deleter_and_allocator<Up,Del,Alloc>(temp_owner.ptr,static_cast<Del&&>(dt),alloc);
				this->data=unfancy(temp_owner.ptr);
				this->ctrl_block=unfancy(allocation_temp_owner.ptr);
				temp_owner.call_deleter=false;
				allocation_temp_owner.ptr=nullptr;
			} 
			constexpr sPtr(CtrlBlock* ctrl_block,remove_extent_t<T>* ptr)noexcept:PtrBase<T>(ptr),ctrl_block(ctrl_block){// Solo las funciones de casteo pueden usar este constructor
				if(ctrl_block!=nullptr) ctrl_block->sharedCount++;
			}
			constexpr void release()noexcept{
				if(this->ctrl_block!=nullptr){//	El puntero actual deja de apuntar a la memoria de la que era dueño, si esta no tiene mas dueños se libera
					if((--this->ctrl_block->sharedCount)==0){	//Si sharedCount llega a 0 borra la memoria a la que apunta
						this->ctrl_block->destroy_object();
						if(--this->ctrl_block->weakCount==0)	//Solo borra el contador de referencias si weakCount tambien es 0. En caso de que 
							this->ctrl_block->delete_this();		//weakCount no sea 0, ctrl_block no se borrara y el destructor del wPtr que lo mantiene vivo se ocupara de esto
					}
				}
				this->data=nullptr;
				this->ctrl_block=nullptr;
			}
		public:
			using element_type=typename PtrBase<T>::element_type;
			template<typename U>
			constexpr sPtr(const wPtr<U>& weakPtr)noexcept:PtrBase<T>(weakPtr.data),ctrl_block(weakPtr.ctrl_block){	//construye un puntero fuerte en base a uno debil
				if(weakPtr.ctrl_block==nullptr || weakPtr.ctrl_block->sharedCount==0){
					this->ctrl_block=nullptr;
					this->data=nullptr;
				}
				else
					this->ctrl_block->sharedCount++;
			}
			constexpr sPtr()noexcept{}
			constexpr sPtr(decltype(nullptr))noexcept{}
			template<typename U>
			constexpr explicit sPtr(U* dat){
				if(dat) setp(dat);
			}
			template<typename U,typename Del>
			constexpr sPtr(U* dat,Del deleter){
				if(dat) setpd(dat,static_cast<Del&&>(deleter));
			}
			template<typename U,typename Del,typename Alloc>
			constexpr sPtr(U* dat,Del deleter,Alloc alloc):PtrBase<T>(dat),ctrl_block(nullptr){
				if(dat) setpda(dat,static_cast<Del&&>(deleter),alloc);
			}
			template<typename U,typename Del>
			constexpr sPtr(uPtr<U,Del>&& unique){
				if(unique.get()){
					setpd<typename uPtr<U,Del>::pointer,
						If_t<is_reference_v<Del>,
							reference_wrapper<remove_reference_t<Del>>//lo mismo que decltype(ref(unique.get_deleter()))
							,Del>>(unique.drop(),static_cast<Del&&>(unique.get_deleter()));
				}
			}
			constexpr sPtr(const sPtr& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){//Si no declaro este constructor el compilador lo declara de forma implicita y prefiere llamarlo antes instanciar un template, lo que genera comportamiento indefinido.
				if(this->ctrl_block!=nullptr) this->ctrl_block->sharedCount++;
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr sPtr(const sPtr<U>& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
				if(this->ctrl_block!=nullptr) this->ctrl_block->sharedCount++;
			}
			constexpr sPtr(sPtr<T>&& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
				other.data=nullptr;
				other.ctrl_block=nullptr;
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr sPtr(sPtr<U>&& other)noexcept:PtrBase<T>(other.data),ctrl_block(other.ctrl_block){
				other.data=nullptr;
				other.ctrl_block=nullptr;
			}
			template<typename U>
			constexpr sPtr(const sPtr<U>& other,remove_extent_t<T>* alias_ptr)noexcept:PtrBase<T>(nullptr),ctrl_block(other.ctrl_block){
				if(this->ctrl_block!=nullptr){
					this->ctrl_block->sharedCount++;
					this->data=alias_ptr;
				}
			}
			template<typename U>
			constexpr sPtr(sPtr<U>&& other,remove_extent_t<T>* alias_ptr)noexcept:PtrBase<T>(other.ctrl_block!=nullptr ? alias_ptr : nullptr),ctrl_block(other.ctrl_block){
				other.ctrl_block=nullptr;
				other.data=nullptr;
			}
			constexpr sPtr& operator=(decltype(nullptr))noexcept{
				this->release();
				return *this;
			}
			template<typename U>
			constexpr sPtr& operator=(U* dat){
				this->release();
				if(dat) setp(dat);
				return *this;
			}
			template<typename U>
			constexpr sPtr& reasign(U* dat){
				this->release();
				if(dat) setp(dat);
				return *this;
			}
			template<typename U,typename Del>
			constexpr sPtr& reasign(U* dat,Del&& deleter){
				this->release();
				if(dat) setpd(dat,static_cast<Del&&>(deleter));
				return *this;
			}
			template<typename U,typename Del,typename Alloc>
			constexpr sPtr& reasign(U* dat,Del&& deleter,Alloc alloc){
				this->release();
				if(dat) setpda(dat,static_cast<Del&&>(deleter),alloc);
				return *this;
			}
			template<typename U,typename Del,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr sPtr& operator=(uPtr<U,Del>&& unique){
				this->release();
				if(unique.get()){
					setpd<typename uPtr<U,Del>::pointer,
						If_t<is_reference_v<Del>,
							reference_wrapper<remove_reference_t<Del>>//lo mismo que decltype(ref(unique.get_deleter()))
							,Del>>(unique.drop(),static_cast<Del&&>(unique.get_deleter()));
				}
				return *this;
			}
			constexpr sPtr& operator=(const sPtr& other)noexcept{	//Con este pasa lo mismo que con el constructor de copia, asi que tambien lo tengo que declarar
				if(other.ctrl_block!=nullptr) other.ctrl_block->sharedCount++;				
				this->release();
				this->data=other.data;
				this->ctrl_block=other.ctrl_block;
				return *this;
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr sPtr& operator=(const sPtr<U>& other)noexcept{
				if(other.ctrl_block!=nullptr) other.ctrl_block->sharedCount++;
				this->release();
				this->data=other.data;
				this->ctrl_block=other.ctrl_block;
				return *this;
			}
			constexpr sPtr& operator=(sPtr&& other)noexcept{
				if(this==&other) return *this;
				this->release();// No es necesario comprobar si es el mismo ctrl_block que el de other porq si se esta moviendo un sPtr a otro
				this->data=other.data;// y ninguno apunta a null, eso significa que sharedCount es mas de 2.
				this->ctrl_block=other.ctrl_block;
				other.data=nullptr;
				other.ctrl_block=nullptr;
				return *this;
			}
			template<typename U,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr sPtr& operator=(sPtr<U>&& other)noexcept{
				this->release();
				this->data=other.data;
				this->ctrl_block=other.ctrl_block;
				other.data=nullptr;
				other.ctrl_block=nullptr;
				return *this;
			}
			constexpr size_t currentOwners()const noexcept{
				return this->ctrl_block==nullptr? 0 : (size_t)this->ctrl_block->sharedCount;
			}
			~sPtr()noexcept{
				release();
			}
			template<typename u> friend class sPtr;
			template<typename u> friend class wPtr;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> static_ptr_cast(const sPtr<u>&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> reinterpret_ptr_cast(const sPtr<u>&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> const_ptr_cast(const sPtr<u>&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> dynamic_ptr_cast(const sPtr<u>&);
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> static_ptr_cast(sPtr<u>&&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> reinterpret_ptr_cast(sPtr<u>&&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> const_ptr_cast(sPtr<u>&&)noexcept;
			template<typename t,typename u> friend constexpr enable_if_t<have_same_extent_v<t,u>,sPtr<t>> dynamic_ptr_cast(sPtr<u>&&);
			template<typename t,typename ...Args> constexpr friend enable_if_t<!is_array_v<t>,sPtr<t>> make_sPtr(Args&&...);
			template<typename t> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> make_sPtr();
			template<typename t> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> make_sPtr(const remove_extent_t<t>&);
			template<typename t> friend enable_if_t<is_no_size_array_v<t>,sPtr<t>> make_sPtr(size_t);
			template<typename t> friend enable_if_t<is_no_size_array_v<t>,sPtr<t>> make_sPtr(size_t,const remove_extent_t<t>&);
			template<typename t,typename Alloc,typename... Args> friend constexpr enable_if_t<!is_array_v<t>,sPtr<t>> alloc_sPtr(const Alloc&,Args&&...);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> alloc_sPtr(const Alloc&);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_size_array_v<t>,sPtr<t>> alloc_sPtr(const Alloc&,const remove_extent_t<t>&);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_no_size_array_v<t>,sPtr<t>> alloc_sPtr(size_t,const Alloc&);
			template<typename t,typename Alloc> friend constexpr enable_if_t<is_no_size_array_v<t>,sPtr<t>> alloc_sPtr(size_t,const Alloc&,const remove_extent_t<t>&);
	};
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> static_ptr_cast(const sPtr<U>& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		return sPtr<T>(other.ctrl_block,static_cast<remove_extent_t<T>*>(&*other));
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> reinterpret_ptr_cast(const sPtr<U>& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		return sPtr<T>(other.ctrl_block,reinterpret_cast<remove_extent_t<T>*>(&*other));
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> const_ptr_cast(const sPtr<U>& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		return sPtr<T>(other.ctrl_block,const_cast<remove_extent_t<T>*>(&*other));
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> dynamic_ptr_cast(const sPtr<U>& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		remove_extent_t<T>* ptr=dynamic_cast<remove_extent_t<T>*>(&*other);
		return ptr!=nullptr? sPtr<T>(other.ctrl_block,ptr) : sPtr<T>();
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> static_ptr_cast(sPtr<U>&& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		sPtr<T> new_ptr;
		new_ptr.data=static_cast<remove_extent_t<T>*>(other.data);
		new_ptr.ctrl_block=other.ctrl_block;
		other.ctrl_block=nullptr;
		other.data=nullptr;
		return new_ptr;
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> reinterpret_ptr_cast(sPtr<U>&& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		sPtr<T> new_ptr;
		new_ptr.data=reinterpret_cast<remove_extent_t<T>*>(other.data);
		new_ptr.ctrl_block=other.ctrl_block;
		other.ctrl_block=nullptr;
		other.data=nullptr;
		return new_ptr;
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> const_ptr_cast(sPtr<U>&& other)noexcept{//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
		sPtr<T> new_ptr;
		new_ptr.data=const_cast<remove_extent_t<T>*>(other.data);
		new_ptr.ctrl_block=other.ctrl_block;
		other.ctrl_block=nullptr;
		other.data=nullptr;
		return new_ptr;
	}
	template<typename T,typename U>
	constexpr enable_if_t<have_same_extent_v<T,U>,sPtr<T>> dynamic_ptr_cast(sPtr<U>&& other){//static_assert(have_same_extent_v<T,U>,"Both types must have same array qualifier. Ej: T=int[],U=float[] / T=Object[32],U=long[32] / T=double,U=char");
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
		ptr.ctrl_block=new Block_object<remove_cv_t<remove_cv_t<T>>>((Args&&)args...);
		ptr.data = &static_cast<Block_object<remove_cv_t<T>>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> make_sPtr(){
		sPtr<T> ptr;
		ptr.ctrl_block=new Block_object<remove_cv_t<T>>();
		ptr.data=static_cast<Block_object<remove_cv_t<T>>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> make_sPtr(const remove_extent_t<T>& init){
		sPtr<T> ptr;
		ptr.ctrl_block=new Block_object<remove_cv_t<T>>(init);
		ptr.data=static_cast<Block_object<remove_cv_t<T>>*>(ptr.ctrl_block)->obj;
		return ptr;
	}
	template<typename T>
	enable_if_t<is_no_size_array_v<T>,sPtr<T>> make_sPtr(size_t n){
		sPtr<T> ptr;
		CtrlBlock* bytes=(CtrlBlock*)::operator new(sizeof(Block_object<T>)+sizeof(remove_extent_t<T>)*n);
		GlobalDeleteGuard guard{bytes};
		new(bytes) Block_object<remove_cv_t<T>>(n);
		ptr.ctrl_block=bytes;
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object<remove_cv_t<T>>));
		guard.ptr=nullptr;
		return ptr;
	}
	template<typename T>
	enable_if_t<is_no_size_array_v<T>,sPtr<T>> make_sPtr(size_t n,const remove_extent_t<T>& init){
		sPtr<T> ptr;
		CtrlBlock* bytes=(CtrlBlock*)::operator new(sizeof(Block_object<T>)+sizeof(remove_extent_t<T>)*n);
		GlobalDeleteGuard guard{bytes};
		new(bytes) Block_object<remove_cv_t<T>>(n,init);
		ptr.ctrl_block=bytes;
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object<remove_cv_t<T>>));
		guard.ptr=nullptr;
		return ptr;
	}
	template<typename T,typename Alloc,typename... Args>
	constexpr enable_if_t<!is_array_v<T>,sPtr<T>> alloc_sPtr(const Alloc& alloc,Args&&... args){
		sPtr<T> ptr;
		using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator<T,Alloc>>;
		BlockAlloc block_allocator(alloc);
		AllocConstructPtr<BlockAlloc> allocation_temp_owner(block_allocator);
		allocation_temp_owner.allocate();
		new(unfancy(allocation_temp_owner.ptr)) Block_object_and_allocator<remove_cv_t<T>,Alloc>(alloc,static_cast<Args&&>(args)...);
		ptr.ctrl_block=unfancy(allocation_temp_owner.ptr);
		ptr.data=&allocation_temp_owner.ptr->obj;
		allocation_temp_owner.ptr=nullptr;
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> alloc_sPtr(const Alloc& alloc){
		sPtr<T> ptr;
		using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator<T,Alloc>>;
		BlockAlloc block_allocator(alloc);
		AllocConstructPtr allocation_temp_owner(block_allocator);
		allocation_temp_owner.allocate();
		new(unfancy(allocation_temp_owner.ptr)) Block_object_and_allocator<remove_cv_t<T>,Alloc>(alloc);
		ptr.ctrl_block=unfancy(allocation_temp_owner.ptr);
		ptr.data=allocation_temp_owner.ptr->obj;
		allocation_temp_owner.ptr=nullptr;
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_size_array_v<T>,sPtr<T>> alloc_sPtr(const Alloc& alloc,const remove_extent_t<T>& init){
		sPtr<T> ptr;
		using BlockAlloc=typename allocator_traits<Alloc>::template rebind_alloc<Block_object_and_allocator<T,Alloc>>;
		BlockAlloc block_allocator(alloc);
		AllocConstructPtr allocation_temp_owner(block_allocator);
		allocation_temp_owner.allocate();
		new(unfancy(allocation_temp_owner.ptr)) Block_object_and_allocator<remove_cv_t<T>,Alloc>(alloc,init);
		ptr.ctrl_block=unfancy(allocation_temp_owner.ptr);
		ptr.data=allocation_temp_owner.ptr->obj;
		allocation_temp_owner.ptr=nullptr;
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_no_size_array_v<T>,sPtr<T>> alloc_sPtr(size_t n,const Alloc& alloc){
		sPtr<T> ptr;
		using ByteAlloc=typename allocator_traits<Alloc>::template rebind_alloc<char>;
		ByteAlloc byte_alloc(alloc);
		AllocatorDeleteGuard<ByteAlloc> guard{byte_alloc,sizeof(Block_object_and_allocator<T,Alloc>)+n*sizeof(remove_extent_t<T>)};
		new(reinterpret_cast<void*>(unfancy(guard.ptr))) Block_object_and_allocator<remove_cv_t<T>,Alloc>(n,alloc);
		ptr.ctrl_block=reinterpret_cast<CtrlBlock*>(unfancy(guard.ptr));
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object_and_allocator<remove_cv_t<T>,Alloc>));
		guard.ptr=nullptr;
		return ptr;
	}
	template<typename T,typename Alloc>
	constexpr enable_if_t<is_no_size_array_v<T>,sPtr<T>> alloc_sPtr(size_t n,const Alloc& alloc,const remove_extent_t<T>& init){
		sPtr<T> ptr;
		using ByteAlloc=typename allocator_traits<Alloc>::template rebind_alloc<char>;
		ByteAlloc byte_alloc(alloc);
		AllocatorDeleteGuard<ByteAlloc> guard{byte_alloc,sizeof(Block_object_and_allocator<T,Alloc>)+n*sizeof(remove_extent_t<T>)};
		new(reinterpret_cast<void*>(unfancy(guard.ptr))) Block_object_and_allocator<remove_cv_t<T>,Alloc>(n,alloc,init);
		ptr.ctrl_block=reinterpret_cast<CtrlBlock*>(unfancy(guard.ptr));
		ptr.data=reinterpret_cast<remove_extent_t<T>*>(reinterpret_cast<char*>(ptr.ctrl_block)+sizeof(Block_object_and_allocator<remove_cv_t<T>,Alloc>));
		guard.ptr=nullptr;
		return ptr;
	}
}