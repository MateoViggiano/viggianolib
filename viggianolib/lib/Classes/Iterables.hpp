#pragma once
namespace mpv{
	struct Fibonacci{
		size_t count,max;
		Fibonacci(size_t max):count(0),max{max}{}
		Fibonacci(size_t start,size_t max):count(start),max{max}{}
		struct iterator{
			size_t pos,a=1,b=0,c=0;
			iterator(size_t pos):pos(pos){
				for(size_t i=0;i<pos;i++){
					c=a+b;
					a=b;
					b=c;
				}
			}	
			size_t operator*(){
				return c;
			}
			iterator& operator++(){
				pos++;
				c=a+b;
				a=b;
				b=c;
				return *this;
			}
			iterator operator++(int){
				iterator aux=*this;
				pos++;
				c=a+b;
				a=b;
				b=c;
				return aux;
			}
			bool operator!=(size_t n){
				return this->pos<n;
			}
		};
		iterator begin()const{
			return iterator(count);
		}
		size_t end()const{
			return max;
		}
	};
	// struct Factorial{
	// 	size_t count,max;
	// 	Factorial(size_t max):count(0),max(max){}
	// 	Factorial(size_t start,size_t max):count(start),max(max){}
	// };
	template<typename T=ptrdiff_t,typename U=T>
	struct Range{
		T start,stop;
		U step;
		Range(T start,T stop,U step):start(start),stop(stop),step(step){}
		Range(T start,T stop):start(start),stop(stop),step(1){}
		Range(T stop):start(0),stop(stop),step(1){}
		T operator[](size_t index)const{
			return start+index*step;			
		}
		size_t len()const{
			if constexpr(is_signed_v<U>){
				if((step>=0 and start>=stop) or (step<0 and start<stop)) return 0;
			}else{
				if(start>=stop) return 0;
			}
			return U(stop-start)/step + size_t((stop-start)%step!=0);
		}
		struct iterator{
			T pos;U step;
			iterator(T pos,U step):pos(pos),step(step){}
			T& operator*(){
				return pos;
			}
			iterator& operator++(){
				pos+=step;
				return *this;
			}
			iterator operator++(int){
				pos+=step;
				return iterator(pos-step,step);
			}
			bool operator!=(T stop)const{
				if constexpr(U(-1)>U()){
					return this->pos<stop;
				}else{
					return step>=U() ? pos<stop : pos>stop;			
				}
			}
		};
		iterator begin()const{
			return iterator(start,step);
		}
		T end()const{
			return stop;
		}
	};
}