#pragma once
//ordenamiento
namespace mpv{
	template<typename> class Optional;
	template<typename> class less;
	namespace dflt{//default types
		template <typename T>
		void insertion_sort(T a[],size_t n){
			T aux;
			for(size_t i=1,j; i<n; i++){
				for(j=i,aux=(T&&)a[i]; j>0 and aux<a[j-1]; j--){ //alternativa
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T>
		void insertion_sort_reverse(T a[],size_t n){
			T aux;
			for(size_t i=1,j; i<n; i++) {
				for(j=i,aux=(T&&)a[i]; j>0 and aux>a[j-1]; j--) {
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T,typename D>
		void insertion_sort(T a[],size_t n,D(*func)(const T&)){
			T aux;
			for(size_t i=1,j; i<n; i++){
				for(j=i,aux=(T&&)a[i]; j>0 and func(aux)<func(a[j-1]); j--){
					a[j]=(T&&)a[j-1];
				}
				a[j]=(T&&)aux;
			}
		}
		template <typename T,typename Cmp>
		void insertion_sort(T a[],size_t n,Cmp&& cmp=Cmp{}){
			T aux;
			for(size_t i=1,j; i<n; i++){
				for(j=i,aux=(T&&)a[i]; j>0 and cmp(aux,a[j-1]); j--){
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
			while(n--)
				*(dest++)=*(source++);
		}
		template<typename T>
		inline void memmove(T* dest,T* source,size_t n){
			while(n--)
				*(dest++)=static_cast<T&&>(*(source++));
		}
		template<typename T>
		inline void reversecopy(T* dest,const T* source,size_t n){
			dest+=n-1;
			source+=n-1;
			while(n--)
				*(dest--)=*(source--);
		}
		template<typename T>
		inline void reversemove(T* dest,T* source,size_t n){
			dest+=n-1;
			source+=n-1;
			while(n--)
				*(dest--)=static_cast<T&&>(*(source--));
		}
		template<typename T>
		inline void safecopy(T* dest,const T* source,size_t n){
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
		template<typename T>
		inline void safemove(T* dest,T* source,size_t n){
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
		template<typename T>
		inline void memcopyconstruct(T* dest,const T* source,size_t n){
			while(n--)
				new(dest++) T(*(source++));
		}
		template<typename T>
		inline void memmoveconstruct(T* dest,T* source,size_t n){
			while(n--)
				new(dest++) T(static_cast<T&&>(*(source++)));
		}
		template<typename T>
		inline void memdestructor(T* mem,size_t n){
			while(n--)
				(mem++)->~T();
		}
	}
	namespace al{
		template <typename Alloc>
		void insertion_sort(typename allocator_traits<Alloc>::pointer a,typename allocator_traits<Alloc>::size_type n){
			using T=typename allocator_traits<Alloc>::value_type;
			T aux;
			for(typename allocator_traits<Alloc>::size_type i=1,j; i<n; i++){
				for(j=i,aux=(T&&)a[i]; j>0 and aux<a[j-1]; j--){ //alternativa
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
				for(j=i,aux=(T&&)a[i]; j>0 and aux>a[j-1]; j--) {
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
				for(j=i,aux=(T&&)a[i]; j>0 and func(aux)<func(a[j-1]); j--){
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
				for(j=i,aux=(T&&)a[i]; j>0 and cmp(aux,a[j-1]); j--){
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
				allocator_traits<Alloc>::construct(alloc,a++,fillwith);
		}
		template<typename Alloc>
		inline void memcopy(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::const_pointer source,typename allocator_traits<Alloc>::size_type n){
			while(n--)
				*(dest++)=*(source++);
		}
		template<typename Alloc>
		inline void memmove(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::pointer source,typename allocator_traits<Alloc>::size_type n){
			while(n--)
				*(dest++)=static_cast<typename allocator_traits<Alloc>::value_type&&>(*(source++));
		}
		template<typename Alloc>
		inline void reversecopy(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::const_pointer source,typename allocator_traits<Alloc>::size_type n){
			dest+=n-1;
			source+=n-1;
			while(n--)
				*(dest--)=*(source--);
		}
		template<typename Alloc>
		inline void reversemove(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::pointer source,typename allocator_traits<Alloc>::size_type n){
			dest+=n-1;
			source+=n-1;
			while(n--)
				*(dest--)=static_cast<typename allocator_traits<Alloc>::value_type&&>(*(source--));
		}
		template<typename Alloc>
		inline void safecopy(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::const_pointer source,typename allocator_traits<Alloc>::size_type n){
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
		template<typename Alloc>
		inline void safemove(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::pointer source,typename allocator_traits<Alloc>::size_type n){
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
		template<typename Alloc>
		inline void memcopyconstruct(Alloc& alloc,typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::pointer source,typename allocator_traits<Alloc>::size_type n){
			while(n--)
				allocator_traits<Alloc>::construct(alloc,dest++,*(source++));
		}
		template<typename Alloc>
		inline void memmoveconstruct(Alloc& alloc,typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::pointer source,typename allocator_traits<Alloc>::size_type n){
			while(n--)
				allocator_traits<Alloc>::construct(alloc,dest++,static_cast<typename allocator_traits<Alloc>::value_type&&>(*(source++)));
		}

		template<typename Alloc>
		inline void memdestructor(Alloc& alloc,typename allocator_traits<Alloc>::pointer mem,typename allocator_traits<Alloc>::size_type n){
			while(n--)
				allocator_traits<Alloc>::destroy(alloc,mem++);
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
	void getarray(T a[],int n,const char str[]="[{i}]=",bool reverse=false){
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
	void printarray(T a[],int n, const char *str="{a}",const char* end=", ",const char* first="[",const char *last="]\n",bool reverse=false){
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