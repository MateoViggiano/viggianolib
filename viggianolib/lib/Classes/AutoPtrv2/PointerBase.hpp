#pragma once
namespace mpv{
    template<typename T,typename Tp>
    struct Ptr__ COUNT_IT{
		using element_type=T;
        protected:
            Tp data;
            Ptr__(element_type* data):data(data){}
        public:
			add_lvalue_reference_t<element_type> operator*()const{
				return *data;
			}
			Tp operator->()const{
				return data;
			}
			Tp get()const{
				return data;
			}
			template<typename D,typename Dp> bool operator==(const Ptr__<D,Dp>& other)const{
				return this->data==other.get();
			}
			template<typename D,typename Dp> bool operator!=(const Ptr__<D,Dp>& other)const{
				return this->data!=other.get();
			}
			template<typename D,typename Dp> bool operator<=(const Ptr__<D,Dp>& other)const{
				return this->data<=other.get();
			}
			template<typename D,typename Dp> bool operator>=(const Ptr__<D,Dp>& other)const{
				return this->data>=other.get();
			}
			template<typename D,typename Dp> bool operator< (const Ptr__<D,Dp>& other)const{
				return this->data<other.get();
			}
			template<typename D,typename Dp> bool operator> (const Ptr__<D,Dp>& other)const{
				return this->data>other.get();
			}
			bool operator==(decltype(nullptr))const{
				return this->data==Tp();
			}
			bool operator!=(decltype(nullptr))const{
				return this->data!=Tp();
			}
			bool operator<=(decltype(nullptr))const{
				return this->data<=Tp();
			}
			bool operator>=(decltype(nullptr))const{
				return this->data>=Tp();
			}
			bool operator< (decltype(nullptr))const{
				return this->data<Tp();
			}
			bool operator> (decltype(nullptr))const{
				return this->data>Tp();
			}
			operator bool()const{
				return data!=Tp();
			}
    };
	template<typename T,typename Tp=remove_extent_t<T>*>
	class PtrBase:public Ptr__<T,Tp>{
		protected:
			PtrBase(Tp data):Ptr__<T,Tp>(data){}
	};
	template<typename T,typename Tp>
	class PtrBase<T[],Tp>:public Ptr__<T,Tp>{
		protected:
			PtrBase(Tp data):Ptr__<T,Tp>(data){}
		public:
			add_lvalue_reference_t<T> operator[](size_t index)const{
				return Ptr__<T,Tp>::data[index];
		}
	};
	template<typename T,size_t N,typename Tp>
	class PtrBase<T[N],Tp>:public Ptr__<T,Tp>{
		protected:
			PtrBase(Tp data):Ptr__<T,Tp>(data){}
		public:
			add_lvalue_reference_t<T> operator[](size_t index)const{
				return Ptr__<T,Tp>::data[index];
			}
			static constexpr size_t len=N;
			static constexpr size_t size=sizeof(T[N]);

			using iterator=random_access_iterator<default_types<T>>;
			iterator begin()const{
				return iterator(this->data);
			}
			iterator end()const{
				return iterator(this->data+len);
			}
			iterator rbegin()const{
				return iterator(this->data+len-1);
			}
			iterator rend()const{
				return iterator(this->data-1);
			}
	};
}
#include"Unique_pointer.hpp"
#include"Shared_pointer.hpp"
#include"Weak_pointer.hpp"