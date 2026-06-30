#pragma once
namespace mpv{
	template<typename T,bool=is_empty_v<T> && !is_final_v<T>> 
	struct EBCO:T{	// Empty Base Class Optimization
		private:
			using base_type=T;
		public:
			constexpr explicit EBCO()noexcept(is_nothrow_default_constructible_v<T>):T(){}
			template<typename U,enable_if_t<!is_same_v<remove_cvref_t<U>,EBCO>,int> = 0> constexpr explicit EBCO(U&& val)noexcept(is_nothrow_constructible_v<T,U>):T(static_cast<U&&>(val)){}
			constexpr T& get_val()noexcept{ return *this; }
			constexpr const T& get_val()const noexcept{ return *this; }
	};
	template<typename T>
	struct EBCO<T,false>{	// EBCO not possible
		private:
			T val;
		public:
			constexpr explicit EBCO()noexcept(is_nothrow_default_constructible_v<T>):val(){}
			template<typename U,enable_if_t<!is_same_v<remove_cvref_t<U>,EBCO>,int> = 0> constexpr explicit EBCO(U&& val)noexcept(is_nothrow_constructible_v<T,U>):val(static_cast<U&&>(val)){}
			constexpr T& get_val()noexcept{ return val;}
			constexpr const T& get_val()const noexcept{ return val; }
	};

	struct arg1_tag{};// indica que el primer argumento no es parte de los argumentos variadicos, sino que va al primer elemento
	template<typename T1,typename T2,bool=is_empty_v<T1> && !is_final_v<T1>>
	struct CompressedPair final:private T1{
		T2 V2;
		template<typename A,typename... B>
		constexpr CompressedPair(arg1_tag,A&& v1,B&&... v2)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_constructible<T2,B...>>):T1(static_cast<A&&>(v1)),V2(static_cast<B&&>(v2)...){}
		template<typename... B>
		constexpr explicit CompressedPair(B&&... v2)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_constructible<T2,B...>>):T1(),V2(static_cast<B&&>(v2)...){}
		constexpr T1& getV1()noexcept{ return *this; }
		constexpr const T1& getV1()const noexcept{ return const_cast<CompressedPair&>(*this); }
	};
	template<typename T1,typename T2>
	struct CompressedPair<T1,T2,false>final{
		T1 V1;
		T2 V2;
		template<typename A,typename... B>
		constexpr CompressedPair(arg1_tag,A&& v1,B&&... v2)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_constructible<T2,B...>>):V1(static_cast<A&&>(v1)),V2(static_cast<B&&>(v2)...){}
		template<typename... B>
		constexpr CompressedPair(B&&... v2)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_constructible<T2,B...>>):V1(),V2(static_cast<B&&>(v2)...){}
		constexpr T1& getV1()noexcept{ return V1; }
		constexpr const T1& getV1()const noexcept{ return V1; }
	};
	struct arg1and2_tag{};// indica que el primer y segundo argumento no son parte de los argumentos variadicos, sino que van al primer y segundo elemento
	struct arg2_tag{};// indica que el primer argumento no es parte de los argumentos variadicos, sino que va al segundo elemento
	template<typename T1,typename T2,typename T3,bool=is_empty_v<T1> && !is_final_v<T1>>
	struct CompressedTriple final:private T1{
		CompressedPair<T2,T3> pair;
		template<typename A,typename B,typename... C>
		constexpr CompressedTriple(arg1and2_tag,A&& v1,B&& v2,C&&... v3)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_constructible<T2,B>,is_nothrow_constructible<T3,C...>>):T1(static_cast<A&&>(v1)),pair(arg1_tag{},static_cast<B&&>(v2),static_cast<C&&>(v3)...){}
		template<typename A,typename... C>
		constexpr CompressedTriple(arg1_tag,A&& v1,C&&... v3)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_default_constructible<T2>,is_nothrow_constructible<T3,C...>>):T1(static_cast<A&&>(v1)),pair(static_cast<C&&>(v3)...){}
		template<typename B,typename... C>
		constexpr CompressedTriple(arg2_tag,B&& v2,C&&... v3)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_constructible<T2,B>,is_nothrow_constructible<T3,C...>>):T1(),pair(arg1_tag{},static_cast<B&&>(v2),static_cast<C&&>(v3)...){}
		template<typename... C>
		constexpr CompressedTriple(C&&... v3)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_default_constructible<T2>,is_nothrow_constructible<T3,C...>>):T1(),pair(static_cast<C&&>(v3)...){}
		constexpr T1& getV1()noexcept{return *this;}
		constexpr T2& getV2()noexcept{return pair.getV1();}
		constexpr T3& getV3()noexcept{return pair.V2;}
		constexpr const T1& getV1()const noexcept{return const_cast<CompressedTriple&>(*this);}
		constexpr const T2& getV2()const noexcept{return pair.getV1();}
		constexpr const T3& getV3()const noexcept{return pair.V2;}
	};
	template<typename T1,typename T2,typename T3>
	struct CompressedTriple<T1,T2,T3,false> final{
		T1 V1;
		CompressedPair<T2,T3> pair;
		template<typename A,typename B,typename... C>
		constexpr CompressedTriple(arg1and2_tag,A&& v1,B&& v2,C&&... v3)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_constructible<T2,B>,is_nothrow_constructible<T3,C...>>):V1(static_cast<A&&>(v1)),pair(arg1_tag{},static_cast<B&&>(v2),static_cast<C&&>(v3)...){}
		template<typename A,typename... C>
		constexpr CompressedTriple(arg1_tag,A&& v1,C&&... v3)noexcept(conjunction_v<is_nothrow_constructible<T1,A>,is_nothrow_default_constructible<T2>,is_nothrow_constructible<T3,C...>>):V1(static_cast<A&&>(v1)),pair(static_cast<C&&>(v3)...){}
		template<typename B,typename... C>
		constexpr CompressedTriple(arg2_tag,B&& v2,C&&... v3)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_constructible<T2,B>,is_nothrow_constructible<T3,C...>>):V1(),pair(arg1_tag{},static_cast<B&&>(v2),static_cast<C&&>(v3)...){}
		template<typename... C>
		constexpr CompressedTriple(C&&... v3)noexcept(conjunction_v<is_nothrow_default_constructible<T1>,is_nothrow_default_constructible<T2>,is_nothrow_constructible<T3,C...>>):T1(),pair(static_cast<C&&>(v3)...){}
		constexpr T1& getV1()noexcept{return V1;}
		constexpr T2& getV2()noexcept{return pair.getV1();}
		constexpr T3& getV3()noexcept{return pair.V2;}
		constexpr const T1& getV1()const noexcept{return V1;}
		constexpr const T2& getV2()const noexcept{return pair.getV1();}
		constexpr const T3& getV3()const noexcept{return pair.V2;}
	};
}