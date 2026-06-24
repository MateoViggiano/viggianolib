#pragma once

namespace mpv{
	template<typename T>
	constexpr enable_if_t<is_scalar_v<T>,bool> is_all_bits_zero(const T& val){
		constexpr T zero{};
		if constexpr(USE_MEMCPY){
			return memcmp(&val,&zero,sizeof(T))==0;
		}
		else{
			for(size_t i=0;i<sizeof(T);i++){
				if(((char*)&val)[i]!=((char*)&val)[i]) return false;
			}
			return true;
		}
	}
	template<typename T,size_t N>
	constexpr enable_if_t<is_swappable_v<T>> swap(T(&a)[N],T(&b)[N])noexcept(is_nothrow_swappable_v<T>){
		if(&a!=&b){
			for(T* afirst=a,* bfirst=b,* alast=a+N;afirst!=alast;afirst++,bfirst++)
				swap(*afirst,*bfirst);
		}
		
	}
	template<typename T>
	constexpr enable_if_t<is_move_constructible_v<T> && is_move_assignable_v<T>> swap(T& v1,T& v2)noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>){
		T aux=static_cast<T&&>(v1);
		v1=static_cast<T&&>(v2);
		v2=static_cast<T&&>(aux);
	}
#ifndef abs
	template<typename T>
	constexpr T abs(T n){
		if constexpr(is_unsigned_v<T>) return n;
		else return n<0? -n : n;
	}
#endif
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,T> mod(T a, T b){
        if((a>=0 && b>=0) || (a<0 && b<0)){
            return a%b;
        }
        else 
            return a%b==0 ? 0 : b+a%b;
    }
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,bool> divides_to(T a, T b){
        return b==0 ? true : b%a==0;
    }
	template<typename T>
	constexpr enable_if_t<is_integral_v<T>,T> gcd(T a,T b){
		if constexpr(is_signed_v<T>){
			if(a<0) a=-a;
			if(b<0) b=-b;
		}
		while(a!=0 && b!=0){
			if(a>b) a=a%b;
			else b=b%a;
		}
		return a==0 ? b : a; 
	}
	template<typename T>
	constexpr enable_if_t<is_integral_v<T>,T> lcm(T a,T b){
		return abs(a*b)/gcd(a,b);
	}
	inline constexpr double PI=3.1415926535897932384626433832795028841971693993751058209749445923078164062862;
	inline constexpr double sin(double a){// POLINOMIO DE TAYLOR DE SENO CENTRADO EN 0
		a-=2*PI*((long long)(a/(2*PI)));
		if(a>PI) a-=2*PI;
		else if(a<-PI) a+=2*PI;
		bool neg=false;
		if(a>PI/2){
			neg=true;
			a-=PI;
		}
		else if(a<-PI/2){
			neg=true;
			a+=PI;
		}
		double fact=1;
		double pow=1,ret=0;
		for(size_t i=1;i<20;i++){
			fact*=i;
			pow*=a;
			if(i%4==1){
				ret+=pow/fact;
			}		
			else if(i%4==3){
				ret-=pow/fact;
			}
		}
		return neg? -ret : ret;
	}
	inline constexpr double cos(double a){// POLINOMIO DE TAYLOR DE COSENO CENTRADO EN 0
		a-=2*PI*((long long)(a/(2*PI)));
		if(a>PI) a-=2*PI;
		else if(a<-PI) a+=2*PI;
		bool neg=false;
		if(a>PI/2){
			neg=true;
			a-=PI;
		}
		else if(a<-PI/2){
			neg=true;
			a+=PI;
		}
		double fact=1;
		double pow=1,ret=1;
		for(size_t i=1;i<23;i++){
			fact*=i;
			pow*=a;
			if(i%4==0){
				ret+=pow/fact;
			}		
			else if(i%4==2){
				ret-=pow/fact;
			}
		}
		return neg? -ret : ret;
	}
	inline constexpr double tan(double a){
		return sin(a)/cos(a);
	}
	template<unsigned long long Precision=1000000000000>
	constexpr double sqrt(double radicand){
		bool inverse=false;
		if(radicand<1){
			radicand=1/radicand;
			inverse=true;
		}
		double max=radicand,min=0,n=max/2;
		while(true){
			double attempt=n*n;
			if(abs(attempt-radicand)<1/static_cast<double>(Precision)){
				return inverse? 1/n : n;
			}
			else if(attempt>radicand){
				max=n;
				n=min+(max-min)/2;
			}
			else/*(attempt<radicand)*/{
				min=n;
				n=min+(max-min)/2;
			}
		}
	}
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,T> factorial(T n){
        T res=1;
        while(n) res*=n--;
        return res;
    }
	template<typename T>
	constexpr enable_if_t<is_integral_v<T>,T> variations(T a,T b){
		T res=1;
		T c=a-b;
		while(c++<a){
			res*=c;
		}
		return res;
	}
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,T> combinations(T a,T b){
		return variations(a,b)/factorial(b);
    }
	template<typename T,unsigned long long Precision=100000000000000>
	constexpr bool eq(T x,T y){
		if constexpr(is_floating_point_v<T>) return x-y>=0? (x-y)<1./Precision : (x-y)>-1./Precision;
		else return x==y;
	}
    template<typename T,unsigned long long Precision=100000000000000>
    constexpr bool is_zero(T v){
		//if constexpr(is_floating_point_v<T>) return v>=0? v<1e-15 : v>-1e-15;
		if constexpr(is_floating_point_v<T>) return v>=0? v<1./Precision : v>-1./Precision;
		else return v==0;
    }
	template<typename T>
	constexpr T minus1pow(T e){
		return e%2==0? 1 : -1;
	}
	template<typename T>
	constexpr bool get_bit(unsigned n,T v){//from left
		v<<=n;
		v>>=sizeof(T)*8-1;// +n-n
		v<<=sizeof(T)*8-1-n;
		return v;
	}
	template<typename T>
	constexpr bool get_bit_from_right(unsigned n,T v){
		v>>=n;
		v<<=sizeof(T)*8-1;
		v>>=sizeof(T)*8-1-n;
		return v;
	}
	template<typename Lambda,typename Iterable>
	constexpr enable_if_t<is_iterable_v<Iterable>,bool> any(const Iterable&& iterable,Lambda&& func=Lambda{}){
		for(const auto& x:iterable){
			if(func(x)) return true;
		}
		return false;
	}
	template<typename Lambda,typename Iterable>
	constexpr enable_if_t<is_iterable_v<Iterable>> foreach(Iterable&& iterable,Lambda&& func=Lambda{}){
		for(auto& x:iterable)
			func(x);
	}
	template<typename Iterable,typename D>
	constexpr enable_if_t<is_iterable_v<Iterable>,bool> contains(const Iterable&& iterable,const D& val){
		for(const auto& x:iterable){
			if(x==val) return true;
		}
		return false;
	}
	template<typename Iterable,typename D>
	constexpr enable_if_t<is_iterable_v<Iterable>,typename remove_cvref_t<Iterable>::size_type> count(Iterable&& iterable,const D& val){
		typename remove_cvref_t<Iterable>::size_type counter=0;
		for(const auto& x:iterable){
			if(x==val) counter++;
		}
		return counter;
	}
	template<typename Out,typename T,size_t size>
	enable_if_t<!is_same_v<remove_cv_t<T>,char> && !is_same_v<remove_cv_t<T>,wchar_t>,Out&> operator<<(Out& stream,T (&a)[size]){
        stream<<'[';
        for(size_t i=0;i<size;i++){
			if constexpr(is_same_v<remove_cv_t<T>,unsigned char> || is_same_v<remove_cv_t<T>,signed char>){
				stream<<static_cast<short>(a[i]);
			}
            else stream<<a[i];
			if(i<size-1) stream<<", ";
        }
        stream<<']';
		return stream;
	}
}
