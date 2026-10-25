#pragma once
namespace mpv{
	template<typename Container>
	class Viewer{
		private:
			Container& cont;
		public:
		using value_type=typename Container::value_type;
		using size_type=typename Container::size_type;
		using difference_type=typename Container::difference_type;
		using pointer=typename Container::pointer;
		using const_pointer=typename Container::const_pointer;
		using reference=typename Container::reference;
		using const_reference=typename Container::const_reference;
		using iterator=typename Container::iterator;
		using const_iterator=typename Container::const_iterator;
		constexpr Viewer(Container& cont):cont(cont){};
		constexpr Viewer(Container&& cont)=delete;
		constexpr Viewer(const Container&& cont)=delete;
		constexpr const Container& get_container()const noexcept{ return cont; }
		constexpr size_type size()const noexcept{
			return cont.size();
		}
		constexpr bool empty()const noexcept{
			return cont.empty();
		}
		constexpr const_iterator find(const value_type& val)const noexcept(noexcept(mpv::find(this->begin(),this->end(),val))){
			return mpv::find(this->begin(),this->end(),val);
		}
		constexpr iterator find(const value_type& val)noexcept(noexcept(mpv::find(this->begin(),this->end(),val))){
			return mpv::find(this->begin(),this->end(),val);
		}
		constexpr size_type count(const value_type& val)const noexcept(noexcept(mpv::count<const_iterator,value_type,size_type>(this->begin(),this->end(),val))){
			return mpv::count<const_iterator,value_type,size_type>(this->begin(),this->end(),val);
		}
		constexpr bool contains(const value_type& val)const noexcept(noexcept(mpv::contains<const_iterator,value_type>(this->begin(),this->end(),val))){
			return mpv::contains<const_iterator,value_type>(this->begin(),this->end(),val);
		}
		constexpr size_type index_of(const value_type& val)const noexcept(noexcept(mpv::index_of<const_iterator,value_type,size_type>(this->begin(),this->end(),val))){
			return mpv::index_of<const_iterator,value_type,size_type>(this->begin(),this->end(),val);
		}
		template<typename Lambda>
		constexpr bool any(Lambda&& func=Lambda{})const noexcept(noexcept(fake_copy_init<bool>(func(declval<const_reference>())))){
			for(const_reference x:cont)
				if(func(x))return true;
			return false;
		}
		template<typename Lambda>
		constexpr void foreach(Lambda&& func=Lambda{})const noexcept(noexcept(const_cast<const Container&>(cont).foreach(static_cast<Lambda&&>(func)))){
			const_cast<const Container&>(cont).foreach(static_cast<Lambda&&>(func));
		}
		template<typename Lambda>
		constexpr void foreach(Lambda&& func=Lambda{})noexcept(noexcept(cont.foreach(static_cast<Lambda&&>(func)))){
			cont.foreach(static_cast<Lambda&&>(func));
		}
		constexpr iterator begin()noexcept{
			return cont.begin();
		}
		constexpr iterator end()noexcept{
			return cont.end();
		}
		constexpr const_iterator begin()const noexcept{
			return const_cast<const Container&>(cont).begin();
		}
		constexpr const_iterator end()const noexcept{
			return const_cast<const Container&>(cont).end();
		}
	};
	template<typename Container>
	class ConstViewer{
		private:
			const Container& cont;
		public:
		using value_type=typename Container::value_type;
		using size_type=typename Container::size_type;
		using difference_type=typename Container::difference_type;
		using pointer=typename Container::const_pointer;
		using const_pointer=typename Container::const_pointer;
		using reference=typename Container::const_reference;
		using const_reference=typename Container::const_reference;
		using iterator=typename Container::const_iterator;
		using const_iterator=typename Container::const_iterator;
		constexpr ConstViewer(const Container& cont):cont(cont){};
		constexpr ConstViewer(Container&& cont)=delete;
		constexpr ConstViewer(const Container&& cont)=delete;
		constexpr const Container& get_container()const noexcept{ return cont; }
		constexpr size_type size()const noexcept{
			return cont.size();
		}
		constexpr bool empty()const noexcept{
			return cont.empty();
		}
		constexpr const_iterator find(const value_type& val)const noexcept(noexcept(mpv::find(this->begin(),this->end(),val))){
			return mpv::find(this->begin(),this->end(),val);
		}
		constexpr size_type count(const value_type& val)const noexcept(noexcept(mpv::count<const_iterator,value_type,size_type>(this->begin(),this->end(),val))){
			return mpv::count<const_iterator,value_type,size_type>(this->begin(),this->end(),val);
		}
		constexpr bool contains(const value_type& val)const noexcept(noexcept(mpv::contains<const_iterator,value_type>(this->begin(),this->end(),val))){
			return mpv::contains<const_iterator,value_type>(this->begin(),this->end(),val);
		}
		constexpr size_type index_of(const value_type& val)const noexcept(noexcept(mpv::index_of<const_iterator,value_type,size_type>(this->begin(),this->end(),val))){
			return mpv::index_of<const_iterator,value_type,size_type>(this->begin(),this->end(),val);
		}
		template<typename Lambda>
		constexpr bool any(Lambda&& func=Lambda{})const noexcept(noexcept(fake_copy_init<bool>(func(*cont.begin())))){
			for(const_reference x:cont)
				if(func(x))return true;
			return false;
		}
		template<typename Lambda>
		constexpr void foreach(Lambda&& func=Lambda{})const noexcept(noexcept(cont.foreach(static_cast<Lambda&&>(func)))){
			cont.foreach(static_cast<Lambda&&>(func));
		}
		constexpr const_iterator begin()const noexcept{
			return cont.begin();
		}
		constexpr const_iterator end()const noexcept{
			return cont.end();
		}
	};
	struct Fibonacci{
		size_t count,max;
		constexpr Fibonacci(size_t max)noexcept:count(0),max{max}{}
		constexpr Fibonacci(size_t start,size_t max)noexcept:count(start),max{max}{}
		struct iterator{
			using iterator_category=input_iterator_tag;
			using value_type=size_t;
			using difference_type=ptrdiff_t;
			using pointer=size_t*;
			using reference=size_t;
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
		constexpr Range()noexcept:start{},stop{},step{}{}
		constexpr Range(T start,T stop,U step)noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>):start(start),stop(stop),step(step){}
		constexpr Range(T start,T stop)noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>):start(start),stop(stop),step(1){}
		constexpr Range(T stop)noexcept(is_nothrow_copy_constructible_v<T> && is_nothrow_copy_constructible_v<U>):start(0),stop(stop),step(1){}
		constexpr T operator[](size_t index)const noexcept(noexcept(start+U(index)*step)){
			return start+U(index)*step;			
		}
		constexpr size_t size()const noexcept(is_scalar_v<T> && is_scalar_v<U>){
			if constexpr(is_signed_v<U>){
				if((step>=0 && start>=stop) || (step<0 && start<stop)) return 0;
			}else{
				if(start>=stop) return 0;
			}
			return (U(stop)-U(start))/step + ((U(stop)-U(start))%step!=0);
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
			constexpr reference operator[](difference_type n)noexcept(noexcept(pos+step*n)){
				return pos+step*n;
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
			friend constexpr iterator operator+(difference_type n,const iterator& it)noexcept(noexcept(iterator(it.pos+it.step*n,it.step))){
				return iterator(it.pos+it.step*n,it.step);
			}
			constexpr iterator operator-(difference_type n)const noexcept(noexcept(iterator(pos-step*n,step))){
				return iterator(pos-step*n,step);
			}
			constexpr difference_type operator-(const iterator& other)const noexcept{
				return (difference_type(this->pos)-difference_type(other.pos))/difference_type(step);
			}
			constexpr bool operator!=(iterator other)const noexcept{
				return pos!=other.pos;
			}
			constexpr bool operator==(iterator other)const noexcept{
				return pos==other.pos;
			}
			constexpr bool operator<(iterator other)const noexcept{
				return step>=0 ? pos<other.pos : pos>other.pos;
			}
			constexpr bool operator>(iterator other)const noexcept{
				return step>=0 ? pos>other.pos : pos<other.pos;
			}
			constexpr bool operator<=(iterator other)const noexcept{
				return step>=0 ? pos<=other.pos : pos>=other.pos;
			}
			constexpr bool operator>=(iterator other)const noexcept{
				return step>=0 ? pos>=other.pos : pos<=other.pos;
			}
		};
		constexpr iterator begin()const noexcept(noexcept(fake_copy_init<iterator>(iterator(start,step)))){
			return iterator(start,step);
		}
		constexpr iterator end()const noexcept(noexcept(iterator(start+U(size())*step,step))){
			return iterator(start+U(size())*step,step);
		}
	};
	template<typename T,typename U> Range(T,U)->Range<T,U>;
    template<typename Out,typename T,typename U>
    Out& operator<<(Out& stream,const Range<T,U>& range){
        stream<<"[";
        auto it(range.begin());
        auto it_end(range.end());
        while(it!=it_end){
            stream<<""<<*it;
            if(++it!=it_end)
                stream<<", ";
        }
		stream<<"]";
        return stream;
    }
}