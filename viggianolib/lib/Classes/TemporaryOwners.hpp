#pragma once
namespace mpv{
	template<typename T>
	struct TemporaryOwner{
		remove_extent_t<T>* ptr;
		constexpr explicit TemporaryOwner(remove_extent_t<T>* const ptr)noexcept:ptr(ptr){}
		TemporaryOwner(const TemporaryOwner&)=delete;
		TemporaryOwner& operator=(const TemporaryOwner&)=delete;
		~TemporaryOwner(){
			if constexpr(is_no_size_array_v<T>) delete[] ptr;
			else 								delete ptr;
		}
	};
	template<typename Tp,typename Del>
	struct TemporaryOwnerDel{
		Tp ptr;
		Del& dt;
		bool call_deleter=true;
		constexpr explicit TemporaryOwnerDel(const Tp ptr,Del& dt)noexcept:ptr(ptr),dt(dt){}
		TemporaryOwnerDel(const TemporaryOwnerDel&)=delete;
		TemporaryOwnerDel& operator=(const TemporaryOwnerDel&)=delete;
		~TemporaryOwnerDel(){
			if(call_deleter) dt(ptr);
		}
	};
	struct GlobalDeleteGuard{
		void* ptr;
		~GlobalDeleteGuard()noexcept{
			::operator delete(ptr);
		}
	};
	template<typename Alloc>
	struct AllocatorDeleteGuard{
		Alloc& al;
		typename allocator_traits<Alloc>::pointer ptr;
		size_t size;
		constexpr AllocatorDeleteGuard(Alloc& al,const size_t n):al(al),ptr(allocator_traits<Alloc>::allocate(al,n)),size(n){}
		~AllocatorDeleteGuard(){
			if(ptr) allocator_traits<Alloc>::deallocate(al,ptr,size);
		}
		AllocatorDeleteGuard(const AllocatorDeleteGuard&)=delete;
		AllocatorDeleteGuard& operator=(const AllocatorDeleteGuard&)=delete;
	};
    template<typename Alloc>
    struct AllocConstructPtr{
        using pointer=typename allocator_traits<Alloc>::pointer;
        Alloc& al;
        pointer ptr;
        constexpr explicit AllocConstructPtr(Alloc& al):al(al),ptr(nullptr){}
        constexpr pointer release()noexcept{
            pointer aux=ptr;
            ptr=nullptr;
            return aux;
        }
        constexpr void allocate(){
            ptr=nullptr;
            ptr=allocator_traits<Alloc>::allocate(al,1);
        }
        ~AllocConstructPtr()noexcept{
            if(ptr) allocator_traits<Alloc>::deallocate(al,ptr,1);
        }
        AllocConstructPtr(const AllocConstructPtr&)=delete;
        AllocConstructPtr& operator=(const AllocConstructPtr&)=delete;
    };
	template<typename Alloc,typename It>
	struct DestroyGuard{
		Alloc& al;
		It iter;
		bool destroy=true;
		constexpr DestroyGuard(Alloc& al,It iter)noexcept(is_nothrow_constructible_v<It>):al(al),iter(iter){}
		~DestroyGuard()noexcept{
			if constexpr(!Uses_default_destroy_v<Alloc,iter_value_t<It>*> || !is_trivially_destructible_v<iter_value_t<It>>){
				if(destroy) allocator_traits<Alloc>::destroy(al,unfancy(iter));
			}
		}
		DestroyGuard(const DestroyGuard&)=delete;
		DestroyGuard& operator=(const DestroyGuard&)=delete;
	};
	template<typename Alloc,typename It>
	struct DestroySequenceGuard{// User should increment count every time an element is constructed and set it back to zero when all the elements are successfully constructed. If count is not zero when DestroySequenceGuard reaches the end of it's scope, it will assume an exception occurred during the construction of the elements and will destroy all the constructed elements
		using size_type=typename allocator_traits<Alloc>::size_type;
		Alloc& al;
		It iter;
		size_type count=0;
		constexpr DestroySequenceGuard(Alloc& al,It iter)noexcept(is_nothrow_constructible_v<It>):al(al),iter(iter){}
		constexpr DestroySequenceGuard(Alloc& al,It iter,size_type count)noexcept(is_nothrow_constructible_v<It>):al(al),iter(iter),count(count){}
		~DestroySequenceGuard()noexcept(noexcept(iter++)){
			if constexpr(!Uses_default_destroy_v<Alloc,iter_value_t<It>*> || !is_trivially_destructible_v<iter_value_t<It>>){
				while(count-->0) allocator_traits<Alloc>::destroy(al,unfancy(iter++));
			}
		}
		DestroySequenceGuard(const DestroySequenceGuard&)=delete;
		DestroySequenceGuard& operator=(const DestroySequenceGuard&)=delete;
	};//It probably only makes sens to use DestroySequenceGuard with an array
	template<typename T>
	struct DefaultDestroyArrayGuard{
		T* array;
		size_t count=0;
		constexpr DefaultDestroyArrayGuard(T* array)noexcept:array(array){}
		constexpr DefaultDestroyArrayGuard(T* array,size_t count)noexcept:array(array),count(count){}
		~DefaultDestroyArrayGuard()noexcept{
			if(array==nullptr) return;
			while(count>0) (array+--count)->~T();
		}
		DefaultDestroyArrayGuard(const DefaultDestroyArrayGuard&)=delete;
		DefaultDestroyArrayGuard& operator=(const DefaultDestroyArrayGuard&)=delete;
	};
}