#pragma once
namespace mpv{
	struct Fibonacci{
		size_t count,max;
		constexpr Fibonacci(size_t max)noexcept:count(0),max{max}{}
		constexpr Fibonacci(size_t start,size_t max)noexcept:count(start),max{max}{}
		struct iterator{
			using iterator_category=forward_iterator_tag;
			using value_type=size_t;
			using pointer=size_t*;
			using reference=size_t&;
			size_t pos,a=1,b=0,c=0;
			constexpr iterator(size_t pos)noexcept:pos(pos){
				for(size_t i=0;i<pos;i++){
					c=a+b;
					a=b;
					b=c;
				}
			}	
			constexpr size_t operator*()noexcept{
				return c;
			}
			constexpr iterator& operator++()noexcept{
				pos++;
				c=a+b;
				a=b;
				b=c;
				return *this;
			}
			constexpr iterator operator++(int)noexcept{
				iterator aux=*this;
				pos++;
				c=a+b;
				a=b;
				b=c;
				return aux;
			}
			constexpr bool operator!=(size_t n)noexcept{
				return this->pos<n;
			}
		};
		constexpr iterator begin()const noexcept{
			return iterator(count);
		}
		constexpr size_t end()const noexcept{
			return max;
		}
	};
	template<typename T,typename U=T>
	struct Range{
		T start,stop;
		U step;
		constexpr Range(T start,T stop,U step)noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>):start(start),stop(stop),step(step){}
		constexpr Range(T start,T stop)noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>):start(start),stop(stop),step(1){}
		constexpr Range(T stop)noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>):start(0),stop(stop),step(1){}
		constexpr T operator[](size_t index)const noexcept(noexcept(start+index*step)){
			return start+index*step;			
		}
		constexpr size_t size()const noexcept(is_scalar_v<T> && is_scalar_v<U>){
			if constexpr(is_signed_v<U>){
				if((step>=0 && start>=stop) || (step<0 && start<stop)) return 0;
			}else{
				if(start>=stop) return 0;
			}
			return U(stop-start)/step + size_t((stop-start)%step!=0);
		}
		struct iterator{
			using iterator_category=random_access_iterator_tag;
			using value_type=remove_cv_t<T>;
			using difference_type=ptrdiff_t;
			using pointer=const T*;
			using reference=value_type;
			T pos;U step;
			constexpr iterator(T pos,U step)noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>):pos(pos),step(step){}
			constexpr reference operator*()noexcept{
				return pos;
			}
			constexpr reference operator[](difference_type n)noexcept(noexcept(pos+n*step)){
				return pos+n*step;
			}
			constexpr iterator& operator++()noexcept(noexcept(pos+=step)){
				pos+=step;
				return *this;
			}
			constexpr iterator operator++(int)noexcept(noexcept(pos+=step) && noexcept(iterator(pos-step,step))){
				pos+=step;
				return iterator(pos-step,step);
			}
			constexpr iterator& operator--()noexcept(noexcept(pos-=step)){
				pos-=step;
				return *this;
			}
			constexpr iterator operator--(int)noexcept(noexcept(pos-=step) && noexcept(iterator(pos+step,step))){
				pos-=step;
				return iterator(pos+step,step);
			}
			constexpr iterator& operator+=(difference_type n)noexcept(noexcept(pos+=step*n)){
				pos+=step*n;
				return *this;
			}
			constexpr iterator& operator-=(difference_type n)noexcept(noexcept(pos-=step*n)){
				pos-=step*n;
				return *this;
			}
			constexpr iterator operator+(difference_type n)const noexcept(noexcept(iterator(pos+step*n,step))){
				return iterator(pos+step*n,step);
			}
			constexpr iterator operator-(difference_type n)const noexcept(noexcept(iterator(pos-step*n,step))){
				return iterator(pos-step*n,step);
			}
			constexpr bool operator!=(iterator other)const noexcept{
				if constexpr(U(-1)>U()){
					return this->pos<other.pos;
				}else{
					return step>=U() ? pos<other.pos : pos>other.pos;			
				}
			}
		};
		constexpr iterator begin()const noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>){
			return iterator(start,step);
		}
		constexpr iterator end()const noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>){
			return iterator(stop,step);
		}
	};
	template<typename T,typename U> Range(T,U)->Range<T,U>;
    template<typename Out,typename T,typename U>
    Out& operator<<(Out& stream,const Range<T,U>& range){
        stream<<'[';
        auto it(range.begin());
        auto it_end(range.end());
        while(it!=it_end){
            stream<<*it;
            if(++it!=it_end)
                stream<<", ";
        }
        return stream<<']';
    }
}