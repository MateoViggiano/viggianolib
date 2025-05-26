#pragma once

#ifdef _INC_STRING
#define STRING_INCLUDED 1
#else
#define STRING_INCLUDED 0
#endif

#if STRING_INCLUDED == 0
#define MEMCPY(TO,FROM,SIZE) ;	//evita error de compilacion si memcpy no existe
#define MEMMOVE(TO,FROM,SIZE) ;	//evita error de compilacion si memmove no existe
#define STRCPY(TO,FROM) ;
#define WCSCPY(TO,FROM) ;
#define RETURN_STRLEN(S) ;
#define RETURN_WCSLEN(S) ;
#else
#define MEMCPY(TO,FROM,SIZE) ::memcpy(TO,FROM,SIZE);
#define MEMMOVE(TO,FROM,SIZE) ::memmove(TO,FROM,SIZE);
#define STRCPY(TO,FROM) ::strcpy(TO,FROM);
#define WCSCPY(TO,FROM) ::wcscpy(TO,FROM);
#define RETURN_STRLEN(S) return ::strlen(S);
#define RETURN_WCSLEN(S) return ::wcslen(S);
#endif

namespace mpv{
	template<typename> class Optional;
	template<typename> class less;
	namespace dflt{//default types
		template<typename T>
		size_t strsize(const T* str){
			if constexpr(!(is_same_v<T,char> || is_same_v<T,wchar_t>) || !STRING_INCLUDED){
				size_t i=0;
				while(str[i]) ++i;
				return i;				
			}
			else if constexpr(is_same_v<T,char>){
				RETURN_STRLEN(str);
			}
			else RETURN_WCSLEN(str);
		}// se asume que los elementos que pertenecen a los strings son pod
		template<typename T>
		void strcopy(T* dest,const T* source){
			if constexpr(!(is_same_v<T,char> || is_same_v<T,wchar_t>) || !STRING_INCLUDED){
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
			T aux;
			for(size_t i=1,j; i<n; i++){
				for(j=i,aux=(T&&)a[i]; j>0 && aux<a[j-1]; j--){ //alternativa
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T>
		void insertion_sort_reverse(T a[],size_t n){
			T aux;
			for(size_t i=1,j; i<n; i++) {
				for(j=i,aux=(T&&)a[i]; j>0 && aux>a[j-1]; j--) {
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T,typename D>
		void insertion_sort(T a[],size_t n,D(*func)(const T&)){
			T aux;
			for(size_t i=1,j; i<n; i++){
				for(j=i,aux=(T&&)a[i]; j>0 && func(aux)<func(a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T,typename Cmp>
		void insertion_sort(T a[],size_t n,Cmp&& cmp=Cmp{}){
			T aux;
			for(size_t i=1,j; i<n; i++){
				for(j=i,aux=(T&&)a[i]; j>0 && cmp(aux,a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template<typename T>
		void reverse(T a[],size_t n){
			size_t i,j;
			T aux;
			for(i=0,j=n-1;i!=n/2;i++,j--){
				aux=(T&&)a[i];
				a[i]=(T&&)a[j];
				a[j]=(T&&)aux;
			}
		}
		template<typename T,typename D>
		long long find(T* array,size_t n,const D& key){
			for(size_t i=0;i<n;i++){
				if(array[i]==key)
					return i;
			}
			return -1;
		}
		template<typename T>
		inline bool memcomp(const T* a,const T* b,size_t n){// Retorna 0 si son iguales.
			for(size_t i=0;i<n;i++){
				if(a[i]!=b[i])
					return 1;
			}
			return 0;
		}
		template<typename T,typename D,typename Cmp=less<T>>
		ssize_t binary_search(T* array,size_t n,const D& key,Cmp&& cmp=Cmp{}){
			T* start=array,* end=(array+n)-1;
			while(end>=start){
				T* index=(end-start)/2+start;
				if(cmp(*index,key))
					start=index+1;
				else if(cmp(key,*index))
					end=index-1;
				else
					return (index-array);
			}
			return -1;
		}
		template <typename T>
		inline void memfill(T* a,size_t n,const T& fillwith){
			while(n--)
				*(a++)=fillwith;
		}
		template<typename T>
		inline void memfillconstruct(T* a,size_t n,const T& fillwith){
			while(n--)
				new(a++) T(fillwith);
		}
		template<typename T>
		inline void memcopy(T* dest,const T* source,size_t n){
			if constexpr(!is_trivially_copy_assignable_v<T> || !STRING_INCLUDED){
				while(n--)
					*(dest++)=*(source++);				
			}
			else{MEMCPY(dest,source,n*sizeof(T));}
		}
		template<typename T>
		inline void memmove(T* dest,T* source,size_t n){
			if constexpr(!is_trivially_move_assignable_v<T> || !STRING_INCLUDED){
				while(n--)
					*(dest++)=static_cast<T&&>(*(source++));
			}
			else{MEMCPY(dest,source,n*sizeof(T));}
		}
		template<typename T>
		inline void reversecopy(T* dest,const T* source,size_t n){
			if constexpr(!is_trivially_copy_assignable_v<T> || !STRING_INCLUDED){
				dest+=n-1;
				source+=n-1;
				while(n--)
					*(dest--)=*(source--);
			}
			else{MEMMOVE(dest,source,n*sizeof(T));}
		}
		template<typename T>
		inline void reversemove(T* dest,T* source,size_t n){
			if constexpr(!is_trivially_move_assignable_v<T> || !STRING_INCLUDED){
				dest+=n-1;
				source+=n-1;
				while(n--)
					*(dest--)=static_cast<T&&>(*(source--));
			}
			else{MEMMOVE(dest,source,n*sizeof(T));}
		}
		template<typename T>
		inline void safecopy(T* dest,const T* source,size_t n){
			if constexpr(!is_trivially_copy_assignable_v<T> || !STRING_INCLUDED){
				if(source>=dest)
					while(n--)
						*(dest++)=*(source++);
				else{
					dest+=n-1;
					source+=n-1;
					while(n--)
						*(dest--)=*(source--);
				}
			}
			else{MEMMOVE(dest,source,n*sizeof(T));}
		}
		template<typename T>
		inline void safemove(T* dest,T* source,size_t n){
			if constexpr(!is_trivially_move_assignable_v<T> || !STRING_INCLUDED){
				if(source>=dest)
					while(n--)
						*(dest++)=static_cast<T&&>(*(source++));
				else{
					dest+=n-1;
					source+=n-1;
					while(n--)
						*(dest--)=static_cast<T&&>(*(source--));
				}
			}
			else{MEMMOVE(dest,source,n*sizeof(T));}
		}
		template<typename T>
		inline void memdefaultconstruct(T* dest,size_t n){
			if constexpr(!is_trivially_default_constructible_v<T>){
				while(n--)
					new(dest++) T();
			}
		}
		template<typename T>
		inline void memcopyconstruct(T* dest,const T* source,size_t n){
			if constexpr(!is_trivially_copy_constructible_v<T> || !STRING_INCLUDED){
				while(n--)
					new(dest++) T(*(source++));
			}
			else{MEMCPY(dest,source,n*sizeof(T));}
		}
		template<typename T>
		inline void memmoveconstruct(T* dest,T* source,size_t n){
			if constexpr(!is_trivially_move_constructible_v<T> || !STRING_INCLUDED){
				while(n--)
					new(dest++) T(static_cast<T&&>(*(source++)));
			}
			else{MEMCPY(dest,source,n*sizeof(T));}
		}
		template<typename T>
		inline void memdestructor(T* mem,size_t n){
			if constexpr(!is_trivially_destructible_v<T>)
				while(n--)
					(mem++)->~T();
		}
	}
	namespace al{
		template<typename Alloc>
		typename allocator_traits<Alloc>::size_type strsize(typename allocator_traits<Alloc>::const_pointer str){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !(is_same_v<typename allocator_traits<Alloc>::value_type,char> || is_same_v<typename allocator_traits<Alloc>::value_type,wchar_t>) || !STRING_INCLUDED){
				typename allocator_traits<Alloc>::size_type i=0;
				while(str[i]) ++i;
				return i;				
			}
			else if constexpr(is_same_v<typename allocator_traits<Alloc>::value_type,char>){
				RETURN_STRLEN(str);
			}
			else RETURN_WCSLEN(str);
		}// se asume que los elementos que pertenecen a los strings son pod
		template<typename Alloc>
		void strcopy(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::const_pointer source){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !(is_same_v<typename allocator_traits<Alloc>::value_type,char> || is_same_v<typename allocator_traits<Alloc>::value_type,wchar_t>) || !STRING_INCLUDED){
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
			T aux;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++){
				for(j=i,aux=(T&&)a[i]; j>0 && aux<a[j-1]; j--){ //alternativa
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename Alloc>
		void insertion_sort_reverse(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n){
			using T=typename allocator_traits<Alloc>::value_type;
			T aux;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++) {
				for(j=i,aux=(T&&)a[i]; j>0 && aux>a[j-1]; j--) {
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename Alloc,typename D>
		void insertion_sort(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n,D(*func)(const typename allocator_traits<Alloc>::value_type&)){
			using T=typename allocator_traits<Alloc>::value_type;
			T aux;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++){
				for(j=i,aux=(T&&)a[i]; j>0 && func(aux)<func(a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename Alloc,typename Cmp>
		void insertion_sort(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n,Cmp&& cmp=Cmp{}){
			using T=typename allocator_traits<Alloc>::value_type;
			T aux;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++){
				for(j=i,aux=(T&&)a[i]; j>0 && cmp(aux,a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template<typename Alloc>
		void reverse(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n){
			using T=typename allocator_traits<Alloc>::value_type;
			typename allocator_traits<Alloc>::size_type i,j;
			T aux;
			for(i=0,j=n-1;i!=n/2;i++,j--){
				aux=(T&&)a[i];
				a[i]=(T&&)a[j];
				a[j]=(T&&)aux;
			}
		}
		template<typename Alloc,typename D>
		long long find(typename allocator_traits<Alloc>::pointer array,typename allocator_traits<Alloc>::size_type n,const D& key){
			for(typename allocator_traits<Alloc>::size_type i=0;i<n;i++){
				if(array[i]==key)
					return i;
			}
			return -1;
		}
		template<typename Alloc>
		inline bool memcomp(typename allocator_traits<Alloc>::const_pointer a,typename allocator_traits<Alloc>::const_pointer b,typename allocator_traits<Alloc>::size_type n){// Retorna 0 si son iguales.
			for(typename allocator_traits<Alloc>::size_type i=0;i<n;i++){
				if(a[i]!=b[i])
					return 1;
			}
			return 0;
		}
		template<typename Alloc,typename D, typename Cmp>
		Optional<typename allocator_traits<Alloc>::size_type> binary_search(typename allocator_traits<Alloc>::pointer array,typename allocator_traits<Alloc>::size_type n,const D& key,Cmp&& cmp=Cmp{}){
			typename allocator_traits<Alloc>::pointer start=array,end=(array+n)-1;
			while(end>=start){
				typename allocator_traits<Alloc>::pointer index=(end-start)/2+start;
				if(cmp(*index,key))
					start=index+1;
				else if(cmp(key,*index))
					end=index-1;
				else return index-array;
			}
			return {};
		}
		template <typename Alloc>
		inline void memfill(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n,const typename allocator_traits<Alloc>::value_type& fillwith){
			while(n--)
				*(a++)=fillwith;
		}
		template<typename Alloc>
		inline void memfillconstruct(Alloc& alloc,typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n,const typename allocator_traits<Alloc>::value_type& fillwith){
			while(n--)
				allocator_traits<Alloc>::construct(alloc,unfancy(a++),fillwith);
		}
		template<typename Alloc>
		inline void memcopy(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::const_pointer source,typename allocator_traits<Alloc>::size_type n){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !is_trivially_copy_assignable_v<typename allocator_traits<Alloc>::value_type> || !STRING_INCLUDED){
				while(n--)
					*(dest++)=*(source++);
			}
			else{MEMCPY(dest,source,n*sizeof(typename allocator_traits<Alloc>::value_type));}
		}//RECORDATORIO: memcopy y memmove asignan los valores, no construyen, por lo que en caso de que estas operaciones sean triviales y no se este usando un puntero sintetico, no necesito tener en cuenta si el Allocator usa un constructor customizado, puedo simplemente copiarlas con memcpy
		template<typename Alloc>
		inline void memmove(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::pointer source,typename allocator_traits<Alloc>::size_type n){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !is_trivially_move_assignable_v<typename allocator_traits<Alloc>::value_type> || !STRING_INCLUDED){
				while(n--)
					*(dest++)=static_cast<typename allocator_traits<Alloc>::value_type&&>(*(source++));			
			}
			else{MEMCPY(dest,source,n*sizeof(typename allocator_traits<Alloc>::value_type));}
		}
		template<typename Alloc>
		inline void reversecopy(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::const_pointer source,typename allocator_traits<Alloc>::size_type n){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !is_trivially_copy_assignable_v<typename allocator_traits<Alloc>::value_type> || !STRING_INCLUDED){
				dest+=n-1;
				source+=n-1;
				while(n--)
					*(dest--)=*(source--);
			}
			else{MEMMOVE(dest,source,n*sizeof(typename allocator_traits<Alloc>::value_type));}
		}
		template<typename Alloc>
		inline void reversemove(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::pointer source,typename allocator_traits<Alloc>::size_type n){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !is_trivially_move_assignable_v<typename allocator_traits<Alloc>::value_type> || !STRING_INCLUDED){
				dest+=n-1;
				source+=n-1;
				while(n--)
					*(dest--)=static_cast<typename allocator_traits<Alloc>::value_type&&>(*(source--));
			}
			else{MEMMOVE(dest,source,n*sizeof(typename allocator_traits<Alloc>::value_type));}
		}
		template<typename Alloc>
		inline void safecopy(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::const_pointer source,typename allocator_traits<Alloc>::size_type n){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !is_trivially_copy_assignable_v<typename allocator_traits<Alloc>::value_type> || !STRING_INCLUDED){
				if(source>=dest)
					while(n--)
						*(dest++)=*(source++);
				else{
					dest+=n-1;
					source+=n-1;
					while(n--)
						*(dest--)=*(source--);
				}
			}
			else{MEMMOVE(dest,source,n*sizeof(typename allocator_traits<Alloc>::value_type));}
		}
		template<typename Alloc>
		inline void safemove(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::pointer source,typename allocator_traits<Alloc>::size_type n){
			if constexpr(is_fancy_v<typename allocator_traits<Alloc>::pointer> || !is_trivially_move_assignable_v<typename allocator_traits<Alloc>::value_type> || !STRING_INCLUDED){
				if(source>=dest)
					while(n--)
						*(dest++)=static_cast<typename allocator_traits<Alloc>::value_type&&>(*(source++));
				else{
					dest+=n-1;
					source+=n-1;
					while(n--)
						*(dest--)=static_cast<typename allocator_traits<Alloc>::value_type&&>(*(source--));
				}
			}
			else{MEMMOVE(dest,source,n*sizeof(typename allocator_traits<Alloc>::value_type));}
		}
		template<typename Alloc>
		inline void memdefaultconstruct(Alloc& alloc,typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::size_type n){
			using val_type=typename allocator_traits<Alloc>::value_type;
			if constexpr(!Uses_default_construct_v<Alloc,typename allocator_traits<Alloc>::value_type*>){
				while(n--)
					allocator_traits<Alloc>::construct(alloc,unfancy(dest++));				
			}
			else if constexpr(!is_trivially_default_constructible_v<val_type>){
				while(n--)
					new(dest++) val_type();
			}//else do nothing
		}
		template<typename Alloc>
		inline void memcopyconstruct(Alloc& alloc,typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::const_pointer source,typename allocator_traits<Alloc>::size_type n){
			using val_type=typename allocator_traits<Alloc>::value_type;
			if constexpr(!Uses_default_construct_v<Alloc,typename allocator_traits<Alloc>::value_type*,decltype(*source)>){
				while(n--)
					allocator_traits<Alloc>::construct(alloc,unfancy(dest++),*(source++));
			}
			else if constexpr(!is_trivially_copy_constructible_v<val_type> || !STRING_INCLUDED){
				while(n--)	
					new(dest++) val_type(*(source++));
			}
			else{MEMCPY(dest,source,n*sizeof(val_type));}
		}
		template<typename Alloc>
		inline void memmoveconstruct(Alloc& alloc,typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::pointer source,typename allocator_traits<Alloc>::size_type n){
			using val_type=typename allocator_traits<Alloc>::value_type;
			if constexpr(!Uses_default_construct_v<Alloc,typename allocator_traits<Alloc>::value_type*,val_type&&>){
				while(n--)
					allocator_traits<Alloc>::construct(alloc,unfancy(dest++),static_cast<typename allocator_traits<Alloc>::value_type&&>(*(source++)));
			}
			else if constexpr(!is_trivially_move_constructible_v<val_type> || !STRING_INCLUDED){
				while(n--)
					new(dest++) val_type(static_cast<val_type&&>(*(source++)));
			}
			else{MEMCPY(dest,source,n*sizeof(val_type));}
		}

		template<typename Alloc>
		inline void memdestructor(Alloc& alloc,typename allocator_traits<Alloc>::pointer mem,typename allocator_traits<Alloc>::size_type n){
			using val_type=typename allocator_traits<Alloc>::value_type;
			if constexpr(!Uses_default_destroy_v<Alloc,typename allocator_traits<Alloc>::value_type*>){
				while(n--)
					allocator_traits<Alloc>::destroy(alloc,unfancy(mem++));
			}
			else if constexpr(!is_trivially_destructible_v<val_type>){
				while(n--)
					(mem++)->~val_type();
			}//else{do nothing}
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