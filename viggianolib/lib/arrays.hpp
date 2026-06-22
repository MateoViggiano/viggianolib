#pragma once
#include"Classes/Optional.hpp"
#include"Classes/TemporaryOwners.hpp"
// #ifdef _INC_STRING
// #define USE_MEMCPY 1
// #else
// #define USE_MEMCPY 0
// #endif

// #if USE_MEMCPY == 0
// #define MEMCPY(TO,FROM,SIZE) ;	//evita error de compilacion si memcpy no existe
// #define MEMMOVE(TO,FROM,SIZE) ;	//evita error de compilacion si memmove no existe
// #define STRCPY(TO,FROM) ;
// #define WCSCPY(TO,FROM) ;
// #define RETURN_STRLEN(S) ;
// #define RETURN_WCSLEN(S) ;
// #else
// #define MEMCPY(TO,FROM,SIZE) ::memcpy(TO,FROM,SIZE);
// #define MEMMOVE(TO,FROM,SIZE) ::memmove(TO,FROM,SIZE);
// #define STRCPY(TO,FROM) ::strcpy(TO,FROM);
// #define WCSCPY(TO,FROM) ::wcscpy(TO,FROM);
// #define RETURN_STRLEN(S) return ::strlen(S);
// #define RETURN_WCSLEN(S) return ::wcslen(S);
// #endif

namespace mpv{
	template<typename> class Optional;
	namespace dflt{//default types
		template<typename T>
		size_t strsize(const T* str){
			if constexpr(!(is_same_v<T,char> || is_same_v<T,wchar_t>) || !USE_MEMCPY){
				size_t i=0;
				while(str[i]) ++i;
				return i;				
			}
			else if constexpr(is_same_v<T,char>){
				RETURN_STRLEN(str);
			}
			else{RETURN_WCSLEN(str);}
		}// se asume que los elementos que pertenecen a los strings son pod
		template<typename T>
		void strcopy(T* dest,const T* source){
			if constexpr(!(is_same_v<T,char> || is_same_v<T,wchar_t>) || !USE_MEMCPY){
				size_t i=0;
				while(source[i]){
					dest[i]=source[i];
					i++;
				}
				dest[i]=T();				
			}
			else if constexpr(is_same_v<T,char>){
				STRCPY(dest,source);
			}
			else{
				WCSCPY(dest,source);
			}
		}
		template<typename T>
		signed char strcomp(const T* str1,const T* str2){
			size_t sz1=strsize(str1);
			size_t sz2=strsize(str2);
			if(sz1>sz2)
				return 2;
			if(sz1<sz2)
				return -2;
			else{
				for(size_t i=0;i<sz1;i++){
					if(str1[i]>str2[i])
						return 1;
					if(str1[i]<str2[i])
						return -1;
				}
				return 0;
			}
		}
		template <typename T>
		void insertion_sort(T a[],size_t n){
			for(size_t i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && aux<a[j-1]; j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T>
		void insertion_sort_reverse(T a[],size_t n){
			for(size_t i=1,j; i<n; i++) {
				T aux=(T&&)a[i];
				for(j=i; j>0 && aux>a[j-1]; j--) {
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T,typename D>
		void insertion_sort(T a[],size_t n,D(*func)(const T&)){
			for(size_t i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && func(aux)<func(a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T,typename Cmp>
		void insertion_sort(T a[],size_t n,Cmp&& cmp=Cmp{}){
			for(size_t i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && cmp(aux,a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}

	}
	namespace al{
		template<typename Alloc>
		typename allocator_traits<Alloc>::size_type strsize(typename allocator_traits<Alloc>::const_pointer str){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !(is_same_v<typename allocator_traits<Alloc>::value_type,char> || is_same_v<typename allocator_traits<Alloc>::value_type,wchar_t>) || !USE_MEMCPY){
				typename allocator_traits<Alloc>::size_type i=0;
				while(str[i]) ++i;
				return i;				
			}
			else if constexpr(is_same_v<typename allocator_traits<Alloc>::value_type,char>){
				RETURN_STRLEN(str);
			}
			else{RETURN_WCSLEN(str);}
		}// se asume que los elementos que pertenecen a los strings son pod
		template<typename Alloc>
		void strcopy(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::const_pointer source){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !(is_same_v<typename allocator_traits<Alloc>::value_type,char> || is_same_v<typename allocator_traits<Alloc>::value_type,wchar_t>) || !USE_MEMCPY){
				typename allocator_traits<Alloc>::size_type i=0;
				while(source[i]){
					dest[i]=source[i];
					i++;
				}
				dest[i]=typename allocator_traits<Alloc>::value_type();				
			}
			else if constexpr(is_same_v<typename allocator_traits<Alloc>::value_type,char>){
				STRCPY(dest,source);
			}
			else{
				WCSCPY(dest,source);
			}
		}
		template<typename Alloc>
		signed char strcomp(typename allocator_traits<Alloc>::const_pointer str1,typename allocator_traits<Alloc>::const_pointer str2){
			typename allocator_traits<Alloc>::size_type sz1=strsize<Alloc>(str1);
			typename allocator_traits<Alloc>::size_type sz2=strsize<Alloc>(str2);
			if(sz1>sz2)
				return 2;
			if(sz1<sz2)
				return -2;
			else{
				for(typename allocator_traits<Alloc>::size_type i=0;i<sz1;i++){
					if(str1[i]>str2[i])
						return 1;
					if(str1[i]<str2[i])
						return -1;
				}
				return 0;
			}
		}
		template <typename Alloc>
		void insertion_sort(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n){
			using T=typename allocator_traits<Alloc>::value_type;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && aux<a[j-1]; j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename Alloc>
		void insertion_sort_reverse(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n){
			using T=typename allocator_traits<Alloc>::value_type;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++) {
				T aux=(T&&)a[i];
				for(j=i; j>0 && aux>a[j-1]; j--) {
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename Alloc,typename D>
		void insertion_sort(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n,D(*func)(const typename allocator_traits<Alloc>::value_type&)){
			using T=typename allocator_traits<Alloc>::value_type;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && func(aux)<func(a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename Alloc,typename Cmp>
		void insertion_sort(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n,Cmp&& cmp=Cmp{}){
			using T=typename allocator_traits<Alloc>::value_type;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++){
				T aux=(T&&)a[i];
				for(j=i; j>0 && cmp(aux,a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
	}

#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
	// Input/Output de arrays
	template<typename T>
	void str__Print(T a,int b,T* dir,const char *str){
		int sz=0,i,j;
		while(str[sz]){
			++sz;
		}
		++sz;
		char* c=new char[sz]{};
		for(i=0,j=0; i<sz; i++,j++) {
			c[j]=str[i];
			if(str[i]=='{') {
				c[j]='\0';
				printf("%s",c);
				if(str[i+1]=='a') {
					std::cout<<a;
				}
				else if(str[i+1]=='i') {
					printf("%d",b);
				}
				else if(str[i+1]=='d') {
					std::cout<<dir;
				}
				i+=2;
				j=-1;
			}
		}
		printf("%s",c);
		delete[] c;
	}
	//Array Input
	template<typename T>
	void getarray(T a[],long long n,const char str[]="[{i}]=",bool reverse=false){
		int i;
		if(reverse==false) {
			for(i=0; i<n; i++) {
				str__Print(a[i],i,a+i,str);
				std::cin>>a[i];
			}
		}
		else {
			for(i=n-1; i>=0; i--) {
				str__Print(a[i],i,a+i,str);
				std::cin>>a[i];
			}
		}
	}
	//Array Output
	
	template<typename T>
	void printarray(T a[],long long n, const char *str="{a}",const char* end=", ",const char* first="[",const char *last="]\n",bool reverse=false){
		int i;
		std::cout<<first;
		if(reverse==false) {
			for(i=0; i<n; i++) {
				str__Print(a[i],i,a+i,str);
				if(i!=n-1)
					std::cout<<end;
			}
		}
		else {
			for(i=n-1; i>=0; i--) {
				str__Print(a[i],i,a+i,str);
				if(i!=0)
					std::cout<<end;
			}
		}
		std::cout<<last;
	}
#endif
}