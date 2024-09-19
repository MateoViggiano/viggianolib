#pragma once
#include"../arrays.hpp"
#ifndef VECTOR_MMS
#define VECTOR_MMS 64
#endif
namespace mpv{
    template<typename T,typename Alloc=allocator<T>,typename realloc_params=params<typename allocator_traits<Alloc>::template rebind_traits<T>::size_type,VECTOR_MMS,2>>
    class Vector:EBCO<rebind_alloc<Alloc,T>> COUNT_IT_ {static_assert(realloc_params::p1>0,"min_maxSize cannot be less than 1");
        private:
            using AlTy=rebind_alloc<Alloc,T>;
            using AlTy_traits=allocator_traits<AlTy>;
			struct Val_types{
				using value_type=T;
				using size_type=typename AlTy_traits::size_type;
				using difference_type=typename AlTy_traits::difference_type;
				using pointer=typename AlTy_traits::pointer;
				using const_pointer=typename AlTy_traits::const_pointer;
				using reference=T&;
				using const_reference=const T&;
			};
            static constexpr typename Val_types::size_type min_maxSize=realloc_params::p1;
            static constexpr typename Val_types::size_type realloc_factor=realloc_params::p2;
            static constexpr bool POCCA=AlTy_traits::propagate_on_container_copy_assignment::value;
            static constexpr bool POCMA=AlTy_traits::propagate_on_container_move_assignment::value;
            static constexpr bool POCS=AlTy_traits::propagate_on_container_swap::value;
            static constexpr bool ALWAYS_EQ=AlTy_traits::is_always_equal::value;
        public:
			using allocator_type=Alloc;
			using value_type=typename Val_types::value_type;
			using size_type=typename Val_types::size_type;
			using difference_type=typename Val_types::difference_type;
			using pointer=typename Val_types::pointer;
			using const_pointer=typename Val_types::const_pointer;
			using reference=typename Val_types::reference;
			using const_reference=typename Val_types::const_reference;
            using iterator=random_access_iterator<Val_types>;
            using const_iterator=const_random_access_iterator<Val_types>;
        private:
            size_type size=0;
            size_type maxSize=0;
            pointer array=nullptr;
            inline void destroy_and_free(){
                for(size_type i{};i<size;i++)
                    AlTy_traits::destroy(this->get_val(),array+i);
                AlTy_traits::deallocate(this->get_val(),array,maxSize);
            }
            inline void reset(){
                this->size=size_type();
                this->maxSize=size_type();
                this->array=pointer();
            }
            inline void destroy_free_and_reset(){
                for(size_type i{};i<size;i++)
                    AlTy_traits::destroy(this->get_val(),array+i);
                AlTy_traits::deallocate(this->get_val(),array,maxSize);
                this->size=size_type();
                this->maxSize=size_type();
                this->array=pointer();
            }
            inline void copy_elements(const Vector& other){
                this->size=other.size;
                this->maxSize=other.maxSize;                
                this->array=AlTy_traits::allocate(this->get_val(),this->maxSize);
                al::memcopyconstruct(this->get_val(),this->array,other.array,other.size);
            }
            inline void copy_elements(Vector&& other){
                this->size=other.size;
                this->maxSize=other.maxSize;                
                this->array=AlTy_traits::allocate(this->get_val(),this->maxSize);
                al::memmoveconstruct(this->get_val(),this->array,other.array,other.size);
                other.destroy_free_and_reset();
            }
            inline void move_elements(Vector& other){
                this->size=other.size;
                this->maxSize=other.maxSize;
                this->array=other.array;
                other.reset();
            }
            inline void copy_elements_keeping_this_allocation(const Vector& other){//keeps this allocation if possible
                if(this->maxSize<other.size){
                    this->destroy_and_free();
                    this->copy_elements(other);
                }
                else{
                    if(this->size>=other.size){
                        al::memdestructor(this->get_val(),this->array+other.size,this->size-other.size);
                        al::memcopy<AlTy>(this->array,other.array,other.size); 
                    }
                    else{
                        al::memcopy<AlTy>(this->array,other.array,this->size);
                        al::memcopyconstruct(this->get_val(),this->array+this->size,other.array+this->size,other.size-this->size);
                    }
                    this->size=other.size;                    
                }
            }
            inline void copy_elements_keeping_this_allocation(Vector&& other){//keeps this allocation if possible
                if(this->maxSize<other.size){
                    this->destroy_and_free();
                    this->copy_elements(static_cast<Vector&&>(other));
                }
                else{
                    if(this->size>=other.size){
                        al::memdestructor(this->get_val(),this->array+other.size,this->size-other.size);
                        al::memmove<AlTy>(this->array,other.array,other.size); 
                    }
                    else{
                        al::memmove<AlTy>(this->array,other.array,this->size);
                        al::memmoveconstruct(this->get_val(),this->array+this->size,other.array+this->size,other.size-this->size);
                    }
                    this->size=other.size;
                    other.destroy_free_and_reset();                    
                }
            }
            inline Vector& add(const Vector& v1,const Vector& v2){// Estas funciones no se encargan de calcular size.
                this->maxSize=this->size*realloc_factor;
                this->array=AlTy_traits::allocate(this->get_val(),this->maxSize);
                al::memcopyconstruct(this->get_val(),this->array,v1.array,v1.size);
                al::memcopyconstruct(this->get_val(),this->array+v1.size,v2.array,v2.size);
                return *this;
            }
            inline Vector& add(const Vector& v1,Vector&& v2){
                this->maxSize=this->size*realloc_factor;
                this->array=AlTy_traits::allocate(this->get_val(),this->maxSize);
                al::memcopyconstruct(this->get_val(),this->array,v1.array,v1.size);
                al::memmoveconstruct(this->get_val(),this->array+v1.size,v2.array,v2.size);
                v2.destroy_free_and_reset();
                return *this;
            }
            inline Vector& add(Vector&& v1,const Vector& v2){
                this->maxSize=this->size*realloc_factor;
                this->array=AlTy_traits::allocate(this->get_val(),this->maxSize);
                al::memmoveconstruct(this->get_val(),this->array,v1.array,v1.size);
                al::memcopyconstruct(this->get_val(),this->array+v1.size,v2.array,v2.size);
                v1.destroy_free_and_reset();
                return *this;
            }
            inline Vector& add(Vector&& v1,Vector&& v2){
                this->maxSize=this->size*realloc_factor;
                this->array=AlTy_traits::allocate(this->get_val(),this->maxSize);
                al::memmoveconstruct(this->get_val(),this->array,v1.array,v1.size);
                al::memmoveconstruct(this->get_val(),this->array+v1.size,v2.array,v2.size);
                v1.destroy_free_and_reset();
                v2.destroy_free_and_reset();
                return *this;
            }
        public:
            Vector(){}
            explicit Vector(const Alloc& al):EBCO<AlTy>(al){}
            Vector(reserve_tag,size_type reserved,const Alloc& al=Alloc()):EBCO<AlTy>(al),maxSize(reserved),array(reserved>0 ? AlTy_traits::allocate(this->get_val(),reserved) : nullptr){}
            Vector(const Vector& other):EBCO<AlTy>(AlTy_traits::select_on_container_copy_construction(other.get_val())),size(other.size),maxSize(other.maxSize),array(other.maxSize>0 ? AlTy_traits::allocate(this->get_val(),other.maxSize) : nullptr){
                al::memcopyconstruct(this->get_val(),this->array,other.array,other.size);
            }
            Vector(const Vector& other,const Alloc& al):EBCO<AlTy>(al),size(other.size),maxSize(other.maxSize),array(other.maxSize>0 ? AlTy_traits::allocate(this->get_val(),other.maxSize) : nullptr){
                al::memcopyconstruct(this->get_val(),this->array,other.array,other.size);
            }
            Vector(Vector&& other):EBCO<AlTy>(static_cast<AlTy&&>(other.get_val())),size(other.size),maxSize(other.maxSize),array(other.array){
                other.reset();
            }
            Vector(Vector&& other,const Alloc& al):EBCO<AlTy>(al),size(other.size),maxSize(other.maxSize){
                if(this->get_val()==other.get_val()){
                    this->array=other.array;
                    other.reset();
                }
                else{
                    this->array=AlTy_traits::allocate(this->get_val(),maxSize);
                    al::memmoveconstruct(this->get_val(),this->array,other.array,size);
                    other.destroy_free_and_reset();
                }
            }
            Vector(size_type sz):size(sz),maxSize(size*realloc_factor),array(size>0 ? AlTy_traits::allocate(this->get_val(),maxSize) : nullptr){
                for(size_type i=0;i<size;i++)
                    AlTy_traits::construct(this->get_val(),array+i);
            }
            Vector(size_type sz,const Alloc& al):EBCO<AlTy>(al),size(sz),maxSize(size*realloc_factor),array(size>0 ? AlTy_traits::allocate(this->get_val(),maxSize) : nullptr){
                for(size_type i=0;i<size;i++)
                    AlTy_traits::construct(this->get_val(),array+i);
            }
            Vector(size_type sz,const_reference fillwith,const Alloc& al=Alloc()):EBCO<AlTy>(al),size(sz),maxSize(size*realloc_factor),array(size>0 ? AlTy_traits::allocate(this->get_val(),maxSize) : nullptr){
                al::memfillconstruct(this->get_val(),array,fillwith,size);
            }
            Vector(std::initializer_list<value_type> initializerList):size(initializerList.size()),maxSize(size*realloc_factor),array(size>0 ? AlTy_traits::allocate(this->get_val(),maxSize) : nullptr){
                size_type i=0;
                for(const_reference v:initializerList)
                    AlTy_traits::construct(this->get_val(),array+i++,v);
            }
            Vector(std::initializer_list<value_type> initializerList,const Alloc& al):EBCO<AlTy>(al),size(initializerList.size()),maxSize(size*realloc_factor),array(size>0 ? AlTy_traits::allocate(this->get_val(),maxSize) : nullptr){
                size_type i=0;
                for(const_reference v:initializerList)
                    AlTy_traits::construct(this->get_val(),array+i++,v);
            }
           template<typename Iterable,enable_if_t<is_iterable_v<Iterable> && !has_len_v<Iterable> && !has_size_v<Iterable>>* = nullptr>
           Vector(const Iterable& lis){
				for(const_reference v:lis)
                   this->append(v);
           }
           template<typename Iterable,enable_if_t<is_iterable_v<Iterable> && (has_len_v<Iterable> || (has_len_v<Iterable> && has_size_v<Iterable>))>* = nullptr>
           Vector(const Iterable& lis):size(lis.len()),maxSize(size*realloc_factor),array(size>0 ? AlTy_traits::allocate(this->get_val(),maxSize) : nullptr){
               size_type i=0;
				for(const_reference v:lis)
                   AlTy_traits::construct(this->get_val(),array+i++,v);
           }
           template<typename Iterable,enable_if_t<is_iterable_v<Iterable> && has_size_v<Iterable> && !has_len_v<Iterable>>* = nullptr>
           Vector(const Iterable& lis):size(lis.size()),maxSize(size*realloc_factor),array(size>0 ? AlTy_traits::allocate(this->get_val(),maxSize) : nullptr){
               size_type i=0;
				for(const_reference v:lis)
                   AlTy_traits::construct(this->get_val(),array+i++,v);
           }
           template<typename Iterable,enable_if_t<is_iterable_v<Iterable> && !has_len_v<Iterable> && !has_size_v<Iterable>>* = nullptr>
           Vector(const Iterable& lis,const Alloc& al):EBCO<AlTy>(al){
				for(const_reference v:lis)
                   this->append(v);
           }
           template<typename Iterable,enable_if_t<is_iterable_v<Iterable> && (has_len_v<Iterable> || (has_len_v<Iterable> && has_size_v<Iterable>))>* = nullptr>
           Vector(const Iterable& lis,const Alloc& al):EBCO<AlTy>(al),size(lis.len()),maxSize(size*realloc_factor),array(size>0 ? AlTy_traits::allocate(this->get_val(),maxSize) : nullptr){
               size_type i=0;
				for(const_reference v:lis)
                   AlTy_traits::construct(this->get_val(),array+i++,v);
           }
           template<typename Iterable,enable_if_t<is_iterable_v<Iterable> && has_size_v<Iterable> && !has_len_v<Iterable>>* = nullptr>
           Vector(const Iterable& lis,const Alloc& al):EBCO<AlTy>(al),size(lis.size()),maxSize(size*realloc_factor),array(size>0 ? AlTy_traits::allocate(this->get_val(),maxSize) : nullptr){
               size_type i=0;
				for(const_reference v:lis)
                   AlTy_traits::construct(this->get_val(),array+i++,v);
           }
            Vector& operator=(const Vector& other){
                if(this==&other) return *this;
                if constexpr(POCCA and not ALWAYS_EQ){
                    if(this->get_val()!=other.get_val()){
                        this->destroy_and_free();
                        this->get_val()=other.get_val();
                        this->copy_elements(other);
                        return *this;
                    }
                }
                this->copy_elements_keeping_this_allocation(other);
                return *this;
            }
            Vector& operator=(Vector&& other){
                if(this==&other) return *this;
                if constexpr(not ALWAYS_EQ and not POCMA){
                    if(this->get_val()!=other.get_val()){
                        this->copy_elements_keeping_this_allocation(static_cast<Vector&&>(other));
                        return *this;
                    }
                }
                this->destroy_and_free();
                if constexpr(POCMA) this->get_val()=static_cast<AlTy&&>(other.get_val());
                this->move_elements(other);
                return *this;
            }
            Vector operator+(const Vector& other)const&{
                Vector new_vec;
                new_vec.size=this->size+other.size;
                new_vec.maxSize=new_vec.size*realloc_factor;
                new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxSize);
                al::memcopyconstruct(new_vec.get_val(),new_vec.array,this->array,this->size);
                al::memcopyconstruct(new_vec.get_val(),new_vec.array+this->size,other.array,other.size);
                return new_vec;
            }
            Vector operator+(Vector&& other)const&{
                //if(this==&other) return *this+other;     //shouldn't add an object with an rvalue ref to itself
                Vector new_vec;
                new_vec.size=this->size+other.size;
                if constexpr(not ALWAYS_EQ and not POCMA){
                    if(new_vec.get_val()!=other.get_val())
                        return new_vec.add(*this,static_cast<Vector&&>(other));
                }
                if(new_vec.size>other.maxSize){
                    return new_vec.add(*this,static_cast<Vector&&>(other));
                }
                else{
                    new_vec.maxSize=other.maxSize;
                    new_vec.array=other.array;
                    if constexpr(not ALWAYS_EQ and POCMA) new_vec.get_val()=static_cast<AlTy&&>(other.get_val());
                    if(this->size>other.size){
                        al::memmoveconstruct(new_vec.get_val(),new_vec.array+this->size,other.array,other.size);
                        al::memcopy<AlTy>(new_vec.array,this->array,other.size);
                        al::memcopyconstruct(new_vec.get_val(),new_vec.array+other.size,this->array+other.size,this->size-other.size);
                    }
                    else{
                        al::memmoveconstruct(new_vec.get_val(),new_vec.array+other.size,other.array+other.size-this->size,this->size);
                        al::reversemove<AlTy>(new_vec.array+this->size,other.array,other.size-this->size);
                        al::memcopy<AlTy>(new_vec.array,this->array,this->size);
                    }
                    other.reset();
                    return new_vec;                    
                }
            }
            Vector operator+(const Vector& other)&&{
                Vector new_vec;
                new_vec.size=this->size+other.size;
                if constexpr(not ALWAYS_EQ and not POCMA){
                    if(new_vec.get_val()!=this->get_val())
                        return new_vec.add(static_cast<Vector&&>(*this),other);
                }
                if(new_vec.size>this->maxSize){
                    return new_vec.add(static_cast<Vector&&>(*this),other);
                }
                else{
                    new_vec.maxSize=this->maxSize;
                    new_vec.array=this->array;         
                    if constexpr(POCMA) new_vec.get_val()=static_cast<AlTy&&>(this->get_val());           
                    al::memcopyconstruct(new_vec.get_val(),new_vec.array+this->size,other.array,other.size);
                    this->reset();
                    return new_vec;                    
                }
            }
            Vector operator+(Vector&& other)&&{
                Vector new_vec;
                new_vec.size=this->size+other.size;
                if constexpr(ALWAYS_EQ or POCMA){
                    if(new_vec.size<=this->maxSize){
                        new_vec.maxSize=this->maxSize;
                        new_vec.array=this->array;
                        if constexpr(not ALWAYS_EQ and POCMA) new_vec.get_val()=static_cast<AlTy&&>(this->get_val());
                        al::memmoveconstruct(new_vec.get_val(),new_vec.array+this->size,other.array,other.size);
                        other.destroy_free_and_reset();
                        this->reset();
                        return new_vec;
                    }
                    else if(new_vec.size<=other.maxSize){
                        new_vec.maxSize=other.maxSize;
                        new_vec.array=other.array;
                        if constexpr(not ALWAYS_EQ and POCMA) new_vec.get_val()=static_cast<AlTy&&>(other.get_val());
                        if(this->size>other.size){
                            al::memmoveconstruct(new_vec.get_val(),new_vec.array+this->size,other.array,other.size);
                            al::memmove<AlTy>(new_vec.array,this->array,other.size);
                            al::memmoveconstruct(new_vec.get_val(),new_vec.array+other.size,this->array+other.size,this->size-other.size);
                        }
                        else{
                            al::memmoveconstruct(new_vec.get_val(),new_vec.array+other.size,other.array+other.size-this->size,this->size);
                            al::reversemove<AlTy>(new_vec.array+this->size,other.array,other.size-this->size);
                            al::memmove<AlTy>(new_vec.array,this->array,this->size);
                        }
                        this->destroy_free_and_reset();
                        other.reset();
                        return new_vec;
                    }
                    else
                        return new_vec.add(static_cast<Vector&&>(*this),static_cast<Vector&&>(other));
                }
                else{
                    if(new_vec.get_val()==this->get_val() and new_vec.size<=this->maxSize){
                        new_vec.maxSize=this->maxSize;
                        new_vec.array=this->array;
                        al::memmoveconstruct(new_vec.get_val(),new_vec.array+this->size,other.array,other.size);
                        other.destroy_free_and_reset();
                        this->reset();
                        return new_vec;
                    }
                    else if(new_vec.get_val()==other.get_val() and new_vec.size<=other.maxSize){
                        new_vec.maxSize=other.maxSize;
                        new_vec.array=other.array;
                        if(this->size>other.size){
                            al::memmoveconstruct(new_vec.get_val(),new_vec.array+this->size,other.array,other.size);
                            al::memmove<AlTy>(new_vec.array,this->array,other.size);
                            al::memmoveconstruct(new_vec.get_val(),new_vec.array+other.size,this->array+other.size,this->size-other.size);
                        }
                        else{
                            al::memmoveconstruct(new_vec.get_val(),new_vec.array+other.size,other.array+other.size-this->size,this->size);
                            al::reversemove<AlTy>(new_vec.array+this->size,other.array,other.size-this->size);
                            al::memmove<AlTy>(new_vec.array,this->array,this->size);
                        }
                        this->destroy_free_and_reset();
                        other.reset();
                        return new_vec;
                    }
                    else
                        return new_vec.add(static_cast<Vector&&>(*this),static_cast<Vector&&>(other));
                }
            }
            Vector& operator+=(Vector&& other){
                if(this->maxSize<this->size+other.size){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),(this->size+other.size)*realloc_factor);
                    al::memmoveconstruct(this->get_val(),new_array,this->array,this->size);
                    al::memmoveconstruct(this->get_val(),new_array+this->size,other.array,other.size);
                    this->destroy_and_free();//Para el correcto funcionamiento de esta funcion no se deben cambiar los valores de array,size ni maxSize antes de ser llamada.
                    this->maxSize=(this->size+other.size)*realloc_factor;
                    this->array=new_array;
                }
                else{
                    al::memmoveconstruct(this->get_val(),this->array+this->size,other.array,other.size);
                }
                this->size+=other.size;
                other.destroy_free_and_reset();
                return *this;
            }
            Vector& operator+=(const Vector& other){
                if(this->maxSize<this->size+other.size){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),(this->size+other.size)*realloc_factor);
                    if(this->array==other.array){
                        al::memcopyconstruct(this->get_val(),new_array,this->array,this->size);
                        al::memmoveconstruct(this->get_val(),new_array+this->size,other.array,other.size);// Muevo other porque es el mismo objeto que this, osea que de todas formas se va a borrar.
                    }
                    else{
                        al::memmoveconstruct(this->get_val(),new_array,this->array,this->size);
                        al::memcopyconstruct(this->get_val(),new_array+this->size,other.array,other.size);
                    }
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxSize=(this->size+other.size)*realloc_factor;
                }
                else{
                    al::memcopyconstruct(this->get_val(),this->array+this->size,other.array,other.size);
                }
                this->size+=other.size;
                return *this;
            }
            Vector operator*(size_type num)const{
                Vector new_vec;
                if(num==0)
                    return new_vec;
                new_vec.maxSize=this->size*num*realloc_factor;
                new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxSize);
                for(size_type i=0;i<num;i++){
                    al::memcopyconstruct(new_vec.get_val(),new_vec.array+new_vec.size,this->array,this->size);
                    new_vec.size+=this->size;
                }
                return new_vec;
            }
            Vector& operator*=(size_type num){
                if(num==0){
                    al::memdestructor(this->get_val(),array,size);
                    size=0;
                }
                else{
                    if(size*num>this->maxSize){
                        pointer new_array=AlTy_traits::allocate(this->get_val(),size*num*realloc_factor);
                        al::memmoveconstruct(this->get_val(),new_array,array,size);
                        this->destroy_and_free();
                        this->array=new_array;
                        this->maxSize=size*num*realloc_factor;
                    }
                    for(size_type i=1,prevSize=size;i<num;i++,size+=prevSize)
                        al::memcopyconstruct(this->get_val(),array+size,array,prevSize);
                }
                return *this;
            }
            const_reference operator[](size_type index)const{
                return array[index];
            }
            reference operator[](size_type index){
                return array[index];
            }
            const_reference back()const{
                return array[size-1];
            }
            reference back(){
                return array[size-1];
            }
            bool operator==(const Vector& other)const{
                if(this->size!=other.size)
                    return false;
                for(size_type i=0;i<size;i++){
                    if(this->array[i]!=other.array[i])
                        return false;
                }
                return true;
            }
            bool operator!=(const Vector& other)const{
                if(this->size!=other.size)
                    return true;
                for(size_type i=0;i<size;i++){
                    if(this->array[i]!=other.array[i])
                        return true;
                }
                return false;
            }
            template<typename... Args>
            void emplace_back(Args&&... args){
                if(size==maxSize){//si no tiene espacio para ningun elemento mas
                    size_type new_maxSize = maxSize==0 ? min_maxSize : maxSize*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(), new_maxSize);
                    al::memmoveconstruct(this->get_val(),new_array,array,size);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxSize=new_maxSize;
                }
                AlTy_traits::construct(this->get_val(),array+size++,static_cast<Args&&>(args)...);
            }
            void append(const_reference data){
                if(size==maxSize){//si no tiene espacio para ningun elemento mas
                    size_type new_maxSize = maxSize==0 ? min_maxSize : maxSize*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_maxSize);
                    al::memmoveconstruct(this->get_val(),new_array,array,size);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxSize=new_maxSize;
                }
                AlTy_traits::construct(this->get_val(),array+size++,data);
            }
            void append(value_type&& data){
                if(size==maxSize){//si no tiene espacio para ningun elemento mas
                    size_type new_maxSize = maxSize==0 ? min_maxSize : maxSize*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(), new_maxSize);
                    al::memmoveconstruct(this->get_val(),new_array,array,size);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxSize=new_maxSize;
                }
                AlTy_traits::construct(this->get_val(),array+size++,static_cast<value_type&&>(data));
            }
            Vector& operator+=(std::initializer_list<value_type> initializerList){
               {size_type new_size=size+initializerList.size();
                if(new_size>maxSize){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_size*realloc_factor);
                    al::memmoveconstruct(this->get_val(),new_array,array,size);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxSize=new_size*realloc_factor;
                }}
                for(const_reference v:initializerList)
                    AlTy_traits::construct(this->get_val(),array+size++,v);
                return *this;
            }
            template<typename... Args>
            void emplace(size_type index,Args&&... args){
                if(size==maxSize){
                    size_type new_maxSize = maxSize==0 ? min_maxSize : maxSize*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_maxSize);
                    al::memmoveconstruct(this->get_val(),new_array,array,index);
                    al::memmoveconstruct(this->get_val(),new_array+index+1,array+index,size-index);
                    AlTy_traits::construct(this->get_val(),new_array+index,static_cast<Args&&>(args)...);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxSize=new_maxSize;
                    this->size++;
                }
                else{
                    if(index==size)
                        AlTy_traits::construct(this->get_val(),array+size++,static_cast<Args&&>(args)...);
                    else{
                        AlTy_traits::construct(this->get_val(),array+size,static_cast<value_type&&>(array[size-1]));
                        al::reversemove<AlTy>(array+index+1,array+index,(size++)-index-1);
                        array[index]=value_type(static_cast<Args&&>(args)...);
                    }
                }
            }
            void insert(size_type index,const_reference data){
                if(size==maxSize){
                    size_type new_maxSize = maxSize==0 ? min_maxSize : maxSize*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_maxSize);
                    al::memmoveconstruct(this->get_val(),new_array,array,index);
                    al::memmoveconstruct(this->get_val(),new_array+index+1,array+index,size-index);
                    AlTy_traits::construct(this->get_val(),new_array+index,data);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxSize=new_maxSize;
                    this->size++;
                }
                else{
                    if(index==size)
                        AlTy_traits::construct(this->get_val(),array+size++,data);
                    else{
                        AlTy_traits::construct(this->get_val(),array+size,static_cast<value_type&&>(array[size-1]));
                        al::reversemove<AlTy>(array+index+1,array+index,(size++)-index-1);
                        array[index]=data;
                    }
                }
            }
            void insert(size_type index,value_type&& data){
                if(size==maxSize){
                    size_type new_maxSize = maxSize==0 ? min_maxSize : maxSize*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_maxSize);
                    al::memmoveconstruct(this->get_val(),new_array,array,index);
                    al::memmoveconstruct(this->get_val(),new_array+index+1,array+index,size-index);
                    AlTy_traits::construct(this->get_val(),new_array+index,static_cast<value_type&&>(data));
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxSize=new_maxSize;
                    this->size++;
                }
                else{
                    if(index==size)
                        AlTy_traits::construct(this->get_val(),array+size++,data);
                    else{
                        AlTy_traits::construct(this->get_val(),array+size,static_cast<value_type&&>(array[size-1]));
                        al::reversemove<AlTy>(array+index+1,array+index,(size++)-index-1);
                        array[index]=static_cast<value_type&&>(data);
                    }
                }
            }
            void del(size_type index){
                al::memmove<AlTy>(array+index,array+index+1,(--size)-index);
                AlTy_traits::destroy(this->get_val(),array+size);
            }
            value_type pop(size_type index){
                value_type aux=static_cast<value_type&&>(array[index]);
                al::memmove<AlTy>(array+index,array+index+1,(--size)-index);
                AlTy_traits::destroy(this->get_val(),array+size);
                return aux;
            }
            void del_back(){
                AlTy_traits::destroy(this->get_val(),array+(--size));
            }
            value_type pop_back(){
                value_type aux=static_cast<value_type&&>(array[size-1]);
                AlTy_traits::destroy(this->get_val(),array+(--size));
                return aux;
            }
            Vector concat(size_type index,const Vector& other)const{
                Vector new_vec(reserve_tag{},(this->size+other.size)*realloc_factor);
                new_vec.size=this->size+other.size;
                al::memcopyconstruct(new_vec.get_val(),new_vec.array,this->array,index);
                al::memcopyconstruct(new_vec.get_val(),new_vec.array+index,other.array,other.size);
                al::memcopyconstruct(new_vec.get_val(),new_vec.array+index+other.size,this->array+index,this->size-index);
                return new_vec;
            }
#define CONCAT_MOVE  new_vec.maxSize=new_vec.size*realloc_factor;\
                new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxSize);\
                al::memcopyconstruct(new_vec.get_val(),new_vec.array,this->array,index);\
                al::memmoveconstruct(new_vec.get_val(),new_vec.array+index,other.array,other.size);\
                al::memcopyconstruct(new_vec.get_val(),new_vec.array+index+other.size,this->array+index,this->size-index);\
                other.destroy_free_and_reset();\
                return new_vec;

            Vector concat(size_type index,Vector&& other)const{
                Vector new_vec;
                new_vec.size=this->size+other.size;
                if constexpr(not ALWAYS_EQ and not POCMA){
                    if(new_vec.get_val()!=other.get_val()){
                        CONCAT_MOVE
                    }
                }
                if(new_vec.size>other.maxSize){
                    CONCAT_MOVE
                }
                else{
                    new_vec.array=other.array;
                    new_vec.maxSize=other.maxSize;
                    if constexpr(POCMA) new_vec.get_val()=static_cast<AlTy&&>(other.get_val());
                    if(index<other.size){
                        al::memmoveconstruct(new_vec.get_val(),new_vec.array+other.size,other.array+other.size-index,index);
                        al::reversemove<AlTy>(new_vec.array+index,other.array,other.size-index);
                        al::memcopy<AlTy>(new_vec.array,this->array,index);                        
                    }
                    else{
                        al::memmoveconstruct(new_vec.get_val(),new_vec.array+index,other.array,other.size);
                        al::memcopy<AlTy>(new_vec.array,this->array,other.size);
                        al::memcopyconstruct(new_vec.get_val(),new_vec.array+other.size,this->array+other.size,index-other.size);
                    }
                    al::memcopyconstruct(new_vec.get_val(),new_vec.array+index+other.size,this->array+index,new_vec.size-(index+other.size));
                    other.reset();
                    return new_vec;
                }
            }
            void join(size_type index,const Vector& other){
                if(other.size==0) return;
                size_type new_size=this->size+other.size;
                if(this->size==0 and index==0){
                    *this=other;
                    return;
                }
                if(new_size>maxSize or this==&other){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_size*realloc_factor);
                    al::memcopyconstruct(this->get_val(),new_array+index,other.array,other.size);// Esta tiene que estar primera porque this y other pueden ser el mismo objeto, asi que primero hay que copiarlo y despues moverlo
                    al::memmoveconstruct(this->get_val(),new_array,this->array,index);
                    al::memmoveconstruct(this->get_val(),new_array+index+other.size,this->array+index,this->size-index);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxSize=new_size*realloc_factor;
                }
                else{
                    if(index+other.size>=this->size){
                        al::memmoveconstruct(this->get_val(),this->array+new_size-(this->size-index),this->array+index,this->size-index);
                        al::memcopy<AlTy>(this->array+index,other.array,this->size-index);
                        al::memcopyconstruct(this->get_val(),this->array+this->size,other.array+this->size-index,other.size-(this->size-index));                        
                    }
                    else{
                        al::memmoveconstruct(this->get_val(),this->array+this->size,this->array+this->size-other.size,other.size);
                        al::reversemove<AlTy>(this->array+index+other.size,this->array+index,this->size-index-other.size);
                        al::memcopy<AlTy>(this->array+index,other.array,other.size);
                    }
                }
                this->size=new_size;
            }
            void join(size_type index,Vector&& other){
                if(other.size==0) return;
                size_type new_size=this->size+other.size;
                if(this->size==0 and index==0){
                    *this=static_cast<Vector&&>(other);
                    return;
                }
                if(new_size>this->maxSize){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_size*realloc_factor);
                    al::memmoveconstruct(this->get_val(),new_array,this->array,index);
                    al::memmoveconstruct(this->get_val(),new_array+index,other.array,other.size);
                    al::memmoveconstruct(this->get_val(),new_array+index+other.size,this->array+index,this->size-index);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxSize=new_size*realloc_factor;
                }
                else{
                    if(index+other.size>=this->size){
                        al::memmoveconstruct(this->get_val(),this->array+new_size-(this->size-index),this->array+index,this->size-index);
                        al::memmove<AlTy>(this->array+index,other.array,this->size-index);
                        al::memmoveconstruct(this->get_val(),this->array+this->size,other.array+this->size-index,other.size-(this->size-index));
                    }
                    else{
                        al::memmoveconstruct(this->get_val(),this->array+this->size,this->array+this->size-other.size,other.size);
                        al::reversemove<AlTy>(this->array+index+other.size,this->array+index,this->size-index-other.size);
                        al::memmove<AlTy>(this->array+index,other.array,other.size);
                    }
                }
                other.destroy_free_and_reset();
                this->size=new_size;
            }
            Vector sublist(size_type start,size_type end)const{
                Vector new_vec;
                new_vec.size=end-start;
                if(new_vec.size!=0){
                    new_vec.maxSize=new_vec.size*realloc_factor;
                    new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxSize);
                    al::memcopyconstruct(new_vec.get_val(),new_vec.array,this->array+start,new_vec.size);
                }
                return new_vec;
            }
            Vector sublist(size_type start)const{
                Vector new_vec;
                new_vec.size=size-start;
                if(new_vec.size!=0){
                    new_vec.maxSize=new_vec.size*realloc_factor;
                    new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxSize);
                    al::memcopyconstruct(new_vec.get_val(),new_vec.array,this->array+start,new_vec.size);
                }
                return new_vec;
            }
            Vector cut(size_type start,size_type end){
                Vector new_vec;
                new_vec.size=end-start;
                if(new_vec.size!=0){
                    new_vec.maxSize=new_vec.size*realloc_factor;
                    new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxSize);
                    al::memmoveconstruct(new_vec.get_val(),new_vec.array,this->array+start,new_vec.size);
                    al::memmove<AlTy>(this->array+start,this->array+end,this->size-end);
                    this->size-=new_vec.size;
                    al::memdestructor(this->get_val(),this->array+this->size,new_vec.size);
                }
                return new_vec;
            }
            Vector cut(size_type start){
                Vector new_vec;
                new_vec.size=size-start;
                if(new_vec.size!=0){
                    new_vec.maxSize=new_vec.size*realloc_factor;
                    new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxSize);
                    al::memmoveconstruct(new_vec.get_val(),new_vec.array,this->array+start,new_vec.size);
                    this->size=start;
                    al::memdestructor(this->get_val(),this->array+this->size,new_vec.size);
                }
                return new_vec;
            }
            void wipe(size_type start,size_type end){
                size_type deletedSize=end-start;
                al::memmove<AlTy>(this->array+start,this->array+end,this->size-end);
                this->size-=deletedSize;
                al::memdestructor(this->get_val(),this->array+this->size,deletedSize);
            }
            void wipe(size_type start){
                al::memdestructor(this->get_val(),this->array+start,size-start);
                this->size=start;
            }
            void reserve(size_type reserve_size){
                if(reserve_size>maxSize){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),reserve_size);
                    al::memmoveconstruct(this->get_val(),new_array,this->array,size);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxSize=reserve_size;
                }
            }
            void resize(size_type new_size){
                if(new_size>maxSize){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_size*realloc_factor);
                    al::memmoveconstruct(this->get_val(),new_array,array,size);
                    for(size_type i=size;i<new_size;i++)
                        AlTy_traits::construct(this->get_val(),new_array+i);
                    destroy_and_free();
                    array=new_array;
                    maxSize=new_size*realloc_factor;
                }
                else{
                    if(size>new_size)
                        al::memdestructor(this->get_val(),array+new_size,size-new_size);
                    else if(size<new_size)
                        for(size_type i=size;i<new_size;i++)
                            AlTy_traits::construct(this->get_val(),array+i);
                }
                size=new_size;
            }
            template<typename D>
            Optional<size_type> find(const D& key,size_type start,size_type end)const{	//retorna -1 si no se encontro el elemento
                for(size_type i=start;i<end;i++){
                    if(array[i]==key)
                        return i;
                }
                return {};
            }
            template<typename D>
            Optional<size_type> find(const D& key,size_type start=0)const{	//retorna -1 si no se encontro el elemento
                for(size_type i=start;i<size;i++){
                    if(array[i]==key)
                        return i;
                }
                return {};
            }
            template<typename D,typename Cmp=less<T>>
            Optional<size_type> binary_search(const D& key,Cmp&& cmp=Cmp{})const{
                return al::binary_search<AlTy,D,Cmp&&>(array,size,key,static_cast<Cmp&&>(cmp));
            }
            template<typename D>
            size_type count(const D& data)const{
                size_type counter=0;
                for(size_type i=0;i<size;i++)
                    if(array[i]==data)
                        counter++;
                return counter;
            }
            template<typename D>
			bool has(const D& data)const{
				for(size_type i=0;i<size;i++){
					if(array[i]==data)
						return true;
				}
				return false;
			}
            template<typename D>
            size_type remove_all(const D& data){
                size_type i=0,j=0;
                while(j<size){
                    if(array[j]!=data){
                        if(i!=j)
                            array[i]=static_cast<value_type&&>(array[j]);
                        i++;
                    }
                    j++;
                }
                al::memdestructor<AlTy>(this->get_val(),array+i,size-i);
                size=i;
                return j-i;
            }
            template<typename D>
            bool remove(const D& data){
                for(size_type i=0;i<size;i++){
                    if(array[i]==data){
                        al::memmove<AlTy>(array+i,array+i+1,(--size)-i);
                        AlTy_traits::destroy(this->get_val(),array+size);
                        return true;
                    }
                }
                return false;
            }
            size_type len()const{
                return size;
            }
            size_type get_maxSize()const{
                return maxSize;
            }
            const_pointer get_array()const{
                return array;
            }
            pointer get_array(){
                return array;
            }
            void sort(){
                al::insertion_sort<Alloc>(array,size);
            }
            void sort_reverse(){
                al::insertion_sort_reverse<Alloc>(array,size);
            }
            template<typename D>
            void sort(D(*func)(const_reference)){
                al::insertion_sort<Alloc>(array,size,func);
            }
            template<typename Cmp>
            void sort(Cmp&& cmp=Cmp{}){
                al::insertion_sort<Alloc>(array,size,static_cast<Cmp&&>(cmp));
            }
			template<typename Lambda>
			bool any(Lambda&& func=Lambda{})const{
                for(const_reference val:*this)
                    if(func(val))return 1;
                return 0;
			}
			template<typename Lambda>
			void forEach(Lambda&& func=Lambda{}){
				for(reference val:*this)
					func(val);
			}
            template<typename,typename,typename> friend class Vector;
			template<typename Lambda>
			auto map(Lambda&& func=Lambda{})const{
                Vector<decltype(func(declval<reference>())),rebind_alloc<Alloc,decltype(func(declval<reference>()))>,realloc_params> new_vec(reserve_tag{},this->maxSize);
                //Vector<decltype(func(*this->array)),rebind_alloc<Alloc,decltype(func(*this->array))>,realloc_params> new_vec(reserve_tag{},this->maxSize);
                new_vec.size=this->size;
                for(size_type i=0;i<this->size;i++)
                    decltype(new_vec)::AlTy_traits::construct(new_vec.get_val(),new_vec.array+i,func(this->array[i]));
				return new_vec;
			}
			template<typename Lambda>
			Vector filter(Lambda&& func=Lambda{})const{
				Vector new_vec(reserve_tag{},this->maxSize);
				for(size_type i=0;i<this->size;i++)
					if(func(this->array[i])) 
                        AlTy_traits::construct(new_vec.get_val(),new_vec.array+new_vec.size++,this->array[i]);
				return new_vec;
			}
            void reverse(){
                al::reverse<Alloc>(array,size);
            }
			bool empty()const{
				return size==0;
			}
            void clear(){
                al::memdestructor(this->get_val(),array,size);
                size=0;
            }
            void free(){
                destroy_free_and_reset();
            }
            ~Vector(){
                destroy_and_free();
            }
            iterator begin(){
                return iterator(array);
            }
            iterator end(){
                return iterator(array+size);
            }
            iterator rbegin(){
                return iterator(array+size-1);
            }
            iterator rend(){
                return iterator(array-1);
            }
            const_iterator begin()const{
                return const_iterator(array);
            }
            const_iterator end()const{
                return const_iterator(array+size);
            }
            const_iterator rbegin()const{
                return const_iterator(array+size-1);
            }
            const_iterator rend()const{
                return const_iterator(array-1);
            }
            template<typename t,typename alloc,typename rp> friend void debug(const Vector<t,alloc,rp>&);
    };
    template<typename Out,typename T,typename Alloc,typename realloc_params>
    Out& operator<<(Out& stream,const Vector<T,Alloc,realloc_params>& list){
        stream<<'[';
        for(size_t i=0,size=list.len();i<size;i++){
            stream<<list[i];
            if(i<size-1)
                stream<<", ";		
        }
        return stream<<']';
    }
#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
    template<typename T,typename Alloc,typename realloc_params>
    void debug(const Vector<T,Alloc,realloc_params>& vec){
        std::cout<<"maxSize: "<<vec.maxSize<<std::endl
                 <<"size: "<<vec.size<<std::endl
                 <<"array: "<<vec.array<<std::endl;
    }
#endif
}