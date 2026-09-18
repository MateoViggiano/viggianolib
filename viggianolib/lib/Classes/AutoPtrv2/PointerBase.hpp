#pragma once
namespace mpv{
	template<typename T,typename Tp> class PtrBase;

    template<typename T,typename Tp>
    struct Ptr__ COUNT_IT{
		template<typename,typename> friend class PtrBase;
		using element_type=T;
        protected:
            Tp data;
            constexpr Ptr__(Tp data)noexcept(is_nothrow_constructible_v<Tp,Tp&>):data(data){}
        public:
			constexpr add_lvalue_reference_t<element_type> operator*()const noexcept(noexcept(fake_copy_init<add_lvalue_reference_t<element_type>>(*this->data))){
				return *data;
			}
			constexpr Tp operator->()const noexcept(noexcept(fake_copy_init<Tp>(this->data))){
				return data;
			}
			constexpr Tp get()const noexcept(noexcept(fake_copy_init<Tp>(this->data))){
				return data;
			}
			constexpr explicit operator bool()const noexcept(noexcept(fake_copy_init<bool>(this->data!=nullptr))){
				return data!=nullptr;
			}
    };
	template<typename T,typename Tp,typename D,typename Dp>
	constexpr bool operator==(const Ptr__<T,Tp>& p1,const Ptr__<D,Dp>& p2)noexcept(noexcept(fake_copy_init<bool>(p1.get()==p2.get()))){
		return p1.get()==p2.get();
	}
	template<typename T,typename Tp,typename D,typename Dp>
	constexpr bool operator!=(const Ptr__<T,Tp>& p1,const Ptr__<D,Dp>& p2)noexcept(noexcept(fake_copy_init<bool>(p1.get()!=p2.get()))){
		return p1.get()!=p2.get();
	}
	template<typename T,typename Tp,typename D,typename Dp>
	constexpr bool operator<(const Ptr__<T,Tp>& p1,const Ptr__<D,Dp>& p2)noexcept(noexcept(fake_copy_init<bool>(mpv::less<>{}(p1.get(),p2.get())))){
		return mpv::less<>{}(p1.get(),p2.get());
	}
	template<typename T,typename Tp,typename D,typename Dp>
	constexpr bool operator>(const Ptr__<T,Tp>& p1,const Ptr__<D,Dp>& p2)noexcept(noexcept(fake_copy_init<bool>(mpv::less<>{}(p2.get(),p1.get())))){
		return mpv::less<>{}(p2.get(),p1.get());
	}
	template<typename T,typename Tp,typename D,typename Dp>
	constexpr bool operator<=(const Ptr__<T,Tp>& p1,const Ptr__<D,Dp>& p2)noexcept(noexcept(fake_copy_init<bool>(!mpv::less<>{}(p2.get(),p1.get())))){
		return !mpv::less<>{}(p2.get(),p1.get());
	}
	template<typename T,typename Tp,typename D,typename Dp>
	constexpr bool operator>=(const Ptr__<T,Tp>& p1,const Ptr__<D,Dp>& p2)noexcept(noexcept(fake_copy_init<bool>(!mpv::less<>{}(p1.get(),p2.get())))){
		return !mpv::less<>{}(p1.get(),p2.get());
	}
	template<typename T,typename Tp>
	constexpr bool operator==(const Ptr__<T,Tp>& p1,decltype(nullptr))noexcept(noexcept(fake_copy_init<bool>(p1.get()==nullptr))){
		return p1.get()==nullptr;
	}
	template<typename T,typename Tp>
	constexpr bool operator!=(const Ptr__<T,Tp>& p1,decltype(nullptr))noexcept(noexcept(fake_copy_init<bool>(p1.get()!=nullptr))){
		return p1.get()!=nullptr;
	}
	template<typename D,typename Dp>
	constexpr bool operator==(decltype(nullptr),const Ptr__<D,Dp>& p2)noexcept(noexcept(fake_copy_init<bool>(nullptr==p2.get()))){
		return nullptr==p2.get();
	}
	template<typename D,typename Dp>
	constexpr bool operator!=(decltype(nullptr),const Ptr__<D,Dp>& p2)noexcept(noexcept(fake_copy_init<bool>(nullptr!=p2.get()))){
		return nullptr!=p2.get();
	}
	template<typename T,typename Tp=remove_extent_t<T>*>
	class PtrBase:public Ptr__<T,Tp>{
		public:
			constexpr PtrBase()noexcept(noexcept(Ptr__<T,Tp>(nullptr))):Ptr__<T,Tp>(nullptr){}
			constexpr PtrBase(Tp data)noexcept(noexcept(Ptr__<T,Tp>(data))):Ptr__<T,Tp>(data){}
	};
	template<typename T,typename Tp>
	class PtrBase<T[],Tp>:public Ptr__<T,Tp>{
		public:
			constexpr PtrBase()noexcept(noexcept(Ptr__<T,Tp>(nullptr))):Ptr__<T,Tp>(nullptr){}
			constexpr PtrBase(Tp data)noexcept(noexcept(Ptr__<T,Tp>(data))):Ptr__<T,Tp>(data){}
			constexpr add_lvalue_reference_t<T> operator[](size_t index)const noexcept(noexcept(fake_copy_init<add_lvalue_reference_t<T>>(Ptr__<T,Tp>::data[index]))){
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
		public:
			constexpr PtrBase()noexcept(noexcept(Ptr__<T,Tp>(nullptr))):Ptr__<T,Tp>(nullptr){}
			constexpr PtrBase(Tp data)noexcept(noexcept(Ptr__<T,Tp>(data))):Ptr__<T,Tp>(data){}

			constexpr add_lvalue_reference_t<T> operator[](size_t index)const noexcept(noexcept(fake_copy_init<add_lvalue_reference_t<T>>(Ptr__<T,Tp>::data[index]))){
				return Ptr__<T,Tp>::data[index];
			}
			static constexpr size_t byte_size=sizeof(T[N]);
			static constexpr size_t length=N;

			using iterator=random_access_iterator<Val_types>;
			using const_iterator=const_random_access_iterator<Val_types>;
			constexpr iterator begin()const noexcept(noexcept(iterator(this->data))){//data!=nullptr
				return iterator(this->data);
			}
			constexpr iterator end()const noexcept(noexcept(iterator(this->data+length))){//data!=nullptr
				return iterator(this->data+length);
			}
	};
}
#include"Unique_pointer.hpp"
#include"Shared_pointer.hpp"
#include"Weak_pointer.hpp"