#pragma once
#ifdef _WIN64
	typedef unsigned long long int size_t;
	typedef signed long long int ssize_t;
	typedef signed long long int ptrdiff_t;
#else
    typedef unsigned int size_t;
	typedef signed int ssize_t;
	typedef signed int ptrdiff_t;
#endif

#if (!defined(_INITIALIZER_LIST) && !defined(_INITIALIZER_LIST_)) || defined(USE_MPV_INITIALIZER_LIST)
#define MPV_INITIALIZER_LIST
#define _INITIALIZER_LIST
#define _INITIALIZER_LIST_
namespace std{
  	template<class T>
  	class initializer_list{
		private:
			const T* beg;
			size_t len;
			constexpr initializer_list(const T* beg,size_t len)noexcept:beg(beg),len(len){}

		public:
			constexpr initializer_list(const T* beg,const T* end)noexcept:beg(beg),len(end-beg){}
			constexpr initializer_list() noexcept :beg(nullptr),len(0){}
			constexpr const T* begin()const noexcept{return beg;}
			constexpr const T* end()const noexcept{return beg+len;}
			constexpr size_t size()const noexcept{return len;}
 	};
	template <class T>
	constexpr const T* begin(initializer_list<T> Ilist) noexcept {
		return Ilist.begin();
	}
	template <class T>
	constexpr const T* end(initializer_list<T> Ilist) noexcept {
		return Ilist.end();
	}
}
#endif

namespace mpv{
	class ObjectCounter{
		private:
#if defined(_ATOMIC_) || defined(_GLIBCXX_ATOMIC)
			static std::atomic<long long> count;
#else 
			static long long count;
#endif
		public:
			ObjectCounter(){count++;}
			ObjectCounter(const ObjectCounter&){count++;}
			ObjectCounter(ObjectCounter&&){count++;}
			void operator=(const ObjectCounter&){}
			~ObjectCounter(){count--;}
			static long long use(){return count;}
	};
#if defined(_ATOMIC_) || defined(_GLIBCXX_ATOMIC)
	std::atomic<long long> ObjectCounter::count=0;
#else 
	long long ObjectCounter::count=0;
#endif
#if defined(_DEBUG) || defined(COUNT_OBJECTS)
#define COUNT_IT :public ObjectCounter
#define COUNT_IT_ ,public ObjectCounter
#else
#define COUNT_IT
#define COUNT_IT_
#endif
}
#ifdef __clang__
void operator delete(void* x,size_t n)noexcept;
void operator delete[](void*,size_t)noexcept;
#endif
#ifndef _NEW
	#define _NEW 
	#ifndef __PLACEMENT_NEW_INLINE
		#define __PLACEMENT_NEW_INLINE
		inline void* operator new(size_t s,void* mem)noexcept{(void)s;return mem;}
	#endif
	#ifndef __PLACEMENT_VEC_NEW_INLINE
		#define __PLACEMENT_VEC_NEW_INLINE
		inline void* operator new[](size_t s,void* mem)noexcept{(void)s;return mem;}
	#endif
#endif
#include"lib/metafunctions/metafunctions.hpp"
namespace mpv{
	template<typename T>
	void swap(T& v1,T& v2){
		T aux=static_cast<T&&>(v1);
		v1=static_cast<T&&>(v2);
		v2=static_cast<T&&>(aux);
	}
	template<typename T>
	inline constexpr T abs(T n){
		if constexpr(is_unsigned_v<T>) return n;
		else return n<0? -n : n;
	}
	template<size_t Precision=1000000000000>
	double sqrt(double radicand){
		bool inverse=false;
		if(radicand<1){
			radicand=1/radicand;
			inverse=true;
		}
		double max=radicand,min=0,n=max/2;
		while(true){
			double attempt=n*n;
			if(abs(attempt-radicand)<1/static_cast<double>(Precision)){
				return inverse? 1/n : n;
			}
			else if(attempt>radicand){
				max=n;
				n=min+(max-min)/2;
			}
			else/*(attempt<radicand)*/{
				min=n;
				n=min+(max-min)/2;
			}
		}
	}
    template<typename T,size_t Precision=100000000000000>
    bool is_zero(T v){
		//if(is_floating_point_v<T>) return v>=0? v<1e-15 : v>-1e-15;
		if(is_floating_point_v<T>) return v>=0? v<1./Precision : v>-1./Precision;
		else return v==0;
    }
	template<typename T>
	signed char minus1pow(T e){
		return e%2==0? 1 : -1;
	}
}

#include "lib/testObj.hpp"
#include "lib/arrays.hpp"
#include "lib/classes.hpp"
#include "lib/parsing.hpp"