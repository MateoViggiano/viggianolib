#pragma once
#include"../arrays.hpp"
namespace mpv{
	template<typename T=char,typename Alloc=allocator<T>,typename params=params<typename allocator_traits<Alloc>::template rebind_traits<T>::size_type,32,3>>
	class Str:EBCO<rebind_alloc<Alloc,T>> COUNT_IT_{
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
            static constexpr typename Val_types::size_type ss_cap=params::p1;
            static constexpr typename Val_types::size_type M=params::p2;
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
		static_assert(ss_cap>=sizeof(pointer) && M>1,"ss_cap must be >=sizeof(pointer) and M must be >1");
		union sbo{
			value_type buffer[ss_cap];
			pointer heap;
		};
		private:
		/*length no puede ser >= a ss_cap sin tener que asignar memoria, una vez que se esta usando memoria dinamica
		length debe se siempre <= a maxLen (puede llegar a ser igual a maxLen porque maxLen no cuenta el \0
											como un lugar, en cambio ss_cap si) */
			size_type length;// Es el tamaanio del string, sin tener en cuenta el '\0'
			size_type maxLen;/* Es el valor maximo al que puede llegar length antes de tener que reasignar memoria. 
		Cuando se necesita reasignar, el espacio que se asigna es de maxLen+1(para el \0) ya que maxLen y length(que no tiene en cuenta el \0)
		deben(por conveniencia) poder coincidir en un mismo numero sin tener que reasignar.
		Si maxLen es 0, significa que se esta usando el buffer para almacenar el string. Si maxLen>0 el buffer se usa para almacenar un puntero al string en memoria dinamica.*/
			sbo text;
			inline void alloc_size(){
				if(length>=ss_cap){
					maxLen=length*M;
					text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
					text.heap[length]=T();
				}
				else text.buffer[length]=T();
			}
		public:
			pointer get_stack(){
				return this->text.buffer;
			}
			pointer get_heap(){
				return this->text.heap;
			}
			const_pointer c_str()const{	//devuelve un puntero a la memoria que se esta usando actualmente en la union. NO se puede llamar si se modifico maxLen.
				if(maxLen==0)
					return this->text.buffer;	
				else
					return this->text.heap;
			}
			pointer c_str(){	//devuelve un puntero a la memoria que se esta usando actualmente en la union. NO se puede llamar si se modifico maxLen.
				if(maxLen==0)
					return this->text.buffer;	
				else
					return this->text.heap;
			}
			Str():length(0),maxLen(0),text{}{}
			Str(const Alloc& al):EBCO<AlTy>(al){}
			explicit Str(size_type sz):length(sz),maxLen(0){
				if(sz>=ss_cap){
					maxLen=length*M;
					text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
					text.heap[length]=T();
				}
				else text.buffer[length]=T();
			}
			explicit Str(size_type sz,const Alloc& al):EBCO<AlTy>(al),length(sz),maxLen(0){
				if(sz>=ss_cap){
					maxLen=length*M;
					text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
					text.heap[length]=T();
				}
				else text.buffer[length]=T();
			}
			Str(const Str& other):EBCO<AlTy>(AlTy_traits::select_on_container_copy_construction(other.get_val())),length(other.length),maxLen(0){
				if(other.length<ss_cap){
					al::memcopy<AlTy>(this->text.buffer,other.c_str(),other.length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
					al::memcopy<AlTy>(this->text.heap,other.c_str(),other.length+1);
				}
			}
			Str(const Str& other,const Alloc& al):EBCO<AlTy>(al),length(other.length),maxLen(0){
				if(other.length<ss_cap){
					al::memcopy<AlTy>(this->text.buffer,other.c_str(),other.length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
					al::memcopy<AlTy>(this->text.heap,other.c_str(),other.length+1);
				}
			}
			Str(const_pointer str):length(al::strsize<AlTy>(str)),maxLen(0){
				if(this->length<ss_cap){
					al::memcopy<AlTy>(this->text.buffer,str,length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
					al::memcopy<AlTy>(this->text.heap,str,length+1);
				}
			}
			Str(const_pointer str,const Alloc& al):EBCO<AlTy>(al),length(al::strsize<AlTy>(str)),maxLen(0){
				if(this->length<ss_cap){
					al::memcopy<AlTy>(this->text.buffer,str,length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
					al::memcopy<AlTy>(this->text.heap,str,length+1);
				}
			}
			Str(const_pointer str,size_type n):length(n),maxLen(0){
				if(this->length<ss_cap){
					al::memcopy<AlTy>(this->text.buffer,str,n);
					this->text.buffer[length]=T();
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
					al::memcopy<AlTy>(this->text.heap,str,n);
					this->text.heap[length]=T();
				}
			}
			Str(const_pointer str,size_type n,const Alloc& al):EBCO<AlTy>(al),length(n),maxLen(0){
				if(this->length<ss_cap){
					al::memcopy<AlTy>(this->text.buffer,str,n);
					this->text.buffer[length]=T();
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
					al::memcopy<AlTy>(this->text.heap,str,n);
					this->text.heap[length]=T();
				}
			}
			explicit Str(T chr):length(1),maxLen(0){
				this->text.buffer[0]=chr;
				this->text.buffer[1]=T();
			}
			Str(T chr,size_type n):length(n),maxLen(0){
				if(this->length<ss_cap){
					al::memfill<AlTy>(this->text.buffer,chr,n);
					this->text.buffer[length]=T();
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
					al::memfill<AlTy>(this->text.heap,chr,n);
					this->text.heap[length]=T();
				}
			}
			Str(Str&& other):EBCO<AlTy>(static_cast<AlTy&&>(other.get_val())),length(other.length),maxLen(other.maxLen){
				if(other.maxLen==0)
					al::memcopy<AlTy>(this->text.buffer,other.text.buffer,other.length+1);
				else
					this->text.heap=other.text.heap;
				other.text.heap=nullptr;
				other.length=0;
				other.maxLen=0;
			}
			Str(Str&& other,const Alloc& al):EBCO<AlTy>(al),length(other.length),maxLen(other.maxLen){
				if(other.maxLen==0)
					al::memcopy<AlTy>(this->text.buffer,other.text.buffer,other.length+1);
				else{
					if(this->get_val()==other.get_val())
						this->text.heap=other.text.heap;
					else{
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
						al::memcopy<AlTy>(this->text.heap,other.text.heap,other.length+1);
						AlTy_traits::deallocate(this->get_val(),other.text.heap,other.maxLen+1);
					}
				}
				other.text.heap=nullptr;
				other.length=0;
				other.maxLen=0;
			}
#define COPY_ALLOC	if constexpr(POCCA && !ALWAYS_EQ){\
						if(this->get_val()!=other.get_val())\
							this->get_val()=other.get_val();}
			Str& operator=(const Str& other){
				if(this==&other) return *this;
				if(this->maxLen==0){
					if(other.length<ss_cap){
						COPY_ALLOC
						al::memcopy<AlTy>(this->text.buffer,other.c_str(),other.length+1);// +1 para copiar el \0 tambien
					}
					else{
						COPY_ALLOC
						this->maxLen=other.length*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
						al::memcopy<AlTy>(this->text.heap,other.c_str(),other.length+1);
					}
				}
				else{
					if(other.length<=this->maxLen){
						if constexpr(POCCA && !ALWAYS_EQ){
							if(this->get_val()!=other.get_val()){
								AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
								this->get_val()=other.get_val();
								this->maxLen=other.maxLen;
								this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
							}
						}
						al::memcopy<AlTy>(this->text.heap,other.c_str(),other.length+1);
					}
					else{
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						COPY_ALLOC
						this->maxLen=other.length*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
						al::memcopy<AlTy>(this->text.heap,other.c_str(),other.length+1);
					}
				}
				this->length=other.length;
				return *this;
			}
			Str& operator=(const_pointer str){
				this->length=al::strsize<AlTy>(str);
				if(this->maxLen==0){
					if(this->length<ss_cap)
					al::memcopy<AlTy>(this->text.buffer,str,this->length+1);
					else{
						this->maxLen=this->length*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
						al::memcopy<AlTy>(this->text.heap,str,this->length+1);
					}
				}
				else{
					if(this->length<=this->maxLen)
					al::memcopy<AlTy>(this->text.heap,str,this->length+1);
					else{
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=this->length*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
						al::memcopy<AlTy>(this->text.heap,this->length+1);
					}
				}
				return *this;
			}
			Str& operator=(T chr){
				this->length=1;
				if(this->maxLen==0){
					this->text.buffer[0]=chr;
					this->text.buffer[1]=T();
				}
				else{
					this->text.heap[0]=chr;
					this->text.heap[1]=T();
				}
				return *this;
			}
			Str& operator=(Str&& other){
				if(this==&other) return *this;
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->get_val()!=other.get_val()){
						if(this->maxLen==0){
							if(other.maxLen==0)
								al::memcopy<AlTy>(this->text.buffer,other.text.buffer,other.length+1);
							else{
								this->maxLen=other.maxLen;
								this->text.heap=AlTy_traits::allocate(this->get_val(),this->text.heap,maxLen+1);
								al::memcopy<AlTy>(this->text.heap,other.text.heap,other.length+1);
								AlTy_traits::deallocate(other.get_val(),other.text.heap,other.maxLen+1);
								other.maxLen=0;
							}
						}
						else{
							if(other.maxLen==0)
								al::memcopy<AlTy>(this->text.heap,other.text.buffer,other.length+1);
							else{
								AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
								this->maxLen=other.maxLen;
								this->text.heap=AlTy_traits::allocate(this->get_val(),this->text.heap,maxLen+1);
								al::memcopy<AlTy>(this->text.heap,other.text.heap,other.length+1);
								AlTy_traits::deallocate(other.get_val(),other.text.heap,other.maxLen+1);
								other.maxLen=0;
							}
						}
						this->length=other.length;
						other.length=0;
						other.text.heap=nullptr;
						return *this;						
					}
				}
				if(this->maxLen==0){
					if(other.maxLen==0)
						al::memcopy<AlTy>(this->text.buffer,other.text.buffer,other.length+1);
					else{
						this->maxLen=other.maxLen;
						this->text.heap=other.text.heap;
						other.maxLen=0;
					}
				}
				else{
					if(other.maxLen==0)
						al::memcopy<AlTy>(this->text.heap,other.text.buffer,other.length+1);
					else{
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=other.maxLen;
						this->text.heap=other.text.heap;
						other.maxLen=0;
					}
				}
				if constexpr(POCMA) this->get_val()=static_cast<AlTy&&>(other.get_val());
				this->length=other.length;
				other.length=0;
				other.text.heap=nullptr;
				return *this;
			}
			Str operator+(const Str& other)const&{
				Str new_string(this->length+other.length);//Constructor que asigna un length(ya tiene en cuenta el espacio extra para el \0, al cual asigna \0) y reserva la memoria necesaria
				al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
				al::memcopy<AlTy>(new_string.c_str()+this->length,other.c_str(),other.length);
				return new_string;
			}

			Str operator+(const_pointer str)const&{
				Str new_string(this->length+al::strsize<AlTy>(str));
				al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
				al::memcopy<AlTy>(new_string.c_str()+this->length,str,new_string.length-this->length+1);//strcopy<AlTy>(new_string.c_str()+this->length,str);
				return new_string;
			}
			Str operator+(T chr)const&{
				Str new_string(this->length+1);
				al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
				new_string.c_str()[this->length]=chr;
				return new_string;
			}
			Str operator+(Str&& other)const&{
				Str new_string;//El \0 no se asigna solo como en las funciones anteriores que usan Str(size_type)
				new_string.length=this->length+other.length;
				if(new_string.length==0) return new_string;
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->get_val()!=new_string.get_val()){
						new_string.alloc_size();//Asigna la memoria necesaria y pone el \0 al final
						al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
						al::memcopy<AlTy>(new_string.c_str()+this->length,other.c_str(),other.length);
						return new_string;
					}
				}
				if(new_string.length<=other.maxLen){
					if constexpr(POCMA) new_string.get_val()=static_cast<AlTy&&>(other.get_val());
					new_string.text.heap=other.text.heap;
					al::reversecopy<AlTy>(new_string.text.heap+this->length,other.text.heap,other.length+1);
					al::memcopy<AlTy>(new_string.text.heap,this->c_str(),this->length);
					new_string.maxLen=other.maxLen;
					other.length=0;
					other.maxLen=0;
					other.text.heap=nullptr;
				}
				else{
					new_string.alloc_size();
					al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
					al::memcopy<AlTy>(new_string.c_str()+this->length,other.c_str(),other.length);
				}
				return new_string;
			}
			Str operator+(const Str& other)&&{
				Str new_string;
				new_string.length=this->length+other.length;
				if(new_string.length==0) return new_string;
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->get_val()!=new_string.get_val()){
						new_string.alloc_size();
						al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
						al::memcopy<AlTy>(new_string.c_str()+this->length,other.c_str(),other.length);
						return new_string;
					}
				}
				if(new_string.length<=this->maxLen){
					if constexpr(POCMA) new_string.get_val()=static_cast<AlTy&&>(this->get_val());
					new_string.text.heap=this->text.heap;
					al::memcopy<AlTy>(new_string.text.heap+this->length,other.c_str(),other.length+1);
					new_string.maxLen=this->maxLen;
					this->length=0;
					this->maxLen=0;
					this->text.heap=nullptr;
				}
				else{
					new_string.alloc_size();
					al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
					al::memcopy<AlTy>(new_string.c_str()+this->length,other.c_str(),other.length);
				}
				return new_string;
			}
			Str operator+(const_pointer str)&&{
				size_type str_size=al::strsize<AlTy>(str);
				Str new_string;
				new_string.length=this->length+str_size;
				if(new_string.length==0) return new_string;
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->get_val()!=new_string.get_val()){
						new_string.alloc_size();
						al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
						al::memcopy<AlTy>(new_string.c_str()+this->length,str,str_size+1);//strcopy<AlTy>(new_string.c_str()+this->length,str);
						return new_string;						
					}
				}
				if(new_string.length<=this->maxLen){
					if constexpr(POCMA) new_string.get_val()=static_cast<AlTy&&>(this->get_val());
					new_string.text.heap=this->text.heap;
					al::memcopy<AlTy>(new_string.text.heap+this->length,str,str_size+1);//strcopy<AlTy>(new_string.text.heap+this->length,str);
					new_string.maxLen=this->maxLen;
					this->length=0;
					this->maxLen=0;
					this->text.heap=nullptr;
				}
				else{
					new_string.alloc_size();
					al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
					al::memcopy<AlTy>(new_string.c_str()+this->length,str,str_size+1);//strcopy<AlTy>(new_string.c_str()+this->length,str);
				}
				return new_string;
			}
			Str operator+(T chr)&&{
				Str new_string;
				new_string.length=this->length+1;
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->get_val()!=new_string.get_val()){
						new_string.alloc_size();
						al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
						new_string.c_str()[this->length]=chr;
						return new_string;						
					}
				}
				if(new_string.length<=this->maxLen){
					if constexpr(POCMA) new_string.get_val()=static_cast<AlTy&&>(this->get_val());
					new_string.text.heap=this->text.heap;
					new_string.text.heap[this->length]=chr;
					new_string.text.heap[new_string.length]=T();
					new_string.maxLen=this->maxLen;
					this->length=0;
					this->maxLen=0;
					this->text.heap=nullptr;
				}
				else{
					new_string.alloc_size();
					al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
					new_string.c_str()[this->length]=chr;
				}
				return new_string;
			}
			Str operator+(Str&& other)&&{
				Str new_string;
				new_string.length=this->length+other.length;
				if(new_string.length==0) return new_string;
				if((POCMA || new_string.get_val()==this->get_val()) && new_string.length<=this->maxLen){// maxLen>0 means there is allocated memory. new_len can't be 0 here, so if this condition
					if constexpr(POCMA)	new_string.get_val()=static_cast<AlTy&&>(this->get_val());	   // is true I can assume there is allocated memory
					new_string.text.heap=this->text.heap;
					al::memcopy<AlTy>(new_string.text.heap+this->length,other.c_str(),other.length+1);
					new_string.maxLen=this->maxLen;
					this->length=0;
					this->maxLen=0;
					this->text.heap=nullptr;
				}
				else if((POCMA || new_string.get_val()==other.get_val()) && new_string.length<=other.maxLen){
					if constexpr(POCMA)	new_string.get_val()=static_cast<AlTy&&>(other.get_val());
					new_string.text.heap=other.text.heap;
					al::reversecopy<AlTy>(new_string.text.heap+this->length,other.text.heap,other.length+1);
					al::memcopy<AlTy>(new_string.text.heap,this->c_str(),this->length);
					new_string.maxLen=other.maxLen;
					other.length=0;
					other.maxLen=0;
					other.text.heap=nullptr;
				}
				else{
					new_string.alloc_size();
					al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->length);
					al::memcopy<AlTy>(new_string.c_str()+this->length,other.c_str(),other.length);
				}
				return new_string;
			}
			Str& operator+=(const Str& other){
				size_type new_len=this->length+other.length;
				if(this->maxLen==0){
					if(new_len<ss_cap)
						al::reversecopy<AlTy>(this->text.buffer+this->length,other.c_str(),other.length+1);//reversecopy porque this y other podrian ser el mismo objeto
					else{
						this->maxLen=new_len*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxLen+1);
						al::memcopy<AlTy>(new_text,this->text.buffer,this->length);
						al::memcopy<AlTy>(new_text+this->length,other.c_str(),other.length+1);
						this->text.heap=new_text;
					}
				}
				else{
					if(new_len<=this->maxLen)
						al::reversecopy<AlTy>(this->text.heap+this->length,other.c_str(),other.length+1);
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_len*M+1);
						al::memcopy<AlTy>(new_text,this->text.heap,this->length);
						al::memcopy<AlTy>(new_text+this->length,other.c_str(),other.length+1);
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_text;
					}
				}
				this->length=new_len;
				return *this;
			}
			Str& operator+=(Str&& other){//si this y other son el mismo objeto es comportamiento indeinido
				size_type new_len=this->length+other.length;
				if(this->maxLen==0){
					if(new_len<ss_cap)
						al::memcopy<AlTy>(this->text.buffer+this->length,other.c_str(),other.length+1);
					else if((POCMA || this->get_val()==other.get_val()) && new_len<=other.maxLen){
						if constexpr(POCMA) this->get_val()=static_cast<AlTy&&>(other.get_val());
						al::reversecopy<AlTy>(other.text.heap+this->length,other.text.heap,other.length+1);
						al::memcopy<AlTy>(other.text.heap,this->text.buffer,this->length);
						this->text.heap=other.text.heap;
						this->maxLen=other.maxLen;
						other.length=0;
						other.maxLen=0;
						other.text.heap=nullptr;
					}
					else{
						this->maxLen=new_len*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxLen+1);
						al::memcopy<AlTy>(new_text,this->text.buffer,this->length);
						al::memcopy<AlTy>(new_text+this->length,other.c_str(),other.length+1);
						this->text.heap=new_text;
					}
				}
				else{
					if(new_len<=this->maxLen)
						al::memcopy<AlTy>(this->text.heap+this->length,other.c_str(),other.length+1);
					else if((POCMA || this->get_val()==other.get_val()) && new_len<=other.maxLen){
						if constexpr(POCMA) this->get_val()=static_cast<AlTy&&>(other.get_val());
						al::reversecopy<AlTy>(other.text.heap+this->length,other.text.heap,other.length+1);
						al::memcopy<AlTy>(other.text.heap,this->text.heap,this->length);
						this->text.heap=other.text.heap;
						this->maxLen=other.maxLen;
						other.length=0;
						other.maxLen=0;
						other.text.heap=nullptr;
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_len*M+1);
						al::memcopy<AlTy>(new_text,this->text.heap,this->length);
						al::memcopy<AlTy>(new_text+this->length,other.c_str(),other.length+1);
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_text;
					}
				}
				this->length=new_len;
				return *this;
			}
			Str& operator+=(const_pointer str){
				size_type str_size=al::strsize<AlTy>(str);
				size_type new_len=this->length+str_size;
				if(this->maxLen==0){
					if(new_len<ss_cap)
						al::reversecopy<AlTy>(this->text.buffer+this->length,str,str_size+1);//reversecopy porq this->c_str() puede ser str
					else{
						this->maxLen=new_len*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxLen+1);
						al::memcopy<AlTy>(new_text,this->text.buffer,this->length);
						al::memcopy<AlTy>(new_text+this->length,str,str_size+1);//strcopy<AlTy>(new_text+this->length,str);
						this->text.heap=new_text;
					}
				}
				else{
					if(new_len<=this->maxLen)
						al::reversecopy<AlTy>(this->text.heap+this->length,str,str_size+1);
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_len*M+1);
						al::memcopy<AlTy>(new_text,this->text.heap,this->length);
						al::memcopy<AlTy>(new_text+this->length,str,str_size+1);//strcopy<AlTy>(new_text+this->length,str);
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_text;
					}
				}
				this->length=new_len;
				return *this;
			}
			Str& operator+=(T chr){
				size_type new_len=this->length+1;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						this->text.buffer[this->length]=chr;
						this->text.buffer[new_len]=T();
					}
					else{
						this->maxLen=new_len*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxLen+1);
						al::memcopy<AlTy>(new_text,this->text.buffer,this->length);
						new_text[this->length]=chr;
						new_text[new_len]=T();
						this->text.heap=new_text;
					}
				}
				else{
					if(new_len<=this->maxLen){
						this->text.heap[this->length]=chr;
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_len*M+1);
						al::memcopy<AlTy>(new_text,this->text.heap,this->length);
						new_text[this->length]=chr;
						new_text[new_len]=T();
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_text;
					}
				}
				this->length=new_len;
				return *this;
			}
			Str operator*(unsigned int num)const{
				Str new_string(this->length*num);
				pointer new_text=new_string.c_str();
				const_pointer this_text=this->c_str();
				for(size_type index=0;index<new_string.length;index+=this->length)
					al::memcopy<AlTy>(new_text+index,this_text,this->length);
				return new_string;
			}
			Str& operator*=(unsigned int num){
				size_type new_len=this->length*num;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						for(size_type index=this->length;index<new_len;index+=this->length)
							al::memcopy<AlTy>(this->text.buffer+index,this->text.buffer,this->length);
						this->text.buffer[new_len]=T();
					}
					else{
						this->maxLen=new_len*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxLen+1);
						for(size_type index=0;index<new_len;index+=this->length)
							al::memcopy<AlTy>(new_text+index,this->text.buffer,this->length);
						new_text[new_len]=T();
						this->text.heap=new_text;
					}
				}
				else{
					if(new_len<=this->maxLen){
						for(size_type index=this->length;index<new_len;index+=this->length)
							al::memcopy<AlTy>(this->text.heap+index,this->text.heap,this->length);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_len*M+1);
						for(size_type index=0;index<new_len;index+=this->length)
							al::memcopy<AlTy>(new_text+index,this->text.heap,this->length);
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						new_text[new_len]=T();
						this->text.heap=new_text;
					}
				}
				this->length=new_len;
				return *this;
			}
			void operator%=(Str& other){
				if constexpr(!ALWAYS_EQ && !POCS){
					if(this->get_val()!=other.get_val()){
						Str aux=static_cast<Str&&>(*this);
						*this=static_cast<Str&&>(other);
						other=static_cast<Str&&>(aux);
						return;
					}
				}
				if constexpr(POCS && !ALWAYS_EQ){
					AlTy aux=static_cast<AlTy&&>(this->get_val());
					this->get_val()=static_cast<AlTy&&>(other.get_val());
					other.get_val()=static_cast<AlTy&&>(aux);
				}
				sbo string_aux=this->text;
				size_type size_aux=this->length;
				size_type maxLen_aux=this->maxLen;
				this->text=other.text;
				this->length=other.length;
				this->maxLen=other.maxLen;
				other.text=string_aux;
				other.length=size_aux;
				other.maxLen=maxLen_aux;
			}
			signed char cmp(const Str& other)const{
				if(this->length>other.length) return 2;
				else if(this->length<other.length) return -2;
				else{
					const_pointer a=this->c_str();
					const_pointer b=other.c_str();
					for(size_type i=0;i<this->length;i++){
						if(a[i]!=b[i]){
							return a[i]>b[i] ? 1 : -1;						
						}
					}
					return 0;
				}
			}
			bool operator==(const Str& other)const{
				return this->cmp(other)==0;
			}
			bool operator!=(const Str& other)const{
				return this->cmp(other)!=0;
			}
			bool operator>(const Str& other)const{
				return this->cmp(other)>0;
			}
			bool operator<(const Str& other)const{
				return this->cmp(other)<0;
			}
			bool operator>=(const Str& other)const{
				return this->cmp(other)>=0;
			}
			bool operator<=(const Str& other)const{
				return this->cmp(other)<=0;
			}
			const T& operator[](size_type index)const{
				return this->c_str()[index];
			}
			T& operator[](size_type index){
				return this->c_str()[index];
			}
			void insert(size_type index,const Str& other){
				size_type new_len=this->length+other.length;
				if(this->maxLen==0){
					if(new_len<ss_cap && this!=&other){
						pointer insert_pos=this->text.buffer+index;
						al::reversecopy<AlTy>(insert_pos+other.length,insert_pos,this->length-index);
						al::memcopy<AlTy>(insert_pos,other.c_str(),other.length);
						this->text.buffer[new_len]=T();
					}
					else{
						this->maxLen=new_len*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxLen+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.buffer,index);
						al::memcopy<AlTy>(insert_pos,other.c_str(),other.length);
						al::memcopy<AlTy>(insert_pos+other.length,this->text.buffer+index,this->length-index);
						new_text[new_len]=T();
						this->text.heap=new_text;
					}
				}
				else{
					if(new_len<=this->maxLen && this!=&other){
						pointer insert_pos=this->text.heap+index;
						al::reversecopy<AlTy>(insert_pos+other.length,insert_pos,this->length-index);
						al::memcopy<AlTy>(insert_pos,other.c_str(),other.length);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_len*M+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.heap,index);
						al::memcopy<AlTy>(insert_pos,other.c_str(),other.length);
						al::memcopy<AlTy>(insert_pos+other.length,this->text.heap+index,this->length-index);
						new_text[new_len]=T();
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_text;
					}
				}
				this->length=new_len;
			}
			void insert(size_type index,const_pointer str){
				size_type str_size=al::strsize<AlTy>(str);
				size_type new_len=this->length+str_size;
				if(this->maxLen==0){
					if(new_len<ss_cap && this->text.buffer!=str){
						pointer insert_pos=this->text.buffer+index;
						al::reversecopy<AlTy>(insert_pos+str_size,insert_pos,this->length-index);
						al::memcopy<AlTy>(insert_pos,str,str_size);
						this->text.buffer[new_len]=T();
					}
					else{
						this->maxLen=new_len*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxLen+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.buffer,index);
						al::memcopy<AlTy>(insert_pos,str,str_size);
						al::memcopy<AlTy>(insert_pos+str_size,this->text.buffer+index,this->length-index);
						new_text[new_len]=T();
						this->text.heap=new_text;
					}
				}
				else{
					if(new_len<=this->maxLen && this->text.heap!=str){
						pointer insert_pos=this->text.heap+index;
						al::reversecopy<AlTy>(insert_pos+str_size,insert_pos,this->length-index);
						al::memcopy<AlTy>(insert_pos,str,str_size);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_len*M+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.heap,index);
						al::memcopy<AlTy>(insert_pos,str,str_size);
						al::memcopy<AlTy>(insert_pos+str_size,this->text.heap+index,this->length-index);
						new_text[new_len]=T();
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_text;
					}
				}
				this->length=new_len;
			}
			void insert(size_type index,T chr){
				size_type new_len=this->length+1;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						pointer insert_pos=this->text.buffer+index;
						al::reversecopy<AlTy>(insert_pos+1,insert_pos,this->length-index);
						*insert_pos=chr;
						this->text.buffer[new_len]=T();
					}
					else{
						this->maxLen=new_len*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxLen+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.buffer,index);
						*insert_pos=chr;
						al::memcopy<AlTy>(insert_pos+1,this->text.buffer+index,this->length-index);
						new_text[new_len]=T();
					}
				}
				else{
					if(new_len<=this->maxLen){
						pointer insert_pos=this->text.heap+index;
						al::reversecopy<AlTy>(insert_pos+1,insert_pos,this->length-index);
						*insert_pos=chr;
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_len*M+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.heap,index);
						*insert_pos=chr;
						al::memcopy<AlTy>(insert_pos+1,this->text.heap+index,this->length-index);
						new_text[new_len]=T();
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_text;
					}
				}
				this->length=new_len;
			}
			void del_back(){
				this->c_str()[--length]=T();
			}
			T pop_back(){
				T chr=this->c_str()[length-1];
				this->c_str()[--length]=T();
				return chr;
			}
			void erase(size_type start,size_type end){
				size_type new_len=this->length-(end-start);
				pointer this_text=this->c_str();
				al::memcopy<AlTy>(this_text+start,this_text+end,this->length-end);
				this_text[new_len]=T();
				this->length=new_len;
			}
			void erase(size_type start){
				this->c_str()[start]=T();
				this->length=start;
			}
			void remove(const Str& substring,size_type start,size_type end){
				Optional<size_type> index=this->find(substring,start,end);
				if(index.has_value()){
					this->erase(index.value(),index.value()+substring.length);	
				}
			}
			void remove(const_pointer substring,size_type start,size_type end){
				Optional<size_type> index=this->find(substring,start,end);
				if(index.has_value()){
					this->erase(index.value(),index.value()+al::strsize<AlTy>(substring));	
				}
			}
			void rremove(const Str& substring,size_type start,size_type end){
				Optional<size_type> index=this->rfind(substring,start,end);
				if(index.has_value()){
					this->erase(index.value(),index.value()+substring.length);	
				}
			}
			void rremove(const_pointer substring,size_type start,size_type end){
				Optional<size_type> index=this->rfind(substring,start,end);
				if(index.has_value()){
					this->erase(index.value(),index.value()+al::strsize<AlTy>(substring));	
				}
			}
			void remove(const Str& substring,size_type start=0){
				Optional<size_type> index=this->find(substring,start);
				if(index.has_value()){
					this->erase(index.value(),index.value()+substring.length);	
				}
			}
			void remove(const_pointer substring,size_type start=0){
				Optional<size_type> index=this->find(substring,start);
				if(index.has_value()){
					this->erase(index.value(),index.value()+al::strsize<AlTy>(substring));	
				}
			}
			void rremove(const Str& substring,size_type start=0){
				Optional<size_type> index=this->rfind(substring,start);
				if(index.has_value()){
					this->erase(index.value(),index.value()+substring.length);	
				}
			}
			void rremove(const_pointer substring,size_type start=0){
				Optional<size_type> index=this->rfind(substring,start);
				if(index.has_value()){
					this->erase(index.value(),index.value()+al::strsize<AlTy>(substring));	
				}
			}
			void outplace(const Str& substring,size_type start,size_type end){
				size_type new_len=this->length-(end-start)+substring.length;
				const_pointer substring_text=substring.c_str();
				pointer this_text=this->c_str();
				if((new_len<ss_cap && this->maxLen==0) || new_len<=this->maxLen){			// v for \0
					al::safecopy<AlTy>(this_text+start+substring.length,this_text+end,this->length-end+1);// copyes memory correctly even if source and dest overlap
					al::memcopy<AlTy>(this_text+start,substring_text,substring.length);
				}
				else{
					pointer new_text=AlTy_traits::allocate(this->get_val(),new_len*M+1);
					al::memcopy<AlTy>(new_text,this_text,start);
					al::memcopy<AlTy>(new_text+start,substring_text,substring.length);
					al::memcopy<AlTy>(new_text+start+substring.length,this_text+end,this->length-end+1);
					if(this->maxLen>0) AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
					this->text.heap=new_text;
					this->maxLen=new_len*M;
				}
				this->length=new_len;
			}
			template<bool whole_words_only=false>
			void replace(const Str& substring1,const Str& substring2,Optional<size_type> max_count={}){
				size_type count=this->count<whole_words_only>(substring1);
				if(max_count.has_value() && count>max_count.value())
					count=max_count.value();
				if(count==0) return;
				using AlInt=rebind_alloc<AlTy,size_type>;
				AlInt uint_alloc(this->get_val());
				size_type* array=allocator_traits<AlInt>::allocate(uint_alloc,count);
				for(size_type i=0,last=0;i<count;last=array[i++]+substring1.length)
					array[i]=this->find<whole_words_only>(substring1,last).value();
				for(size_type i=0;i<count;i++)
					array[i]=array[i]+(substring2.length-substring1.length)*i;
				if(substring1.length==0){
					for(size_type i=0;i<count;i++)
						array[i]+=i;
				}
				size_type new_len=this->length+(count*substring2.length-count*substring1.length);
				const_pointer substring2_text=substring2.c_str();
				if(this->maxLen==0){
					if(new_len<=this->length){
						for(size_type i=array[0],j=array[0],k=0;i<new_len;){
							if(k<count && i==array[k]){
								al::memcopy<AlTy>(this->text.buffer+array[k++],substring2_text,substring2.length);
								i+=substring2.length;
								j+=substring1.length;
							}
							else{
								this->text.buffer[i]=this->text.buffer[j];
								i++;
								j++;
							}
						}
						this->text.buffer[new_len]=T();
					}
					else{
						this->maxLen=new_len*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxLen+1);
						for(size_type i=0,j=0,k=0;i<new_len;){
							if(k<count && i==array[k]){
								al::memcopy<AlTy>(new_text+array[k++],substring2_text,substring2.length);
								i+=substring2.length;
								j+=substring1.length;
							}
							else{
								new_text[i]=this->text.buffer[j];
								i++;
								j++;
							}
						}
						new_text[new_len]=T();
						this->text.heap=new_text;
					}
				}
				else{
					if(new_len<=this->length){//aunque new_len<=maxLen si new_len<=length no se cumple se necesita reasignar memoria
						for(size_type i=array[0],j=array[0],k=0;i<new_len;){
							if(k<count && i==array[k]){
								al::memcopy<AlTy>(this->text.heap+array[k++],substring2_text,substring2.length);
								i+=substring2.length;
								j+=substring1.length;
							}
							else{
								this->text.heap[i]=this->text.heap[j];
								i++;
								j++;
							}
						}
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_len*M+1);
						for(size_type i=0,j=0,k=0;i<new_len;){
							if(k<count && i==array[k]){
								al::memcopy<AlTy>(new_text+array[k++],substring2_text,substring2.length);
								i+=substring2.length;
								j+=substring1.length;
							}
							else{
								new_text[i]=this->text.heap[j];
								i++;
								j++;
							}
						}
						new_text[new_len]=T();
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_text;						
					}
				}
				this->length=new_len;
				allocator_traits<AlInt>::deallocate(uint_alloc,array,count);
			}
			Str substr(size_type start,size_type end)const{
				Str new_string(end-start);//el tamaño del nuevo string lo genera el constructor
				al::memcopy<AlTy>(new_string.c_str(),this->c_str()+start,new_string.length);
				return new_string;
			}
			Str substr(size_type start)const{
				Str new_string(length-start);
				al::memcopy<AlTy>(new_string.c_str(),this->c_str()+start,new_string.length);
				return new_string;
			}
			Str extract(size_type index,size_type pos)const{
				Str new_string(pos);
				al::memcopy<AlTy>(new_string.c_str(),this->c_str()+index,pos);
				return new_string;
			}
			Str extract(size_type index)const{
				Str new_string(length-index);
				al::memcopy<AlTy>(new_string.c_str(),this->c_str()+index,length-index);
				return new_string;
			}
			template<T SPACE=' '>
			void noExtraSpaces(){
				pointer this_text=this->c_str();
				size_type i=0,j=0;
				while(j<this->length){
					if(this_text[j]==SPACE && this_text[j+1]==SPACE){
						while(j<this->length && this_text[j]==this_text[j+1]){
							j++;
						}						
					}
					else{
						this_text[i++]=this_text[j++];
					}
				}
				this->length=i;
				this_text[i]=T();
			}
			template<T SPACE=' '>
			void lstrip(){
				pointer this_text=this->c_str();
				if(this_text[0]!=SPACE) return;// Para no tener que copiar el string completo si es que no hay espacios que borrar
				size_type count=0;
				while(count<this->length && this_text[count]==SPACE)
					count++;
				this->length-=count;
				al::memcopy<AlTy>(this_text,this_text+count,this->length+1);
			}
			template<T SPACE=' '>
			void rstrip(){
				pointer this_text=this->c_str();
				while(length>0 && this_text[length-1]==SPACE){
					length--;
				}
				this_text[length]=T();
			}
			template<T SPACE=' '>
			void strip(){
				rstrip<SPACE>();
				lstrip<SPACE>();
			}
			void upper(){
				pointer this_text=this->c_str();
				for(size_type i=0;i<length;i++)
					if(this_text[i]>=97 && this_text[i]<=122)
						this_text[i]-=32;
			}
			void lower(){
				pointer this_text=this->c_str();
				for(size_type i=0;i<length;i++)
					if(this_text[i]>=65 && this_text[i]<=90)
						this_text[i]+=32;
			}
			template<T SPACE=' '>
			Str noExtraSpaces_cpy()const{
				const_pointer this_text=this->c_str();
				size_type count=0;
				for(size_type i=0;i<this->length;i++)
					if(this_text[i]==SPACE && this_text[i+1]==SPACE)
						count++;
				Str new_string(this->length-count);
				pointer new_text=new_string.c_str();
				for(size_type i=0,j=0;i<this->length;i++){
					if(!(this_text[i]==SPACE && this_text[i+1]==SPACE)){
						new_text[j]=this_text[i];
						j++;
					}
				}
				return new_string;
			}
			template<T SPACE=' '>
 			Str lstrip_cpy()const{
				if(this->length==0) return Str();
				const_pointer this_text=this->c_str();
				size_type count=0;
				while(count<this->length && this_text[count]==SPACE)
					count++;
				Str new_string(this->length-count);
				pointer new_text=new_string.c_str();
				for(size_type i=0;count<this->length;i++,count++)
					new_text[i]=this_text[count];
				return new_string;
			}
			template<T SPACE=' '>
 			Str rstrip_cpy()const{
				const_pointer this_text=this->c_str();
				size_type count=this->length;
				while(count>0 && this_text[count-1]==SPACE)
					count--;
				Str new_string(count);
				pointer new_text=new_string.c_str();
				for(count=0;count<new_string.length;count++)
					new_text[count]=this_text[count];
				return new_string;
			}
			template<T SPACE=' '>
			Str strip_cpy()const{
				if(this->length==0) return Str();
				const_pointer this_text=this->c_str();
				size_type lcount=0,rcount=this->length;
				while(lcount<this->length && this_text[lcount]==SPACE)
					lcount++;
				if(lcount<this->length)//Por si el string solamente contiene espacios
					while(/* rcount>0 &&  */this_text[rcount-1]==SPACE)
						rcount--;					
				Str new_string(rcount-lcount);
				pointer new_text=new_string.c_str();
				for(rcount=0;rcount<new_string.length;lcount++,rcount++)
					new_text[rcount]=this_text[lcount];
				return new_string;
			}
			Str upper_cpy()const{
				Str new_string(this->length);
				if(this->length>=ss_cap)
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.heap[i]>=97 && this->text.heap[i]<=122 ? this->text.heap[i]-32:this->text.heap[i];
				else
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.buffer[i]>=97 && this->text.buffer[i]<=122 ? this->text.buffer[i]-32:this->text.buffer[i];
				return new_string;
			}
			Str lower_cpy()const{
				Str new_string(this->length);
				if(this->length>=ss_cap)
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.heap[i]>=65 && this->text.heap[i]<=90 ? this->text.heap[i]+32:this->text.heap[i];
				else
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.buffer[i]>=65 && this->text.buffer[i]<=90 ? this->text.buffer[i]+32:this->text.buffer[i];
				return new_string;
			}
			template<bool whole_words_only=false>
			Optional<size_type> find(const Str& substring,size_type start,size_type end)const{
				if(end<start+substring.length) return {};
				if(substring.length==0) return start;
				const_pointer c_substring=substring.c_str(), this_text=this->c_str();
				for(size_type i=start;i<=end-substring.length;i++){
					if constexpr(whole_words_only){
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (i==end-substring.length || is_special(this_text[i+substring.length-1]) || is_special(this_text[i+substring.length]))) return i;
					}else{
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length)) return i;
					}
				}
				return {};
			}
			template<bool whole_words_only=false>
			Optional<size_type> rfind(const Str& substring,size_type start,size_type end)const{
				if(end<start+substring.length) return {};
				if(substring.length==0) return end;
				const_pointer c_substring=substring.c_str(), this_text=this->c_str();
				for(size_type i=end-substring.length;i>=start;i--){
					if constexpr(whole_words_only){ // i==start para q no mire una posicon a la izquierda de start, porq esa posicion no esta en el rango de busqueda que se especifico en los parametros de la funcion(start y end).
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (i==end-substring.length || is_special(this_text[i+substring.length-1]) || is_special(this_text[i+substring.length]))) return i;
					}else{																								// Y i==end-substring lo mismo que i==start pero a la derecha del string
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length)) return i;
					}
					if(i==0) return {};//because i is unsigned
				}
				return {};
			}
			template<bool whole_words_only=false>
			size_type count(const Str& substring,size_type start,size_type end)const{
				if(end<start+substring.length) return 0;
				if(substring.length==0) return end-start+1;
				const_pointer c_substring=substring.c_str(), this_text=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=end-substring.length;i++){
					if constexpr(whole_words_only){
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (i==end-substring.length || is_special(this_text[i+substring.length-1]) || is_special(this_text[i+substring.length]))){
							counter++;
							i+=substring.length-1;
						}
					}else{
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length)){
							counter++;
							i+=substring.length-1;
						}
					}
				}
				return counter;
			}
			template<bool whole_words_only=false>
			Optional<size_type> find(const_pointer c_substring,size_type start,size_type end)const{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(end<start+substring_size) return {};
				if(substring_size==0) return start;
				const_pointer this_text=this->c_str();
				for(size_type i=start;i<=end-substring_size;i++){
					if constexpr(whole_words_only){
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (i==end-substring_size || is_special(this_text[i+substring_size-1]) || is_special(this_text[i+substring_size]))) return i;
					}else{
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size)) return i;
					}
				}
				return {};
			}
			template<bool whole_words_only=false>
			Optional<size_type> rfind(const_pointer c_substring,size_type start,size_type end)const{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(end<start+substring_size) return {};
				if(substring_size==0) return end;
				const_pointer this_text=this->c_str();
				for(size_type i=end-substring_size;i>=start;i--){
					if constexpr(whole_words_only){
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (i==end-substring_size || is_special(this_text[i+substring_size-1]) || is_special(this_text[i+substring_size]))) return i;
					}else{
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size)) return i;
					}
					if(i==0) return {};
				}
				return {};
			}
			template<bool whole_words_only=false>
			size_type count(const_pointer c_substring,size_type start,size_type end)const{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(end<start+substring_size) return 0;
				if(substring_size==0) return end-start+1;
				const_pointer this_text=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=end-substring_size;i++){
					if constexpr(whole_words_only){
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (i==end-substring_size || is_special(this_text[i+substring_size-1]) || is_special(this_text[i+substring_size]))){
							counter++;
							i+=substring_size-1;
						}
					}else{
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size)){
							counter++;
							i+=substring_size-1;
						}
					}
				}
				return counter;
			}
			template<bool whole_words_only=false>
			Optional<size_type> find(const Str& substring,size_type start=0)const{
				if(this->length<start+substring.length) return {};
				if(substring.length==0) return start;
				const_pointer c_substring=substring.c_str(), this_text=this->c_str();
				for(size_type i=start;i<=this->length-substring.length;i++){
					if constexpr(whole_words_only){		// comprobar si i==end-substring.length no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (is_special(this_text[i+substring.length-1]) || is_special(this_text[i+substring.length]))) return i;
					}else{
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length)) return i;
					}
				}
				return {};
			}
			template<bool whole_words_only=false>
			Optional<size_type> rfind(const Str& substring,size_type start=0)const{
				if(this->length<start+substring.length) return {};
				if(substring.length==0) return this->length;//this->length==end
				const_pointer c_substring=substring.c_str(), this_text=this->c_str();
				for(size_type i=this->length-substring.length;i>=start;i--){
					if constexpr(whole_words_only){		// comprobar si i==end-substring.length no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (is_special(this_text[i+substring.length-1]) || is_special(this_text[i+substring.length]))) return i;
					}else{
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length)) return i;
					}
					if(i==0) return {};
				}
				return {};
			}
			template<bool whole_words_only=false>
			size_type count(const Str& substring,size_type start=0)const{
				if(this->length<start+substring.length) return 0;
				if(substring.length==0) return this->length-start+1;
				const_pointer c_substring=substring.c_str(), this_text=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=this->length-substring.length;i++){
					if constexpr(whole_words_only){
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (is_special(this_text[i+substring.length-1]) || is_special(this_text[i+substring.length]))){
							counter++;
							i+=substring.length-1;
						}
					}else{
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring.length)){
							counter++;
							i+=substring.length-1;
						}
					}
				}
				return counter;
			}
			template<bool whole_words_only=false>
			Optional<size_type> find(const_pointer c_substring,size_type start=0)const{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(this->length<start+substring_size) return {};
				if(substring_size==0) return start;
				const_pointer this_text=this->c_str();
				for(size_type i=start;i<=this->length-substring_size;i++){
					if constexpr(whole_words_only){		// comprobar si i==end-c_substring.length no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (is_special(this_text[i+substring_size-1]) || is_special(this_text[i+substring_size]))) return i;
					}else{
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size)) return i;
					}
				}
				return {};
			}
			template<bool whole_words_only=false>
			Optional<size_type> rfind(const_pointer c_substring,size_type start=0)const{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(this->length<start+substring_size) return {};
				if(substring_size==0) return this->length;//this->length==end
				const_pointer this_text=this->c_str();
				for(size_type i=this->length-substring_size;i>=start;i--){
					if constexpr(whole_words_only){		// comprobar si i==end-substring_size no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (is_special(this_text[i+substring_size-1]) || is_special(this_text[i+substring_size]))) return i;
					}else{
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size)) return i;
					}
					if(i==0) return {};
				}
				return {};
			}
			template<bool whole_words_only=false>
			size_type count(const_pointer c_substring,size_type start=0)const{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(this->length<start+substring_size) return 0;
				if(substring_size==0) return this->length-start+1;
				const_pointer this_text=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=this->length-substring_size;i++){
					if constexpr(whole_words_only){
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size) && (i==start || is_special(this_text[i]) || is_special(this_text[i-1])) && (is_special(this_text[i+substring_size-1]) || is_special(this_text[i+substring_size]))){
							counter++;
							i+=substring_size-1;
						}
					}else{
						if(!al::memcomp<AlTy>(c_substring,this_text+i,substring_size)){
							counter++;
							i+=substring_size-1;
						}
					}
				}
				return counter;
			}
			template<template<typename...> typename Container>
			Container<Str> split(const Str& substring)const{
				Container<Str> list;
				if(substring.length==0){
					//if(this->length==0) list.emplace_back("");else //asi funcionaria como el split de java
					for(size_type i=0;i<this->length;i++){
						list.emplace_back((*this)[i]);
					}
				}
				else{
					size_type start=0,end=0;
					while(start<=this->length){
						end=this->find(substring,start).value_or(this->length);
						list.append(this->substr(start,end));
						start=end+substring.length;
					}					
				}
				return list;
			}
			size_type maxsize()const{
				return this->maxLen;
			}
			size_type size()const{
				return this->length;
			}
			bool empty()const{
				return !this->length;
			}
			bool endswith(const Str& substring)const{
				if(this->length>=substring.length)
					return !al::memcomp<AlTy>(this->c_str()+this->length-substring.length,substring.c_str(),substring.length);
				else return 0;
			}
			bool endswith(const_pointer substring)const{
				size_type substr_size=al::strsize<AlTy>(substring);
				if(this->length>=substr_size)
					return !al::memcomp<AlTy>(this->c_str()+this->length-substr_size,substring,substr_size);
				else return 0;
			}
			bool startswith(const Str& substring)const{
				if(this->length>=substring.length)
					return !al::memcomp<AlTy>(this->c_str(),substring.c_str(),substring.length);
				else return 0;
			}
			bool startswith(const_pointer substring)const{
				size_type substr_size=al::strsize<AlTy>(substring);
				if(this->length>=substr_size)
					return !al::memcomp<AlTy>(this->c_str(),substring,substr_size);
				else return 0;
			}
			bool continueswith(const Str& substring,size_type start)const{
				if(this->length-start>=substring.length)
					return !al::memcomp<AlTy>(this->c_str()+start,substring.c_str(),substring.length);
				else return 0;
			}
			bool continueswith(const_pointer substring,size_type start)const{
				size_type substr_size=al::strsize<AlTy>(substring);
				if(this->length-start>=substr_size)
					return !al::memcomp<AlTy>(this->c_str()+start,substring,substr_size);
				else return 0;
			}
			void reverse(){
				al::reverse<AlTy>(c_str(),length);
			}
			template<typename Uint>
			enable_if_t<is_integral_v<Uint> && !is_bool_v<Uint> && is_unsigned_v<Uint>,Uint> parse(){
				const_pointer array=c_str();
				Uint num=0;
				size_type i=0;
				while(array[i]==' ' || array[i]=='\n' || array[i]=='\r' || array[i]=='\t')i++;
				for(;array[i]>='0' && array[i]<='9';i++){
					num*=10;
					num+=array[i]-48;// 'n'- 48 == n
				}
				return num;
			}
			template<typename Int>
			enable_if_t<is_integral_v<Int> && is_signed_v<Int>,Int> parse(){
				const_pointer array=c_str();
				Int num=0;
				size_type i=0;
				bool neg=false;
				while(array[i]==' ' || array[i]=='\n' || array[i]=='\r' || array[i]=='\t')i++;
				if(array[i]=='-'){
					neg=true;
					i++;
				}else if(array[i]=='+')i++;
				for(;array[i]>='0' && array[i]<='9';i++){
					num*=10;
					num+=array[i]-48;
				}
				return neg ? -num : num;
			}
			template<typename Float>
			enable_if_t<is_floating_point_v<Float>,Float> parse(){
				const_pointer array=c_str();
				Float num=0;
				size_type i=0;
				bool neg=false;
				while(array[i]==' ' || array[i]=='\n' || array[i]=='\r' || array[i]=='\t')i++;
				if(array[i]=='-'){
					neg=true;
					i++;
				}else if(array[i]=='+')i++;
				for(;array[i]>='0' && array[i]<='9';i++){
					num*=10;
					num+=array[i]-48;
				}
				if(array[i]=='.'){i++;
					for(Float div=10;array[i]>='0' && array[i]<='9';i++,div*=10){
						num+=((array[i]-48)/div);
					}		
				}
				if(array[i]=='e' || array[i]=='E'){i++;
					Float e=10;
					unsigned int e_num=0;
					if(array[i]=='-'){
						e=0.1;
						i++;
					}else if(array[i]=='+')i++;
					for(;array[i]>='0' && array[i]<='9';i++){
						e_num*=10;
						e_num+=array[i]-48;
					}
					for(unsigned int j=0;j<e_num;j++){
						num*=e;
					}
				}
				return neg ? -num : num;
			}
			void clear(){
				length=0;
				c_str()[0]=T();
			}
			void free(){
				if(maxLen!=0) AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
				maxLen=0;
				text.heap=nullptr;
				length=0;
			}
			~Str(){
				if(maxLen!=0) AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
			}
			iterator begin(){
				return iterator(&(this->c_str()[0]));
			}
			iterator end(){
				return iterator(&(this->c_str()[length]));
			}
			iterator rbegin(){
				return iterator(&(this->c_str()[length-1]));
			}
			iterator rend(){
				return iterator(&(this->c_str()[-1]));
			}
			const_iterator begin()const{
				return const_iterator(&(this->c_str()[0]));
			}
			const_iterator end()const{
				return const_iterator(&(this->c_str()[length]));
			}
			const_iterator rbegin()const{
				return const_iterator(&(this->c_str()[length-1]));
			}
			const_iterator rend()const{
				return const_iterator(&(this->c_str()[-1]));
			}
			template<typename t,typename a,typename p> friend Str<t,a,p> operator+(typename Str<t,a,p>::const_pointer,const Str<t,a,p>&);
			template<typename t,typename a,typename p> friend Str<t,a,p> operator+(typename Str<t,a,p>::value_type,const Str<t,a,p>&);

			template<typename In>
			void read(In& stream,size_type length){
				this->clear();
				this->length=length;
				pointer text;
				if(this->maxLen==0){
					if(this->length<ss_cap)
						text=this->text.buffer;
					else{
						this->maxLen=this->length*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
						text=this->text.heap;
					}
				}
				else{
					if(this->length<=this->maxLen)
						text=this->text.heap;
					else{
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=this->length*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
						text=this->text.heap;
					}
				}
				stream.read(text,this->length);
				text[this->length]=T();
			}

			template<typename In>
			void read(In& stream){
				this->clear();
				size_type current_pos=stream.tellg();
				stream.seekg(0,In::ios_base::end);
				this->length=(size_type)stream.tellg()-current_pos;
				stream.seekg(current_pos);
				pointer text;
				if(this->maxLen==0){
					if(this->length<ss_cap)
						text=this->text.buffer;
					else{
						this->maxLen=this->length*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
						text=this->text.heap;
					}
				}
				else{
					if(this->length<=this->maxLen)
						text=this->text.heap;
					else{
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxLen+1);
						this->maxLen=this->length*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxLen+1);
						text=this->text.heap;
					}
				}
				stream.read(text,this->length);
				text[this->length]=T();
			}
			template<typename In>
			void readtext(In& stream){
				this->clear();
				T c;
				while(stream.get(c))
					*this+=c;
			}

	};
	template<typename T,typename Alloc,typename params>
	Str<T,Alloc,params> operator+(typename Str<T,Alloc,params>::const_pointer Cstr,const Str<T,Alloc,params>& str){
		using AlTy=typename Str<T,Alloc,params>::AlTy;
		auto Cstr_size=al::strsize<AlTy>(Cstr);
		Str<T,Alloc,params> new_str(Cstr_size+str.length);
		al::memcopy<AlTy>(new_str.c_str(),Cstr,Cstr_size);
		al::memcopy<AlTy>(new_str.c_str()+Cstr_size,str.c_str(),str.length);
		return new_str;
	}
	template<typename T,typename Alloc,typename params>
	Str<T,Alloc,params> operator+(typename Str<T,Alloc,params>::value_type chr,const Str<T,Alloc,params>& str){
		using AlTy=typename Str<T,Alloc,params>::AlTy;
		Str<T,Alloc,params> new_str(1+str.length);
		new_str[0]=chr;
		al::memcopy<AlTy>(new_str.c_str()+1,str.c_str(),str.length);
		return new_str;
	}
	template<typename Out,typename T,typename Alloc,typename params>
	Out& operator<<(Out& stream,const Str<T,Alloc,params>& string){
		typename Str<T,Alloc,params>::const_pointer c_string=string.c_str();
		//for(typename Str<T,Alloc,params>::size_type i=0;i<string.size();i++) stream.put(c_string[i]);
		stream.write(c_string,string.size());
		return stream;
	}
	#ifndef STR_ENDCHAR
	#define STR_ENDCHAR '\n'
	#endif
	template<typename In,typename T,typename Alloc,typename params,T endchar=STR_ENDCHAR>
	In& operator>>(In& stream,Str<T,Alloc,params>& string){
		string.clear();
		T c;
		while(stream.get(c)){
			if(c==endchar)
				break;
			string+=c;
		}
		return stream;
	}
	using String=Str<char>;
	using WString=Str<wchar_t>;
}