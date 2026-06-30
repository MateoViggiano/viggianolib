#pragma once
#include"../../classes.hpp"
namespace mpv{
	template<typename T>
	struct default_delete{
		constexpr default_delete()noexcept=default;
		template<typename U>constexpr default_delete(const default_delete<U>&)noexcept{}
		template<typename U>constexpr void operator()(U* p)noexcept{delete p;}
	};
	template<typename T>struct default_delete<T[]>{
		constexpr default_delete()noexcept=default;
		template<typename U>constexpr default_delete(const default_delete<U[]>&)noexcept{}
		template<typename U,size_t N>constexpr default_delete(const default_delete<U[N]>&)noexcept{}
		template<typename U>constexpr void operator()(U* p)noexcept{delete[] p;}
	};
	template<typename T,size_t N>struct default_delete<T[N]>{
		constexpr default_delete()noexcept=default;
		template<typename U>constexpr default_delete(const default_delete<U[N]>&)noexcept{}
		template<typename U>constexpr void operator()(U* p)noexcept{delete[] p;}
	};
	template<typename Alloc>
	struct alloc_wrapper_delete:EBCO<Alloc>{
		using pointer=typename allocator_traits<Alloc>::pointer;
		typename allocator_traits<Alloc>::size_type size;
		constexpr alloc_wrapper_delete(size_t size)noexcept:size(size){}
		constexpr alloc_wrapper_delete(const Alloc& al,size_t size):EBCO<Alloc>(al),size(size){}
		template<typename pU>constexpr void operator()(pU p)noexcept{
			for(typename allocator_traits<Alloc>::size_type i=0;i<size;i++)
				allocator_traits<Alloc>::destroy(this->get_val(),unfancy(p+i));
			allocator_traits<Alloc>::deallocate(this->get_val(),p,size);
		}
	};
	template<typename T,typename deleter,typename=void> struct p_ty{using pointer=remove_extent_t<T>*;};
	template<typename T,typename deleter> 				struct p_ty<T,deleter,void_t<typename remove_reference_t<deleter>::pointer>>{using pointer=typename remove_reference_t<deleter>::pointer;};

	template<typename T,typename Del=default_delete<T>>
	class uPtr:public PtrBase<T,typename p_ty<T,Del>::pointer>,protected EBCO<Del>{static_assert(!is_rvalue_ref<Del>::value,"deleter cannot be an rvalue ref");
		public:
			using pointer=typename p_ty<T,Del>::pointer;
			using element_type=remove_extent_t<T>;
			using deleter_type=Del;
			static_assert(!(is_array_v<T> && sizeof(element_type)!=sizeof(typename pointer_traits<pointer>::element_type)));
			uPtr(const uPtr&)=delete;
			uPtr& operator=(const uPtr&)=delete;
			constexpr uPtr()noexcept:PtrBase<T,pointer>(nullptr),EBCO<Del>(Del()){}

			constexpr uPtr(decltype(nullptr))noexcept:PtrBase<T,pointer>(nullptr),EBCO<Del>(Del()){}

			constexpr explicit uPtr(pointer dat)noexcept:PtrBase<T,pointer>(dat),EBCO<Del>(Del()){static_assert(!is_pointer<Del>::value,"Shouldn't default construct pointer to deleter");}

			constexpr uPtr(pointer dat,const Del& new_del)noexcept:PtrBase<T,pointer>(dat),EBCO<Del>(new_del){}

			template<typename Del2=Del>//Esto es porque si no uso un template el compilador va a evaluar la funcion y va a generar un error.
			constexpr uPtr(pointer dat,enable_if_t<!is_lvalue_ref_v<Del2>,Del&&> new_del)noexcept:PtrBase<T,pointer>(dat),EBCO<Del>(static_cast<Del&&>(new_del)){}
							//	  ^^^^^  Uso el enable_if porq en caso de que Del sea una referencia, por el colapso de referencias esta funcion quedaria igual a la de arriba y habria un error de funcion ambigua.

			constexpr uPtr(uPtr&& other)noexcept:PtrBase<T,pointer>(other.data),EBCO<Del>(static_cast<Del&&>(other.get_val())){
				other.data=nullptr;
			}
			template<typename U,typename Del2,enable_if_t<is_convertible_v<typename uPtr<U,Del2>::pointer,pointer> && (have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>)),int> = 0>
			constexpr uPtr(uPtr<U,Del2>&& other)noexcept:PtrBase<T,pointer>(other.data),EBCO<Del>(static_cast<Del2&&>(other.get_val())){
				static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
				other.data=nullptr;
			}
			constexpr uPtr& operator=(decltype(nullptr))noexcept{
				if(this->data!=nullptr) this->get_val()(this->data);
				this->data=nullptr;
				return *this;
			}
			constexpr uPtr& operator=(pointer dat)noexcept{
				if(this->data!=nullptr) this->get_val()(this->data);
				this->data=dat;
				return *this;
			}
			constexpr uPtr& operator=(uPtr<T,Del>&& other)noexcept{
				if(this->data!=nullptr) this->get_val()(this->data);
				this->get_val()=static_cast<Del&&>(other.get_val());
				this->data=other.data;
				other.data=nullptr;
				return *this;
			}
			template<typename U,typename Del2,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			constexpr uPtr& operator=(uPtr<U,Del2>&& other)noexcept{
				static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
				if(this->data!=nullptr) this->get_val()(this->data);
				this->get_val()=static_cast<Del2&&>(other.get_val());
				this->data=other.data;
				other.data=nullptr;
				return *this;
			}
			constexpr pointer drop()noexcept{
				pointer p=this->data;
				this->data=nullptr;
				return p;
			}
			constexpr Del& get_deleter()noexcept{
				return this->get_val();
			}
			~uPtr()noexcept{
				if(this->data!=nullptr) this->get_val()(this->data);
			}
		template<typename,typename>friend class uPtr;
		template<typename t,typename del1,typename u,typename del2> friend constexpr enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> static_ptr_cast(uPtr<u,del2>&&)noexcept;
		template<typename t,typename del1,typename u,typename del2> friend constexpr enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> reinterpret_ptr_cast(uPtr<u,del2>&&)noexcept;
		template<typename t,typename del1,typename u,typename del2> friend constexpr enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> const_ptr_cast(uPtr<u,del2>&&)noexcept;
		template<typename t,typename del1,typename u,typename del2> friend constexpr enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> dynamic_ptr_cast(uPtr<u,del2>&&);
	};
	template<typename T,typename Del1=default_delete<T>,typename U,typename Del2=default_delete<U>>
	constexpr enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> static_ptr_cast(uPtr<U,Del2>&& other)noexcept{
		static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=static_cast<typename uPtr<T,Del1>::pointer>(other.data);
		other.data=nullptr;
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
	template<typename T,typename Del1=default_delete<T>,typename U,typename Del2=default_delete<U>>
	constexpr enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> reinterpret_ptr_cast(uPtr<U,Del2>&& other)noexcept{
		static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=reinterpret_cast<typename uPtr<T,Del1>::pointer>(other.data);
		other.data=nullptr;
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
	template<typename T,typename Del1=default_delete<T>,typename U,typename Del2=default_delete<U>>
	constexpr enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> const_ptr_cast(uPtr<U,Del2>&& other)noexcept{
		static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=const_cast<typename uPtr<T,Del1>::pointer>(other.data);
		other.data=nullptr;
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
	template<typename T,typename Del1=default_delete<T>,typename U,typename Del2=default_delete<U>>
	constexpr enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> dynamic_ptr_cast(uPtr<U,Del2>&& other){
		static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=dynamic_cast<typename uPtr<T,Del1>::pointer>(other.data);
		if(p==nullptr) other.get_val()(other.data);
		other.data=nullptr;
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
}