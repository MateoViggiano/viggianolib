#pragma once
#include"..\..\classes.hpp"
namespace mpv{
	template<typename T>
	struct defaultDelete{
		constexpr defaultDelete()noexcept=default;
		template<typename U>constexpr defaultDelete(const defaultDelete<U>&)noexcept{}
		template<typename U>constexpr void operator()(U* p)noexcept{delete p;}
	};
	template<typename T>struct defaultDelete<T[]>{
		constexpr defaultDelete()noexcept=default;
		template<typename U>constexpr defaultDelete(const defaultDelete<U[]>&)noexcept{}
		template<typename U,size_t N>constexpr defaultDelete(const defaultDelete<U[N]>&)noexcept{}
		template<typename U>constexpr void operator()(U* p)noexcept{delete[] p;}
	};
	template<typename T,size_t N>struct defaultDelete<T[N]>{
		constexpr defaultDelete()noexcept=default;
		template<typename U>constexpr defaultDelete(const defaultDelete<U[N]>&)noexcept{}
		template<typename U>constexpr void operator()(U* p)noexcept{delete[] p;}
	};
	template<typename T,typename deleter,typename=void> struct p_ty{using pointer=remove_extent_t<T>*;};
	template<typename T,typename deleter> 				struct p_ty<T,deleter,void_t<typename remove_reference_t<deleter>::pointer>>{using pointer=typename remove_reference_t<deleter>::pointer;};

	template<typename T,typename Del=defaultDelete<T>>
	class uPtr:public PtrBase<T,typename p_ty<T,Del>::pointer>,private EBCO<Del>{static_assert(not is_rvalue_ref<Del>::value,"deleter cannot be an rvalue ref");
		public:
			using pointer=typename p_ty<T,Del>::pointer;
			using element_type=remove_extent_t<T>;
			using deleter_type=Del;

			uPtr(const uPtr&)=delete;
			uPtr& operator=(const uPtr&)=delete;
			constexpr uPtr():PtrBase<T,pointer>(pointer()),EBCO<Del>(Del()){
				INCUNIQUEPOINTERS
			}
			explicit uPtr(pointer dat):PtrBase<T,pointer>(dat),EBCO<Del>(Del()){static_assert(not is_pointer<Del>::value,"Shouldn't default construct pointer to deleter");
				INCUNIQUEPOINTERS
			}
			uPtr(pointer dat,const Del& new_del):PtrBase<T,pointer>(dat),EBCO<Del>(new_del){
				INCUNIQUEPOINTERS
			}
			template<typename Del2=Del>//Esto es porque si uso un template el compilador va a evaluar la funcion y va a generar un error.
			uPtr(pointer dat,enable_if_t<!is_lvalue_ref_v<Del2>,Del&&> new_del):PtrBase<T,pointer>(dat),EBCO<Del>(static_cast<Del&&>(new_del)){
				INCUNIQUEPOINTERS//	  ^^^^^
			}// Uso el enable_if porq en caso de que Del sea una referencia, por el colapso de referencias esta funcion quedaria igual a la de arriba y habria un error de funcion ambigua.
			uPtr(uPtr&& other):PtrBase<T,pointer>(other.data),EBCO<Del>(static_cast<Del&&>(other.get_val())){
				other.data=pointer();
				INCUNIQUEPOINTERS
			}
			template<typename U,typename Del2,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			uPtr(uPtr<U,Del2>&& other):PtrBase<T,pointer>(other.data),EBCO<Del>(static_cast<Del2&&>(other.get_val())){
				static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
				other.data=typename uPtr<U,Del2>::pointer();
				INCUNIQUEPOINTERS
			}
			uPtr& operator=(decltype(nullptr)){
				if(this->data!=pointer()) this->get_val()(this->data);
				this->data=pointer();
				return *this;
			}
			uPtr& operator=(pointer dat){
				if(this->data!=pointer()) this->get_val()(this->data);
				this->data=dat;
				return *this;
			}
			uPtr& operator=(uPtr<T,Del>&& other){
				if(this->data!=pointer()) this->get_val()(this->data);
				this->get_val()=static_cast<Del&&>(other.get_val());
				this->data=other.data;
				other.data=typename uPtr<T,Del>::pointer();
				return *this;
			}
			template<typename U,typename Del2,enable_if_t<have_same_extent_v<T,U> || (is_same_v<remove_extent_t<T>,remove_extent_t<U>> && is_no_size_array_v<T> && is_size_array_v<U>),int> = 0>
			uPtr& operator=(uPtr<U,Del2>&& other){
				static_assert(!is_array_v<T> || sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
				if(this->data!=pointer()) this->get_val()(this->data);
				this->get_val()=static_cast<Del2&&>(other.get_val());
				this->data=other.data;
				other.data=typename uPtr<U,Del2>::pointer();
				return *this;
			}
			pointer drop(){
				pointer p=this->data;
				this->data=pointer();
				return p;
			}
			Del& get_deleter(){
				return this->get_val();
			}
			~uPtr(){
				DECUNIQUEPOINTERS
				if(this->data!=pointer()) this->get_val()(this->data);
			}
		template<typename,typename>friend class uPtr;
		template<typename t,typename del1,typename u,typename del2> friend enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> static_ptr_cast(uPtr<u,del2>&&);
		template<typename t,typename del1,typename u,typename del2> friend enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> reinterpret_ptr_cast(uPtr<u,del2>&&);
		template<typename t,typename del1,typename u,typename del2> friend enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> const_ptr_cast(uPtr<u,del2>&&);
		template<typename t,typename del1,typename u,typename del2> friend enable_if_t<have_same_extent_v<t,u>,uPtr<t,del1>> dynamic_ptr_cast(uPtr<u,del2>&&);
	};
	template<typename T,typename Del1=defaultDelete<T>,typename U,typename Del2=defaultDelete<U>>
	enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> static_ptr_cast(uPtr<U,Del2>&& other){
		static_assert(!is_array_v<T> or sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=static_cast<typename uPtr<T,Del1>::pointer>(other.data);
		other.data=typename uPtr<U,Del2>::pointer();
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
	template<typename T,typename Del1=defaultDelete<T>,typename U,typename Del2=defaultDelete<U>>
	enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> reinterpret_ptr_cast(uPtr<U,Del2>&& other){
		static_assert(!is_array_v<T> or sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=reinterpret_cast<typename uPtr<T,Del1>::pointer>(other.data);
		other.data=typename uPtr<U,Del2>::pointer();
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
	template<typename T,typename Del1=defaultDelete<T>,typename U,typename Del2=defaultDelete<U>>
	enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> const_ptr_cast(uPtr<U,Del2>&& other){
		static_assert(!is_array_v<T> or sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=const_cast<typename uPtr<T,Del1>::pointer>(other.data);
		other.data=typename uPtr<U,Del2>::pointer();
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
	template<typename T,typename Del1=defaultDelete<T>,typename U,typename Del2=defaultDelete<U>>
	enable_if_t<have_same_extent_v<T,U>,uPtr<T,Del1>> dynamic_ptr_cast(uPtr<U,Del2>&& other){
		static_assert(!is_array_v<T> or sizeof(remove_extent_t<T>)==sizeof(remove_extent_t<U>),"sizeof(T) and sizeof(U) must be equal if they are array types");
		typename uPtr<T,Del1>::pointer p=dynamic_cast<typename uPtr<T,Del1>::pointer>(other.data);
		if(p==nullptr) other.get_val()(other.data);
		other.data=typename uPtr<U,Del2>::pointer();
		return uPtr<T,Del1>(p,static_cast<Del2&&>(other.get_val()));
	}
}