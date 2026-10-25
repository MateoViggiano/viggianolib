#pragma once

namespace mpv{
	template<typename T>
	constexpr const T& max(const T& x)noexcept{
		return x;
	}
	template<typename T>
	constexpr const T& max(const T& x,const T& y)noexcept(noexcept(fake_copy_init<bool>(x>y))){
		return x>y?x:y;
	}
	template<typename T,typename... Args>
	constexpr const T& max(const T& x,const T& y,const Args&... args)noexcept(noexcept(fake_copy_init<bool>(x>y))){
		return max(x,max(y,args...));
	}
	template<typename T>
	constexpr const T& min(const T& x)noexcept{
		return x;
	}
	template<typename T>
	constexpr const T& min(const T& x,const T& y)noexcept(noexcept(fake_copy_init<bool>(x<y))){
		return x<y?x:y;
	}
	template<typename T,typename... Args>
	constexpr const T& min(const T& x,const T& y,const Args&... args)noexcept(noexcept(fake_copy_init<bool>(x<y))){
		return min(x,min(y,args...));
	}
	template<typename T>
	constexpr enable_if_t<is_scalar_v<T>,bool> is_all_bits_zero(const T& val)noexcept{
		constexpr T zero{};
		if constexpr(USE_MEMCPY){
			return memcmp(&val,&zero,sizeof(T))==0;
		}
		else{
			for(size_t i=0;i<sizeof(T);i++){
				if(((char*)&val)[i]!=((char*)&zero)[i]) return false;
			}
			return true;
		}
	}
	template<typename T>
	constexpr enable_if_t<is_move_constructible_v<T> && is_move_assignable_v<T>> swap(T& v1,T& v2)noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>){
		T aux(static_cast<T&&>(v1));
		v1=static_cast<T&&>(v2);
		v2=static_cast<T&&>(aux);
	}
	template<typename T,size_t N>
	constexpr enable_if_t<is_swappable_v<T>> swap(T(&a)[N],T(&b)[N])noexcept(is_nothrow_swappable_v<T>){
		if(&a!=&b){
			for(T* afirst=a,* bfirst=b,* alast=a+N;afirst!=alast;afirst++,bfirst++)
				swap(*afirst,*bfirst);
		}
		
	}
#ifndef abs
	template<typename T>
	constexpr T abs(T n)noexcept{
		if constexpr(is_unsigned_v<T>) return n;
		else return n<0? -n : n;
	}
#endif
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,T> mod(T a, T b)noexcept{
        if((a>=0 && b>=0) || (a<0 && b<0)){
            return a%b;
        }
        else 
            return a%b==0 ? 0 : b+a%b;
    }
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,bool> divides_to(T a, T b)noexcept{// a shouldn't be 0
        return b==0 ? true : b%a==0;
    }
	template<typename T>
	constexpr enable_if_t<is_integral_v<T>,T> gcd(T a,T b)noexcept{
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
	constexpr enable_if_t<is_integral_v<T>,T> lcm(T a,T b)noexcept{
		T d=gcd(a,b);
		return d==0 ? 0 : abs((a/d)*b);
	}
	inline constexpr double PI=3.1415926535897932384626433832795028841971693993751058209749445923078164062862;
	inline constexpr double sin(double a)noexcept{// POLINOMIO DE TAYLOR DE SENO CENTRADO EN 0
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
	inline constexpr double cos(double a)noexcept{// POLINOMIO DE TAYLOR DE COSENO CENTRADO EN 0
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
	inline constexpr double tan(double a)noexcept{
		return sin(a)/cos(a);
	}
	template<typename T,unsigned long long Precision=10000000000000000>
	constexpr bool eq(T x,T y)noexcept(noexcept(fake_copy_init<bool>(x==y))){
		if constexpr(is_floating_point_v<T>) return x-y>=0? (x-y)<1./Precision : (x-y)>-1./Precision;
		else return x==y;
	}
    template<typename T,unsigned long long Precision=10000000000000000>
    constexpr bool is_zero(T v)noexcept(noexcept(fake_copy_init<bool>(v==0))){
		if constexpr(is_floating_point_v<T>) return v>=0? v<1./Precision : v>-1./Precision;
		else return v==0;
    }
#ifndef USE_BUILTIN_SQRT
#define USE_BUILTIN_SQRT 1
#endif
	template<unsigned long long Precision=1000000000000000000>
	constexpr double sqrt(double radicand)noexcept{
		if constexpr(USE_BUILTIN_SQRT) return __builtin_sqrt(radicand);
		if(radicand<0.) return 0./0.;
		else if(radicand<1e-300) return 0.;
		bool inverse=false;
		if(radicand<1){
			radicand=1/radicand;
			inverse=true;
		}
		double max=radicand,min=0.,n=max/2.;
		while(true){
			double attempt=n*n,prevn=n;
			if(abs(attempt-radicand)<1./Precision){
				return inverse? 1./n : n;
			}
			else if(attempt>radicand){
				max=n;
				n=min+(max-min)/2.;
			}
			else/*(attempt<radicand)*/{
				min=n;
				n=min+(max-min)/2.;
			}
			if(n==prevn) return inverse? 1./n : n;
		}
	}
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,T> factorial(T n)noexcept{
        T res=1;
        while(n) res*=n--;
        return res;
    }
	template<typename T>
	constexpr enable_if_t<is_integral_v<T>,T> variations(T a,T b)noexcept{
		T res=1;
		T c=a-b;
		while(c++<a){
			res*=c;
		}
		return res;
	}
    template<typename T>
    constexpr enable_if_t<is_integral_v<T>,T> combinations(T a,T b)noexcept{
		return variations(a,b)/factorial(b);
    }
	template<typename Cont,typename Cont2>
	constexpr void permutaciones_rec(Cont& vec,typename Cont::size_type i,Cont& sol,Cont2& sols){
		if(i==vec.size()){
			sols.push_back(sol);
		}
		else for(typename Cont::size_type j=i;j<vec.size();j++){
			sol[i]=vec[j];
// vec[j] ya fue usado, entonces lo 'escondo' en la posicion i y ademas dejo vec[i] en la posicion j que esta en rango para que la siguiente llamada lo pueda usar
			swap(vec[i],vec[j]);//							 ^ la cual no se va a usar en la proxima recursion
			permutaciones_rec(vec,i+1,sol,sols);// y hago recursion en i+1																				todo esto es cuando i!=j, en el caso de 
			swap(vec[i],vec[j]);// lo deshago para que el vector quede igual																		  i==j (la primera iteracion) vec[i] se hace un 
		}//																																			auto swap y queda atras en la proxima llamada recursiva
	}
	template<template<typename...> class Cont,typename T,typename Al,typename... Args>
	constexpr rebind_t<Cont<T,Al,Args...>,Cont<T,Al,Args...>,rebind_alloc<Al,Cont<T,Al,Args...>>,Args...> generate_permutations(Cont<T,Al,Args...> cont){
		rebind_t<Cont<T,Al,Args...>,Cont<T,Al,Args...>,rebind_alloc<Al,Cont<T,Al,Args...>>,Args...> sols;
		Cont<T,Al,Args...> sol(cont.size());
		permutaciones_rec(cont,typename Cont<T,Al,Args...>::size_type{0},sol,sols);
		return sols;
	}
	template<typename T>
	constexpr long long minus1pow(T e)noexcept{
		return e%2==0? 1 : -1;
	}
	template<typename T>
	constexpr bool get_bit(unsigned n,T v)noexcept{//from left
		v<<=n;
		v>>=sizeof(T)*8-1;// +n-n
		v<<=sizeof(T)*8-1-n;
		return v;
	}
	template<typename T>
	constexpr bool get_bit_from_right(unsigned n,T v)noexcept{
		v>>=n;
		v<<=sizeof(T)*8-1;
		v>>=sizeof(T)*8-1-n;
		return v;
	}
	template<typename Lambda,typename Iterable>
	constexpr enable_if_t<is_iterable_v<Iterable>,bool> any(Iterable&& iterable,Lambda&& func=Lambda{})noexcept(noexcept(fake_copy_init<bool>(func(mpv::declval<const remove_cvref_t<decltype(*begin(iterable))>&>())))){
		for(const auto& x:iterable){
			if(func(x)) return true;
		}
		return false;
	}
	template<typename Lambda,typename Iterable>
	constexpr enable_if_t<is_iterable_v<Iterable>> foreach(Iterable&& iterable,Lambda&& func=Lambda{})noexcept(noexcept(func(*begin(iterable)))){
		for(auto& x:iterable)
			func(x);
	}
	template<typename Iterable,typename D>
	constexpr enable_if_t<is_iterable_v<Iterable>,bool> contains(Iterable&& iterable,const D& val)noexcept(noexcept(fake_copy_init<bool>(mpv::declval<const remove_cvref_t<decltype(*begin(iterable))>&>()==val))){
		for(const auto& x:iterable){
			if(x==val) return true;
		}
		return false;
	}
	template<typename Iterable,typename D>
	constexpr enable_if_t<is_iterable_v<Iterable>,typename remove_cvref_t<Iterable>::size_type> count(Iterable&& iterable,const D& val)noexcept(noexcept(fake_copy_init<bool>(mpv::declval<const remove_cvref_t<decltype(*begin(iterable))>&>()==val))){
		typename remove_cvref_t<Iterable>::size_type counter=0;
		for(const auto& x:iterable){
			if(x==val) counter++;
		}
		return counter;
	}
	template<typename Out,typename T,size_t size>
	enable_if_t<!is_same_v<remove_cv_t<T>,char> && !is_same_v<remove_cv_t<T>,wchar_t>,Out&> operator<<(Out& stream,const T (&a)[size]){
        stream<<"[";
        for(size_t i=0;i<size;i++){
			if constexpr(is_same_v<remove_cv_t<T>,unsigned char> || is_same_v<remove_cv_t<T>,signed char>){
				stream<<""<<static_cast<short>(a[i]);
			}
            else stream<<""<<a[i];
			if(i<size-1) stream<<", ";
        }
        stream<<"]";
		return stream;
	}
}
