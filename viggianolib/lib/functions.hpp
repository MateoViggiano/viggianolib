#pragma once

namespace mpv{
	template<typename T>
	void swap(T& v1,T& v2){
		T aux=static_cast<T&&>(v1);
		v1=static_cast<T&&>(v2);
		v2=static_cast<T&&>(aux);
	}
	template<typename T>
	inline constexpr T abs(T n){
		if constexpr(is_unsigned_v<T>) return n;
		else return n<0? -n : n;
	}
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,T> mod(T a, T b){
        if((a>=0 && b>=0) || (a<0 && b<0)){
            return a%b;
        }
        else 
            return a%b==0 ? 0 : b+a%b;
    }
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,bool> divides(T a, T b){
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
	template<size_t Precision=1000000000000>
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
	template<typename T,size_t Precision=100000000000000>
	constexpr bool eq(T x,T y){
		if constexpr(is_floating_point_v<T>) return x-y>=0? (x-y)<1./Precision : (x-y)>-1./Precision;
		else return x==y;
	}
    template<typename T,size_t Precision=100000000000000>
    constexpr bool is_zero(T v){
		//if constexpr(is_floating_point_v<T>) return v>=0? v<1e-15 : v>-1e-15;
		if constexpr(is_floating_point_v<T>) return v>=0? v<1./Precision : v>-1./Precision;
		else return v==0;
    }
	template<typename T>
	constexpr signed char minus1pow(T e){
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

}
#include"classes.hpp"
namespace mpv{
	template<typename T>
	constexpr enable_if_t<is_integral_v<T>,Pair<T,T>> linear_comb_gcd(T a,T b){
		Pair<T,T> res;

	}
}