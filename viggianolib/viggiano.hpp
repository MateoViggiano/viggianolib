#pragma once
#ifdef __linux__
typedef long unsigned int size_t;
	typedef long int ssize_t;
	typedef long int ptrdiff_t;
#elif defined(_WIN64)
	typedef long long unsigned int size_t;
	typedef long long int ssize_t;
	typedef long long signed int ptrdiff_t;
#else
    typedef long unsigned int size_t;
	typedef signed long int ssize_t;
	typedef signed long int ptrdiff_t;
#endif

#include "initializer_list"
#if (defined(_ATOMIC_) || defined(_GLIBCXX_ATOMIC)) && !defined(DISABLE_ATOMIC)
#define USE_ATOMIC
#endif
namespace mpv{
	class ObjectCounter{
		private:
#ifdef USE_ATOMIC
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
#ifdef USE_ATOMIC
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
#ifndef __linux__
#ifdef __clang__
void operator delete(void* x,size_t n)noexcept;
void operator delete[](void*,size_t)noexcept;
#endif //__clang__
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
#endif //__linux__
#if (defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_) || defined(_INC_STDIO)) && defined(DEBUG)
#define DEBUG_PRINT(...) printf(__VA_ARGS__);
#else
#define DEBUG_PRINT(...) 
#endif

#include "lib/metafunctions/metafunctions.hpp"
#include "lib/functions.hpp"
#include "lib/testObj.hpp"
#include "lib/arrays.hpp"
#include "lib/classes.hpp"
#include "lib/parsing.hpp"