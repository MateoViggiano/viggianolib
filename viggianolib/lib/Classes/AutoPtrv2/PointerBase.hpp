#pragma once
namespace mpv{
    template<typename T,typename Tp>
    struct Ptr__ COUNT_IT{
		using element_type=T;
        protected:
            Tp data;
            constexpr Ptr__(Tp data):data(data){}
        public:
			constexpr add_lvalue_reference_t<element_type> operator*()const{
				return *data;
			}
			constexpr Tp operator->()const{
				return data;
			}
			constexpr Tp get()const{
				return data;
			}
			template<typename D,typename Dp> constexpr bool operator==(const Ptr__<D,Dp>& other)const{
				return this->data==other.get();
			}
			template<typename D,typename Dp> constexpr bool operator!=(const Ptr__<D,Dp>& other)const{
				return this->data!=other.get();
			}
			template<typename D,typename Dp> constexpr bool operator<=(const Ptr__<D,Dp>& other)const{
				return this->data<=other.get();
			}
			template<typename D,typename Dp> constexpr bool operator>=(const Ptr__<D,Dp>& other)const{
				return this->data>=other.get();
			}
			template<typename D,typename Dp> constexpr bool operator< (const Ptr__<D,Dp>& other)const{
				return this->data<other.get();
			}
			template<typename D,typename Dp> constexpr bool operator> (const Ptr__<D,Dp>& other)const{
				return this->data>other.get();
			}
			constexpr bool operator==(decltype(nullptr))const{
				return this->data==nullptr;
			}
			constexpr bool operator!=(decltype(nullptr))const{
				return this->data!=nullptr;
			}
			constexpr bool operator<=(decltype(nullptr))const{
				return this->data<=nullptr;
			}
			constexpr bool operator>=(decltype(nullptr))const{
				return this->data>=nullptr;
			}
			constexpr bool operator< (decltype(nullptr))const{
				return this->data<nullptr;
			}
			constexpr bool operator> (decltype(nullptr))const{
				return this->data>nullptr;
			}
			constexpr operator bool()const{
				return data!=nullptr;
			}
    };
	template<typename T,typename Tp=remove_extent_t<T>*>
	class PtrBase:public Ptr__<T,Tp>{
		protected:
			constexpr PtrBase():Ptr__<T,Tp>(nullptr){}
			constexpr PtrBase(Tp data):Ptr__<T,Tp>(data){}
	};
	template<typename T,typename Tp>
	class PtrBase<T[],Tp>:public Ptr__<T,Tp>{
		protected:
			constexpr PtrBase():Ptr__<T,Tp>(nullptr){}
			constexpr PtrBase(Tp data):Ptr__<T,Tp>(data){}
		public:
			constexpr add_lvalue_reference_t<T> operator[](size_t index)const{
				return Ptr__<T,Tp>::data[index];
		}
	};
	template<typename T,size_t N,typename Tp>
	class PtrBase<T[N],Tp>:public Ptr__<T,Tp>{
		struct Val_types{
			using value_type=typename pointer_traits<Tp>::element_type;
			using difference_type=typename pointer_traits<Tp>::difference_type;
			using size_type=make_unsigned_t<difference_type>;
			using pointer=Tp;
			using const_pointer=typename pointer_traits<Tp>::template rebind<const value_type>;
			using reference=T&;
			using const_reference=const T&;
		};
		protected:
			constexpr PtrBase():Ptr__<T,Tp>(nullptr){}
			constexpr PtrBase(Tp data):Ptr__<T,Tp>(data){}
		public:
			constexpr add_lvalue_reference_t<T> operator[](size_t index)const{
				return Ptr__<T,Tp>::data[index];
			}
			static constexpr size_t len=N;
			static constexpr size_t size=sizeof(T[N]);

			using iterator=random_access_iterator<Val_types>;
			constexpr iterator begin()const{
				return this->data;
			}
			constexpr iterator end()const{
				return this->data+len;
			}
			constexpr iterator rbegin()const{
				return this->data+len-1;
			}
			constexpr iterator rend()const{
				return this->data-1;
			}
	};
}
#include"Unique_pointer.hpp"
#include"Shared_pointer.hpp"
#include"Weak_pointer.hpp"