#pragma once
#include"../arrays.hpp"
#include"../str.hpp"
namespace mpv{
	template<typename T=char,typename Alloc=allocator<T>,typename params=params<typename allocator_traits<Alloc>::template rebind_traits<T>::size_type,16,3>>
	class Str COUNT_IT{
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
			constexpr sbo()noexcept:heap(nullptr){};
		};
		private:
		/* maxLen es el valor maximo al que puede llegar length antes de tener que reasignar memoria. 
		Cuando se necesita reasignar, el espacio que se asigna es de maxLen+1(para el \0) ya que maxLen y length(que no tiene en cuenta el \0)
		deben(por conveniencia) poder coincidir en un mismo numero sin tener que reasignar.
		Si maxLen es 0, significa que se esta usando el buffer para almacenar el string. Si maxLen>0 el buffer se usa para almacenar un puntero al string en memoria dinamica.*/
			CompressedPair<AlTy,size_type> cp=CompressedPair<AlTy,size_type>(0);//contiene el allocator y maxLen

			size_type length;// Es el tamaanio del string, sin tener en cuenta el '\0'
		/*length no puede ser >= a ss_cap sin tener que asignar memoria, una vez que se esta usando memoria dinamica
		length debe se siempre <= a maxLen (puede llegar a ser igual a maxLen porque maxLen no cuenta el \0
											como un lugar, en cambio ss_cap si) */
			sbo text;
#define maxLen cp.V2
#define alloc cp.getV1()
			template<typename It>
			constexpr iterator insert_counted_range(const_iterator pos,It first,size_type count){
				size_type new_len=this->length+count,index=pos-begin();
				pointer ret;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						value_type* insert_pos=this->text.buffer+index;
						copy_reverse_n(insert_pos+count,insert_pos,this->length-index);
						copy_n(insert_pos,first,count);
						this->text.buffer[new_len]=T();
						ret=pointer(this->text.buffer+index);
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),this->text.buffer,index);
						copy_n(insert_pos,first,count);
						copy_n(insert_pos+count,this->text.buffer+index,this->length-index);
						new_array[new_len]=T();
						this->text.heap=new_array;
						ret=new_array+index;
					}
				}
				else{
					if(new_len<=this->maxLen){
						value_type* insert_pos=unfancy(this->text.heap)+index;
						copy_reverse_n(insert_pos+count,insert_pos,this->length-index);
						copy_n(insert_pos,first,count);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),unfancy(this->text.heap),index);
						copy_n(insert_pos,first,count);
						copy_n(insert_pos+count,unfancy(this->text.heap)+index,this->length-index);
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
					ret=this->text.heap+index;
				}
				this->length=new_len;
				return ret;
			}
            template<typename It>
            constexpr iterator insert_uncounted_range(const_iterator pos,It first,It last){
                size_type index=pos-begin(),prev_length=length;
                while(first!=last) push_back(*first++);
				value_type* array=c_str();
                rotate(array+index,array+prev_length,array+length);
                return pointer(array+index);
            }
		public:
			constexpr const T* c_str()const noexcept{//devuelve un puntero a la memoria que se esta usando actualmente en la union. NO se puede llamar si se modifico maxLen.
				return maxLen==0 ? this->text.buffer : unfancy(this->text.heap);
			}
			constexpr T* c_str()noexcept{//devuelve un puntero a la memoria que se esta usando actualmente en la union. NO se puede llamar si se modifico maxLen.
				return maxLen==0 ? this->text.buffer : unfancy(this->text.heap);
			}
			constexpr Str()noexcept:cp(0),length(0),text{}{}
			constexpr Str(const Alloc& al)noexcept:cp(arg1_tag{},al,0),length(0){}
			template<typename It,enable_if_t<is_iterator_v<It>,int> = 0>
			constexpr Str(It first,It last,const Alloc& al=Alloc{})noexcept:cp(arg1_tag{},al,0),length(0){
                insert(begin(),first,last);
			}
			constexpr explicit Str(size_type sz):cp(0),length(sz){
				if(sz>=ss_cap){
					maxLen=length*M;
					text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					text.heap[length]=T();
				}
				else text.buffer[length]=T();
			}
			constexpr explicit Str(size_type sz,const Alloc& al):cp(arg1_tag{},al,0),length(sz){
				if(sz>=ss_cap){
					maxLen=length*M;
					text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					text.heap[length]=T();
				}
				else text.buffer[length]=T();
			}
			constexpr Str(const Str& other):cp(arg1_tag{},AlTy_traits::select_on_container_copy_construction(other.alloc),0),length(other.length){
				if(other.length<ss_cap){
					copy_n(this->text.buffer,other.c_str(),other.length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),other.c_str(),other.length+1);
				}
			}
			constexpr Str(const Str& other,const Alloc& al):cp(arg1_tag{},al,0),length(other.length){
				if(other.length<ss_cap){
					copy_n(this->text.buffer,other.c_str(),other.length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),other.c_str(),other.length+1);
				}
			}
			constexpr Str(const value_type* str):cp(0),length(al::strsize<AlTy>(str)){
				if(this->length<ss_cap){
					copy_n(this->text.buffer,str,length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),str,length+1);
				}
			}
			constexpr Str(const value_type* str,const Alloc& al):cp(arg1_tag{},al,0),length(al::strsize<AlTy>(str)){
				if(this->length<ss_cap){
					copy_n(this->text.buffer,str,length+1);
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),str,length+1);
				}
			}
			constexpr Str(const value_type* str,size_type n):cp(0),length(n){
				if(this->length<ss_cap){
					copy_n(this->text.buffer,str,n);
					this->text.buffer[length]=T();
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),str,n);
					this->text.heap[length]=T();
				}
			}
			constexpr Str(const value_type* str,size_type n,const Alloc& al):cp(arg1_tag{},al,0),length(n){
				if(this->length<ss_cap){
					copy_n(this->text.buffer,str,n);
					this->text.buffer[length]=T();
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					copy_n(unfancy(this->text.heap),str,n);
					this->text.heap[length]=T();
				}
			}
			constexpr explicit Str(T chr)noexcept:cp(0),length(1){
				this->text.buffer[0]=chr;
				this->text.buffer[1]=T();
			}
			constexpr Str(T chr,const Alloc& al)noexcept:cp(arg1_tag{},al,0),length(1){
				this->text.buffer[0]=chr;
				this->text.buffer[1]=T();
			}
			constexpr Str(size_type n,T chr):cp(0),length(n){
				if(this->length<ss_cap){
					fill_n(this->text.buffer,n,chr);
					this->text.buffer[length]=T();
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					fill_n(unfancy(this->text.heap),n,chr);
					this->text.heap[length]=T();
				}
			}
			constexpr Str(size_type n,T chr,const Alloc& al):cp(arg1_tag{},al,0),length(n){
				if(this->length<ss_cap){
					fill_n(this->text.buffer,n,chr);
					this->text.buffer[length]=T();
				}
				else{
					this->maxLen=this->length*M;
					this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
					fill_n(unfancy(this->text.heap),n,chr);
					this->text.heap[length]=T();
				}
			}
			constexpr Str(Str&& other)noexcept:cp(arg1_tag{},static_cast<AlTy&&>(other.alloc),other.maxLen),length(other.length){
				if(other.maxLen==0)
					copy_n(this->text.buffer,other.text.buffer,other.length+1);
				else
					this->text.heap=other.text.heap;
				other.text.heap=nullptr;
				other.length=0;
				other.maxLen=0;
			}
			constexpr Str(Str&& other,const Alloc& al)noexcept(ALWAYS_EQ):cp(arg1_tag{},al,other.maxLen),length(other.length){
				if(other.maxLen==0)
					copy_n(this->text.buffer,other.text.buffer,other.length+1);
				else{
					if(this->alloc==other.alloc)
						this->text.heap=other.text.heap;
					else{
						this->text.heap=AlTy_traits::allocate(this->alloc,maxLen+1);
						copy_n(unfancy(this->text.heap),unfancy(other.text.heap),other.length+1);
						AlTy_traits::deallocate(this->alloc,other.text.heap,other.maxLen+1);
					}
				}
				other.text.heap=nullptr;
				other.length=0;
				other.maxLen=0;
			}
			constexpr Str& operator=(const Str& other){
				if(this==&other) return *this;
				if(this->maxLen==0){
					pocca(this->alloc,other.alloc);
					if(other.length<ss_cap){
						copy_n(this->text.buffer,other.c_str(),other.length+1);// +1 para copiar el \0 tambien
					}
					else{
						this->text.heap=AlTy_traits::allocate(this->alloc,other.length*M+1);
						this->maxLen=other.length*M;
						copy_n(unfancy(this->text.heap),other.c_str(),other.length+1);
					}
				}
				else{
					if(other.length<=this->maxLen){
						if constexpr(POCCA && !ALWAYS_EQ){
							if(this->alloc!=other.alloc){
								pointer new_array=AlTy_traits::allocate(this->alloc,other.maxLen+1);
								AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
								this->text.heap=new_array;
								pocca(this->alloc,other.alloc);
								this->maxLen=other.maxLen;
							}
						}
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,other.length*M+1);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->text.heap=new_array;
						pocca(this->alloc,other.alloc);
						this->maxLen=other.length*M;
					}
					copy_n(unfancy(this->text.heap),other.c_str(),other.length+1);
				}
				this->length=other.length;
				return *this;
			}
			constexpr Str& operator=(const value_type* str){
				size_type new_length=al::strsize<AlTy>(str);
				if(this->maxLen==0){
					if(new_length<ss_cap)
						copy_n(this->text.buffer,str,new_length+1);
					else{
						this->text.heap=AlTy_traits::allocate(this->alloc,new_length*M+1);
						this->maxLen=new_length*M;
						copy_n(unfancy(this->text.heap),str,new_length+1);
					}
				}
				else{
					if(new_length>this->maxLen){
						pointer new_array=AlTy_traits::allocate(this->alloc,new_length*M+1);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_length*M;
						this->text.heap=new_array;
						
					}
					copy_n(unfancy(this->text.heap),str,new_length+1);
				}
				this->length=new_length;
				return *this;
			}
			constexpr Str& operator=(T chr)noexcept{
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
			constexpr Str& operator=(Str&& other)noexcept(ALWAYS_EQ || POCMA){
				if(this==&other) return *this;
				if constexpr(!ALWAYS_EQ && !POCMA){
					if(this->alloc!=other.alloc){
						if(this->maxLen==0){
							if(other.maxLen==0)
								copy_n(this->text.buffer,other.text.buffer,other.length+1);
							else{
								this->text.heap=AlTy_traits::allocate(this->alloc,other.maxLen+1);
								this->maxLen=other.maxLen;
								copy_n(unfancy(this->text.heap),unfancy(other.text.heap),other.length+1);
							}
						}
						else{
							if(other.maxLen==0)
								copy_n(unfancy(this->text.heap),other.text.buffer,other.length+1);
							else{
								pointer new_array=AlTy_traits::allocate(this->alloc,other.maxLen+1);
								AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
								this->text.heap=new_array;
								this->maxLen=other.maxLen;
								copy_n(unfancy(this->text.heap),unfancy(other.text.heap),other.length+1);
							}
						}
						this->length=other.length;
						return *this;						
					}
				}
				if(this->maxLen==0){
					if(other.maxLen==0)
						copy_n(this->text.buffer,other.text.buffer,other.length+1);
					else{
						this->maxLen=other.maxLen;
						this->text.heap=other.text.heap;
						other.maxLen=0;
					}
				}
				else{
					if(other.maxLen==0)
						copy_n(unfancy(this->text.heap),other.text.buffer,other.length+1);
					else{
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=other.maxLen;
						this->text.heap=other.text.heap;
						other.maxLen=0;
					}
				}
				pocma(this->alloc,other.alloc);
				this->length=other.length;
				other.length=0;
				other.text.heap=nullptr;
				return *this;
			}
			constexpr Str operator+(const Str& other)const&{
				Str new_string(this->length+other.length,AlTy_traits::select_on_container_copy_construction(alloc));//Constructor que asigna un length(ya tiene en cuenta el espacio extra para el \0, al cual asigna \0) y reserva la memoria necesaria
				copy_n(new_string.c_str(),this->c_str(),this->length);
				copy_n(new_string.c_str()+this->length,other.c_str(),other.length);
				return new_string;
			}

			constexpr Str operator+(const value_type* str)const&{
				Str new_string(this->length+al::strsize<AlTy>(str),AlTy_traits::select_on_container_copy_construction(alloc));
				copy_n(new_string.c_str(),this->c_str(),this->length);
				copy_n(new_string.c_str()+this->length,str,new_string.length-this->length+1);//strcopy<AlTy>(new_string.c_str()+this->length,str);
				return new_string;
			}
			constexpr Str operator+(T chr)const&{
				Str new_string(this->length+1,AlTy_traits::select_on_container_copy_construction(alloc));
				copy_n(new_string.c_str(),this->c_str(),this->length);
				new_string.c_str()[this->length]=chr;
				return new_string;
			}
			constexpr Str operator+(Str&& other)const&{
				Str new_string(static_cast<Str&&>(other));
				size_type new_len=new_string.length+this->length;
				if(new_string.maxLen==0){
					if(new_len<ss_cap){
						copy_reverse_n(new_string.text.buffer+this->length,new_string.text.buffer,new_string.length+1);
						copy_n(new_string.text.buffer,this->c_str(),this->length);
					}
					else{
						pointer new_array=AlTy_traits::allocate(new_string.alloc,new_len*M+1);
						new_string.maxLen=new_len*M;
						copy_n(unfancy(new_array)+this->length,new_string.text.buffer,new_string.length+1);
						copy_n(unfancy(new_array),this->c_str(),this->length);
						new_string.text.heap=new_array;
					}
				}
				else{
					if(new_len<=new_string.maxLen){
						copy_reverse_n(unfancy(new_string.text.heap)+this->length,unfancy(new_string.text.heap),new_string.length+1);
						copy_n(unfancy(new_string.text.heap),this->c_str(),this->length);
					}
					else{
						pointer new_array=AlTy_traits::allocate(new_string.alloc,new_len*M+1);
						copy_n(unfancy(new_array)+this->length,unfancy(new_string.text.heap),new_string.length+1);
						copy_n(unfancy(new_array),this->c_str(),this->length);
						AlTy_traits::deallocate(new_string.alloc,new_string.text.heap,new_string.maxLen+1);
						new_string.maxLen=new_len*M;
						new_string.text.heap=new_array;
					}
				}
				new_string.length=new_len;
				return new_string;
			}
			constexpr Str operator+(const Str& other)&&{
				Str new_string(static_cast<Str&&>(*this));
				new_string+=other;
				return new_string;
			}
			constexpr Str operator+(const value_type* str)&&{
				Str new_string(static_cast<Str&&>(*this));
				new_string+=str;
				return new_string;
			}
			constexpr Str operator+(T chr)&&{
				Str new_string(static_cast<Str&&>(*this));
				new_string+=chr;
				return new_string;
			}
			constexpr Str operator+(Str&& other)&&{
				Str new_string(static_cast<Str&&>(*this));
				new_string+=static_cast<Str&&>(other);
				return new_string;
			}
			constexpr Str& operator+=(const Str& other){
				size_type new_len=this->length+other.length;
				if(this->maxLen==0){
					if(new_len<ss_cap)
						copy_reverse_n(this->text.buffer+this->length,other.c_str(),other.length+1);//copy_reverse porque this y other podrian ser el mismo objeto
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						copy_n(unfancy(new_array),this->text.buffer,this->length);
						copy_n(unfancy(new_array)+this->length,other.c_str(),other.length+1);
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen)
						copy_reverse_n(unfancy(this->text.heap)+this->length,other.c_str(),other.length+1);
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						copy_n(unfancy(new_array),unfancy(this->text.heap),this->length);
						copy_n(unfancy(new_array)+this->length,other.c_str(),other.length+1);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr Str& operator+=(Str&& other){//si this y other son el mismo objeto es comportamiento indeinido
				size_type new_len=this->length+other.length;
				if(this->maxLen==0){
					if(new_len<ss_cap)
						copy_n(this->text.buffer+this->length,other.c_str(),other.length+1);
					else if((POCMA || this->alloc==other.alloc) && new_len<=other.maxLen){
						pocma(this->alloc,other.alloc);
						copy_reverse_n(unfancy(other.text.heap)+this->length,unfancy(other.text.heap),other.length+1);
						copy_n(unfancy(other.text.heap),this->text.buffer,this->length);
						this->text.heap=other.text.heap;
						this->maxLen=other.maxLen;
						other.length=0;
						other.maxLen=0;
						other.text.heap=nullptr;
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						copy_n(unfancy(new_array),this->text.buffer,this->length);
						copy_n(unfancy(new_array)+this->length,other.c_str(),other.length+1);
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen)
						copy_n(unfancy(this->text.heap)+this->length,other.c_str(),other.length+1);
					else if((POCMA || this->alloc==other.alloc) && new_len<=other.maxLen){
						pocma(this->alloc,other.alloc);
						copy_reverse_n(unfancy(other.text.heap)+this->length,unfancy(other.text.heap),other.length+1);
						copy_n(unfancy(other.text.heap),unfancy(this->text.heap),this->length);
						this->text.heap=other.text.heap;
						this->maxLen=other.maxLen;
						other.length=0;
						other.maxLen=0;
						other.text.heap=nullptr;
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						copy_n(unfancy(new_array),unfancy(this->text.heap),this->length);
						copy_n(unfancy(new_array)+this->length,other.c_str(),other.length+1);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr Str& operator+=(const value_type* str){
				size_type str_size=al::strsize<AlTy>(str);
				size_type new_len=this->length+str_size;
				if(this->maxLen==0){
					if(new_len<ss_cap)
						copy_reverse_n(this->text.buffer+this->length,str,str_size+1);//reversecopy porq this->c_str() puede ser str
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						copy_n(unfancy(new_array),this->text.buffer,this->length);
						copy_n(unfancy(new_array)+this->length,str,str_size+1);//strcopy<AlTy>(new_array+this->length,str);
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen)
						copy_reverse_n(unfancy(this->text.heap)+this->length,str,str_size+1);
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						copy_n(unfancy(new_array),unfancy(this->text.heap),this->length);
						copy_n(unfancy(new_array)+this->length,str,str_size+1);//strcopy<AlTy>(new_array+this->length,str);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr Str& operator+=(T chr){
				push_back(chr);
				return *this;
			}
			constexpr Str operator*(unsigned int num)const{
				Str new_string(this->length*num,AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_array=new_string.c_str();
				const value_type* this_array=this->c_str();
				for(size_type index=0;index<new_string.length;index+=this->length)
					copy_n(new_array+index,this_array,this->length);
				return new_string;
			}
			constexpr Str& operator*=(unsigned int num){
				size_type new_len=this->length*num;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						for(size_type index=this->length;index<new_len;index+=this->length)
							copy_n(this->text.buffer+index,this->text.buffer,this->length);
						this->text.buffer[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						for(size_type index=0;index<new_len;index+=this->length)
							copy_n(unfancy(new_array)+index,this->text.buffer,this->length);
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen){
						for(size_type index=this->length;index<new_len;index+=this->length)
							copy_n(unfancy(this->text.heap)+index,unfancy(this->text.heap),this->length);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						for(size_type index=0;index<new_len;index+=this->length)
							copy_n(unfancy(new_array)+index,unfancy(this->text.heap),this->length);
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr bool operator==(const Str& other)const{
				if(this->length!=other.length) return false;
				else return mpv::equal_n(this->c_str(),other.c_str(),length);
			}
			constexpr bool operator!=(const Str& other)const{
				if(this->length!=other.length) return true;
				else return !mpv::equal_n(this->c_str(),other.c_str(),length);
			}
			constexpr bool operator<(const Str& other)const{
				return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
			}
			constexpr bool operator>(const Str& other)const{
				return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
			}
			constexpr bool operator<=(const Str& other)const{
				return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
			}
			constexpr bool operator>=(const Str& other)const{
				return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
			}
			constexpr const T& operator[](size_type index)const noexcept{
				return this->c_str()[index];
			}
			constexpr T& operator[](size_type index)noexcept{
				return this->c_str()[index];
			}
			constexpr void push_back(T chr){
				size_type new_len=this->length+1;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						this->text.buffer[length]=chr;
						this->text.buffer[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						copy_n(unfancy(new_array),this->text.buffer,length);
						new_array[length]=chr;
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen){
						this->text.heap[length]=chr;
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						copy_n(unfancy(new_array),unfancy(this->text.heap),length);
						new_array[length]=chr;
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
			}
			constexpr Str& insert(size_type index,T chr){
				size_type new_len=this->length+1;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						value_type* insert_pos=this->text.buffer+index;
						copy_reverse_n(insert_pos+1,insert_pos,this->length-index);
						*insert_pos=chr;
						this->text.buffer[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),this->text.buffer,index);
						*insert_pos=chr;
						copy_n(insert_pos+1,this->text.buffer+index,this->length-index);
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen){
						value_type* insert_pos=unfancy(this->text.heap)+index;
						copy_reverse_n(insert_pos+1,insert_pos,this->length-index);
						*insert_pos=chr;
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),unfancy(this->text.heap),index);
						*insert_pos=chr;
						copy_n(insert_pos+1,unfancy(this->text.heap)+index,this->length-index);
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr iterator insert(const_iterator pos,T chr){
				size_type index=pos-begin();
				insert(index,chr);
				return pointer(c_str()+index);
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
			constexpr iterator insert(const_iterator pos,const size_type count,T val){
				size_type new_len=this->length+count,index=pos-begin();
				pointer ret;
				if(this->maxLen==0){
					if(new_len<ss_cap){
						value_type* insert_pos=this->text.buffer+index;
						copy_reverse_n(insert_pos+count,insert_pos,this->length-index);
						fill_n(insert_pos,count,val);
						this->text.buffer[new_len]=T();
						ret=pointer(this->text.buffer+index);
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),this->text.buffer,index);
						fill_n(insert_pos,count,val);
						copy_n(insert_pos+count,this->text.buffer+index,this->length-index);
						new_array[new_len]=T();
						this->text.heap=new_array;
						ret=new_array+index;
					}
				}
				else{
					if(new_len<=this->maxLen){
						value_type* insert_pos=unfancy(this->text.heap)+index;
						copy_reverse_n(insert_pos+count,insert_pos,this->length-index);
						fill_n(insert_pos,count,val);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),unfancy(this->text.heap),index);
						fill_n(insert_pos,count,val);
						copy_n(insert_pos+count,unfancy(this->text.heap)+index,this->length-index);
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
					ret=this->text.heap+index;
				}
				this->length=new_len;
				return ret;
			}
			constexpr Str& insert(size_type index,const Str& other){
				size_type new_len=this->length+other.length;
				if(this->maxLen==0){
					if(new_len<ss_cap && this!=&other){
						value_type* insert_pos=this->text.buffer+index;
						copy_reverse_n(insert_pos+other.length,insert_pos,this->length-index);
						copy_n(insert_pos,other.c_str(),other.length);
						this->text.buffer[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),this->text.buffer,index);
						copy_n(insert_pos,other.c_str(),other.length);
						copy_n(insert_pos+other.length,this->text.buffer+index,this->length-index);
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen && this!=&other){
						value_type* insert_pos=unfancy(this->text.heap)+index;
						copy_reverse_n(insert_pos+other.length,insert_pos,this->length-index);
						copy_n(insert_pos,other.c_str(),other.length);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),unfancy(this->text.heap),index);
						copy_n(insert_pos,other.c_str(),other.length);
						copy_n(insert_pos+other.length,unfancy(this->text.heap)+index,this->length-index);
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr Str& insert(size_type index,const value_type* str){
				size_type str_size=al::strsize<AlTy>(str);
				size_type new_len=this->length+str_size;
				if(this->maxLen==0){
					if(new_len<ss_cap && this->text.buffer!=str){
						value_type* insert_pos=this->text.buffer+index;
						copy_reverse_n(insert_pos+str_size,insert_pos,this->length-index);
						copy_n(insert_pos,str,str_size);
						this->text.buffer[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),this->text.buffer,index);
						copy_n(insert_pos,str,str_size);
						copy_n(insert_pos+str_size,this->text.buffer+index,this->length-index);
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->maxLen && unfancy(this->text.heap)!=str){
						value_type* insert_pos=unfancy(this->text.heap)+index;
						copy_reverse_n(insert_pos+str_size,insert_pos,this->length-index);
						copy_n(insert_pos,str,str_size);
						this->text.heap[new_len]=T();
					}
					else{
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						value_type* insert_pos=unfancy(new_array)+index;
						copy_n(unfancy(new_array),unfancy(this->text.heap),index);
						copy_n(insert_pos,str,str_size);
						copy_n(insert_pos+str_size,unfancy(this->text.heap)+index,this->length-index);
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;
					}
				}
				this->length=new_len;
				return *this;
			}
			constexpr void del_back()noexcept{
				this->c_str()[--length]=T();
			}
			constexpr T pop_back()noexcept{
				T chr=this->c_str()[length-1];
				this->c_str()[--length]=T();
				return chr;
			}
			constexpr Str& erase(size_type start,size_type end)noexcept{
				size_type new_len=this->length-(end-start);
				value_type* this_array=this->c_str();
				copy_n(this_array+start,this_array+end,this->length-end);
				this_array[new_len]=T();
				this->length=new_len;
				return *this;
			}
			constexpr Str& erase(size_type start)noexcept{
				this->c_str()[start]=T();
				this->length=start;
				return *this;
			}
			constexpr Str& remove(const Str& substring,size_type start,size_type end)noexcept{
				size_type index=this->find(substring,start,end);
				if(index!=length){
					this->erase(index,index+substring.length);	
				}
				return *this;
			}
			constexpr Str& remove(const value_type* substring,size_type start,size_type end)noexcept{
				size_type index=this->find(substring,start,end);
				if(index!=length){
					this->erase(index,index+al::strsize<AlTy>(substring));	
				}
				return *this;
			}
			constexpr Str& rremove(const Str& substring,size_type start,size_type end)noexcept{
				size_type index=this->rfind(substring,start,end);
				if(index!=length){
					this->erase(index,index+substring.length);	
				}
				return *this;
			}
			constexpr Str& rremove(const value_type* substring,size_type start,size_type end)noexcept{
				size_type index=this->rfind(substring,start,end);
				if(index!=length){
					this->erase(index,index+al::strsize<AlTy>(substring));	
				}
				return *this;
			}
			constexpr Str& remove(const Str& substring,size_type start=0)noexcept{
				size_type index=this->find(substring,start);
				if(index!=length){
					this->erase(index,index+substring.length);	
				}
				return *this;
			}
			constexpr Str& remove(const value_type* substring,size_type start=0)noexcept{
				size_type index=this->find(substring,start);
				if(index!=length){
					this->erase(index,index+al::strsize<AlTy>(substring));	
				}
				return *this;
			}
			constexpr Str& rremove(const Str& substring,size_type start=0)noexcept{
				size_type index=this->rfind(substring,start);
				if(index!=length){
					this->erase(index,index+substring.length);	
				}
				return *this;
			}
			constexpr Str& rremove(const value_type* substring,size_type start=0)noexcept{
				size_type index=this->rfind(substring,start);
				if(index!=length){
					this->erase(index,index+al::strsize<AlTy>(substring));	
				}
				return *this;
			}
			constexpr Str& outplace(const Str& substring,size_type start,size_type end){
				size_type new_len=this->length-(end-start)+substring.length;
				const value_type* substring_text=substring.c_str();
				value_type* this_array=this->c_str();
				if((new_len<ss_cap && this->maxLen==0) || new_len<=this->maxLen){			//		  vv for \0
					mpv::copy_overlap_n(this_array+start+substring.length,this_array+end,this->length-end+1);// copyes memory correctly even if source and dest overlap
					copy_n(this_array+start,substring_text,substring.length);
				}
				else{
					pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
					copy_n(unfancy(new_array),this_array,start);
					copy_n(unfancy(new_array)+start,substring_text,substring.length);
					copy_n(unfancy(new_array)+start+substring.length,this_array+end,this->length-end+1);
					if(this->maxLen>0) AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
					this->text.heap=new_array;
					this->maxLen=new_len*M;
				}
				this->length=new_len;
				return *this;
			}
			template<bool whole_words_only=false>
			Str& replace(const Str& substring1,const Str& substring2,Optional<size_type> max_count={}){
				size_type count=this->count<whole_words_only>(substring1);
				if(max_count.has_value() && count>max_count.value())
					count=max_count.value();
				if(count==0) return *this;
				using AlInt=rebind_alloc<AlTy,size_type>;
				AlInt uint_alloc(this->alloc);
				//size_type* array=allocator_traits<AlInt>::allocate(uint_alloc,count);
				mpv::uPtr<size_type[],alloc_wrapper_delete<AlInt>> array(allocator_traits<AlInt>::allocate(uint_alloc,count),alloc_wrapper_delete<AlInt>(uint_alloc,count));
				for(size_type i=0,last=0;i<count;last=array[i++]+substring1.length)
					allocator_traits<AlInt>::construct(uint_alloc,&array[i],this->find<whole_words_only>(substring1,last));
				for(size_type i=0;i<count;i++)
					array[i]=array[i]+(substring2.length-substring1.length)*i;
				if(substring1.length==0){
					for(size_type i=0;i<count;i++)
						array[i]+=i;
				}
				size_type new_len=this->length+(count*substring2.length-count*substring1.length);
				const value_type* substring2_text=substring2.c_str();
				if(this->maxLen==0){
					if(new_len<=this->length){
						for(size_type i=array[0],j=array[0],k=0;i<new_len;){
							if(k<count && i==array[k]){
								copy_n(this->text.buffer+array[k++],substring2_text,substring2.length);
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
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						this->maxLen=new_len*M;
						for(size_type i=0,j=0,k=0;i<new_len;){
							if(k<count && i==array[k]){
								copy_n(unfancy(new_array)+array[k++],substring2_text,substring2.length);
								i+=substring2.length;
								j+=substring1.length;
							}
							else{
								new_array[i]=this->text.buffer[j];
								i++;
								j++;
							}
						}
						new_array[new_len]=T();
						this->text.heap=new_array;
					}
				}
				else{
					if(new_len<=this->length){//aunque new_len<=maxLen si new_len<=length no se cumple se necesita reasignar memoria
						for(size_type i=array[0],j=array[0],k=0;i<new_len;){
							if(k<count && i==array[k]){
								copy_n(unfancy(this->text.heap)+array[k++],substring2_text,substring2.length);
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
						pointer new_array=AlTy_traits::allocate(this->alloc,new_len*M+1);
						for(size_type i=0,j=0,k=0;i<new_len;){
							if(k<count && i==array[k]){
								copy_n(unfancy(new_array)+array[k++],substring2_text,substring2.length);
								i+=substring2.length;
								j+=substring1.length;
							}
							else{
								new_array[i]=this->text.heap[j];
								i++;
								j++;
							}
						}
						new_array[new_len]=T();
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->maxLen=new_len*M;
						this->text.heap=new_array;						
					}
				}
				this->length=new_len;
				return *this;
				//allocator_traits<AlInt>::deallocate(uint_alloc,array,count);
			}
			constexpr Str substr(size_type start,size_type end)const{
				Str new_string(end-start,AlTy_traits::select_on_container_copy_construction(alloc));//el tamaÃƒÂ±o del nuevo string lo genera el constructor
				copy_n(new_string.c_str(),this->c_str()+start,new_string.length);
				return new_string;
			}
			constexpr Str substr(size_type start)const{
				Str new_string(length-start,AlTy_traits::select_on_container_copy_construction(alloc));
				copy_n(new_string.c_str(),this->c_str()+start,new_string.length);
				return new_string;
			}
			constexpr Str extract(size_type index,size_type pos)const{
				Str new_string(pos,AlTy_traits::select_on_container_copy_construction(alloc));
				copy_n(new_string.c_str(),this->c_str()+index,pos);
				return new_string;
			}
			constexpr Str extract(size_type index)const{
				Str new_string(length-index,AlTy_traits::select_on_container_copy_construction(alloc));
				copy_n(new_string.c_str(),this->c_str()+index,length-index);
				return new_string;
			}
			template<T SPACE=' '>
			constexpr void noExtraSpaces()noexcept{
				value_type* this_array=this->c_str();
				size_type i=0,j=0;
				while(j<this->length){
					if(this_array[j]==SPACE && this_array[j+1]==SPACE){
						while(j<this->length && this_array[j]==this_array[j+1]){
							j++;
						}						
					}
					else{
						this_array[i++]=this_array[j++];
					}
				}
				this->length=i;
				this_array[i]=T();
			}
			template<T SPACE=' '>
			constexpr void lstrip()noexcept{
				value_type* this_array=this->c_str();
				if(this_array[0]!=SPACE) return;// Para no tener que copiar el string completo si es que no hay espacios que borrar
				size_type count=0;
				while(count<this->length && this_array[count]==SPACE)
					count++;
				this->length-=count;
				copy_n(this_array,this_array+count,this->length+1);
			}
			template<T SPACE=' '>
			constexpr void rstrip()noexcept{
				value_type* this_array=this->c_str();
				while(length>0 && this_array[length-1]==SPACE){
					length--;
				}
				this_array[length]=T();
			}
			template<T SPACE=' '>
			constexpr void strip()noexcept{
				rstrip<SPACE>();
				lstrip<SPACE>();
			}
			constexpr void upper()noexcept{
				value_type* this_array=this->c_str();
				for(size_type i=0;i<length;i++)
					if(this_array[i]>=97 && this_array[i]<=122)
						this_array[i]-=32;
			}
			constexpr void lower()noexcept{
				value_type* this_array=this->c_str();
				for(size_type i=0;i<length;i++)
					if(this_array[i]>=65 && this_array[i]<=90)
						this_array[i]+=32;
			}
			template<T SPACE=' '>
			constexpr Str noExtraSpaces_cpy()const{
				const value_type* this_array=this->c_str();
				size_type count=0;
				for(size_type i=0;i<this->length;i++)
					if(this_array[i]==SPACE && this_array[i+1]==SPACE)
						count++;
				Str new_string(this->length-count,AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_array=new_string.c_str();
				for(size_type i=0,j=0;i<this->length;i++){
					if(!(this_array[i]==SPACE && this_array[i+1]==SPACE)){
						new_array[j]=this_array[i];
						j++;
					}
				}
				return new_string;
			}
			template<T SPACE=' '>
 			constexpr Str lstrip_cpy()const{
				if(this->length==0) return Str();
				const value_type* this_array=this->c_str();
				size_type count=0;
				while(count<this->length && this_array[count]==SPACE)
					count++;
				Str new_string(this->length-count,AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_array=new_string.c_str();
				for(size_type i=0;count<this->length;i++,count++)
					new_array[i]=this_array[count];
				return new_string;
			}
			template<T SPACE=' '>
 			constexpr Str rstrip_cpy()const{
				const value_type* this_array=this->c_str();
				size_type count=this->length;
				while(count>0 && this_array[count-1]==SPACE)
					count--;
				Str new_string(count,AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_array=new_string.c_str();
				for(count=0;count<new_string.length;count++)
					new_array[count]=this_array[count];
				return new_string;
			}
			template<T SPACE=' '>
			constexpr Str strip_cpy()const{
				if(this->length==0) return Str();
				const value_type* this_array=this->c_str();
				size_type lcount=0,rcount=this->length;
				while(lcount<this->length && this_array[lcount]==SPACE)
					lcount++;
				if(lcount<this->length)//Por si el string solamente contiene espacios
					while(/* rcount>0 &&  */this_array[rcount-1]==SPACE)
						rcount--;					
				Str new_string(rcount-lcount,AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_array=new_string.c_str();
				for(rcount=0;rcount<new_string.length;lcount++,rcount++)
					new_array[rcount]=this_array[lcount];
				return new_string;
			}
			constexpr Str upper_cpy()const{
				Str new_string(this->length,AlTy_traits::select_on_container_copy_construction(alloc));
				if(this->length>=ss_cap)
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.heap[i]>=97 && this->text.heap[i]<=122 ? this->text.heap[i]-32:this->text.heap[i];
				else
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.buffer[i]>=97 && this->text.buffer[i]<=122 ? this->text.buffer[i]-32:this->text.buffer[i];
				return new_string;
			}
			constexpr Str lower_cpy()const{
				Str new_string(this->length,AlTy_traits::select_on_container_copy_construction(alloc));
				if(this->length>=ss_cap)
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.heap[i]>=65 && this->text.heap[i]<=90 ? this->text.heap[i]+32:this->text.heap[i];
				else
					for(size_type i=0;i<length;i++)
						new_string[i]=this->text.buffer[i]>=65 && this->text.buffer[i]<=90 ? this->text.buffer[i]+32:this->text.buffer[i];
				return new_string;
			}
			constexpr bool is_alpha()const noexcept{
				const value_type* s=this->c_str();
				for(size_type i=0;i<this->length;i++)
					if(!mpv::is_alpha(s[i])) return false;
				return true;
			}
			constexpr bool is_numeric()const noexcept{
				const value_type* s=this->c_str();
				for(size_type i=0;i<this->length;i++)
					if(!mpv::is_numeric(s[i])) return false;
				return true;
			}			
			constexpr bool is_alnum()const noexcept{
				const value_type* s=this->c_str();
				for(size_type i=0;i<this->length;i++)
					if(!mpv::is_alnum(s[i])) return false;
				return true;
			}
			constexpr bool is_alnum_us()const noexcept{		// Returns true if the string contains only letters, digits or underscores
				const value_type* s=this->c_str();
				for(size_type i=0;i<this->length;i++)
					if(!mpv::is_alnum_us(s[i])) return false;
				return true;
			}
			constexpr bool is_identifier()const noexcept{	// Returns true if the string is a valid name for a variable
				return this->is_alnum_us() && this->length>0 && !mpv::is_numeric(this->c_str()[0]);
			}
			constexpr bool is_int()const noexcept{
				const value_type* s=this->c_str();
				if(!mpv::is_numeric(s[0]) && s[0]!='-') return false;
				for(size_type i=1;i<this->length;i++)
					if(!mpv::is_numeric(s[i])) return false;
				return true;
			}	
			constexpr bool is_float_convertible()const noexcept{
				const value_type* s=c_str();
				if(*s==T()) return false;
				if(*s=='+' || *s=='-') s++;
				if(*s=='e' || *s=='E') return false;
				const char* e_pos=nullptr;
				bool dot_found=false;
				while(*s!=T()){
					if(!mpv::is_numeric(*s)){
						if(*s=='.'){
							if(dot_found || e_pos!=nullptr) return false;
							else dot_found=true;
						}
						else if(*s=='e' || *s=='E'){
							if(e_pos!=nullptr) return false;
							e_pos=s++;
							if(*s==T()) return false;
							if(*s=='+' || *s=='-'){
								e_pos=s;
							}
						}
						else return false;
					}
					s++;
				}
				return s-1!=e_pos;
			}
			constexpr bool contains(T c)const noexcept{
				return mpv::contains(c_str(),c_str()+length,c);
			}
			template<bool whole_words_only=false>
			constexpr bool contains(const Str& substring)const noexcept{
				if(this->length<substring.length) return false;
				if(substring.length==0) return true;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				for(size_type i=0;i<=this->length-substring.length;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring.length) && (i==0 || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))) return true;
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)) return true;
					}
				}
				return false;
			}
			template<bool whole_words_only=false>
			constexpr bool contains(const value_type* c_substring)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(this->length<substring_size) return false;
				if(substring_size==0) return true;
				const value_type* this_array=this->c_str();
				for(size_type i=0;i<=this->length-substring_size;i++){
					if constexpr(whole_words_only){		// comprobar si i==end-c_substring.length no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(equal_n(c_substring,this_array+i,substring_size) && (i==0 || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))) return true;
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)) return true;
					}
				}
				return false;
			}
			template<bool whole_words_only=false>
			constexpr size_type find(const Str& substring,size_type start,size_type end)const noexcept{
				if(end<start+substring.length) return length;
				if(substring.length==0) return start;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				for(size_type i=start;i<=end-substring.length;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring.length || !mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)) return i;
					}
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type rfind(const Str& substring,size_type start,size_type end)const noexcept{
				if(end<start+substring.length) return length;
				if(substring.length==0) return end;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				for(size_type i=end-substring.length;i>=start;i--){
					if constexpr(whole_words_only){ // i==start para q no mire una posicon a la izquierda de start, porq esa posicion no esta en el rango de busqueda que se especifico en los parametros de la funcion(start y end).
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring.length || !mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))) return i;
					}else{																								// Y i==end-substring lo mismo que i==start pero a la derecha del string
						if(equal_n(c_substring,this_array+i,substring.length)) return i;
					}
					if(i==0) return length;//because i is unsigned
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type count(const Str& substring,size_type start,size_type end)const noexcept{
				if(end<start+substring.length) return 0;
				if(substring.length==0) return end-start+1;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=end-substring.length;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring.length || !mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))){
							counter++;
							i+=substring.length-1;
						}
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)){
							counter++;
							i+=substring.length-1;
						}
					}
				}
				return counter;
			}
			template<bool whole_words_only=false>
			constexpr size_type find(const value_type* c_substring,size_type start,size_type end)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(end<start+substring_size) return length;
				if(substring_size==0) return start;
				const value_type* this_array=this->c_str();
				for(size_type i=start;i<=end-substring_size;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring_size || !mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)) return i;
					}
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type rfind(const value_type* c_substring,size_type start,size_type end)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(end<start+substring_size) return length;
				if(substring_size==0) return end;
				const value_type* this_array=this->c_str();
				for(size_type i=end-substring_size;i>=start;i--){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring_size || !mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)) return i;
					}
					if(i==0) return length;
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type count(const value_type* c_substring,size_type start,size_type end)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(end<start+substring_size) return 0;
				if(substring_size==0) return end-start+1;
				const value_type* this_array=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=end-substring_size;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (i==end-substring_size || !mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))){
							counter++;
							i+=substring_size-1;
						}
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)){
							counter++;
							i+=substring_size-1;
						}
					}
				}
				return counter;
			}
			template<bool whole_words_only=false>
			constexpr size_type find(const Str& substring,size_type start=0)const noexcept{
				if(this->length<start+substring.length) return length;
				if(substring.length==0) return start;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				for(size_type i=start;i<=this->length-substring.length;i++){
					if constexpr(whole_words_only){		// comprobar si i==end-substring.length no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)) return i;
					}
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type rfind(const Str& substring,size_type start=0)const noexcept{
				if(this->length<start+substring.length) return length;
				if(substring.length==0) return this->length;//this->length==end
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				for(size_type i=this->length-substring.length;i>=start;i--){
					if constexpr(whole_words_only){		// comprobar si i==end-substring.length no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)) return i;
					}
					if(i==0) return length;
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type count(const Str& substring,size_type start=0)const noexcept{
				if(this->length<start+substring.length) return 0;
				if(substring.length==0) return this->length-start+1;
				const value_type* c_substring=substring.c_str(),* this_array=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=this->length-substring.length;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring.length) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring.length-1]) || !mpv::is_alnum_us(this_array[i+substring.length]))){
							counter++;
							i+=substring.length-1;
						}
					}else{
						if(equal_n(c_substring,this_array+i,substring.length)){
							counter++;
							i+=substring.length-1;
						}
					}
				}
				return counter;
			}
			template<bool whole_words_only=false>
			constexpr size_type find(const value_type* c_substring,size_type start=0)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(this->length<start+substring_size) return length;
				if(substring_size==0) return start;
				const value_type* this_array=this->c_str();
				for(size_type i=start;i<=this->length-substring_size;i++){
					if constexpr(whole_words_only){		// comprobar si i==end-c_substring.length no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)) return i;
					}
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type rfind(const value_type* c_substring,size_type start=0)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(this->length<start+substring_size) return length;
				if(substring_size==0) return this->length;//this->length==end
				const value_type* this_array=this->c_str();
				for(size_type i=this->length-substring_size;i>=start;i--){
					if constexpr(whole_words_only){		// comprobar si i==end-substring_size no hace falta porq el string siempre termina en \0, asi que leer una posicion afuera del string no es comportamiento indefinido, sino que siempre hay un \0
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))) return i;
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)) return i;
					}
					if(i==0) return length;
				}
				return length;
			}
			template<bool whole_words_only=false>
			constexpr size_type count(const value_type* c_substring,size_type start=0)const noexcept{
				size_type substring_size=al::strsize<AlTy>(c_substring);
				if(this->length<start+substring_size) return 0;
				if(substring_size==0) return this->length-start+1;
				const value_type* this_array=this->c_str();
				size_type counter=0;
				for(size_type i=start;i<=this->length-substring_size;i++){
					if constexpr(whole_words_only){
						if(equal_n(c_substring,this_array+i,substring_size) && (i==start || !mpv::is_alnum_us(this_array[i]) || !mpv::is_alnum_us(this_array[i-1])) && (!mpv::is_alnum_us(this_array[i+substring_size-1]) || !mpv::is_alnum_us(this_array[i+substring_size]))){
							counter++;
							i+=substring_size-1;
						}
					}else{
						if(equal_n(c_substring,this_array+i,substring_size)){
							counter++;
							i+=substring_size-1;
						}
					}
				}
				return counter;
			}
			template<template<typename...> typename Container,typename... Args>
			constexpr Container<Str,Args...> split(const value_type* substring)const{
				Container<Str> list;
				size_type substring_length=al::strsize<AlTy>(substring);
				if(substring_length==0){
					//if(this->length==0) list.emplace_back("",AlTy_traits::select_on_container_copy_construction(alloc));else //asi funcionaria como el split de java
					for(size_type i=0;i<this->length;i++){
						list.emplace_back((*this)[i],AlTy_traits::select_on_container_copy_construction(alloc));
					}
				}
				else{
					size_type start=0,end=0;
					while(start<=this->length){
						end=this->find(substring,start);// end=this->length if substring is not found
						list.push_back(this->substr(start,end));
						start=end+substring_length;
					}					
				}
				return list;
			}
			template<template<typename...> typename Container,typename... Args>
			constexpr Container<Str,Args...> split(const value_type* substring,const typename Container<Str,Args...>::allocator_type al)const{
				Container<Str> list(al);
				size_type substring_length=al::strsize<AlTy>(substring);
				if(substring_length==0){
					//if(this->length==0) list.emplace_back("",AlTy_traits::select_on_container_copy_construction(alloc));else //asi funcionaria como el split de java
					for(size_type i=0;i<this->length;i++){
						list.emplace_back((*this)[i],AlTy_traits::select_on_container_copy_construction(alloc));
					}
				}
				else{
					size_type start=0,end=0;
					while(start<=this->length){
						end=this->find(substring,start);
						list.push_back(this->substr(start,end));
						start=end+substring_length;
					}					
				}
				return list;
			}
			template<template<typename...> typename Container,typename... Args>
			constexpr Container<Str,Args...> split(const Str& substring)const{
				Container<Str> list;
				if(substring.length==0){
					//if(this->length==0) list.emplace_back("",AlTy_traits::select_on_container_copy_construction(alloc));else //asi funcionaria como el split de java
					for(size_type i=0;i<this->length;i++){
						list.emplace_back((*this)[i],AlTy_traits::select_on_container_copy_construction(alloc));
					}
				}
				else{
					size_type start=0,end=0;
					while(start<=this->length){
						end=this->find(substring,start);
						list.push_back(this->substr(start,end));
						start=end+substring.length;
					}					
				}
				return list;
			}
			template<template<typename...> typename Container,typename... Args>
			constexpr Container<Str,Args...> split(const Str& substring,const typename Container<Str,Args...>::allocator_type al)const{
				Container<Str> list(al);
				if(substring.length==0){
					//if(this->length==0) list.emplace_back("",AlTy_traits::select_on_container_copy_construction(alloc));else //asi funcionaria como el split de java
					for(size_type i=0;i<this->length;i++){
						list.emplace_back((*this)[i],AlTy_traits::select_on_container_copy_construction(alloc));
					}
				}
				else{
					size_type start=0,end=0;
					while(start<=this->length){
						end=this->find(substring,start);
						list.push_back(this->substr(start,end));
						start=end+substring.length;
					}					
				}
				return list;
			}
			constexpr size_type max_size()const noexcept{
				return this->maxLen;
			}
			constexpr size_type size()const noexcept{
				return this->length;
			}
            constexpr const_reference back()const noexcept{
                return c_str()[length-1];
            }
            constexpr reference back()noexcept{
                return c_str()[length-1];
            }
			constexpr bool empty()const noexcept{
				return !this->length;
			}
			constexpr bool endswith(const Str& substring)const noexcept{
				if(this->length>=substring.length)
					return equal_n(this->c_str()+this->length-substring.length,substring.c_str(),substring.length);
				else return 0;
			}
			constexpr bool endswith(const value_type* substring)const noexcept{
				size_type substr_size=al::strsize<AlTy>(substring);
				if(this->length>=substr_size)
					return equal_n(this->c_str()+this->length-substr_size,substring,substr_size);
				else return 0;
			}
			constexpr bool startswith(const Str& substring)const noexcept{
				if(this->length>=substring.length)
					return equal_n(this->c_str(),substring.c_str(),substring.length);
				else return 0;
			}
			constexpr bool startswith(const value_type* substring)const noexcept{
				size_type substr_size=al::strsize<AlTy>(substring);
				if(this->length>=substr_size)
					return equal_n(this->c_str(),substring,substr_size);
				else return 0;
			}
			constexpr bool continueswith(const Str& substring,size_type start)const noexcept{
				if(this->length-start>=substring.length)
					return equal_n(this->c_str()+start,substring.c_str(),substring.length);
				else return 0;
			}
			constexpr bool continueswith(const value_type* substring,size_type start)const noexcept{
				size_type substr_size=al::strsize<AlTy>(substring);
				if(this->length-start>=substr_size)
					return equal_n(this->c_str()+start,substring,substr_size);
				else return 0;
			}
			constexpr Str& reverse()noexcept{
				value_type* start = c_str();
				mpv::reverse(start,start+length);
				return *this;
			}
			template<typename Lambda>
			constexpr bool any(Lambda&& func=Lambda{})const{
                for(const_reference val:*this)
                    if(func(val))return 1;
                return 0;
			}
			template<typename Lambda>
			constexpr void foreach(Lambda&& func=Lambda{}){
				for(reference val:*this)
					func(val);
			}
			template<typename Lambda>
			constexpr Str filter(Lambda&& func=Lambda{})const{
				Str new_str(AlTy_traits::select_on_container_copy_construction(alloc));
				value_type* new_c_str=new_str.text.buffer;
				const value_type* this_array=this->c_str();
				if(this->length>=ss_cap){
					new_str.text.heap=AlTy_traits::allocate(new_str.alloc,this->length+1);
					new_str.maxLen=this->length;
					new_c_str=unfancy(new_str.text.heap);
				}
				for(size_type i=0;i<this->length;i++)
					if(func(this_array[i]))
						new_c_str[new_str.length++]=this_array[i];
				new_c_str[new_str.length]=T();
				return new_str;
			}
			template<typename Uint>
			constexpr enable_if_t<is_unsigned_v<Uint>,Uint> parse()const noexcept(is_scalar_v<Uint>){
				const value_type* array=c_str();
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
			constexpr enable_if_t<is_signed_v<Int> && !is_floating_point_v<Int>,Int> parse()const noexcept(is_scalar_v<Int>){
				const value_type* array=c_str();
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
			constexpr enable_if_t<is_floating_point_v<Float>,Float> parse()const noexcept(is_scalar_v<Float>){
				const value_type* array=c_str();
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
			constexpr void clear()noexcept{
				length=0;
				c_str()[0]=T();
			}
			constexpr void free()noexcept{
				if(maxLen!=0) AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
				maxLen=0;
				text.heap=nullptr;
				length=0;
			}
            constexpr allocator_type get_allocator()const noexcept{
                return this->alloc;
            }
			~Str()noexcept{
				if(maxLen!=0) AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
			}
			constexpr iterator begin()noexcept{
				return pointer(c_str());
			}
			constexpr iterator end()noexcept{
				return pointer(c_str()+length);
			}
			constexpr const_iterator begin()const noexcept{
				return const_pointer(c_str());
			}
			constexpr const_iterator end()const noexcept{
				return const_pointer(c_str()+length);
			}
			template<typename t,typename a,typename p> friend Str<t,a,p> constexpr operator+(const typename Str<t,a,p>::value_type*,const Str<t,a,p>&);
			template<typename t,typename a,typename p> friend Str<t,a,p> constexpr operator+(typename Str<t,a,p>::value_type,const Str<t,a,p>&);

			template<typename In>
			void read(In& stream,size_type length){
				this->clear();
				value_type* text;
				if(this->maxLen==0){
					if(length<ss_cap)
						text=this->text.buffer;
					else{
						this->text.heap=AlTy_traits::allocate(this->alloc,length*M+1);
						this->maxLen=length*M;
						text=unfancy(this->text.heap);
					}
				}
				else{
					if(length>this->maxLen){
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->text.heap=AlTy_traits::allocate(this->alloc,length*M+1);
						this->maxLen=length*M;
					}
					text=unfancy(this->text.heap);
				}
				this->length=length;
				stream.read(text,length);
				text[length]=T();
			}

			template<typename In>
			void read(In& stream){
				this->clear();
				size_type current_pos=stream.tellg();
				stream.seekg(0,In::ios_base::end);
				this->length=(size_type)stream.tellg()-current_pos;
				stream.seekg(current_pos);
				value_type* text;
				if(this->maxLen==0){
					if(length<ss_cap)
						text=this->text.buffer;
					else{
						this->text.heap=AlTy_traits::allocate(this->alloc,length*M+1);
						this->maxLen=length*M;
						text=unfancy(this->text.heap);
					}
				}
				else{
					if(length>this->maxLen){
						AlTy_traits::deallocate(this->alloc,this->text.heap,this->maxLen+1);
						this->text.heap=AlTy_traits::allocate(this->alloc,length*M+1);
						this->maxLen=length*M;
					}
					text=unfancy(this->text.heap);
				}
				stream.read(text,length);
				text[length]=T();
			}
			template<typename In>
			void readtext(In& stream){
				this->clear();
				T c;
				while(stream.get(c))
					this->push_back(c);
			}

	};
	template<typename T,typename Alloc,typename params>
	constexpr Str<T,Alloc,params> operator+(const typename Str<T,Alloc,params>::value_type* c_string,const Str<T,Alloc,params>& str){
		using AlTy=typename Str<T,Alloc,params>::AlTy;
		using AlTy_traits=typename Str<T,Alloc,params>::AlTy_traits;
		auto c_string_size=al::strsize<AlTy>(c_string);
		Str<T,Alloc,params> new_str(c_string_size+str.length,AlTy_traits::select_on_container_copy_construction(str.alloc));
		copy_n(new_str.c_str(),c_string,c_string_size);
		copy_n(new_str.c_str()+c_string_size,str.c_str(),str.length);
		return new_str;
	}
	template<typename T,typename Alloc,typename params>
	constexpr Str<T,Alloc,params> operator+(typename Str<T,Alloc,params>::value_type chr,const Str<T,Alloc,params>& str){
		using AlTy_traits=typename Str<T,Alloc,params>::AlTy_traits;
		Str<T,Alloc,params> new_str(1+str.length,AlTy_traits::select_on_container_copy_construction(str.alloc));
		new_str[0]=chr;
		copy_n(new_str.c_str()+1,str.c_str(),str.length);
		return new_str;
	}
	template<typename Out,typename T,typename Alloc,typename params>
	Out& operator<<(Out& stream,const Str<T,Alloc,params>& string){
		//const typename Str<T,Alloc,params>::value_type* c_string=string.c_str();
		//stream.write(c_string,string.size());
		return stream<<string.c_str();
	}
	#ifndef STR_ENDLINE
	#define STR_ENDLINE '\n'
	#endif
	template<typename In,typename T,typename Alloc,typename params,T endchar=STR_ENDLINE>
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
	template<typename Int,typename String_type=String,unsigned short base=10>
	constexpr String_type to_str(Int num){
		String_type new_str,aux;
		if(num<0){
			num=-num;
			new_str='-';
		}
		do{
			aux+=to_char(num%base);
			num/=base;
		}while(num);
		for(auto i=aux.size();i>0;i--)
			new_str+=aux[i-1];
		return new_str;
	}
	inline String operator""_s(const char* s,size_t len){
		return String(s,len);
	}
#undef alloc
#undef maxLen
}