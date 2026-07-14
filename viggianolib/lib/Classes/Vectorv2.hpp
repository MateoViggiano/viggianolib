#pragma once
#include"../arrays.hpp"
#ifndef VECTOR_MMS 
#define VECTOR_MMS 16 //min max_size
#endif
namespace mpv{
    template<typename T,typename Alloc=allocator<T>,typename realloc_params=params<typename allocator_traits<Alloc>::template rebind_traits<T>::size_type,VECTOR_MMS,2>>
    class Vector COUNT_IT {static_assert(realloc_params::p1>0,"min_maxLen cannot be less than 1");
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
            // using iterator=pointer;
            // using const_iterator=const_pointer;

        private:
            CompressedPair<AlTy,size_type> cp=CompressedPair<AlTy,size_type>(0);
            size_type length=0;
            pointer array=nullptr;

#define alloc cp.getV1()
#define maxLen cp.V2

            struct Guard{
                AlTy& al;
                size_type length=0,max_length;
                pointer array=nullptr;
                constexpr Guard(AlTy& al,size_type max_length):al(al),max_length(max_length),array(max_length>0? AlTy_traits::allocate(al,max_length) : nullptr){}
                constexpr Guard(AlTy& al,const Vector& v):al(al),max_length(v.length*realloc_factor),array(max_length>0? AlTy_traits::allocate(al,max_length) : nullptr){}
                constexpr void transfer_to(Vector& v)noexcept{
                    v.array=this->array;
                    v.maxLen=this->max_length;
                    v.length=this->length;
                    this->array=nullptr;
                }
                ~Guard()noexcept{
                    if(array){
                        destroy_n(al,array,length);
                        AlTy_traits::deallocate(al,array,max_length);
                    }
                }
                Guard(const Guard&)=delete;
                Guard& operator=(const Guard&)=delete;
            };
            constexpr Vector(reserve_tag,size_type max_length,const Alloc& al):cp(arg1_tag{},al,max_length),array(max_length>0? AlTy_traits::allocate(alloc,max_length) : nullptr){}
            constexpr void destroy_and_free()noexcept{
                if(array){
                    destroy_n(this->alloc,array,length);
                    AlTy_traits::deallocate(this->alloc,array,maxLen);                    
                }
            }
            constexpr void reset()noexcept{
                this->length=0;
                this->maxLen=0;
                this->array=nullptr;
            }
            constexpr void take_content(Vector& other)noexcept{
                this->length=other.length;
                this->maxLen=other.maxLen;
                this->array=other.array;
                other.reset();
            }
            template<typename It>
            constexpr void allocate_and_assign_counted_range(It first,size_type count){// assumes *this has no storage
                Guard guard(alloc,count*realloc_factor);
                copy_construct_n(alloc,guard.array,first,count);
                guard.length=count;
                guard.transfer_to(*this);
            }
            template<typename It>
            constexpr void assign_counted_range(It first,size_type count){
                if(maxLen<count){
                    free_storage();
                    allocate_and_assign_counted_range(first,count);
                }
                else{
                    if(length>=count){
                        destroy_n(alloc,array+count,length-count);length=count;
                        copy_n(array,first,count);
                    }
                    else{
                        advance_copy_n(array,first,length);
                        copy_construct_n(alloc,array+length,first,count-length);length=count;
                    }
                }
            }
            template<typename It>
            constexpr void allocate_and_move_counted_range(It first,size_type count){// assumes *this has no storage
                Guard guard(alloc,count*realloc_factor);
                move_construct_n(alloc,guard.array,first,count);
                guard.length=count;
                guard.transfer_to(*this);
            }
            template<typename It>
            constexpr void move_counted_range(It first,size_type count){
                if(maxLen<count){
                    free_storage();
                    allocate_and_move_counted_range(first,count);
                }
                else{
                    if(length>=count){
                        destroy_n(alloc,array+count,length-count);length=count;
                        move_n(array,first,count);
                    }
                    else{
                        advance_move_n(array,first,length);
                        move_construct_n(alloc,array+length,first,count-length);length=count;
                    }
                }
            }
            template<typename It>
            constexpr iterator insert_uncounted_range(const_iterator pos,It first,It last){
                size_type index=pos-begin(),prev_length=length;
                while(first!=last) push_back(*first++);
                rotate(array+index,array+prev_length,array+length);
                return array+index;
            }
            template<typename It>
            constexpr iterator insert_counted_range(const_iterator pos,It first,size_type count){// can't insert into itself
                size_type index=pos-begin();
                if(length+count>maxLen){
                    Guard guard(alloc,(length+count)*realloc_factor);
                    copy_construct_n(alloc,guard.array+index,first,count);
                    DestroySequenceGuard tempguard(alloc,guard.array+index,count);
                    move_construct_if_nt_n(alloc,guard.array,array,index);
                    tempguard.count=0;
                    guard.length=index+count;
                    move_construct_if_nt_n(alloc,guard.array+index+count,array+index,length-index); guard.length+=length-index;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    if(index+count>=length){
                        move_construct_n(alloc,array+count+index,array+index,length-index);
                        DestroySequenceGuard guard(alloc,array+count+index,length-index);
                        advance_copy_n(array+index,first,length-index);//equivalente a copy_n(array+index,first,length-index);advance(first,length-index);
                        copy_construct_n(alloc,array+length,first,count-(length-index));
                        guard.count=0;
                        length+=count;
                    }
                    else{
                        size_type old_length=length;
                        move_construct_n(alloc,array+old_length,array+old_length-count,count);length+=count;
                        move_reverse_n(array+index+count,array+index,old_length-index-count);
                        copy_n(array+index,first,count);
                    }
                }
                return array+index;
            }
            template<typename It>
            constexpr iterator insert_counted_range_into_itself(const_iterator pos,It first,size_type count){//always reallocates
                size_type index=pos-begin();
                Guard guard(alloc,(length+count)*realloc_factor);
                copy_construct_n(alloc,guard.array+index,first,count);
                DestroySequenceGuard tempguard(alloc,guard.array+index,count);
                move_construct_if_nt_n(alloc,guard.array,array,index);
                tempguard.count=0;
                guard.length=index+count;
                move_construct_if_nt_n(alloc,guard.array+index+count,array+index,length-index); guard.length+=length-index;
                this->destroy_and_free();
                guard.transfer_to(*this);
                return array+index;
            }
            template<typename It>
            constexpr iterator move_insert_uncounted_range(const_iterator pos,It first,It last){
                size_type index=pos-begin(),prev_length=length;
                while(first!=last) push_back(mpv::move(*first++));
                rotate(array+index,array+prev_length,array+length);
                return array+index;
            }
            template<typename It>
            constexpr iterator move_insert_counted_range(const_iterator pos,It first,size_type count){
                size_type index=pos-begin();
                if(length+count>maxLen){
                    Guard guard(alloc,(length+count)*realloc_factor);
                    move_construct_n(alloc,guard.array+index,first,count);
                    DestroySequenceGuard tempguard(alloc,guard.array+index,count);
                    move_construct_if_nt_n(alloc,guard.array,array,index);
                    tempguard.count=0;
                    guard.length=index+count;
                    move_construct_if_nt_n(alloc,guard.array+index+count,array+index,length-index); guard.length+=length-index;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    if(index+count>=length){
                        move_construct_n(alloc,array+count+index,array+index,length-index);
                        DestroySequenceGuard guard(alloc,array+count+index,length-index);
                        advance_move_n(array+index,first,length-index);//equivalente a copy_n(array+index,first,length-index);advance(first,length-index);
                        move_construct_n(alloc,array+length,first,count-(length-index));
                        guard.count=0;
                        length+=count;
                    }
                    else{
                        size_type old_length=length;
                        move_construct_n(alloc,array+old_length,array+old_length-count,count);length+=count;
                        move_reverse_n(array+index+count,array+index,old_length-index-count);
                        move_n(array+index,first,count);
                    }
                }
                return array+index;
            }
            template<typename It>
            constexpr void append_uncounted_range(It first,It last){
                while(first!=last) push_back(*first++);
            }
            template<typename It>
            constexpr void append_counted_range(It first,size_type count){//can append a range into itself
                if(length+count>maxLen){
                    Guard guard(alloc,(length+count)*realloc_factor);
                    copy_construct_n(alloc,guard.array+length,first,count);
                    DestroySequenceGuard tempguard(alloc,guard.array+length,count);
                    move_construct_if_nt_n(alloc,guard.array,array,length); 
                    tempguard.count=0;
                    guard.length=length+count;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    copy_construct_n(alloc,array+length,first,count); length+=count;
                }
            }
            template<typename It>
            constexpr void move_append_uncounted_range(It first,It last){
                while(first!=last) push_back(mpv::move(*first++));
            }
            template<typename It>
            constexpr void move_append_counted_range(It first,size_type count){
                if(length+count>maxLen){
                    Guard guard(alloc,(length+count)*realloc_factor);
                    move_construct_n(alloc,guard.array+length,first,count);
                    DestroySequenceGuard tempguard(alloc,guard.array+length,count);
                    move_construct_if_nt_n(alloc,guard.array,array,length); 
                    tempguard.count=0;
                    guard.length=length+count;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    move_construct_n(alloc,array+length,first,count); length+=count;
                }
            }
        public:
            constexpr Vector()noexcept(is_nothrow_default_constructible_v<AlTy>) = default;
            constexpr explicit Vector(const Alloc& al)noexcept:cp(arg1_tag{},al){}
            constexpr Vector(const Vector& other):cp(arg1_tag{},AlTy_traits::select_on_container_copy_construction(other.alloc),0){
                this->allocate_and_assign_counted_range(other.array,other.length);
            }
            constexpr Vector(const Vector& other,const Alloc& al):cp(arg1_tag{},al,0){
                this->allocate_and_assign_counted_range(other.array,other.length);
            }
            constexpr Vector(Vector&& other)noexcept:cp(arg1_tag{},static_cast<AlTy&&>(other.alloc),other.maxLen),length(other.length),array(other.array){
                other.reset();
            }
            constexpr Vector(Vector&& other,const Alloc& al)noexcept(ALWAYS_EQ):cp(arg1_tag{},al,0){
                if(this->alloc==other.alloc){
                    this->take_content(other);
                }
                else{
                    allocate_and_move_counted_range(other.array,other.length);
                }
            }
            constexpr explicit Vector(size_type sz){
                Guard guard(alloc,sz*realloc_factor);
                default_construct_n(alloc,guard.array,sz);
                guard.length=sz;
                guard.transfer_to(*this);
            }
            constexpr Vector(size_type sz,const Alloc& al):cp(arg1_tag{},al,0){
                Guard guard(alloc,sz*realloc_factor);
                default_construct_n(alloc,guard.array,sz);
                guard.length=sz;
                guard.transfer_to(*this);
            }
            constexpr Vector(size_type sz,const T& fillwith,const Alloc& al=Alloc{}):cp(arg1_tag{},al,0){
                Guard guard(alloc,sz*realloc_factor);
                fill_construct_n(alloc,guard.array,sz,fillwith);
                guard.length=sz;
                guard.transfer_to(*this);
            }
            template<typename It, enable_if_t<is_iterator_v<It>,int> = 0>
            constexpr Vector(It first,It last,const Alloc& al=Alloc{}):cp(arg1_tag{},al,0){
                if constexpr(is_forward_iterator_v<It>){
                    this->allocate_and_assign_counted_range(first,distance(first,last));
                }
                else{
                    while(first!=last) push_back(*first++);
                }
            }
            constexpr Vector(std::initializer_list<value_type> initlist,const Alloc& al=Alloc()):cp(arg1_tag{},al,0){
                this->allocate_and_assign_counted_range(initlist.begin(),initlist.size());
            }
            constexpr Vector& operator=(const Vector& other){
                if(this==&other) return *this;
                if constexpr(POCCA && !ALWAYS_EQ){
                    if(this->alloc!=other.alloc){
                        this->free_storage();
                    }
                }
                pocca(this->alloc,other.alloc);
                this->assign_counted_range(other.array,other.length);
                return *this;
            }
            constexpr Vector& operator=(Vector&& other)noexcept(ALWAYS_EQ || POCMA){
                if(this==&other) return *this;
                if constexpr(!ALWAYS_EQ && !POCMA){
                    if(this->alloc!=other.alloc){
                        this->move_counted_range(other.array,other.length);
                        return *this;
                    }
                }
                this->destroy_and_free();
                pocma(this->alloc,other.alloc);
                this->take_content(other);
                return *this;
            }
            constexpr Vector operator+(const Vector& other)const&{
                Vector new_vec(reserve_tag{},(this->length+other.length)*realloc_factor,AlTy_traits::select_on_container_copy_construction(alloc));
                copy_construct_n(new_vec.alloc,new_vec.array,this->array,this->length);new_vec.length=this->length;
                copy_construct_n(new_vec.alloc,new_vec.array+this->length,other.array,other.length);new_vec.length+=other.length;
                return new_vec;
            }
            constexpr Vector operator+(Vector&& other)const&{
                Vector new_vec(static_cast<Vector&&>(other));
                new_vec.insert_counted_range(new_vec.begin(),this->array,this->length);
                return new_vec;
            }
            constexpr Vector operator+(const Vector& other)&&{
                Vector new_vec(static_cast<Vector&&>(*this));
                new_vec.append_counted_range(other.array,other.length);
                return new_vec;
            }
            constexpr Vector operator+(Vector&& other)&&{
                if(this->length+other.length>other.maxLen || this->length+other.length<=this->maxLen){
                    Vector new_vec(static_cast<Vector&&>(*this));
                    new_vec.move_append_counted_range(other.begin(),other.length);
                    return new_vec;
                }
                else{
                    Vector new_vec(static_cast<Vector&&>(other));
                    new_vec.move_insert_counted_range(new_vec.begin(),this->array,this->length);
                    return new_vec;
                }
            }
            constexpr Vector& operator+=(const Vector& other){
                this->append_counted_range(other.array,other.length);
                return *this;
            }
            constexpr Vector& operator+=(Vector&& other){
                this->move_append_counted_range(other.array,other.length);
                return *this;
            }
            constexpr Vector operator*(size_type num)const{
                Vector new_vec(reserve_tag{},this->length*num*realloc_factor,AlTy_traits::select_on_container_copy_construction(alloc));
                for(size_type i=0;i<num;i++){
                    copy_construct_n(new_vec.alloc,new_vec.array+new_vec.length,this->array,this->length);
                    new_vec.length+=this->length;
                }
                return new_vec;
            }
            constexpr Vector& operator*=(size_type num){
                if(num==0) clear();
                else{
                    if(length*num>this->maxLen){
                        Guard guard(alloc,length*num*realloc_factor);
                        move_construct_n(this->alloc,guard.array,array,length);
                        guard.length=length;
                        this->destroy_and_free();
                        guard.transfer_to(*this);
                    }
                    for(size_type i=1,old_length=length;i<num;i++,length+=old_length)
                        copy_construct_n(this->alloc,array+length,array,old_length);
                }
                return *this;
            }
            template<typename... Args>
            constexpr void emplace_back(Args&&... args){
                if(length==maxLen){
                    Guard guard(alloc,maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor);
                    CONSTRUCT_VARARGS(alloc,guard.array+length,static_cast<Args&&>(args));
                    DestroyGuard tempguard(alloc,guard.array+length);
                    move_construct_if_nt_n(alloc,guard.array,array,length);
                    tempguard.destroy=false;
                    guard.length=length+1;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    CONSTRUCT_VARARGS(this->alloc,array+length,static_cast<Args&&>(args));
                    length++;
                }
            }
            constexpr void push_back(const T& val){emplace_back(val);}
            constexpr void push_back(value_type&& val){emplace_back(static_cast<value_type&&>(val));}
            template<typename... Args>
            constexpr iterator emplace(const_iterator pos,Args&&... args){
                size_type index=pos-begin();
                if(length==maxLen){
                    Guard guard(alloc,maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor);
                    CONSTRUCT_VARARGS(this->alloc,guard.array+index,static_cast<Args&&>(args));
                    DestroyGuard tempguard(alloc,guard.array+index);
                    move_construct_if_nt_n(this->alloc,guard.array,array,index);
                    tempguard.destroy=false;
                    guard.length=index+1;
                    move_construct_if_nt_n(this->alloc,guard.array+index+1,array+index,length-index);guard.length=this->length+1;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    if(index==length){
                        CONSTRUCT_VARARGS(this->alloc,array+length,static_cast<Args&&>(args));length++;
                    }
                    else{
                        CONSTRUCT(this->alloc,array+length,static_cast<value_type&&>(array[length-1]));
                        move_reverse_n(array+index+1,array+index,(length++)-index-1);
                        array[index]=value_type(static_cast<Args&&>(args)...);
                    }
                }
                return array+index;
            }
            template<typename... Args>
            constexpr iterator emplace_at(size_type index,Args&&... args){
                if(length==maxLen){
                    Guard guard(alloc,maxLen==0 ? min_maxLen : (maxLen+1)*realloc_factor);
                    CONSTRUCT_VARARGS(this->alloc,guard.array+index,static_cast<Args&&>(args));
                    DestroyGuard tempguard(alloc,guard.array+index);
                    move_construct_if_nt_n(this->alloc,guard.array,array,index);
                    tempguard.destroy=false;
                    guard.length=index+1;
                    move_construct_if_nt_n(this->alloc,guard.array+index+1,array+index,length-index);guard.length=this->length+1;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    if(index==length){
                        CONSTRUCT_VARARGS(this->alloc,array+length,static_cast<Args&&>(args));length++;
                    }
                    else{
                        CONSTRUCT(this->alloc,array+length,static_cast<value_type&&>(array[length-1]));
                        move_reverse_n(array+index+1,array+index,(length++)-index-1);
                        array[index]=value_type(static_cast<Args&&>(args)...);
                    }
                }
                return array+index;
            }
            constexpr iterator insert(const_iterator pos,const T& val){return emplace(pos,val);}
            constexpr iterator insert(const_iterator pos,value_type&& val){return emplace(pos,static_cast<value_type&&>(val));}
            constexpr iterator insert_at(size_type index,const T& val){return emplace_at(index,val);}
            constexpr iterator insert_at(size_type index,value_type&& val){return emplace_at(index,static_cast<value_type&&>(val));}
            template<typename It>
            constexpr enable_if_t<is_iterator_v<It>,iterator> insert_at(size_type index,It first,It last){
                if constexpr(is_forward_iterator_v<It>){
                    return insert_counted_range(const_iterator(array+index),first,distance(first,last));
                }
                else{
                    return insert_uncounted_range(const_iterator(array+index),first,last);
                }
            }
            template<typename It>
            constexpr enable_if_t<is_iterator_v<It>,iterator> insert(const_iterator pos,It first,It last){
                if constexpr(is_forward_iterator_v<It>){
                    return insert_counted_range(pos,first,distance(first,last));
                }
                else{
                    return insert_uncounted_range(pos,first,last);
                }
            }
            constexpr iterator insert_at(size_type index,const size_type count,const T& val){
                if(length+count>maxLen){
                    Guard guard(alloc,(length+count)*realloc_factor);
                    fill_construct_n(alloc,guard.array+index,count,val);
                    DestroySequenceGuard tempguard(alloc,guard.array+index,count);
                    move_construct_if_nt_n(alloc,guard.array,array,index); 
                    tempguard.count=0;
                    guard.length=index+count;
                    move_construct_if_nt_n(alloc,guard.array+index+count,array+index,length-index); guard.length+=length-index;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    size_type old_length=length;
                    if(index+count>=length){
                        fill_construct_n(alloc,array+old_length,count-(old_length-index),val);length=count+index;
                        move_construct_n(alloc,array+count+index,array+index,old_length-index);length+=old_length-index;
                        fill_n(array+index,old_length-index,val);
                        
                    }
                    else{
                        move_construct_n(alloc,array+old_length,array+old_length-count,count);length+=count;
                        move_reverse_n(array+index+count,array+index,old_length-index-count);
                        fill_n(array+index,count,val);
                    }
                }
                return array+index;
            }
            constexpr iterator insert(const_iterator pos,const size_type count,const T& val){
                return insert_at(pos-begin(),count,val);
            }
            constexpr void del_at(size_type index)noexcept(is_nothrow_move_assignable_v<value_type>){
                mpv::move(array+index,array+index+1,array+length);
                DESTROY(alloc,array+length-1);
                --length;
            }
            constexpr void del(const_iterator pos)noexcept(is_nothrow_move_assignable_v<value_type>){
                mpv::move(array+(pos-begin()),pos+1,const_iterator(array+length));
                DESTROY(alloc,array+length-1);
                --length;
            }
            constexpr value_type pop_at(size_type index)noexcept(is_nothrow_move_constructible_v<value_type> && is_nothrow_move_assignable_v<value_type>){
                value_type aux=static_cast<value_type&&>(array[index]);
                mpv::move(array+index,array+index+1,array+length);
                DESTROY(alloc,array+length-1);
                --length;
                return aux;
            }
            constexpr value_type pop(const_iterator pos)noexcept(is_nothrow_move_constructible_v<value_type> && is_nothrow_move_assignable_v<value_type>){
                iterator it=array+(pos-begin());
                value_type aux=static_cast<value_type&&>(*it);
                mpv::move(it,pos+1,const_iterator(array+length));
                DESTROY(alloc,array+length-1);
                --length;
                return aux;
            }
            constexpr void del_back()noexcept{
                DESTROY(this->alloc,array+length-1);
                --length;
            }
            constexpr value_type pop_back()noexcept(is_nothrow_move_constructible_v<value_type>){
                value_type aux=static_cast<value_type&&>(array[length-1]);
                DESTROY(this->alloc,array+length-1);
                --length;
                return aux;
            }
            constexpr void join(const_iterator pos,const Vector& other){
                if(this==&other) insert_counted_range_into_itself(pos,array,length);
                else insert_counted_range(pos,other.array,other.length);
            }
            constexpr void join(const_iterator pos,Vector&& other){
                move_insert_counted_range(pos,other.array,other.length);
            }
            constexpr Vector concat(const_iterator pos,const Vector& other)const{
                size_type index=pos-begin();
                Vector new_vec(reserve_tag{},(this->length+other.length)*realloc_factor,AlTy_traits::select_on_container_copy_construction(alloc));
                copy_construct_n(new_vec.alloc,new_vec.array,this->array,index);new_vec.length=index;
                copy_construct_n(new_vec.alloc,new_vec.array+index,other.array,other.length);new_vec.length+=other.length;
                copy_construct_n(new_vec.alloc,new_vec.array+index+other.length,this->array+index,this->length-index);new_vec.length+=this->length-index;
                return new_vec;
            }
            constexpr Vector concat(const_iterator pos,Vector&& other)const{
                size_type index=pos-begin();
                Vector new_vec(static_cast<Vector&&>(other));
                if(this->length+new_vec.length>new_vec.maxLen){
                    Guard guard(new_vec.alloc,(this->length+other.length)*realloc_factor);
                    copy_construct_n(guard.al,guard.array,this->array,index);guard.length=index;
                    move_construct_n(guard.al,guard.array+index,new_vec.array,new_vec.length);guard.length+=new_vec.length;
                    copy_construct_n(guard.al,guard.array+index+new_vec.length,this->array+index,this->length-index);guard.length+=this->length-index;
                    new_vec.destroy_and_free();
                    guard.transfer_to(new_vec);
                }
                else{
                    size_type old_length=new_vec.length;
                    if(index<old_length){
                        move_construct_n(new_vec.alloc,new_vec.array+old_length,new_vec.array+old_length-index,index);new_vec.length+=index;
                        move_reverse_n(new_vec.array+index,new_vec.array,old_length-index);
                        copy_n(new_vec.array,this->array,index);
                    }
                    else{
                        move_construct_n(new_vec.alloc,new_vec.array+index,new_vec.array,new_vec.length);
                        DestroySequenceGuard tempguard(new_vec.alloc,new_vec.array+index,new_vec.length);
                        copy_n(new_vec.array,this->array,new_vec.length);
                        copy_construct_n(new_vec.alloc,new_vec.array+new_vec.length,this->array+new_vec.length,index-new_vec.length);
                        tempguard.count=0;
                        new_vec.length+=index;
                    }
                    copy_construct_n(new_vec.alloc,new_vec.array+index+old_length,this->array+index,this->length-index);
                    new_vec.length+=this->length-index;
                }
                return new_vec;
            }
            constexpr Vector sublist(const_iterator first,const_iterator last)const{
                return Vector(first,last,AlTy_traits::select_on_container_copy_construction(alloc));
            }
            constexpr Vector cut(const_iterator first,const_iterator last){
                Vector new_vec(AlTy_traits::select_on_container_copy_construction(alloc));
                new_vec.allocate_and_move_counted_range(first,last-first);
                move_n(this->array+(first-begin()),this->array+(last-begin()),end()-last);
                destroy_n(alloc,this->array+this->length-new_vec.length,new_vec.length);
                this->length-=new_vec.length;
                return new_vec;
            }
            constexpr Vector& wipe(const_iterator first,const_iterator last){
                size_type deleted_size=last-first;
                move_n(this->array+(first-begin()),this->array+(last-begin()),end()-last);
                destroy_n(alloc,this->array+this->length-deleted_size,deleted_size);
                this->length-=deleted_size;
                return *this;
            }
            constexpr bool remove(const T& val){
                for(size_type i=0;i<length;i++){
                    if(array[i]==val){
                        move_n(array+i,array+i+1,(length-1)-i);
                        DESTROY(alloc,array+length-1);
                        --length;
                        return true;
                    }
                }
                return false;
            }
            constexpr size_type remove_all(const T& val){
                size_type i=0,j=0;
                while(j<length){
                    if(array[j]!=val){
                        if(i!=j)
                            array[i]=static_cast<value_type&&>(array[j]);
                        i++;
                    }
                    j++;
                }
                destroy_n(alloc,array+i,length-i);
                length=i;
                return j-i;
            }
            constexpr void resize(size_type new_len){
                if(new_len>maxLen){
                    Guard guard(alloc,new_len*realloc_factor);
                    default_construct_n(alloc,guard.array+length,new_len-length);
                    DestroySequenceGuard tempguard(alloc,guard.array+length,new_len-length);
                    move_construct_if_nt_n(alloc,guard.array,array,length);
                    tempguard.count=0;
                    guard.length=new_len;
                    destroy_and_free();
                    guard.transfer_to(*this);
                }
                else{
                    if(length>new_len){
                        destroy_n(alloc,array+new_len,length-new_len);
                    }
                    else if(length<new_len){
                        default_construct_n(alloc,array+length,new_len-length);
                    }
                    length=new_len;
                }
            }
            constexpr const_iterator find(const T& val)const{
                return mpv::find(begin(),end(),val);
            }
            constexpr iterator find(const T& val){
                return mpv::find(begin(),end(),val);
            }
            template<typename Cmp=less<T>>
            constexpr const_iterator binary_search(const T& val,Cmp&& cmp=Cmp{})const{
                return mpv::binary_search(begin(),end(),val,static_cast<Cmp&&>(cmp));
            }
            template<typename Cmp=less<T>>
            constexpr iterator binary_search(const T& val,Cmp&& cmp=Cmp{}){
                return mpv::binary_search(begin(),end(),val,static_cast<Cmp&&>(cmp));
            }
            constexpr size_type count(const T& val)const{
                return mpv::count<const_iterator,T,size_type>(begin(),end(),val);
            }
			constexpr bool contains(const T& val)const{
				return mpv::contains<const_iterator,T>(begin(),end(),val);
			}
            constexpr size_type index_of(const T& val)const{
                return mpv::index_of<const_iterator,T,size_type>(begin(),end(),val);
            }
            constexpr bool operator==(const Vector& other)const{
                if(this->length!=other.length) return false;
                else return equal(this->begin(),this->end(),other.begin());
            }
            constexpr bool operator!=(const Vector& other)const{
                if(this->length!=other.length) return true;
                else return !equal(this->begin(),this->end(),other.begin());
            }
            constexpr bool operator<(const Vector& other)const{
                return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool operator>(const Vector& other)const{
                return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator<=(const Vector& other)const{
                return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator>=(const Vector& other)const{
                return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr const_reference operator[](size_type index)const noexcept{
                return array[index];
            }
            constexpr reference operator[](size_type index)noexcept{
                return array[index];
            }
            constexpr const_reference back()const noexcept{
                return array[length-1];
            }
            constexpr reference back()noexcept{
                return array[length-1];
            }
            constexpr size_type size()const noexcept{
                return length;
            }
            constexpr size_type max_size()const noexcept{
                return maxLen;
            }
            constexpr const_pointer get_array()const noexcept{
                return array;
            }
            constexpr pointer get_array()noexcept{
                return array;
            }
            constexpr allocator_type get_allocator()const noexcept{
                return this->alloc;
            }
			template<typename Pred=less<>>
			constexpr void sort(Pred pred=Pred{}){
				mpv::insertion_sort(begin(),end(),pred);
			}
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const{
                for(const_reference x:*this)
                    if(func(x))return true;
                return false;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{})const{
				for(const_reference x:*this)
					func(x);
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{}){
				for(reference x:*this)
					func(x);
			}
            template<typename,typename,typename> friend class Vector;
			template<typename Lambda>
			constexpr auto map(Lambda&& func=Lambda{})const{
                Vector<decltype(func(declval<const_reference>())),rebind_alloc<Alloc,decltype(func(declval<const_reference>()))>,realloc_params> new_vec(reserve_tag{},this->maxLen,AlTy_traits::select_on_container_copy_construction(alloc));
                //Vector<decltype(func(*this->array)),rebind_alloc<Alloc,decltype(func(*this->array))>,realloc_params> new_vec(reserve_tag{},this->maxLen);
                for(size_type i=0;i<this->length;i++){
                    CONSTRUCT(new_vec.alloc,new_vec.array+i,func(this->array[i]));
                    ++new_vec.length;
                    //decltype(new_vec)::AlTy_traits::construct(new_vec.alloc,new_vec.array+i,func(this->array[i]));
                }
                    
				return new_vec;
			}
			template<typename Lambda>
			constexpr Vector filter(Lambda&& func=Lambda{})const{
				Vector new_vec(reserve_tag{},this->length,AlTy_traits::select_on_container_copy_construction(alloc));
				for(size_type i=0;i<this->length;i++)
					if(func(this->array[i])){
                        CONSTRUCT(new_vec.alloc,new_vec.array+new_vec.length,this->array[i]);
                        ++new_vec.length;
                        //AlTy_traits::construct(new_vec.alloc,new_vec.array+new_vec.length++,this->array[i]);
                    }
				return new_vec;
			}
            constexpr Vector& reverse(){
                mpv::reverse(begin(),end());
                return *this;
            }
			constexpr bool empty()const noexcept{
				return length==0;
			}
            constexpr void clear()noexcept{
                destroy_n(this->alloc,array,length);
                length=0;
            }
            constexpr void shrink_to_fit(){
                if(length<maxLen){
                    Guard guard(alloc,length);
                    move_construct_if_nt_n(alloc,guard.array,this->array,length);
                    guard.length=length;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
            }
            constexpr void reserve(size_type reserve_size){
                if(reserve_size>maxLen){
                    Guard guard(alloc,reserve_size);
                    move_construct_if_nt_n(alloc,guard.array,array,length);
                    guard.length=length;
                    this->destroy_and_free();
                    guard.transfer_to(*this);
                }
            }
            constexpr void free_storage()noexcept{
                if(array){
                    destroy_n(this->alloc,array,length);
                    AlTy_traits::deallocate(this->alloc,array,maxLen);                    
                }
                this->length=0;
                this->maxLen=0;
                this->array=nullptr;
            }
            ~Vector()noexcept{
                destroy_and_free();
            }
            constexpr iterator begin()noexcept{
                return pointer(array);
            }
            constexpr iterator end()noexcept{
                return pointer(array+length);
            }
            constexpr const_iterator begin()const noexcept{
                return const_pointer(array);
            }
            constexpr const_iterator end()const noexcept{
                return const_pointer(array+length);
            }
#undef alloc
#undef maxLen
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
        std::cout<<"maxLen: "<<vec.max_size()<<std::endl
                 <<"length: "<<vec.size()<<std::endl
                 <<"array: "<<vec.get_array()<<std::endl;
    }
#endif
}