#pragma once
#include"../arrays.hpp"
#ifndef VECTOR_MMS 
#define VECTOR_MMS 64 //min max_size
#endif
namespace mpv{
    template<typename T,typename Alloc=allocator<T>,typename realloc_params=params<typename allocator_traits<Alloc>::template rebind_traits<T>::size_type,VECTOR_MMS,2>>
    class Vector:EBCO<rebind_alloc<Alloc,T>> COUNT_IT_ {static_assert(realloc_params::p1>0,"min_maxLen cannot be less than 1");
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
            static constexpr typename Val_types::size_type min_maxLen=realloc_params::p1;
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
            size_type length=0;
            size_type maxLen=0;
            pointer array=nullptr;
            inline void destroy_and_free(){
                MEMDESTRUCTOR(this->get_val(),array,length);
                AlTy_traits::deallocate(this->get_val(),array,maxLen);
            }
            inline void reset(){
                this->length=size_type();
                this->maxLen=size_type();
                this->array=pointer();
            }
            inline void destroy_free_and_reset(){
                MEMDESTRUCTOR(this->get_val(),array,length);
                AlTy_traits::deallocate(this->get_val(),array,maxLen);
                this->length=size_type();
                this->maxLen=size_type();
                this->array=pointer();
            }
            inline void copy_elements(const Vector& other){
                this->length=other.length;
                this->maxLen=other.maxLen;                
                this->array=AlTy_traits::allocate(this->get_val(),this->maxLen);
                MEMCOPYCONSTRUCT(this->get_val(),this->array,other.array,other.length);
            }
            inline void copy_elements(Vector&& other){
                this->length=other.length;
                this->maxLen=other.maxLen;                
                this->array=AlTy_traits::allocate(this->get_val(),this->maxLen);
                MEMMOVECONSTRUCT(this->get_val(),this->array,other.array,other.length);
                other.destroy_free_and_reset();
            }
            inline void move_elements(Vector& other){
                this->length=other.length;
                this->maxLen=other.maxLen;
                this->array=other.array;
                other.reset();
            }
            inline void copy_elements_keeping_this_allocation(const Vector& other){//keeps this allocation if possible
                if(this->maxLen<other.length){
                    this->destroy_and_free();
                    this->copy_elements(other);
                }
                else{
                    if(this->length>=other.length){
                        MEMDESTRUCTOR(this->get_val(),this->array+other.length,this->length-other.length);
                        al::memcopy<AlTy>(this->array,other.array,other.length); 
                    }
                    else{
                        al::memcopy<AlTy>(this->array,other.array,this->length);
                        MEMCOPYCONSTRUCT(this->get_val(),this->array+this->length,other.array+this->length,other.length-this->length);
                    }
                    this->length=other.length;                    
                }
            }
            inline void copy_elements_keeping_this_allocation(Vector&& other){//keeps this allocation if possible
                if(this->maxLen<other.length){
                    this->destroy_and_free();
                    this->copy_elements(static_cast<Vector&&>(other));
                }
                else{
                    if(this->length>=other.length){
                        MEMDESTRUCTOR(this->get_val(),this->array+other.length,this->length-other.length);
                        al::memmove<AlTy>(this->array,other.array,other.length); 
                    }
                    else{
                        al::memmove<AlTy>(this->array,other.array,this->length);
                        MEMMOVECONSTRUCT(this->get_val(),this->array+this->length,other.array+this->length,other.length-this->length);
                    }
                    this->length=other.length;
                    other.destroy_free_and_reset();                    
                }
            }
            inline Vector& add(const Vector& v1,const Vector& v2){// Estas funciones no se encargan de calcular length.
                this->maxLen=this->length*realloc_factor;
                this->array=AlTy_traits::allocate(this->get_val(),this->maxLen);
                MEMCOPYCONSTRUCT(this->get_val(),this->array,v1.array,v1.length);
                MEMCOPYCONSTRUCT(this->get_val(),this->array+v1.length,v2.array,v2.length);
                return *this;
            }
            inline Vector& add(const Vector& v1,Vector&& v2){
                this->maxLen=this->length*realloc_factor;
                this->array=AlTy_traits::allocate(this->get_val(),this->maxLen);
                MEMCOPYCONSTRUCT(this->get_val(),this->array,v1.array,v1.length);
                MEMMOVECONSTRUCT(this->get_val(),this->array+v1.length,v2.array,v2.length);
                v2.destroy_free_and_reset();
                return *this;
            }
            inline Vector& add(Vector&& v1,const Vector& v2){
                this->maxLen=this->length*realloc_factor;
                this->array=AlTy_traits::allocate(this->get_val(),this->maxLen);
                MEMMOVECONSTRUCT(this->get_val(),this->array,v1.array,v1.length);
                MEMCOPYCONSTRUCT(this->get_val(),this->array+v1.length,v2.array,v2.length);
                v1.destroy_free_and_reset();
                return *this;
            }
            inline Vector& add(Vector&& v1,Vector&& v2){
                this->maxLen=this->length*realloc_factor;
                this->array=AlTy_traits::allocate(this->get_val(),this->maxLen);
                MEMMOVECONSTRUCT(this->get_val(),this->array,v1.array,v1.length);
                MEMMOVECONSTRUCT(this->get_val(),this->array+v1.length,v2.array,v2.length);
                v1.destroy_free_and_reset();
                v2.destroy_free_and_reset();
                return *this;
            }
        public:
            Vector(){}
            explicit Vector(const Alloc& al):EBCO<AlTy>(al){}
            Vector(reserve_tag,size_type reserved,const Alloc& al=Alloc()):EBCO<AlTy>(al),maxLen(reserved),array(reserved>0 ? AlTy_traits::allocate(this->get_val(),reserved) : nullptr){}
            Vector(const Vector& other):EBCO<AlTy>(AlTy_traits::select_on_container_copy_construction(other.get_val())),length(other.length),maxLen(other.maxLen),array(other.maxLen>0 ? AlTy_traits::allocate(this->get_val(),other.maxLen) : nullptr){
                MEMCOPYCONSTRUCT(this->get_val(),this->array,other.array,other.length);
            }
            Vector(const Vector& other,const Alloc& al):EBCO<AlTy>(al),length(other.length),maxLen(other.maxLen),array(other.maxLen>0 ? AlTy_traits::allocate(this->get_val(),other.maxLen) : nullptr){
                MEMCOPYCONSTRUCT(this->get_val(),this->array,other.array,other.length);
            }
            Vector(Vector&& other):EBCO<AlTy>(static_cast<AlTy&&>(other.get_val())),length(other.length),maxLen(other.maxLen),array(other.array){
                other.reset();
            }
            Vector(Vector&& other,const Alloc& al):EBCO<AlTy>(al),length(other.length),maxLen(other.maxLen){
                if(this->get_val()==other.get_val()){
                    this->array=other.array;
                    other.reset();
                }
                else{
                    this->array=AlTy_traits::allocate(this->get_val(),maxLen);
                    MEMMOVECONSTRUCT(this->get_val(),this->array,other.array,length);
                    other.destroy_free_and_reset();
                }
            }
            Vector(size_type sz):length(sz),maxLen(length*realloc_factor),array(length>0 ? AlTy_traits::allocate(this->get_val(),maxLen) : nullptr){
                MEMDEFAULTCONSTRUCT(this->get_val(),array,length);
            }
            Vector(size_type sz,const Alloc& al):EBCO<AlTy>(al),length(sz),maxLen(length*realloc_factor),array(length>0 ? AlTy_traits::allocate(this->get_val(),maxLen) : nullptr){
                MEMDEFAULTCONSTRUCT(this->get_val(),array,length);
            }
            Vector(size_type sz,const_reference fillwith,const Alloc& al=Alloc()):EBCO<AlTy>(al),length(sz),maxLen(length*realloc_factor),array(length>0 ? AlTy_traits::allocate(this->get_val(),maxLen) : nullptr){
                MEMFILLCONSTRUCT(this->get_val(),array,length,fillwith);
            }
            Vector(std::initializer_list<value_type> initializerList):length(initializerList.size()),maxLen(length*realloc_factor),array(length>0 ? AlTy_traits::allocate(this->get_val(),maxLen) : nullptr){
                size_type i=0;
                for(const_reference v:initializerList){
                    CONSTRUCT(this->get_val(),array+i++,v);
                }
            }
            Vector(std::initializer_list<value_type> initializerList,const Alloc& al):EBCO<AlTy>(al),length(initializerList.size()),maxLen(length*realloc_factor),array(length>0 ? AlTy_traits::allocate(this->get_val(),maxLen) : nullptr){
                size_type i=0;
                for(const_reference v:initializerList){
                    CONSTRUCT(this->get_val(),array+i++,v);
                }
            }
            template<typename Iterable,enable_if_t<!is_same_v<remove_cvref_t<Iterable>,Vector> && is_iterable_v<Iterable> && !has_size_v<Iterable>>* = nullptr>
            Vector(Iterable&& lis){
				for(auto& v:lis){
                    if constexpr(elements_are_movable_v<Iterable&&>)
                        this->append(static_cast<typename Iterable::value_type&&>(v));
                    else
                        this->append(v);
                }
            }
            template<typename Iterable,enable_if_t<!is_same_v<remove_cvref_t<Iterable>,Vector> && is_iterable_v<Iterable> && has_size_v<Iterable>>* = nullptr>
            Vector(Iterable&& lis):length(lis.size()),maxLen(length*realloc_factor),array(length>0 ? AlTy_traits::allocate(this->get_val(),maxLen) : nullptr){
                size_type i=0;
				for(auto& v:lis){
                    if constexpr(elements_are_movable_v<Iterable&&>){
                        CONSTRUCT(this->get_val(),array+i++,static_cast<typename Iterable::value_type&&>(v));
                    }
                    else{
                        CONSTRUCT(this->get_val(),array+i++,v);
                    }
                }
           }
            template<typename Iterable,enable_if_t<!is_same_v<remove_cvref_t<Iterable>,Vector> && is_iterable_v<Iterable> && !has_size_v<Iterable>>* = nullptr>
            Vector(Iterable&& lis,const Alloc& al):EBCO<AlTy>(al){
				for(auto& v:lis){
                    if constexpr(elements_are_movable_v<Iterable&&>)
                        this->append(static_cast<typename Iterable::value_type&&>(v));
                    else
                        this->append(v);
                }
            }
            template<typename Iterable,enable_if_t<!is_same_v<remove_cvref_t<Iterable>,Vector> && is_iterable_v<Iterable> && has_size_v<Iterable>>* = nullptr>
            Vector(Iterable&& lis,const Alloc& al):EBCO<AlTy>(al),length(lis.size()),maxLen(length*realloc_factor),array(length>0 ? AlTy_traits::allocate(this->get_val(),maxLen) : nullptr){
                size_type i=0;
				for(auto& v:lis){
                    if constexpr(elements_are_movable_v<Iterable&&>){
                        CONSTRUCT(this->get_val(),array+i++,static_cast<typename Iterable::value_type&&>(v));
                    }
                    else{
                        CONSTRUCT(this->get_val(),array+i++,v);
                    }
                }
            }
            Vector& operator=(const Vector& other){
                if(this==&other) return *this;
                if constexpr(POCCA && !ALWAYS_EQ){
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
                if constexpr(!ALWAYS_EQ && !POCMA){
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
                new_vec.length=this->length+other.length;
                new_vec.maxLen=new_vec.length*realloc_factor;
                new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxLen);
                MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array,this->array,this->length);
                MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array+this->length,other.array,other.length);
                return new_vec;
            }
            Vector operator+(Vector&& other)const&{
                //if(this==&other) return *this+other;     //not nessesary, shouldn't add an object with an rvalue ref to itself
                Vector new_vec;
                new_vec.length=this->length+other.length;
                if constexpr(!ALWAYS_EQ && !POCMA){
                    if(new_vec.get_val()!=other.get_val())
                        return new_vec.add(*this,static_cast<Vector&&>(other));
                }
                if(new_vec.length>other.maxLen){
                    return new_vec.add(*this,static_cast<Vector&&>(other));
                }
                else{
                    new_vec.maxLen=other.maxLen;
                    new_vec.array=other.array;
                    if constexpr(!ALWAYS_EQ && POCMA) new_vec.get_val()=static_cast<AlTy&&>(other.get_val());
                    if(this->length>other.length){
                        MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+this->length,other.array,other.length);
                        al::memcopy<AlTy>(new_vec.array,this->array,other.length);
                        MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array+other.length,this->array+other.length,this->length-other.length);
                    }
                    else{
                        MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+other.length,other.array+other.length-this->length,this->length);
                        al::reversemove<AlTy>(new_vec.array+this->length,other.array,other.length-this->length);
                        al::memcopy<AlTy>(new_vec.array,this->array,this->length);
                    }
                    other.reset();
                    return new_vec;                    
                }
            }
            Vector operator+(const Vector& other)&&{
                Vector new_vec;
                new_vec.length=this->length+other.length;
                if constexpr(!ALWAYS_EQ && !POCMA){
                    if(new_vec.get_val()!=this->get_val())
                        return new_vec.add(static_cast<Vector&&>(*this),other);
                }
                if(new_vec.length>this->maxLen){
                    return new_vec.add(static_cast<Vector&&>(*this),other);
                }
                else{
                    new_vec.maxLen=this->maxLen;
                    new_vec.array=this->array;         
                    if constexpr(POCMA) new_vec.get_val()=static_cast<AlTy&&>(this->get_val());           
                    MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array+this->length,other.array,other.length);
                    this->reset();
                    return new_vec;                    
                }
            }
            Vector operator+(Vector&& other)&&{
                Vector new_vec;
                new_vec.length=this->length+other.length;
                if constexpr(ALWAYS_EQ || POCMA){
                    if(new_vec.length<=this->maxLen){
                        new_vec.maxLen=this->maxLen;
                        new_vec.array=this->array;
                        if constexpr(!ALWAYS_EQ && POCMA) new_vec.get_val()=static_cast<AlTy&&>(this->get_val());
                        MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+this->length,other.array,other.length);
                        other.destroy_free_and_reset();
                        this->reset();
                        return new_vec;
                    }
                    else if(new_vec.length<=other.maxLen){
                        new_vec.maxLen=other.maxLen;
                        new_vec.array=other.array;
                        if constexpr(!ALWAYS_EQ && POCMA) new_vec.get_val()=static_cast<AlTy&&>(other.get_val());
                        if(this->length>other.length){
                            MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+this->length,other.array,other.length);
                            al::memmove<AlTy>(new_vec.array,this->array,other.length);
                            MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+other.length,this->array+other.length,this->length-other.length);
                        }
                        else{
                            MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+other.length,other.array+other.length-this->length,this->length);
                            al::reversemove<AlTy>(new_vec.array+this->length,other.array,other.length-this->length);
                            al::memmove<AlTy>(new_vec.array,this->array,this->length);
                        }
                        this->destroy_free_and_reset();
                        other.reset();
                        return new_vec;
                    }
                    else
                        return new_vec.add(static_cast<Vector&&>(*this),static_cast<Vector&&>(other));
                }
                else{
                    if(new_vec.get_val()==this->get_val() && new_vec.length<=this->maxLen){
                        new_vec.maxLen=this->maxLen;
                        new_vec.array=this->array;
                        MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+this->length,other.array,other.length);
                        other.destroy_free_and_reset();
                        this->reset();
                        return new_vec;
                    }
                    else if(new_vec.get_val()==other.get_val() && new_vec.length<=other.maxLen){
                        new_vec.maxLen=other.maxLen;
                        new_vec.array=other.array;
                        if(this->length>other.length){
                            MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+this->length,other.array,other.length);
                            al::memmove<AlTy>(new_vec.array,this->array,other.length);
                            MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+other.length,this->array+other.length,this->length-other.length);
                        }
                        else{
                            MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+other.length,other.array+other.length-this->length,this->length);
                            al::reversemove<AlTy>(new_vec.array+this->length,other.array,other.length-this->length);
                            al::memmove<AlTy>(new_vec.array,this->array,this->length);
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
                if(this->maxLen<this->length+other.length){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),(this->length+other.length)*realloc_factor);
                    MEMMOVECONSTRUCT(this->get_val(),new_array,this->array,this->length);
                    MEMMOVECONSTRUCT(this->get_val(),new_array+this->length,other.array,other.length);
                    this->destroy_and_free();//Para el correcto funcionamiento de esta funcion no se deben cambiar los valores de array,length ni maxLen antes de ser llamada.
                    this->maxLen=(this->length+other.length)*realloc_factor;
                    this->array=new_array;
                }
                else{
                    MEMMOVECONSTRUCT(this->get_val(),this->array+this->length,other.array,other.length);
                }
                this->length+=other.length;
                other.destroy_free_and_reset();
                return *this;
            }
            Vector& operator+=(const Vector& other){
                if(this->maxLen<this->length+other.length){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),(this->length+other.length)*realloc_factor);
                    if(this->array==other.array){
                        MEMCOPYCONSTRUCT(this->get_val(),new_array,this->array,this->length);
                        MEMMOVECONSTRUCT(this->get_val(),new_array+this->length,other.array,other.length);// Muevo other porque es el mismo objeto que this, osea que de todas formas se va a borrar.
                    }
                    else{
                        MEMMOVECONSTRUCT(this->get_val(),new_array,this->array,this->length);
                        MEMCOPYCONSTRUCT(this->get_val(),new_array+this->length,other.array,other.length);
                    }
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxLen=(this->length+other.length)*realloc_factor;
                }
                else{
                    MEMCOPYCONSTRUCT(this->get_val(),this->array+this->length,other.array,other.length);
                }
                this->length+=other.length;
                return *this;
            }
            Vector operator*(size_type num)const{
                Vector new_vec;
                if(num==0)
                    return new_vec;
                new_vec.maxLen=this->length*num*realloc_factor;
                new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxLen);
                for(size_type i=0;i<num;i++){
                    MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array+new_vec.length,this->array,this->length);
                    new_vec.length+=this->length;
                }
                return new_vec;
            }
            Vector& operator*=(size_type num){
                if(num==0){
                    MEMDESTRUCTOR(this->get_val(),array,length);
                    length=0;
                }
                else{
                    if(length*num>this->maxLen){
                        pointer new_array=AlTy_traits::allocate(this->get_val(),length*num*realloc_factor);
                        MEMMOVECONSTRUCT(this->get_val(),new_array,array,length);
                        this->destroy_and_free();
                        this->array=new_array;
                        this->maxLen=length*num*realloc_factor;
                    }
                    for(size_type i=1,prevSize=length;i<num;i++,length+=prevSize){
                        MEMCOPYCONSTRUCT(this->get_val(),array+length,array,prevSize);
                    }
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
                return array[length-1];
            }
            reference back(){
                return array[length-1];
            }
            bool operator==(const Vector& other)const{
                if(this->length!=other.length)
                    return false;
                for(size_type i=0;i<length;i++){
                    if(this->array[i]!=other.array[i])
                        return false;
                }
                return true;
            }
            bool operator!=(const Vector& other)const{
                if(this->length!=other.length)
                    return true;
                for(size_type i=0;i<length;i++){
                    if(this->array[i]!=other.array[i])
                        return true;
                }
                return false;
            }
            template<typename... Args>
            void emplace_back(Args&&... args){
                if(length==maxLen){//si no tiene espacio para ningun elemento mas
                    size_type new_maxLen = maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(), new_maxLen);
                    MEMMOVECONSTRUCT(this->get_val(),new_array,array,length);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxLen=new_maxLen;
                }
                CONSTRUCT_VARARGS(this->get_val(),array+length++,static_cast<Args&&>(args));
            }
            void append(const_reference data){
                if(length==maxLen){//si no tiene espacio para ningun elemento mas
                    size_type new_maxLen = maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_maxLen);
                    MEMMOVECONSTRUCT(this->get_val(),new_array,array,length);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxLen=new_maxLen;
                }
                CONSTRUCT(this->get_val(),array+length++,data);
            }
            void append(value_type&& data){
                if(length==maxLen){//si no tiene espacio para ningun elemento mas
                    size_type new_maxLen = maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(), new_maxLen);
                    MEMMOVECONSTRUCT(this->get_val(),new_array,array,length);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxLen=new_maxLen;
                }
                CONSTRUCT(this->get_val(),array+length++,static_cast<value_type&&>(data));
            }
            Vector& operator+=(std::initializer_list<value_type> initializerList){
               {size_type new_len=length+initializerList.size();
                if(new_len>maxLen){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_len*realloc_factor);
                    MEMMOVECONSTRUCT(this->get_val(),new_array,array,length);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxLen=new_len*realloc_factor;
                }}
                for(const_reference v:initializerList){
                    CONSTRUCT(this->get_val(),array+length++,v);
                }
                return *this;
            }
            template<typename... Args>
            void emplace(size_type index,Args&&... args){
                if(length==maxLen){
                    size_type new_maxLen = maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_maxLen);
                    MEMMOVECONSTRUCT(this->get_val(),new_array,array,index);
                    MEMMOVECONSTRUCT(this->get_val(),new_array+index+1,array+index,length-index);
                    CONSTRUCT_VARARGS(this->get_val(),new_array+index,static_cast<Args&&>(args));
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxLen=new_maxLen;
                    this->length++;
                }
                else{
                    if(index==length){
                        CONSTRUCT_VARARGS(this->get_val(),array+length++,static_cast<Args&&>(args));
                    }
                    else{
                        CONSTRUCT(this->get_val(),array+length,static_cast<value_type&&>(array[length-1]));
                        al::reversemove<AlTy>(array+index+1,array+index,(length++)-index-1);
                        array[index]=value_type(static_cast<Args&&>(args)...);
                    }
                }
            }
            void insert(size_type index,const_reference data){
                if(length==maxLen){
                    size_type new_maxLen = maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_maxLen);
                    MEMMOVECONSTRUCT(this->get_val(),new_array,array,index);
                    MEMMOVECONSTRUCT(this->get_val(),new_array+index+1,array+index,length-index);
                    CONSTRUCT(this->get_val(),new_array+index,data);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxLen=new_maxLen;
                    this->length++;
                }
                else{
                    if(index==length){
                        CONSTRUCT(this->get_val(),array+length++,data);
                    }
                    else{
                        CONSTRUCT(this->get_val(),array+length,static_cast<value_type&&>(array[length-1]));
                        al::reversemove<AlTy>(array+index+1,array+index,(length++)-index-1);
                        array[index]=data;
                    }
                }
            }
            void insert(size_type index,value_type&& data){
                if(length==maxLen){
                    size_type new_maxLen = maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor;
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_maxLen);
                    MEMMOVECONSTRUCT(this->get_val(),new_array,array,index);
                    MEMMOVECONSTRUCT(this->get_val(),new_array+index+1,array+index,length-index);
                    CONSTRUCT(this->get_val(),new_array+index,static_cast<value_type&&>(data));
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxLen=new_maxLen;
                    this->length++;
                }
                else{
                    if(index==length){
                        CONSTRUCT(this->get_val(),array+length++,data);
                    }
                    else{
                        CONSTRUCT(this->get_val(),array+length,static_cast<value_type&&>(array[length-1]));
                        al::reversemove<AlTy>(array+index+1,array+index,(length++)-index-1);
                        array[index]=static_cast<value_type&&>(data);
                    }
                }
            }
            void del(size_type index){
                al::memmove<AlTy>(array+index,array+index+1,(--length)-index);
                DESTROY(this->get_val(),array+length);
            }
            value_type pop(size_type index){
                value_type aux=static_cast<value_type&&>(array[index]);
                al::memmove<AlTy>(array+index,array+index+1,(--length)-index);
                DESTROY(this->get_val(),array+length);
                return aux;
            }
            void del_back(){
                DESTROY(this->get_val(),array+length-1);
                length--;
            }
            value_type pop_back(){
                value_type aux=static_cast<value_type&&>(array[--length]);
                DESTROY(this->get_val(),array+length);
                return aux;
            }
            Vector concat(size_type index,const Vector& other)const{
                Vector new_vec(reserve_tag{},(this->length+other.length)*realloc_factor);
                new_vec.length=this->length+other.length;
                MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array,this->array,index);
                MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array+index,other.array,other.length);
                MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array+index+other.length,this->array+index,this->length-index);
                return new_vec;
            }
#define CONCAT_MOVE  new_vec.maxLen=new_vec.length*realloc_factor;\
                new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxLen);\
                MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array,this->array,index);\
                MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+index,other.array,other.length);\
                MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array+index+other.length,this->array+index,this->length-index);\
                other.destroy_free_and_reset();\
                return new_vec;

            Vector concat(size_type index,Vector&& other)const{
                Vector new_vec;
                new_vec.length=this->length+other.length;
                if constexpr(!ALWAYS_EQ && !POCMA){
                    if(new_vec.get_val()!=other.get_val()){
                        CONCAT_MOVE
                    }
                }
                if(new_vec.length>other.maxLen){
                    CONCAT_MOVE
                }
                else{
                    new_vec.array=other.array;
                    new_vec.maxLen=other.maxLen;
                    if constexpr(POCMA) new_vec.get_val()=static_cast<AlTy&&>(other.get_val());
                    if(index<other.length){
                        MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+other.length,other.array+other.length-index,index);
                        al::reversemove<AlTy>(new_vec.array+index,other.array,other.length-index);
                        al::memcopy<AlTy>(new_vec.array,this->array,index);                        
                    }
                    else{
                        MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array+index,other.array,other.length);
                        al::memcopy<AlTy>(new_vec.array,this->array,other.length);
                        MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array+other.length,this->array+other.length,index-other.length);
                    }
                    MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array+index+other.length,this->array+index,new_vec.length-(index+other.length));
                    other.reset();
                    return new_vec;
                }
            }
            void join(size_type index,const Vector& other){
                if(other.length==0) return;
                size_type new_len=this->length+other.length;
                if(this->length==0 && index==0){
                    *this=other;
                    return;
                }
                if(new_len>maxLen || this==&other){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_len*realloc_factor);
                    MEMCOPYCONSTRUCT(this->get_val(),new_array+index,other.array,other.length);// Esta tiene que estar primera porque this y other pueden ser el mismo objeto, asi que primero hay que copiarlo y despues moverlo
                    MEMMOVECONSTRUCT(this->get_val(),new_array,this->array,index);
                    MEMMOVECONSTRUCT(this->get_val(),new_array+index+other.length,this->array+index,this->length-index);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxLen=new_len*realloc_factor;
                }
                else{
                    if(index+other.length>=this->length){
                        MEMMOVECONSTRUCT(this->get_val(),this->array+new_len-(this->length-index),this->array+index,this->length-index);
                        al::memcopy<AlTy>(this->array+index,other.array,this->length-index);
                        MEMCOPYCONSTRUCT(this->get_val(),this->array+this->length,other.array+this->length-index,other.length-(this->length-index));                        
                    }
                    else{
                        MEMMOVECONSTRUCT(this->get_val(),this->array+this->length,this->array+this->length-other.length,other.length);
                        al::reversemove<AlTy>(this->array+index+other.length,this->array+index,this->length-index-other.length);
                        al::memcopy<AlTy>(this->array+index,other.array,other.length);
                    }
                }
                this->length=new_len;
            }
            void join(size_type index,Vector&& other){
                if(other.length==0) return;
                size_type new_len=this->length+other.length;
                if(this->length==0 && index==0){
                    *this=static_cast<Vector&&>(other);
                    return;
                }
                if(new_len>this->maxLen){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_len*realloc_factor);
                    MEMMOVECONSTRUCT(this->get_val(),new_array,this->array,index);
                    MEMMOVECONSTRUCT(this->get_val(),new_array+index,other.array,other.length);
                    MEMMOVECONSTRUCT(this->get_val(),new_array+index+other.length,this->array+index,this->length-index);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxLen=new_len*realloc_factor;
                }
                else{
                    if(index+other.length>=this->length){
                        MEMMOVECONSTRUCT(this->get_val(),this->array+new_len-(this->length-index),this->array+index,this->length-index);
                        al::memmove<AlTy>(this->array+index,other.array,this->length-index);
                        MEMMOVECONSTRUCT(this->get_val(),this->array+this->length,other.array+this->length-index,other.length-(this->length-index));
                    }
                    else{
                        MEMMOVECONSTRUCT(this->get_val(),this->array+this->length,this->array+this->length-other.length,other.length);
                        al::reversemove<AlTy>(this->array+index+other.length,this->array+index,this->length-index-other.length);
                        al::memmove<AlTy>(this->array+index,other.array,other.length);
                    }
                }
                other.destroy_free_and_reset();
                this->length=new_len;
            }
            Vector sublist(size_type start,size_type end)const{
                Vector new_vec;
                new_vec.length=end-start;
                if(new_vec.length!=0){
                    new_vec.maxLen=new_vec.length*realloc_factor;
                    new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxLen);
                    MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array,this->array+start,new_vec.length);
                }
                return new_vec;
            }
            Vector sublist(size_type start)const{
                Vector new_vec;
                new_vec.length=length-start;
                if(new_vec.length!=0){
                    new_vec.maxLen=new_vec.length*realloc_factor;
                    new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxLen);
                    MEMCOPYCONSTRUCT(new_vec.get_val(),new_vec.array,this->array+start,new_vec.length);
                }
                return new_vec;
            }
            Vector cut(size_type start,size_type end){
                Vector new_vec;
                new_vec.length=end-start;
                if(new_vec.length!=0){
                    new_vec.maxLen=new_vec.length*realloc_factor;
                    new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxLen);
                    MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array,this->array+start,new_vec.length);
                    al::memmove<AlTy>(this->array+start,this->array+end,this->length-end);
                    this->length-=new_vec.length;
                    MEMDESTRUCTOR(this->get_val(),this->array+this->length,new_vec.length);
                }
                return new_vec;
            }
            Vector cut(size_type start){
                Vector new_vec;
                new_vec.length=length-start;
                if(new_vec.length!=0){
                    new_vec.maxLen=new_vec.length*realloc_factor;
                    new_vec.array=AlTy_traits::allocate(new_vec.get_val(),new_vec.maxLen);
                    MEMMOVECONSTRUCT(new_vec.get_val(),new_vec.array,this->array+start,new_vec.length);
                    this->length=start;
                    MEMDESTRUCTOR(this->get_val(),this->array+this->length,new_vec.length);
                }
                return new_vec;
            }
            void wipe(size_type start,size_type end){
                size_type deletedSize=end-start;
                al::memmove<AlTy>(this->array+start,this->array+end,this->length-end);
                this->length-=deletedSize;
                MEMDESTRUCTOR(this->get_val(),this->array+this->length,deletedSize);
            }
            void wipe(size_type start){
                MEMDESTRUCTOR(this->get_val(),this->array+start,length-start);
                this->length=start;
            }
            void reserve(size_type reserve_size){
                if(reserve_size>maxLen){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),reserve_size);
                    MEMMOVECONSTRUCT(this->get_val(),new_array,this->array,length);
                    this->destroy_and_free();
                    this->array=new_array;
                    this->maxLen=reserve_size;
                }
            }
            void resize(size_type new_len){
                if(new_len>maxLen){
                    pointer new_array=AlTy_traits::allocate(this->get_val(),new_len*realloc_factor);
                    MEMMOVECONSTRUCT(this->get_val(),new_array,array,length);
                    MEMDEFAULTCONSTRUCT(this->get_val(),new_array+length,new_len-length);
                    destroy_and_free();
                    array=new_array;
                    maxLen=new_len*realloc_factor;
                }
                else{
                    if(length>new_len){
                        MEMDESTRUCTOR(this->get_val(),array+new_len,length-new_len);
                    }
                    else if(length<new_len){
                        MEMDEFAULTCONSTRUCT(this->get_val(),array+length,new_len-length);
                    }
                }
                length=new_len;
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
                for(size_type i=start;i<length;i++){
                    if(array[i]==key)
                        return i;
                }
                return {};
            }
            template<typename D,typename Cmp=less<T>>
            Optional<size_type> binary_search(const D& key,Cmp&& cmp=Cmp{})const{
                return al::binary_search<AlTy,D,Cmp&&>(array,length,key,static_cast<Cmp&&>(cmp));
            }
            template<typename D>
            size_type count(const D& data)const{
                size_type counter=0;
                for(size_type i=0;i<length;i++)
                    if(array[i]==data)
                        counter++;
                return counter;
            }
            template<typename D>
			bool has(const D& data)const{
				for(size_type i=0;i<length;i++){
					if(array[i]==data)
						return true;
				}
				return false;
			}
            template<typename D>
            size_type remove_all(const D& data){
                size_type i=0,j=0;
                while(j<length){
                    if(array[j]!=data){
                        if(i!=j)
                            array[i]=static_cast<value_type&&>(array[j]);
                        i++;
                    }
                    j++;
                }
                al::memdestructor(this->get_val(),array+i,length-i);
                length=i;
                return j-i;
            }
            template<typename D>
            bool remove(const D& data){
                for(size_type i=0;i<length;i++){
                    if(array[i]==data){
                        al::memmove<AlTy>(array+i,array+i+1,(--length)-i);
                        DESTROY(this->get_val(),array+length);
                        return true;
                    }
                }
                return false;
            }
            size_type size()const{
                return length;
            }
            size_type get_maxLen()const{
                return maxLen;
            }
            const_pointer get_array()const{
                return array;
            }
            pointer get_array(){
                return array;
            }
            void sort(){
                al::insertion_sort<Alloc>(array,length);
            }
            void sort_reverse(){
                al::insertion_sort_reverse<Alloc>(array,length);
            }
            template<typename D>
            void sort(D(*func)(const_reference)){
                al::insertion_sort<Alloc>(array,length,func);
            }
            template<typename Cmp>
            void sort(Cmp&& cmp=Cmp{}){
                al::insertion_sort<Alloc>(array,length,static_cast<Cmp&&>(cmp));
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
                Vector<decltype(func(declval<reference>())),rebind_alloc<Alloc,decltype(func(declval<reference>()))>,realloc_params> new_vec(reserve_tag{},this->maxLen);
                //Vector<decltype(func(*this->array)),rebind_alloc<Alloc,decltype(func(*this->array))>,realloc_params> new_vec(reserve_tag{},this->maxLen);
                new_vec.length=this->length;
                for(size_type i=0;i<this->length;i++){
                    CONSTRUCT(new_vec.get_val(),new_vec.array+i,func(this->array[i]));
                    //decltype(new_vec)::AlTy_traits::construct(new_vec.get_val(),new_vec.array+i,func(this->array[i]));
                }
                    
				return new_vec;
			}
			template<typename Lambda>
			Vector filter(Lambda&& func=Lambda{})const{
				Vector new_vec(reserve_tag{},this->maxLen);
				for(size_type i=0;i<this->length;i++)
					if(func(this->array[i])){
                        CONSTRUCT(new_vec.get_val(),new_vec.array+new_vec.length++,this->array[i]);
                        //AlTy_traits::construct(new_vec.get_val(),new_vec.array+new_vec.length++,this->array[i]);
                    }
				return new_vec;
			}
            void reverse(){
                al::reverse<Alloc>(array,length);
            }
			bool empty()const{
				return length==0;
			}
            void clear(){
                MEMDESTRUCTOR(this->get_val(),array,length);
                length=0;
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
                return iterator(array+length);
            }
            iterator rbegin(){
                return iterator(array+length-1);
            }
            iterator rend(){
                return iterator(array-1);
            }
            const_iterator begin()const{
                return const_iterator(array);
            }
            const_iterator end()const{
                return const_iterator(array+length);
            }
            const_iterator rbegin()const{
                return const_iterator(array+length-1);
            }
            const_iterator rend()const{
                return const_iterator(array-1);
            }
            template<typename t,typename alloc,typename rp> friend void debug(const Vector<t,alloc,rp>&);
    };
    template<typename Out,typename T,typename Alloc,typename realloc_params>
    Out& operator<<(Out& stream,const Vector<T,Alloc,realloc_params>& list){
        stream<<'[';
        for(typename Vector<T,Alloc,realloc_params>::size_type i=0,length=list.size();i<length;i++){
            stream<<list[i];
            if(i<length-1)
                stream<<", ";
        }
        stream<<']';
        return stream;
    }
#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
    template<typename T,typename Alloc,typename realloc_params>
    void debug(const Vector<T,Alloc,realloc_params>& vec){
        std::cout<<"maxLen: "<<vec.maxLen<<std::endl
                 <<"length: "<<vec.length<<std::endl
                 <<"array: "<<vec.array<<std::endl;
    }
#endif
}