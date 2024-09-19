#pragma once
#include"../arrays.hpp"
namespace mpv{
	template<typename Alloc>
	inline typename allocator_traits<Alloc>::size_type strsize(typename allocator_traits<Alloc>::const_pointer str){
		typename allocator_traits<Alloc>::size_type i=0;
		while(str[i]){
			++i;
		}	
		return i;
	}
	template<typename Alloc>
	inline void strcopy(typename allocator_traits<Alloc>::pointer dest,typename allocator_traits<Alloc>::const_pointer source){
		typename allocator_traits<Alloc>::size_type i=0;
		while(source[i]){
			dest[i]=source[i];
			i++;
		}
		dest[i]=typename allocator_traits<Alloc>::value_type();
	}
	template<typename Alloc>
	inline signed char strcomp(typename allocator_traits<Alloc>::const_pointer str1,typename allocator_traits<Alloc>::const_pointer str2){
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
		static_assert(ss_cap>=sizeof(pointer) and M>1,"ss_cap must be >=sizeof(pointer) and M must be >1");
		union sbo{
			value_type buffer[ss_cap];
			pointer heap;
		};
		private:
		/*size no puede ser >= a ss_cap por sin tener que asignar memoria, una vez que se esta usando memoria dinamica
		size debe se siempre <= a maxSize (puede llegar a ser igual a maxSize porque maxSize no cuenta el \0
											como un lugar, en cambio ss_cap si) */
			size_type size;// Es el tamaanio del string, sin tener en cuenta el '\0'
			size_type maxSize;/* Es el valor maximo al que puede llegar size antes de tener que reasignar memoria. 
		Cuando se necesita reasignar, el espacio que se asigna es de maxSize+1(para el \0) ya que maxSize y size(que no tiene en cuenta el \0)
		deben(por conveniencia) poder coincidir en un mismo numero sin tener que reasignar.
		Si maxSize es 0, significa que se esta usando el buffer para almacenar el string. Si maxSize>0 el buffer se usa para almacenar un puntero al string en memoria dinamica.*/
			sbo text;
			inline void alloc_size(){
				if(size>=ss_cap){
					maxSize=size*M;
					text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
					text.heap[size]=T();
				}
				else text.buffer[size]=T();
			}
		public:
			pointer get_stack(){
				return this->text.buffer;
			}
			pointer get_heap(){
				return this->text.heap;
			}
			const_pointer c_str()const{	//devuelve un puntero a la memoria que se esta usando actualmente en la union. NO se puede llamar si se modifico maxSize.
				if(maxSize==0)
					return this->text.buffer;	
				else
					return this->text.heap;
			}
			pointer c_str(){	//devuelve un puntero a la memoria que se esta usando actualmente en la union. NO se puede llamar si se modifico maxSize.
				if(maxSize==0)
					return this->text.buffer;	
				else
					return this->text.heap;
			}
			Str():size(0),maxSize(0),text{}{INCSTRINGS}
			Str(const Alloc& al):EBCO<AlTy>(al){INCSTRINGS}
			explicit Str(size_type sz):size(sz),maxSize(0){
				if(sz>=ss_cap){
					maxSize=size*M;
					text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
					text.heap[size]=T();
				}
				else text.buffer[size]=T();
				INCSTRINGS
			}
			explicit Str(size_type sz,const Alloc& al):EBCO<AlTy>(al),size(sz),maxSize(0){
				if(sz>=ss_cap){
					maxSize=size*M;
					text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
					text.heap[size]=T();
				}
				else text.buffer[size]=T();
				INCSTRINGS
			}
			Str(const Str& other):EBCO<AlTy>(AlTy_traits::select_on_container_copy_construction(other.get_val())),size(other.size),maxSize(0){
				if(other.size<ss_cap){
					al::memcopy<AlTy>(this->text.buffer,other.c_str(),other.size+1);
				}
				else{
					this->maxSize=this->size*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
					al::memcopy<AlTy>(this->text.heap,other.c_str(),other.size+1);
				}
				INCSTRINGS
			}
			Str(const Str& other,const Alloc& al):EBCO<AlTy>(al),size(other.size),maxSize(0){
				if(other.size<ss_cap){
					al::memcopy<AlTy>(this->text.buffer,other.c_str(),other.size+1);
				}
				else{
					this->maxSize=this->size*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
					al::memcopy<AlTy>(this->text.heap,other.c_str(),other.size+1);
				}
				INCSTRINGS
			}
			Str(const_pointer str):size(strsize<AlTy>(str)),maxSize(0){
				if(this->size<ss_cap){
					strcopy<AlTy>(this->text.buffer,str);
				}
				else{
					this->maxSize=this->size*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
					strcopy<AlTy>(this->text.heap,str);
				}
				INCSTRINGS
			}
			Str(const_pointer str,const Alloc& al):EBCO<AlTy>(al),size(strsize<AlTy>(str)),maxSize(0){
				if(this->size<ss_cap){
					strcopy<AlTy>(this->text.buffer,str);
				}
				else{
					this->maxSize=this->size*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
					strcopy<AlTy>(this->text.heap,str);
				}
				INCSTRINGS
			}
			Str(const_pointer str,size_type n):size(n),maxSize(0){
				if(this->size<ss_cap){
					al::memcopy<AlTy>(this->text.buffer,str,n);
					this->text.buffer[size]=T();
				}
				else{
					this->maxSize=this->size*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
					al::memcopy<AlTy>(this->text.heap,str,n);
					this->text.heap[size]=T();
				}
				INCSTRINGS
			}
			Str(const_pointer str,size_type n,const Alloc& al):EBCO<AlTy>(al),size(n),maxSize(0){
				if(this->size<ss_cap){
					al::memcopy<AlTy>(this->text.buffer,str,n);
					this->text.buffer[size]=T();
				}
				else{
					this->maxSize=this->size*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
					al::memcopy<AlTy>(this->text.heap,str,n);
					this->text.heap[size]=T();
				}
				INCSTRINGS
			}
			explicit Str(T chr):size(1),maxSize(0){
				this->text.buffer[0]=chr;
				this->text.buffer[1]=T();
				INCSTRINGS
			}
			Str(T chr,size_type n):size(n),maxSize(0){
				if(this->size<ss_cap){
					al::memfill<AlTy>(this->text.buffer,chr,n);
					this->text.buffer[size]=T();
				}
				else{
					this->maxSize=this->size*M;
					this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
					al::memfill<AlTy>(this->text.heap,chr,n);
					this->text.heap[size]=T();
				}
				INCSTRINGS
			}
			Str(Str&& other):EBCO<AlTy>(static_cast<AlTy&&>(other.get_val())),size(other.size),maxSize(other.maxSize){
				if(other.maxSize==0)
					al::memcopy<AlTy>(this->text.buffer,other.text.buffer,other.size+1);
				else
					this->text.heap=other.text.heap;
				other.text.heap=nullptr;
				other.size=0;
				other.maxSize=0;
				INCSTRINGS
			}
			Str(Str&& other,const Alloc& al):EBCO<AlTy>(al),size(other.size),maxSize(other.maxSize){
				if(other.maxSize==0)
					al::memcopy<AlTy>(this->text.buffer,other.text.buffer,other.size+1);
				else{
					if(this->get_val()==other.get_val())
						this->text.heap=other.text.heap;
					else{
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
						al::memcopy<AlTy>(this->text.heap,other.text.heap,other.size+1);
						AlTy_traits::deallocate(this->get_val(),other.text.heap,other.maxSize+1);
					}
				}
				other.text.heap=nullptr;
				other.size=0;
				other.maxSize=0;
				INCSTRINGS
			}
#define COPY_ALLOC	if constexpr(POCCA and not ALWAYS_EQ){\
						if(this->get_val()!=other.get_val())\
							this->get_val()=other.get_val();}
			Str& operator=(const Str& other){
				if(this==&other) return *this;
				if(this->maxSize==0){
					if(other.size<ss_cap){
						COPY_ALLOC
						al::memcopy<AlTy>(this->text.buffer,other.c_str(),other.size+1);// +1 para copiar el \0 tambien
					}
					else{
						COPY_ALLOC
						this->maxSize=other.size*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
						al::memcopy<AlTy>(this->text.heap,other.c_str(),other.size+1);
					}
				}
				else{
					if(other.size<=this->maxSize){
						if constexpr(POCCA and not ALWAYS_EQ){
							if(this->get_val()!=other.get_val()){
								AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
								this->get_val()=other.get_val();
								this->maxSize=other.maxSize;
								this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
							}
						}
						al::memcopy<AlTy>(this->text.heap,other.c_str(),other.size+1);
					}
					else{
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						COPY_ALLOC
						this->maxSize=other.size*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
						al::memcopy<AlTy>(this->text.heap,other.c_str(),other.size+1);
					}
				}
				this->size=other.size;
				return *this;
			}
			Str& operator=(const_pointer str){
				this->size=strsize<AlTy>(str);
				if(this->maxSize==0){
					if(this->size<ss_cap)
						strcopy<AlTy>(this->text.buffer,str);
					else{
						this->maxSize=this->size*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
						strcopy<AlTy>(this->text.heap,str);
					}
				}
				else{
					if(this->size<=this->maxSize)
						strcopy<AlTy>(this->text.heap,str);
					else{
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						this->maxSize=this->size*M;
						this->text.heap=AlTy_traits::allocate(this->get_val(),maxSize+1);
						strcopy<AlTy>(this->text.heap,str);
					}
				}
				return *this;
			}
			Str& operator=(T chr){
				this->size=1;
				if(this->maxSize==0){
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
				if constexpr(not ALWAYS_EQ and not POCMA){
					if(this->get_val()!=other.get_val()){
						if(this->maxSize==0){
							if(other.maxSize==0)
								al::memcopy<AlTy>(this->text.buffer,other.text.buffer,other.size+1);
							else{
								this->maxSize=other.maxSize;
								this->text.heap=AlTy_traits::allocate(this->get_val(),this->text.heap,maxSize+1);
								al::memcopy<AlTy>(this->text.heap,other.text.heap,other.size+1);
								AlTy_traits::deallocate(other.get_val(),other.text.heap,other.maxSize+1);
								other.maxSize=0;
							}
						}
						else{
							if(other.maxSize==0)
								al::memcopy<AlTy>(this->text.heap,other.text.buffer,other.size+1);
							else{
								AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
								this->maxSize=other.maxSize;
								this->text.heap=AlTy_traits::allocate(this->get_val(),this->text.heap,maxSize+1);
								al::memcopy<AlTy>(this->text.heap,other.text.heap,other.size+1);
								AlTy_traits::deallocate(other.get_val(),other.text.heap,other.maxSize+1);
								other.maxSize=0;
							}
						}
						this->size=other.size;
						other.size=0;
						other.text.heap=nullptr;
						return *this;						
					}
				}
				if(this->maxSize==0){
					if(other.maxSize==0)
						al::memcopy<AlTy>(this->text.buffer,other.text.buffer,other.size+1);
					else{
						this->maxSize=other.maxSize;
						this->text.heap=other.text.heap;
						other.maxSize=0;
					}
				}
				else{
					if(other.maxSize==0)
						al::memcopy<AlTy>(this->text.heap,other.text.buffer,other.size+1);
					else{
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						this->maxSize=other.maxSize;
						this->text.heap=other.text.heap;
						other.maxSize=0;
					}
				}
				if constexpr(POCMA) this->get_val()=static_cast<AlTy&&>(other.get_val());
				this->size=other.size;
				other.size=0;
				other.text.heap=nullptr;
				return *this;
			}
			Str operator+(const Str& other)const&{
				Str new_string(this->size+other.size);//Constructor que asigna un size(ya tiene en cuenta el espacio extra para el \0, al cual asigna \0) y reserva la memoria necesaria
				al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
				al::memcopy<AlTy>(new_string.c_str()+this->size,other.c_str(),other.size);
				return new_string;
			}

			Str operator+(const_pointer str)const&{
				Str new_string(this->size+strsize<AlTy>(str));
				al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
				strcopy<AlTy>(new_string.c_str()+this->size,str);
				return new_string;
			}
			Str operator+(T chr)const&{
				Str new_string(this->size+1);
				al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
				new_string.c_str()[this->size]=chr;
				return new_string;
			}
			Str operator+(Str&& other)const&{
				Str new_string;//El \0 no se asigna solo como en las funciones anteriores que usan Str(size_type)
				new_string.size=this->size+other.size;
				if(new_string.size==0) return new_string;
				if constexpr(not ALWAYS_EQ and not POCMA){
					if(this->get_val()!=new_string.get_val()){
						new_string.alloc_size();//Asigna la memoria necesaria y pone el \0 al final
						al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
						al::memcopy<AlTy>(new_string.c_str()+this->size,other.c_str(),other.size);
						return new_string;
					}
				}
				if(new_string.size<=other.maxSize){
					if constexpr(POCMA) new_string.get_val()=static_cast<AlTy&&>(other.get_val());
					new_string.text.heap=other.text.heap;
					al::reversecopy<AlTy>(new_string.text.heap+this->size,other.text.heap,other.size+1);
					al::memcopy<AlTy>(new_string.text.heap,this->c_str(),this->size);
					new_string.maxSize=other.maxSize;
					other.size=0;
					other.maxSize=0;
					other.text.heap=nullptr;
				}
				else{
					new_string.alloc_size();
					al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
					al::memcopy<AlTy>(new_string.c_str()+this->size,other.c_str(),other.size);
				}
				return new_string;
			}
			Str operator+(const Str& other)&&{
				Str new_string;
				new_string.size=this->size+other.size;
				if(new_string.size==0) return new_string;
				if constexpr(not ALWAYS_EQ and not POCMA){
					if(this->get_val()!=new_string.get_val()){
						new_string.alloc_size();
						al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
						al::memcopy<AlTy>(new_string.c_str()+this->size,other.c_str(),other.size);
						return new_string;
					}
				}
				if(new_string.size<=this->maxSize){
					if constexpr(POCMA) new_string.get_val()=static_cast<AlTy&&>(this->get_val());
					new_string.text.heap=this->text.heap;
					al::memcopy<AlTy>(new_string.text.heap+this->size,other.c_str(),other.size+1);
					new_string.maxSize=this->maxSize;
					this->size=0;
					this->maxSize=0;
					this->text.heap=nullptr;
				}
				else{
					new_string.alloc_size();
					al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
					al::memcopy<AlTy>(new_string.c_str()+this->size,other.c_str(),other.size);
				}
				return new_string;
			}
			Str operator+(const_pointer str)&&{
				size_type str_size=strsize<AlTy>(str);
				Str new_string;
				new_string.size=this->size+str_size;
				if(new_string.size==0) return new_string;
				if constexpr(not ALWAYS_EQ and not POCMA){
					if(this->get_val()!=new_string.get_val()){
						new_string.alloc_size();
						al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
						strcopy<AlTy>(new_string.c_str()+this->size,str);
						return new_string;						
					}
				}
				if(new_string.size<=this->maxSize){
					if constexpr(POCMA) new_string.get_val()=static_cast<AlTy&&>(this->get_val());
					new_string.text.heap=this->text.heap;
					strcopy<AlTy>(new_string.text.heap+this->size,str);
					new_string.maxSize=this->maxSize;
					this->size=0;
					this->maxSize=0;
					this->text.heap=nullptr;
				}
				else{
					new_string.alloc_size();
					al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
					strcopy<AlTy>(new_string.c_str()+this->size,str);
				}
				return new_string;
			}
			Str operator+(T chr)&&{
				Str new_string;
				new_string.size=this->size+1;
				if constexpr(not ALWAYS_EQ and not POCMA){
					if(this->get_val()!=new_string.get_val()){
						new_string.alloc_size();
						al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
						new_string.c_str()[this->size]=chr;
						return new_string;						
					}
				}
				if(new_string.size<=this->maxSize){
					if constexpr(POCMA) new_string.get_val()=static_cast<AlTy&&>(this->get_val());
					new_string.text.heap=this->text.heap;
					new_string.text.heap[this->size]=chr;
					new_string.text.heap[new_string.size]=T();
					new_string.maxSize=this->maxSize;
					this->size=0;
					this->maxSize=0;
					this->text.heap=nullptr;
				}
				else{
					new_string.alloc_size();
					al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
					new_string.c_str()[this->size]=chr;
				}
				return new_string;
			}
			Str operator+(Str&& other)&&{
				Str new_string;
				new_string.size=this->size+other.size;
				if(new_string.size==0) return new_string;
				if((POCMA || new_string.get_val()==this->get_val()) && new_string.size<=this->maxSize){
					if constexpr(POCMA)	new_string.get_val()=static_cast<AlTy&&>(this->get_val());
					new_string.text.heap=this->text.heap;
					al::memcopy<AlTy>(new_string.text.heap+this->size,other.c_str(),other.size+1);
					new_string.maxSize=this->maxSize;
					this->size=0;
					this->maxSize=0;
					this->text.heap=nullptr;
				}
				else if((POCMA || new_string.get_val()==other.get_val()) && new_string.size<=other.maxSize){
					if constexpr(POCMA)	new_string.get_val()=static_cast<AlTy&&>(other.get_val());
					new_string.text.heap=other.text.heap;
					al::reversecopy<AlTy>(new_string.text.heap+this->size,other.text.heap,other.size+1);
					al::memcopy<AlTy>(new_string.text.heap,this->c_str(),this->size);
					new_string.maxSize=other.maxSize;
					other.size=0;
					other.maxSize=0;
					other.text.heap=nullptr;
				}
				else{
					new_string.alloc_size();
					al::memcopy<AlTy>(new_string.c_str(),this->c_str(),this->size);
					al::memcopy<AlTy>(new_string.c_str()+this->size,other.c_str(),other.size);
				}
				return new_string;
			}
			Str& operator+=(const Str& other){
				size_type new_size=this->size+other.size;
				if(this->maxSize==0){
					if(new_size<ss_cap)
						al::reversecopy<AlTy>(this->text.buffer+this->size,other.c_str(),other.size+1);//reversecopy porque this y other podrian ser el mismo objeto
					else{
						this->maxSize=new_size*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxSize+1);
						al::memcopy<AlTy>(new_text,this->text.buffer,this->size);
						al::memcopy<AlTy>(new_text+this->size,other.c_str(),other.size+1);
						this->text.heap=new_text;
					}
				}
				else{
					if(new_size<=this->maxSize)
						al::reversecopy<AlTy>(this->text.heap+this->size,other.c_str(),other.size+1);
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_size*M+1);
						al::memcopy<AlTy>(new_text,this->text.heap,this->size);
						al::memcopy<AlTy>(new_text+this->size,other.c_str(),other.size+1);
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						this->maxSize=new_size*M;
						this->text.heap=new_text;
					}
				}
				this->size=new_size;
				return *this;
			}
			Str& operator+=(Str&& other){//si this y other son el mismo objeto es comportamiento indeinido
				size_type new_size=this->size+other.size;
				if(this->maxSize==0){
					if(new_size<ss_cap)
						al::memcopy<AlTy>(this->text.buffer+this->size,other.c_str(),other.size+1);
					else if((POCMA || this->get_val()==other.get_val()) && new_size<=other.maxSize){
						if constexpr(POCMA) this->get_val()=static_cast<AlTy&&>(other.get_val());
						al::reversecopy<AlTy>(other.text.heap+this->size,other.text.heap,other.size+1);
						al::memcopy<AlTy>(other.text.heap,this->text.buffer,this->size);
						this->text.heap=other.text.heap;
						this->maxSize=other.maxSize;
						other.size=0;
						other.maxSize=0;
						other.text.heap=nullptr;
					}
					else{
						this->maxSize=new_size*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxSize+1);
						al::memcopy<AlTy>(new_text,this->text.buffer,this->size);
						al::memcopy<AlTy>(new_text+this->size,other.c_str(),other.size+1);
						this->text.heap=new_text;
					}
				}
				else{
					if(new_size<=this->maxSize)
						al::memcopy<AlTy>(this->text.heap+this->size,other.c_str(),other.size+1);
					else if((POCMA || this->get_val()==other.get_val()) && new_size<=other.maxSize){
						if constexpr(POCMA) this->get_val()=static_cast<AlTy&&>(other.get_val());
						al::reversecopy<AlTy>(other.text.heap+this->size,other.text.heap,other.size+1);
						al::memcopy<AlTy>(other.text.heap,this->text.heap,this->size);
						this->text.heap=other.text.heap;
						this->maxSize=other.maxSize;
						other.size=0;
						other.maxSize=0;
						other.text.heap=nullptr;
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_size*M+1);
						al::memcopy<AlTy>(new_text,this->text.heap,this->size);
						al::memcopy<AlTy>(new_text+this->size,other.c_str(),other.size+1);
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						this->maxSize=new_size*M;
						this->text.heap=new_text;
					}
				}
				this->size=new_size;
				return *this;
			}
			Str& operator+=(const_pointer str){
				size_type str_size=strsize<AlTy>(str);
				size_type new_size=this->size+str_size;
				if(this->maxSize==0){
					if(new_size<ss_cap)
						al::reversecopy<AlTy>(this->text.buffer+this->size,str,str_size+1);//reversecopy porq this->c_str() puede ser str
					else{
						this->maxSize=new_size*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxSize+1);
						al::memcopy<AlTy>(new_text,this->text.buffer,this->size);
						strcopy<AlTy>(new_text+this->size,str);
						this->text.heap=new_text;
					}
				}
				else{
					if(new_size<=this->maxSize)
						al::reversecopy<AlTy>(this->text.heap+this->size,str,str_size+1);
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_size*M+1);
						al::memcopy<AlTy>(new_text,this->text.heap,this->size);
						strcopy<AlTy>(new_text+this->size,str);
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						this->maxSize=new_size*M;
						this->text.heap=new_text;
					}
				}
				this->size=new_size;
				return *this;
			}
			Str& operator+=(T chr){
				size_type new_size=this->size+1;
				if(this->maxSize==0){
					if(new_size<ss_cap){
						this->text.buffer[this->size]=chr;
						this->text.buffer[new_size]=T();
					}
					else{
						this->maxSize=new_size*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxSize+1);
						al::memcopy<AlTy>(new_text,this->text.buffer,this->size);
						new_text[this->size]=chr;
						new_text[new_size]=T();
						this->text.heap=new_text;
					}
				}
				else{
					if(new_size<=this->maxSize){
						this->text.heap[this->size]=chr;
						this->text.heap[new_size]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_size*M+1);
						al::memcopy<AlTy>(new_text,this->text.heap,this->size);
						new_text[this->size]=chr;
						new_text[new_size]=T();
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						this->maxSize=new_size*M;
						this->text.heap=new_text;
					}
				}
				this->size=new_size;
				return *this;
			}
			Str operator*(unsigned int num)const{
				Str new_string(this->size*num);
				pointer new_text=new_string.c_str();
				const_pointer this_text=this->c_str();
				for(size_type index=0;index<new_string.size;index+=this->size)
					al::memcopy<AlTy>(new_text+index,this_text,this->size);
				return new_string;
			}
			Str& operator*=(unsigned int num){
				size_type new_size=this->size*num;
				if(this->maxSize==0){
					if(new_size<ss_cap){
						for(size_type index=this->size;index<new_size;index+=this->size)
							al::memcopy<AlTy>(this->text.buffer+index,this->text.buffer,this->size);
						this->text.buffer[new_size]=T();
					}
					else{
						this->maxSize=new_size*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxSize+1);
						for(size_type index=0;index<new_size;index+=this->size)
							al::memcopy<AlTy>(new_text+index,this->text.buffer,this->size);
						new_text[new_size]=T();
						this->text.heap=new_text;
					}
				}
				else{
					if(new_size<=this->maxSize){
						for(size_type index=this->size;index<new_size;index+=this->size)
							al::memcopy<AlTy>(this->text.heap+index,this->text.heap,this->size);
						this->text.heap[new_size]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_size*M+1);
						for(size_type index=0;index<new_size;index+=this->size)
							al::memcopy<AlTy>(new_text+index,this->text.heap,this->size);
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						this->maxSize=new_size*M;
						new_text[new_size]=T();
						this->text.heap=new_text;
					}
				}
				this->size=new_size;
				return *this;
			}
			void operator%=(Str& other){
				if constexpr(not ALWAYS_EQ and not POCS){
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
				size_type size_aux=this->size;
				size_type maxSize_aux=this->maxSize;
				this->text=other.text;
				this->size=other.size;
				this->maxSize=other.maxSize;
				other.text=string_aux;
				other.size=size_aux;
				other.maxSize=maxSize_aux;
			}
			signed char cmp(const Str& other)const{
				if(this->size>other.size) return 2;
				else if(this->size<other.size) return -2;
				else{
					const_pointer a=this->c_str();
					const_pointer b=other.c_str();
					for(size_type i=0;i<this->size;i++){
						if(a[i]>b[i]) return 1;
						else if(a[i]<b[i]) return -1;
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
				size_type new_size=this->size+other.size;
				if(this->maxSize==0){
					if(new_size<ss_cap && this!=&other){
						pointer insert_pos=this->text.buffer+index;
						al::reversecopy<AlTy>(insert_pos+other.size,insert_pos,this->size-index);
						al::memcopy<AlTy>(insert_pos,other.c_str(),other.size);
						this->text.buffer[new_size]=T();
					}
					else{
						this->maxSize=new_size*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxSize+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.buffer,index);
						al::memcopy<AlTy>(insert_pos,other.c_str(),other.size);
						al::memcopy<AlTy>(insert_pos+other.size,this->text.buffer+index,this->size-index);
						new_text[new_size]=T();
						this->text.heap=new_text;
					}
				}
				else{
					if(new_size<=this->maxSize && this!=&other){
						pointer insert_pos=this->text.heap+index;
						al::reversecopy<AlTy>(insert_pos+other.size,insert_pos,this->size-index);
						al::memcopy<AlTy>(insert_pos,other.c_str(),other.size);
						this->text.heap[new_size]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_size*M+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.heap,index);
						al::memcopy<AlTy>(insert_pos,other.c_str(),other.size);
						al::memcopy<AlTy>(insert_pos+other.size,this->text.heap+index,this->size-index);
						new_text[new_size]=T();
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						this->maxSize=new_size*M;
						this->text.heap=new_text;
					}
				}
				this->size=new_size;
			}
			void insert(size_type index,const_pointer str){
				size_type str_size=strsize<AlTy>(str);
				size_type new_size=this->size+str_size;
				if(this->maxSize==0){
					if(new_size<ss_cap && this->text.buffer!=str){
						pointer insert_pos=this->text.buffer+index;
						al::reversecopy<AlTy>(insert_pos+str_size,insert_pos,this->size-index);
						al::memcopy<AlTy>(insert_pos,str,str_size);
						this->text.buffer[new_size]=T();
					}
					else{
						this->maxSize=new_size*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxSize+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.buffer,index);
						al::memcopy<AlTy>(insert_pos,str,str_size);
						al::memcopy<AlTy>(insert_pos+str_size,this->text.buffer+index,this->size-index);
						new_text[new_size]=T();
						this->text.heap=new_text;
					}
				}
				else{
					if(new_size<=this->maxSize && this->text.heap!=str){
						pointer insert_pos=this->text.heap+index;
						al::reversecopy<AlTy>(insert_pos+str_size,insert_pos,this->size-index);
						al::memcopy<AlTy>(insert_pos,str,str_size);
						this->text.heap[new_size]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_size*M+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.heap,index);
						al::memcopy<AlTy>(insert_pos,str,str_size);
						al::memcopy<AlTy>(insert_pos+str_size,this->text.heap+index,this->size-index);
						new_text[new_size]=T();
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						this->maxSize=new_size*M;
						this->text.heap=new_text;
					}
				}
				this->size=new_size;
			}
			void insert(size_type index,T chr){
				size_type new_size=this->size+1;
				if(this->maxSize==0){
					if(new_size<ss_cap){
						pointer insert_pos=this->text.buffer+index;
						al::reversecopy<AlTy>(insert_pos+1,insert_pos,this->size-index);
						*insert_pos=chr;
						this->text.buffer[new_size]=T();
					}
					else{
						this->maxSize=new_size*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxSize+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.buffer,index);
						*insert_pos=chr;
						al::memcopy<AlTy>(insert_pos+1,this->text.buffer+index,this->size-index);
						new_text[new_size]=T();
					}
				}
				else{
					if(new_size<=this->maxSize){
						pointer insert_pos=this->text.heap+index;
						al::reversecopy<AlTy>(insert_pos+1,insert_pos,this->size-index);
						*insert_pos=chr;
						this->text.heap[new_size]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_size*M+1);
						pointer insert_pos=new_text+index;
						al::memcopy<AlTy>(new_text,this->text.heap,index);
						*insert_pos=chr;
						al::memcopy<AlTy>(insert_pos+1,this->text.heap+index,this->size-index);
						new_text[new_size]=T();
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						this->maxSize=new_size*M;
						this->text.heap=new_text;
					}
				}
				this->size=new_size;
			}
			void del_back(){
				this->c_str()[--size]=T();
			}
			T pop_back(){
				T chr=this->c_str()[size-1];
				this->c_str()[--size]=T();
				return chr;
			}
			void erase(size_type start,size_type end){
				size_type new_size=this->size-(end-start);
				pointer this_text=this->c_str();
				al::memcopy<AlTy>(this_text+start,this_text+end,this->size-end);
				this_text[new_size]=T();
				this->size=new_size;
			}
			void erase(size_type start){
				this->c_str()[start]=T();
				this->size=start;
			}
			void remove(const Str& substring,size_type start,size_type end){
				Optional<size_type> index=this->find(substring,start,end);
				if(index.has_value()){
					this->erase(index.value(),index.value()+substring.size);	
				}
			}
			void remove(const_pointer substring,size_type start,size_type end){
				Optional<size_type> index=this->find(substring,start,end);
				if(index.has_value()){
					this->erase(index.value(),index.value()+strsize<AlTy>(substring));	
				}
			}
			void rremove(const Str& substring,size_type start,size_type end){
				Optional<size_type> index=this->rfind(substring,start,end);
				if(index.has_value()){
					this->erase(index.value(),index.value()+substring.size);	
				}
			}
			void rremove(const_pointer substring,size_type start,size_type end){
				Optional<size_type> index=this->rfind(substring,start,end);
				if(index.has_value()){
					this->erase(index.value(),index.value()+strsize<AlTy>(substring));	
				}
			}
			void remove(const Str& substring,size_type start=0){
				Optional<size_type> index=this->find(substring,start);
				if(index.has_value()){
					this->erase(index.value(),index.value()+substring.size);	
				}
			}
			void remove(const_pointer substring,size_type start=0){
				Optional<size_type> index=this->find(substring,start);
				if(index.has_value()){
					this->erase(index.value(),index.value()+strsize<AlTy>(substring));	
				}
			}
			void rremove(const Str& substring,size_type start=0){
				Optional<size_type> index=this->rfind(substring,start);
				if(index.has_value()){
					this->erase(index.value(),index.value()+substring.size);	
				}
			}
			void rremove(const_pointer substring,size_type start=0){
				Optional<size_type> index=this->rfind(substring,start);
				if(index.has_value()){
					this->erase(index.value(),index.value()+strsize<AlTy>(substring));	
				}
			}
			void replace(const Str& substring1,const Str& substring2,Optional<size_type> max_count={}){
				size_type count=this->count(substring1);
				if(max_count.has_value() && count>max_count.value())
					count=max_count.value();
				if(count==0) return;
				using AlInt=rebind_alloc<AlTy,size_type>;
				AlInt uint_alloc(this->get_val());
				size_type* array=allocator_traits<AlInt>::allocate(uint_alloc,count);
				for(size_type i=0,last=0;i<count;last=array[i++]+substring1.size)
					array[i]=this->find(substring1,last).value();
				for(size_type i=0;i<count;i++)
					array[i]=array[i]+(substring2.size-substring1.size)*i;
				size_type new_size=this->size+(count*substring2.size-count*substring1.size);
				const_pointer substring2_text=substring2.c_str();
				if(this->maxSize==0){
					if(new_size<=this->size){
						for(size_type i=array[0],j=array[0],k=0;i<new_size;){
							if(k<count and i==array[k]){
								al::memcopy<AlTy>(this->text.buffer+array[k++],substring2_text,substring2.size);
								i+=substring2.size;
								j+=substring1.size;
							}
							else{
								this->text.buffer[i]=this->text.buffer[j];
								i++;
								j++;
							}
						}
						this->text.buffer[new_size]=T();
					}
					else{
						this->maxSize=new_size*M;
						pointer new_text=AlTy_traits::allocate(this->get_val(),maxSize+1);
						for(size_type i=0,j=0,k=0;i<new_size;){
							if(k<count and i==array[k]){
								al::memcopy<AlTy>(new_text+array[k++],substring2_text,substring2.size);
								i+=substring2.size;
								j+=substring1.size;
							}
							else{
								new_text[i]=this->text.buffer[j];
								i++;
								j++;
							}
						}
						new_text[new_size]=T();
						this->text.heap=new_text;
					}
				}
				else{
					if(new_size<=this->size){//aunque new_size<=maxSize si new_size<=size no se cumple se necesita reasignar memoria
						for(size_type i=array[0],j=array[0],k=0;i<new_size;){
							if(k<count and i==array[k]){
								al::memcopy<AlTy>(this->text.heap+array[k++],substring2_text,substring2.size);
								i+=substring2.size;
								j+=substring1.size;
							}
							else{
								this->text.heap[i]=this->text.heap[j];
								i++;
								j++;
							}
						}
						this->text.heap[new_size]=T();
					}
					else{
						pointer new_text=AlTy_traits::allocate(this->get_val(),new_size*M+1);
						for(size_type i=0,j=0,k=0;i<new_size;){
							if(k<count and i==array[k]){
								al::memcopy<AlTy>(new_text+array[k++],substring2_text,substring2.size);
								i+=substring2.size;
								j+=substring1.size;
							}
							else{
								new_text[i]=this->text.heap[j];
								i++;
								j++;
							}
						}
						new_text[new_size]=T();
						AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
						this->maxSize=new_size*M;
						this->text.heap=new_text;						
					}

				}
				this->size=new_size;
				allocator_traits<AlInt>::deallocate(uint_alloc,array,count);
			}
			Str substr(size_type start,size_type end)const{
				Str new_string(end-start);//el tamaño del nuevo string lo genera el constructor
				al::memcopy<AlTy>(new_string.c_str(),this->c_str()+start,new_string.size);
				return new_string;
			}
			Str substr(size_type start)const{
				Str new_string(size-start);
				al::memcopy<AlTy>(new_string.c_str(),this->c_str()+start,new_string.size);
				return new_string;
			}
			Str extract(size_type index,size_type pos)const{
				Str new_string(pos);
				al::memcopy<AlTy>(new_string.c_str(),this->c_str()+index,pos);
				return new_string;
			}
			Str extract(size_type index)const{
				Str new_string(size-index);
				al::memcopy<AlTy>(new_string.c_str(),this->c_str()+index,size-index);
				return new_string;
			}
			template<T SPACE=' '>
			void noExtraSpaces(){
				pointer this_text=this->c_str();
				size_type i=0,j=0;
				while(j<this->size){
					if(this_text[j]==SPACE && this_text[j+1]==SPACE){
						while(j<this->size && this_text[j]==this_text[j+1]){
							j++;
						}						
					}
					else{
						this_text[i++]=this_text[j++];
					}
				}
				this->size=i;
				this_text[i]=T();
			}
			template<T SPACE=' '>
			void lstrip(){
				pointer this_text=this->c_str();
				if(this_text[0]!=SPACE) return;// Para no tener que copiar el string completo si es que no hay espacios que borrar
				size_type count=0;
				while(count<this->size && this_text[count]==SPACE)
					count++;
				this->size-=count;
				al::memcopy<AlTy>(this_text,this_text+count,this->size+1);
			}
			template<T SPACE=' '>
			void rstrip(){
				pointer this_text=this->c_str();
				while(size>0 && this_text[size-1]==SPACE){
					size--;
				}
				this_text[size]=T();
			}
			template<T SPACE=' '>
			void strip(){
				rstrip<SPACE>();
				lstrip<SPACE>();
			}
			void upper(){
				if(this->size>=ss_cap){
					for(size_type i=0;i<size;i++)
						if(this->text.heap[i]>=97 and this->text.heap[i]<=122)
							this->text.heap[i]-=32;
				}
				else{
					for(size_type i=0;i<size;i++)
						if(this->text.buffer[i]>=97 and this->text.buffer[i]<=122)
							this->text.buffer[i]-=32;
				}
			}
			void lower(){
				if(this->size>=ss_cap){
					for(size_type i=0;i<size;i++)
						if(this->text.heap[i]>=65 and this->text.heap[i]<=90)
							this->text.heap[i]+=32;
				}
				else{
					for(size_type i=0;i<size;i++)
						if(this->text.buffer[i]>=65 and this->text.buffer[i]<=90)
							this->text.buffer[i]+=32;
				}
			}
			template<T SPACE=' '>
			Str noExtraSpaces_cpy()const{
				const_pointer this_text=this->c_str();
				size_type count=0;
				for(size_type i=0;i<this->size;i++)
					if(this_text[i]==SPACE and this_text[i+1]==SPACE)
						count++;
				Str new_string(this->size-count);
				pointer new_text=new_string.c_str();
				for(size_type i=0,j=0;i<this->size;i++){
					if(not(this_text[i]==SPACE and this_text[i+1]==SPACE)){
						new_text[j]=this_text[i];
						j++;
					}
				}
				return new_string;
			}
			template<T SPACE=' '>
 			Str lstrip_cpy()const{
				if(this->size==0) return Str();
				const_pointer this_text=this->c_str();
				size_type count=0;
				while(count<this->size && this_text[count]==SPACE)
					count++;
				Str new_string(this->size-count);
				pointer new_text=new_string.c_str();
				for(size_type i=0;count<this->size;i++,count++)
					new_text[i]=this_text[count];
				return new_string;
			}
			template<T SPACE=' '>
 			Str rstrip_cpy()const{
				const_pointer this_text=this->c_str();
				size_type count=this->size;
				while(count>0 && this_text[count-1]==SPACE)
					count--;
				Str new_string(count);
				pointer new_text=new_string.c_str();
				for(count=0;count<new_string.size;count++)
					new_text[count]=this_text[count];
				return new_string;
			}
			template<T SPACE=' '>
			Str strip_cpy()const{
				if(this->size==0) return Str();
				const_pointer this_text=this->c_str();
				size_type lcount=0,rcount=this->size;
				while(lcount<this->size && this_text[lcount]==SPACE)
					lcount++;
				if(lcount<this->size)//Por si el string solamente contiene espacios
					while(/* rcount>0 &&  */this_text[rcount-1]==SPACE)
						rcount--;					
				Str new_string(rcount-lcount);
				pointer new_text=new_string.c_str();
				for(rcount=0;rcount<new_string.size;lcount++,rcount++)
					new_text[rcount]=this_text[lcount];
				return new_string;
			}
			Str upper_cpy()const{
				Str new_string(this->size);
				if(this->size>=ss_cap)
					for(size_type i=0;i<size;i++)
						new_string[i]=this->text.heap[i]>=97 and this->text.heap[i]<=122 ? this->text.heap[i]-32:this->text.heap[i];
				else
					for(size_type i=0;i<size;i++)
						new_string[i]=this->text.buffer[i]>=97 and this->text.buffer[i]<=122 ? this->text.buffer[i]-32:this->text.buffer[i];
				return new_string;
			}
			Str lower_cpy()const{
				Str new_string(this->size);
				if(this->size>=ss_cap)
					for(size_type i=0;i<size;i++)
						new_string[i]=this->text.heap[i]>=65 and this->text.heap[i]<=90 ? this->text.heap[i]+32:this->text.heap[i];
				else
					for(size_type i=0;i<size;i++)
						new_string[i]=this->text.buffer[i]>=65 and this->text.buffer[i]<=90 ? this->text.buffer[i]+32:this->text.buffer[i];
				return new_string;
			}
			Optional<size_type> find(const Str& substring,size_type start,size_type end)const{
				if(end<substring.size) return {};
				const_pointer substring_text=substring.c_str(), this_text=this->c_str();
				while(start<=end-substring.size){
					if(not al::memcomp<AlTy>(substring_text,this_text+start,substring.size)) return start;
					start++;
				}
				return {};
			}
			Optional<size_type> rfind(const Str& substring,size_type start,size_type end)const{
				if(end<substring.size) return {};
				const_pointer substring_text=substring.c_str(), this_text=this->c_str();
				end-=substring.size;
				while(end>=start){
					if(not al::memcomp<AlTy>(substring_text,this_text+end,substring.size)) return end;
					else if(end--==0) return {};
				}
				return {};
			}
			size_type count(const Str& substring,size_type start,size_type end)const{
				if(end<substring.size) return 0;
				const_pointer substring_text=substring.c_str(), this_text=this->c_str();
				size_type counter=0;
				while(start<=end-substring.size){
					if(not al::memcomp<AlTy>(substring_text,this_text+start,substring.size)){
						counter++;
						start+=substring.size;
					}
					else start++;
				}
				return counter;
			}
			Optional<size_type> find(const_pointer substring,size_type start,size_type end)const{
				size_type substring_size=strsize<AlTy>(substring);
				if(end<substring_size) return {};
				const_pointer this_text=this->c_str();
				while(start<=end-substring.size){
					if(not al::memcomp<AlTy>(substring,this_text+start,substring_size)) return start;
					start++;
				}
				return {};
			}
			Optional<size_type> rfind(const_pointer substring,size_type start,size_type end)const{
				size_type substring_size=strsize<AlTy>(substring);
				if(end<substring_size) return {};
				const_pointer this_text=this->c_str();
				end-=substring_size;
				while(end>=start){
					if(not al::memcomp<AlTy>(substring,this_text+end,substring_size)) return end;
					else if(end--==0) return {};
				}
				return {};
			}
			size_type count(const_pointer substring,size_type start,size_type end)const{
				size_type substring_size=strsize<AlTy>(substring);
				if(end<substring_size) return 0;
				const_pointer this_text=this->c_str();
				size_type counter=0;
				while(start<=end-substring_size){
					if(not al::memcomp<AlTy>(substring,this_text+start,substring_size)){
						counter++;
						start+=substring_size;
					}
					else start++;
				}
				return counter;
			}
			Optional<size_type> find(const Str& substring,size_type start=0)const{
				if(this->size<substring.size) return {};
				const_pointer substring_text=substring.c_str(), this_text=this->c_str();
				while(start<=this->size-substring.size){
					if(not al::memcomp<AlTy>(substring_text,this_text+start,substring.size)) return start;
					start++;
				}
				return {};
			}
			Optional<size_type> rfind(const Str& substring,size_type start=0)const{
				if(this->size<substring.size) return {};
				const_pointer substring_text=substring.c_str(), this_text=this->c_str();
				size_type end=this->size-substring.size;
				while(end>=start){
					if(not al::memcomp<AlTy>(substring_text,this_text+end,substring.size)) return end;
					else if(end--==0) return {};
				}
				return {};
			}
			size_type count(const Str& substring,size_type start=0)const{
				if(this->size<substring.size) return 0;
				const_pointer substring_text=substring.c_str(), this_text=this->c_str();
				size_type counter=0;
				while(start<=this->size-substring.size){
					if(not al::memcomp<AlTy>(substring_text,this_text+start,substring.size)){
						counter++;
						start+=substring.size;
					}
					else start++;
				}
				return counter;
			}
			Optional<size_type> find(const_pointer substring,size_type start=0)const{
				size_type substring_size=strsize<AlTy>(substring);
				if(this->size<substring_size) return {};
				const_pointer this_text=this->c_str();
				while(start<=this->size-substring.size){
					if(not al::memcomp<AlTy>(substring,this_text+start,substring_size)) return start;
					start++;
				}
				return {};
			}
			Optional<size_type> rfind(const_pointer substring,size_type start=0)const{
				size_type substring_size=strsize<AlTy>(substring);
				if(this->size<substring_size) return {};
				const_pointer this_text=this->c_str();
				size_type end=this->size-substring_size;
				while(end>=start){
					if(not al::memcomp<AlTy>(substring,this_text+end,substring_size)) return end;
					else if(end--==0) return {};
				}
				return {};
			}
			size_type count(const_pointer substring,size_type start=0)const{
				size_type substring_size=strsize<AlTy>(substring);
				if(this->size<substring_size) return 0;
				const_pointer this_text=this->c_str();
				size_type counter=0;
				while(start<=this->size-substring_size){
					if(not al::memcomp<AlTy>(substring,this_text+start,substring_size)){
						counter++;
						start+=substring_size;
					}
					else start++;
				}
				return counter;
			}
			size_type maxsize()const{
				return this->maxSize;
			}
			size_type len()const{
				return this->size;
			}
			bool empty()const{
				return not this->size;
			}
			bool endswith(const Str& substring)const{
				if(this->size>=substring.size)
					return not al::memcomp<AlTy>(this->c_str()+this->size-substring.size,substring.c_str(),substring.size);
				else return 0;
			}
			bool endswith(const_pointer substring)const{
				if(this->size>=strsize<AlTy>(substring))
					return not strcomp<AlTy>(this->c_str()+this->size-strsize<AlTy>(substring),substring);
				else return 0;
			}
			bool startswith(const Str& substring)const{
				if(this->size>=substring.size)
					return not al::memcomp<AlTy>(this->c_str(),substring.c_str(),substring.size);
				else return 0;
			}
			bool startswith(const_pointer substring)const{
				if(this->size>=strsize<AlTy>(substring))
					return not al::memcomp<AlTy>(this->c_str(),substring,strsize<AlTy>(substring));
				else return 0;
			}
			bool continueswith(const Str& substring,size_type start)const{
				if(this->size-start>=substring.size)
					return not al::memcomp<AlTy>(this->c_str()+start,substring.c_str(),substring.size);
				else return 0;
			}
			bool continueswith(const_pointer substring,size_type start)const{
				if(this->size-start>=strsize<AlTy>(substring))
					return not al::memcomp<AlTy>(this->c_str()+start,substring,strsize<AlTy>(substring));
				else return 0;
			}
			void reverse(){
				al::reverse<AlTy>(c_str(),size);
			}
			template<typename Uint>
			enable_if_t<is_integral_v<Uint> && !is_bool_v<Uint> && is_unsigned_v<Uint>,Uint> parse(){
				const_pointer array=c_str();
				Uint num=0;
				size_type i=0;
				while(array[i]==' ' or array[i]=='\n' or array[i]=='\r' or array[i]=='\t')i++;
				for(;array[i]>='0' and array[i]<='9';i++){
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
				while(array[i]==' ' or array[i]=='\n' or array[i]=='\r' or array[i]=='\t')i++;
				if(array[i]=='-'){
					neg=true;
					i++;
				}else if(array[i]=='+')i++;
				for(;array[i]>='0' and array[i]<='9';i++){
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
				while(array[i]==' ' or array[i]=='\n' or array[i]=='\r' or array[i]=='\t')i++;
				if(array[i]=='-'){
					neg=true;
					i++;
				}else if(array[i]=='+')i++;
				for(;array[i]>='0' and array[i]<='9';i++){
					num*=10;
					num+=array[i]-48;
				}
				if(array[i]=='.'){i++;
					for(Float div=10;array[i]>='0' and array[i]<='9';i++,div*=10){
						num+=((array[i]-48)/div);
					}		
				}
				if(array[i]=='e' or array[i]=='E'){i++;
					Float e=10;
					unsigned int e_num=0;
					if(array[i]=='-'){
						e=0.1;
						i++;
					}else if(array[i]=='+')i++;
					for(;array[i]>='0' and array[i]<='9';i++){
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
				size=0;
				c_str()[0]=T();
			}
			void free(){
				if(maxSize!=0){
					AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
				}
				maxSize=0;
				text.heap=nullptr;
				size=0;
			}
			~Str(){
				DECSTRINGS
				//REMAININGSTRINGS
				if(maxSize!=0)
					AlTy_traits::deallocate(this->get_val(),this->text.heap,this->maxSize+1);
			}
			iterator begin(){
				return iterator(&this->c_str()[0]);
			}
			iterator end(){
				return iterator(&this->c_str()[size]);
			}
			iterator rbegin(){
				return iterator(&this->c_str()[size-1]);
			}
			iterator rend(){
				return iterator(&this->c_str()[-1]);
			}
			template<typename t,typename a,typename p> friend Str<t,a,p> operator+(typename Str<t,a,p>::const_pointer,const Str<t,a,p>&);
			template<typename t,typename a,typename p> friend Str<t,a,p> operator+(typename Str<t,a,p>::value_type,const Str<t,a,p>&);
	};
	template<typename T,typename Alloc,typename params>
	Str<T,Alloc,params> operator+(typename Str<T,Alloc,params>::const_pointer Cstr,const Str<T,Alloc,params>& str){
		using AlTy=typename Str<T,Alloc,params>::AlTy;
		auto Cstr_size=strsize<AlTy>(Cstr);//hay que hacer algo aca
		Str<T,Alloc,params> new_str(Cstr_size+str.size);
		al::memcopy<AlTy>(new_str.c_str(),Cstr,Cstr_size);
		al::memcopy<AlTy>(new_str.c_str()+Cstr_size,str.c_str(),str.size);
		return new_str;
	}
	template<typename T,typename Alloc,typename params>
	Str<T,Alloc,params> operator+(typename Str<T,Alloc,params>::value_type chr,const Str<T,Alloc,params>& str){
		using AlTy=typename Str<T,Alloc,params>::AlTy;
		Str<T,Alloc,params> new_str(1+str.size);
		new_str[0]=chr;
		al::memcopy<AlTy>(new_str.c_str()+1,str.c_str(),str.size);
		return new_str;
	}
	template<typename Out,typename T,typename Alloc,typename params>
	Out& operator<<(Out& stream,const Str<T,Alloc,params>& string){
		typename Str<T,Alloc,params>::const_pointer c_string=string.c_str();
		for(typename Str<T,Alloc,params>::size_type i=0;i<string.len();i++)
			stream<<c_string[i];
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
	using String=Str<>;
}