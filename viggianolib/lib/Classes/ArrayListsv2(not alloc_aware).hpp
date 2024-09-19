#pragma once
#include"../arrays.hpp"
#ifndef ALIST_MMS
#define ALIST_MMS 64
#endif
namespace mpv{
    template<typename T,ssize_t min_maxSize=ALIST_MMS>
    class Vector COUNT_IT{static_assert(min_maxSize>0,"min_maxSize cannot be less than 1");
        private:
            ssize_t size;
            ssize_t maxSize;
            T* array;
        public:
            Vector():size(0),maxSize(0),array(nullptr){}
            Vector(reserve reserved):size(0),maxSize(reserved.size),array(reserved.size>0? (T*)::operator new(reserved.size*sizeof(T)):nullptr){}
            Vector(Vector& other):size(other.size),maxSize(other.maxSize),array(other.maxSize>0 ? (T*)::operator new(other.maxSize*sizeof(T)):nullptr){
                meminit(this->array,other.array,other.size);
            }
            Vector(const Vector& other):size(other.size),maxSize(other.maxSize),array(other.maxSize>0 ? (T*)::operator new(other.maxSize*sizeof(T)):nullptr){
                meminit(this->array,other.array,other.size);
            }
            Vector(Vector&& other):size(other.size),maxSize(other.maxSize),array(other.array){
                other.size=0;
                other.maxSize=0;
                other.array=nullptr;
            }
            Vector(setSize sz):size(sz.size),maxSize(size*2),array(size>0? (T*)::operator new(maxSize*sizeof(T)):nullptr){
                new(array)T[size];
            }
            Vector(setSize sz,const T& fillwith):size(sz.size),maxSize(size*2),array(size>0? (T*)::operator new(maxSize*sizeof(T)):nullptr){
                memfillinit(array,fillwith,size);
            }
            Vector(std::initializer_list<T> initializerList):size(initializerList.size()),maxSize(size*2),array(size>0? (T*)::operator new(maxSize*sizeof(T)):nullptr){
                ssize_t i=0;
                for(const T& v:initializerList)
                    new(array+i++)T((T&&)v);
            }
            Vector& operator=(const Vector& other){
                if(this->maxSize<other.size){
                    memdestructor(this->array,this->size);
                    ::operator delete(this->array/*,this->maxSize*sizeof(T)*/);
                    this->array=(T*)::operator new(other.maxSize*sizeof(T));
                    this->maxSize=other.maxSize;
                    this->size=other.size;
                    meminit(this->array,other.array,other.size);
                }
                else{
                    if(this->size>=other.size){
                        memdestructor(this->array+other.size,this->size-other.size);
                        memcopy(this->array,other.array,other.size); 
                    }
                    else{
                        memcopy(this->array,other.array,this->size);
                        meminit(this->array+this->size,other.array+this->size,other.size-this->size);
                    }
                    this->size=other.size;
                }
                return *this;
            }
            Vector& operator=(Vector&& other)noexcept{
                if(this==&other)
                    return *this;
                memdestructor(this->array,size);
                ::operator delete(this->array/* ,this->maxSize*sizeof(T) */);
                this->size=other.size;
                this->maxSize=other.maxSize;
                this->array=other.array;
                other.size=0;
                other.maxSize=0;
                other.array=nullptr;
                return *this;
            }
            Vector operator+(const Vector& other)const&{
                Vector new_vec;
                new_vec.size=this->size+other.size;
                new_vec.maxSize=new_vec.size*2;
                new_vec.array=(T*)::operator new(new_vec.maxSize*sizeof(T));
                meminit(new_vec.array,this->array,this->size);
                meminit(new_vec.array+this->size,other.array,other.size);
                return new_vec;
            }
            Vector operator+(Vector&& other)const&{
                Vector new_vec;
                new_vec.size=this->size+other.size;
                if(new_vec.size>other.maxSize){
                    new_vec.maxSize=new_vec.size*2;
                    new_vec.array=(T*)::operator new(new_vec.maxSize*sizeof(T));
                    meminit(new_vec.array,this->array,this->size);
                    memmoveinit(new_vec.array+this->size,other.array,other.size);
                    memdestructor(other.array,other.size);  //destruyo el array de other, que despues dejo vacio, sin liberar la memoria.
                }
                else{
                    new_vec.maxSize=other.maxSize;
                    new_vec.array=other.array;                    
                    if(this->size>other.size){
                        memmoveinit(new_vec.array+this->size,other.array,other.size);
                        memcopy(new_vec.array,this->array,other.size);
                        meminit(new_vec.array+other.size,this->array+other.size,this->size-other.size);
                    }
                    else{
                        memmoveinit(new_vec.array+other.size,other.array+other.size-this->size,this->size);
                        memmove(new_vec.array+this->size,other.array,other.size-this->size);
                        memcopy(new_vec.array,this->array,this->size);
                    }
                    other.array=nullptr;
                    other.maxSize=0;
                }
                other.size=0;
                return new_vec;
            }
            Vector operator+(const Vector& other)&&{
                Vector new_vec;
                new_vec.size=this->size+other.size;
                if(new_vec.size>this->maxSize){
                    new_vec.maxSize=new_vec.size*2;
                    new_vec.array=(T*)::operator new(new_vec.maxSize*sizeof(T));
                    memmoveinit(new_vec.array,this->array,this->size);
                    meminit(new_vec.array+this->size,other.array,other.size);
                    memdestructor(this->array,this->size);  //destruyo el array de other, que despues dejo vacio, sin liberar la memoria.
                }
                else{
                    new_vec.maxSize=this->maxSize;
                    new_vec.array=this->array;                    
                    meminit(new_vec.array+this->size,other.array,other.size);
                    this->array=nullptr;
                    this->maxSize=0;
                }
                this->size=0;
                return new_vec;
            }
            Vector operator+(Vector&& other)&&{
                Vector<T> new_vec;
                new_vec.size=this->size+other.size;
                if(new_vec.size>other.maxSize and new_vec.size>this->maxSize){
                    new_vec.maxSize=new_vec.size*2;
                    new_vec.array=(T*)::operator new(new_vec.maxSize*sizeof(T));
                    memmoveinit(new_vec.array,this->array,this->size);
                    memmoveinit(new_vec.array+this->size,other.array,other.size);
                    memdestructor(this->array,this->size);
                    memdestructor(other.array,other.size);
                }
                else if(other.maxSize>this->maxSize){
                    new_vec.maxSize=other.maxSize;
                    new_vec.array=other.array;                    
                    if(this->size>other.size){
                        memmoveinit(new_vec.array+this->size,other.array,other.size);
                        memmove(new_vec.array,this->array,other.size);
                        memmoveinit(new_vec.array+other.size,this->array+other.size,this->size-other.size);
                    }
                    else{
                        memmoveinit(new_vec.array+other.size,other.array+other.size-this->size,this->size);
                        memmove(new_vec.array+this->size,other.array,other.size-this->size);
                        memmove(new_vec.array,this->array,this->size);
                    }
                    other.array=nullptr;
                    other.maxSize=0;
                    memdestructor(this->array,this->size);
                }
                else{
                    new_vec.maxSize=this->maxSize;
                    new_vec.array=this->array;
                    memmoveinit(new_vec.array+this->size,other.array,other.size);
                    this->array=nullptr;
                    this->maxSize=0;
                    memdestructor(other.array,other.size);
                }
                this->size=0;
                other.size=0;
                return new_vec;
            }
            Vector& operator+=(Vector&& other){
                if(this->maxSize<this->size+other.size){
                    T* aux=this->array;
                    this->maxSize=(this->size+other.size)*2;
                    this->array=(T*)::operator new(this->maxSize*sizeof(T));
                    memmoveinit(this->array,aux,this->size);
                    memmoveinit(this->array+this->size,other.array,other.size);
                    memdestructor(aux,this->size);
                    ::operator delete(aux);
                }
                else{
                    memmoveinit(this->array+this->size,other.array,other.size);
                }
                this->size+=other.size;
                memdestructor(other.array,other.size);  //lo mismo que en operator+(Vector&&)
                other.size=0;
                return *this;
            }
            Vector& operator+=(const Vector& other){
                if(this->maxSize<this->size+other.size){
                    T* aux=this->array;
                    this->maxSize=(this->size+other.size)*2;
                    this->array=(T*)::operator new(this->maxSize*sizeof(T));
                    if(aux==other.array)
                        meminit(this->array,aux,this->size);
                    else
                        memmoveinit(this->array,aux,this->size);
                    meminit(this->array+this->size,other.array,other.size);
                    memdestructor(aux,this->size);
                    ::operator delete(aux);
                }
                else{
                    meminit(this->array+this->size,other.array,other.size);
                }
                this->size+=other.size;
                return *this;
            }
            Vector operator*(const unsigned short num)const{
                Vector new_vec;
                if(num==0)
                    return new_vec;
                new_vec.maxSize=this->size*num*2;
                new_vec.array=(T*)::operator new(new_vec.maxSize*sizeof(T));
                for(unsigned int i=0;i<num;i++){
                    meminit(new_vec.array+new_vec.size,this->array,this->size);
                    new_vec.size+=this->size;
                }
                return new_vec;
            }
            Vector& operator*=(const unsigned short num){
                if(num==0){
                    memdestructor(array,size);
                    size=0;
                }
                else{
                    ssize_t new_size=size*num;
                    T* aux=this->array;
                    maxSize=new_size*2;
                    array=(T*)::operator new(maxSize*sizeof(T));
                    memmoveinit(array,aux,size);
                    memdestructor(aux,size);
                    ::operator delete(aux);
                    for(ssize_t i=1,prevSize=size;i<num;i++,size+=prevSize)
                        meminit(array+size,array,prevSize);
                }
                return *this;
            }
            T& operator[](ssize_t index)const{
                if(index>=0)
                    return array[index];
                else
                    return array[index+size];
            }
            operator T*(){
                return array;
            }
            bool operator==(const Vector& other)const{
                if(this->size!=other.size)
                    return false;
                for(ssize_t i=0;i<size;i++){
                    if(this->array[i]!=other.array[i])
                        return false;
                }
                return true;
            }
            bool operator!=(const Vector& other)const{
                if(this->size!=other.size)
                    return true;
                for(ssize_t i=0;i<size;i++){
                    if(this->array[i]!=other.array[i])
                        return true;
                }
                return false;
            }
            void append(const T& data){
                if(size==maxSize){//no pongo >= porque size+1 nunca deberia ser mayor a maxSize
                    //ssize_t prev_maxSize=maxSize;
                    if(maxSize==0)
                        maxSize=min_maxSize;
                    else
                        maxSize*=2;
                    T* aux=array;
                    array=(T*)::operator new(maxSize*sizeof(T));
                    memmoveinit(array,aux,size);
                    memdestructor(aux,size);
                    ::operator delete(aux/* ,prev_maxSize*sizeof(T) */);
                }
                new(array+size++) T(data);
            }
            void append(T&& data){
                if(size==maxSize){//no pongo >= porque size nunca deberia ser mayor a maxSize
                    //ssize_t prev_maxSize=maxSize;
                    if(maxSize==0)
                        maxSize=min_maxSize;
                    else
                        maxSize*=2;
                    T* aux=array;
                    array=(T*)::operator new(maxSize*sizeof(T));
                    memmoveinit(array,aux,size);
                    memdestructor(aux,size);
                    ::operator delete(aux/* ,prev_maxSize*sizeof(T) */);
                }
                new(array+size++) T((T&&)data);
            }
            Vector& operator+=(std::initializer_list<T> initializerList){
                ssize_t new_size=size+initializerList.size();
                if(new_size>maxSize){
                    T* aux=array;
                    maxSize=new_size*2;
                    array=(T*)::operator new(maxSize*sizeof(T));
                    memmoveinit(array,aux,size);
                    memdestructor(aux,size);
                    ::operator delete(aux);
                }
                ssize_t i = 0;
                for(const T& v:initializerList)
                    new(array+size+i++)T((T&&)v);
                size=new_size;
                return *this;
            }
            void insert(ssize_t index,const T& data){
                if(index<0)
                    index+=size+1;
                if(size==maxSize){
                    //ssize_t prev_maxSize=maxSize;
                    if(maxSize==0)
                        maxSize=min_maxSize;
                    else
                        maxSize*=2;
                    T* aux=array;
                    array=(T*)::operator new(maxSize*sizeof(T));
                    memmoveinit(array,aux,index);
                    memmoveinit(array+index+1,aux+index,size-index);
                    new(array+index)T(data);
                    memdestructor(aux,size++);
                    ::operator delete(aux/* ,prev_maxSize*sizeof(T) */);
                }
                else{
                    new(array+size) T();
                    memmove(array+index+1,array+index,(size++)-index);
                    array[index]=data;
                }
            }
            void insert(ssize_t index,T&& data){
                if(index<0)
                    index+=size+1;
                if(size==maxSize){
                    //ssize_t prev_maxSize=maxSize;
                    if(maxSize==0)
                        maxSize=min_maxSize;
                    else
                        maxSize*=2;
                    T* aux=array;
                    array=(T*)::operator new(maxSize*sizeof(T));
                    memmoveinit(array,aux,index);
                    memmoveinit(array+index+1,aux+index,size-index);
                    new(array+index)T(data);
                    memdestructor(aux,size++);
                    ::operator delete(aux/* ,prev_maxSize*sizeof(T) */);
                }
                else{
                    new(array+size) T();
                    memmove(array+index+1,array+index,(size++)-index);
                    array[index]=(T&&)data;
                }
            }
            void del(ssize_t index=-1){
                if(index<0)
                    index+=size;
                memmove(array+index,array+index+1,(--size)-index);
                array[size].~T();
            }
            T pop(ssize_t index=-1){
                if(index<0)
                    index+=size;
                T aux=(T&&)(array[index]);
                memmove(array+index,array+index+1,(--size)-index);
                array[size].~T();
                return aux;
            }
            Vector concat(ssize_t index,const Vector& other){
                if(index<0)
                    index+=this->size+1;
                Vector new_vec(reserve((this->size+other.size)*2));
                new_vec.size=this->size+other.size;
                meminit(new_vec.array,this->array,index);
                meminit(new_vec.array+index,other.array,other.size);
                meminit(new_vec.array+index+other.size,this->array+index,this->size-index);
                return new_vec;
            }
            Vector concat(ssize_t index,Vector&& other){
                if(index<0)
                    index+=this->size+1;
                ssize_t new_size=this->size+other.size;
                if(new_size>other.maxSize){
                    Vector new_vec(reserve((new_size)*2));
                    new_vec.size=new_size;
                    meminit(new_vec.array,this->array,index);
                    memmoveinit(new_vec.array+index,other.array,other.size);
                    meminit(new_vec.array+index+other.size,this->array+index,this->size-index);
                    return new_vec;                    
                }
                else{
                    Vector new_vec;
                    new_vec.array=other.array;
                    new_vec.maxSize=other.maxSize;
                    new_vec.size=new_size;
                    if(index<other.size){
                        memmoveinit(new_vec.array+other.size,other.array+other.size-index,index);
                        memmove(new_vec.array+index,other.array,other.size-index);
                        memcopy(new_vec.array,this->array,index);                        
                    }
                    else{
                        memmoveinit(new_vec.array+index,other.array,other.size);
                        memcopy(new_vec.array,this->array,other.size);
                        meminit(new_vec.array+other.size,this->array+other.size,index-other.size);
                    }
                    meminit(new_vec.array+index+other.size,this->array+index,new_size-(index+other.size));
                    other.array=nullptr;
                    other.maxSize=0;
                    other.size=0;
                    return new_vec;
                }
            }
            void join(ssize_t index,Vector other){
                if(other.size==0)
                    return;
                if(index<0)
                    index+=this->size+1;
                ssize_t new_size=this->size+other.size;
                if(this->size==0 and index==0){
                    *this=(Vector&&)other;
                    return;
                }
                if(new_size>maxSize){
                    T* aux=this->array;
                    this->maxSize=new_size*2;
                    this->array=(T*)::operator new(this->maxSize*sizeof(T));
                    memmoveinit(this->array,aux,index);
                    memmoveinit(this->array+index,other.array,other.size);
                    memmoveinit(this->array+index+other.size,aux+index,this->size-index);
                    memdestructor(aux,this->size);
                    ::operator delete(aux);
                }
                else{
                    new(this->array+this->size) T[other.size];
                    memmove(this->array+index+other.size,this->array+index,this->size-index);
                    memmove(this->array+index,other.array,other.size);
                }
                this->size=new_size;
            }
            Vector sublist(ssize_t start,ssize_t end=-1)const{
                Vector new_vec;
                if(start<0)
                    start+=size+1;
                if(end<0)
                    end+=size+1;
                new_vec.size=end-start;
                if(new_vec.size!=0){
                    new_vec.maxSize=new_vec.size*2;
                    new_vec.array=(T*)::operator new(new_vec.maxSize*sizeof(T));
                    meminit(new_vec.array,this->array+start,new_vec.size);
                }
                return new_vec;
            }
            Vector cut(ssize_t start,ssize_t end=-1){
                Vector new_vec;
                if(start<0)
                    start+=size+1;
                if(end<0)
                    end+=size+1;
                new_vec.size=end-start;
                if(new_vec.size!=0){
                    new_vec.maxSize=new_vec.size*2;
                    new_vec.array=(T*)::operator new(new_vec.maxSize*sizeof(T));
                    memmoveinit(new_vec.array,this->array+start,new_vec.size);
                    memmove(this->array+start,this->array+end,this->size-end);
                    this->size-=new_vec.size;
                    memdestructor(this->array+this->size,new_vec.size);
                }
                return new_vec;
            }
            void wipe(ssize_t start,ssize_t end=-1){
                if(start<0)
                    start+=size+1;
                if(end<0)
                    end+=size+1;
                ssize_t deletedSize=end-start;
                memmove(this->array+start,this->array+end,this->size-end);
                this->size-=deletedSize;
                memdestructor(this->array+this->size,deletedSize);
            }
            void resize(ssize_t new_size){
                if(new_size>maxSize){
                    T* aux=array;
                    maxSize=new_size*2;
                    array=(T*)::operator new(maxSize*sizeof(T));
                    memmoveinit(array,aux,size);
                    new(array+size) T[new_size-size]{};
                    memdestructor(aux,size);
                    ::operator delete(aux/* ,maxSize*sizeof(T) */);
                }
                else{
                    if(size>new_size)
                        memdestructor(array+new_size,size-new_size);
                    else if(size<new_size)
                        new(array+size) T[new_size-size]{};
                }
                size=new_size;
            }
            template<typename D>
            ssize_t find(const D& key,ssize_t start=0,ssize_t end=-1)const{	//retorna -1 si no se encontro el elemento
                if(start<0)
                    start+=size+1;
                if(end<0)
                    end+=size+1;
                for(ssize_t i=start;i<end;i++){
                    if(array[i]==key)
                        return i;
                }
                return -1;
            }
            template<typename D>
            ssize_t binary_search(const D& key,bool reverse=false){
                return binary_search(array,size,key,reverse);
            }
            template<typename D>
            ssize_t count(const D& data)const{
                ssize_t counter=0;
                for(ssize_t i=0;i<size;i++)
                    if(array[i]==data)
                        counter++;
                return counter;
            }
            template<typename D>
			bool has(const D& data)const{
				for(ssize_t i=0;i<size;i++){
					if(array[i]==data)
						return true;
				}
				return false;
			}
            template<typename D>
            unsigned int remove(const D& data,bool all=false){
                unsigned int found=0;
                for(ssize_t i=0;i<size;i++){
                    if(array[i]==data){
                        memmove(array+i,array+i+1,(--size)-i);
                        array[size].~T();
                        if(not all) return 1;
                        ++found;
                    }
                }
                return found;
            }
            ssize_t len()const{
                return size;
            }
            ssize_t get_maxSize()const{
                return maxSize;
            }
#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
            void print(bool reverse=false){
                if(size==0){
                    std::cout<<"[]";
                    return;
                }
                std::cout<<'[';
                if(not reverse){
                    for(ssize_t i=0;i<size;i++){
                        std::cout<<array[i];
                        if(array+i!=array+size-1)
                            std::cout<<", ";
                    }
                }
                else{
                    for(ssize_t i=size-1;i>=0;i--){
                        std::cout<<array[i];
                        if(array+i!=array)
                            std::cout<<", ";
                    }
                }
                std::cout<<']';
            }
            void println(bool reverse=false){
                print(reverse);
                std::cout<<std::endl;
            }
#endif
            void sort(bool reverse=false)const{
                insertion(array,size,reverse);
            }
            template<typename D>
            void sort(D(*func)(const T&),bool reverse=false)const{
                insertion(array,size,func,reverse);
            }
            template<typename Cmp>
            void sort(const Cmp& cmp)const{
                insertion(array,size,cmp);
            }
			template<typename Lambda>
			bool any(Lambda&& func){
                for(T& val:*this)
                    if(func(val))return 1;
                return 0;
			}
			template<typename Lambda>
			void forEach(Lambda&& func){
				for(T& val:*this)
					func(val);
			}
			template<typename Lambda>
			auto map(Lambda&& func){
				Vector<decltype(func(array[0]))> new_vec(reserve(this->maxSize));
				for(T& val:*this)
					new_vec.append(func(val));
				return new_vec;
			}
			template<typename Lambda>
			Vector filter(Lambda&& func){
				Vector new_vec(reserve(this->maxSize));
				for(T& val:*this){
					if(func(val))new_vec.append(val);
				}
				return new_vec;
			}
            void reverse()const{
                reverse(array,size);
            }
			bool empty()const{
				return size==0;
			}
            void clear(){
                memdestructor(array,size);
                size=0;
            }
            void free(){
                memdestructor(array,size);
                size=0;
                delete[] array;
                array=nullptr;
                maxSize=0;
            }
            T* getbuffer(){
                return array;
            }
            ~Vector(){
                memdestructor(array,size);
                ::operator delete(array/* ,maxSize*sizeof(T) */);
            }
            class iterator COUNT_IT{
                private:
                    T* ptr;
                public:
                    iterator(T* ptr):ptr(ptr){}
                    T& operator*(){
                        return *ptr;
                    }
                    T* operator->(){
                        return ptr;
                    }
                    iterator& operator++(){
                        ++ptr;
                        return *this;
                    }
                    iterator operator++(int){
                        iterator aux=*this;
                        ++ptr;
                        return aux;
                    }
                    iterator& operator--(){
                        --ptr;
                        return *this;
                    }
                    iterator operator--(int){
                        iterator aux=*this;
                        --ptr;
                        return aux;
                    }
                    iterator operator+(ssize_t n){
                        return iterator(this->ptr+n);
                    }
                    iterator operator-(ssize_t n){
                        return iterator(this->ptr-n);
                    }
                    iterator& operator+=(ssize_t n){
                        ptr+=n;
                        return *this;
                    }
                    iterator& operator-=(ssize_t n){
                        ptr-=n;
                        return *this;
                    }
                    bool operator==(const iterator& other)const{
                        return this->ptr==other.ptr;
                    }
                    bool operator!=(const iterator& other)const{
                        return this->ptr!=other.ptr;
                    }
                    bool operator<=(const iterator& other)const{
                        return this->ptr<=other.ptr;
                    }
                    bool operator>=(const iterator& other)const{
                        return this->ptr>=other.ptr;
                    }
                    bool operator<(const iterator& other)const{
                        return this->ptr<other.ptr;
                    }
                    bool operator>(const iterator& other)const{
                        return this->ptr>other.ptr;
                    }
            };
            iterator begin()const{
                return iterator(array);
            }
            iterator end()const{
                return iterator(array+size);
            }
            iterator rbegin()const{
                return iterator(array+size-1);
            }
            iterator rend()const{
                return iterator(array-1);
            }
    };

#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
    template<typename T, ssize_t mms>
    std::ostream& operator<<(std::ostream& stream,const Vector<T,mms>& list){
        stream<<'[';
        for(ssize_t i=0,size=list.len();i<size;i++){
            stream<<list[i];
            if(i<size-1)
                stream<<", ";		
        }
        return stream<<']';
    }
#endif
}