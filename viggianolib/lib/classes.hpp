#pragma once
#if (defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_) || defined(_INC_STDIO)) && defined(SHOWREMAININGOBJECTS)
#define SETCOUNTERS		int remainingControlBlocks=0;\
						int remainingListNodes=0;
#define INCCTRLBLOCKS				remainingControlBlocks++;
#define DECCTRLBLOCKS				remainingControlBlocks--;
#define REMAININGCTRLBLOCKS 		printf("\nREMAINING CONTROL BLOCKS: %d\n",remainingControlBlocks);
#define INCLNODES					remainingListNodes++;
#define DECLNODES					remainingListNodes--;
#define REMAININGLNODES				printf("\nREMAINING LIST NODES: %d\n",remainingListNodes);
#else
#define SETCOUNTERS
#define INCCTRLBLOCKS
#define DECCTRLBLOCKS
#define REMAININGCTRLBLOCKS
#define INCLNODES
#define DECLNODES
#define REMAININGLNODES
#endif
SETCOUNTERS

namespace mpv{
	struct byte{
		union{
			unsigned char uval;
			signed char val;
		};
		bool get(unsigned char bit_index)const{
			switch(bit_index){
				case 0: return val & 0b10000000;
				case 1: return val & 0b01000000;
				case 2: return val & 0b00100000;
				case 3: return val & 0b00010000;
				case 4: return val & 0b00001000;
				case 5: return val & 0b00000100;
				case 6: return val & 0b00000010;
				case 7: return val & 0b00000001;
				default: throw "index should be <8";
			}
		}
		template<unsigned char index>
		inline constexpr bool get()const{
			static_assert(index<8,"index should be <8");
			if constexpr	 (index==0) return val & 0b10000000;
			else if constexpr(index==1) return val & 0b01000000;
			else if constexpr(index==2) return val & 0b00100000;
			else if constexpr(index==3) return val & 0b00010000;
			else if constexpr(index==4) return val & 0b00001000;
			else if constexpr(index==5) return val & 0b00000100;
			else if constexpr(index==6) return val & 0b00000010;
			else if constexpr(index==7) return val & 0b00000001;
		}
		operator unsigned char()const{
			return val;
		}
		byte(unsigned char val):val(val){}
		void operator=(unsigned char new_val){
			val=new_val;
		}
		template<unsigned char size>
		unsigned char get_right_bits()const{
			static_assert(size<=8,"index should be <8");
			if constexpr(size==0) return 0;
			else if constexpr(size==1) return uval & 0b00000001;
			else if constexpr(size==2) return uval & 0b00000011;
			else if constexpr(size==3) return uval & 0b00000111;
			else if constexpr(size==4) return uval & 0b00001111;
			else if constexpr(size==5) return uval & 0b00011111;
			else if constexpr(size==6) return uval & 0b00111111;
			else if constexpr(size==7) return uval & 0b01111111;
			else if constexpr(size==8) return uval & 0b11111111;
		}
		template<unsigned char size>
		unsigned char get_left_bits()const{
			static_assert(size<=8,"size should be <=8");
			return uval>>(8-size);
		}
	};
    template<typename T,T P1,T P2> struct params{
        static constexpr T p1=P1;
        static constexpr T p2=P2;
    };
    template<typename T>
    class less{
        public:
            bool operator()(const T& a,const T& b)const{
                return a<b;
            }
    };
	struct reserve_tag{};
	struct setSize{};
	//	^^^^^^ tags
	template<typename T> struct Wrap{T val;};
	template<typename T1,typename T2>
	struct Pair COUNT_IT{
		T1 d1;
		T2 d2;
		Pair()=default;
		template<typename T,typename D>
		Pair(T&& data1,D&& data2):d1((T&&)data1),d2((D&&)data2){}
	};
	template<typename Out,typename T1,typename T2>
	Out& operator<<(Out& stream,const Pair<T1,T2>& pair){
		stream<<'('<<pair.d1<<','<<pair.d2<<')';
		return stream;
	}
	template<typename T>
	struct default_types{
		using value_type=T;
		using size_type=size_t;
		using difference_type=ptrdiff_t;
		using pointer=T*;
		using const_pointer=const T*;
		using reference=T&;
		using const_reference=const T&;
	};
	template<typename Types>
	class random_access_iterator{
		private:
			typename Types::pointer ptr;
		public:
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::pointer;
			using reference=typename Types::reference;
			random_access_iterator(pointer ptr):ptr(ptr){}
			reference operator*(){
				return *ptr;
			}
			pointer operator->(){
				return ptr;
			}
			random_access_iterator& operator++(){
				++ptr;
				return *this;
			}
			random_access_iterator operator++(int){
				random_access_iterator aux=*this;
				++ptr;
				return aux;
			}
			random_access_iterator& operator--(){
				--ptr;
				return *this;
			}
			random_access_iterator operator--(int){
				random_access_iterator aux=*this;
				--ptr;
				return aux;
			}
			random_access_iterator operator+(difference_type n){
				return random_access_iterator(this->ptr+n);
			}
			random_access_iterator operator-(difference_type n){
				return random_access_iterator(this->ptr-n);
			}
			random_access_iterator& operator+=(difference_type n){
				ptr+=n;
				return *this;
			}
			random_access_iterator& operator-=(difference_type n){
				ptr-=n;
				return *this;
			}
			bool operator==(const random_access_iterator& other)const{
				return this->ptr==other.ptr;
			}
			bool operator!=(const random_access_iterator& other)const{
				return this->ptr!=other.ptr;
			}
			bool operator<=(const random_access_iterator& other)const{
				return this->ptr<=other.ptr;
			}
			bool operator>=(const random_access_iterator& other)const{
				return this->ptr>=other.ptr;
			}
			bool operator<(const random_access_iterator& other)const{
				return this->ptr<other.ptr;
			}
			bool operator>(const random_access_iterator& other)const{
				return this->ptr>other.ptr;
			}
			template<typename> friend class const_random_access_iterator;
	};
	template<typename Types>
	class const_random_access_iterator{
		private:
			typename Types::const_pointer ptr;
		public:
			using value_type=typename Types::value_type;
			using difference_type=typename Types::difference_type;
			using pointer=typename Types::const_pointer;
			using reference=typename Types::const_reference;
			const_random_access_iterator(pointer ptr):ptr(ptr){}
			const_random_access_iterator(random_access_iterator<Types> nonconst_it):ptr(nonconst_it.ptr){}
			reference operator*(){
				return *ptr;
			}
			pointer operator->(){
				return ptr;
			}
			const_random_access_iterator& operator++(){
				++ptr;
				return *this;
			}
			const_random_access_iterator operator++(int){
				const_random_access_iterator aux=*this;
				++ptr;
				return aux;
			}
			const_random_access_iterator& operator--(){
				--ptr;
				return *this;
			}
			const_random_access_iterator operator--(int){
				const_random_access_iterator aux=*this;
				--ptr;
				return aux;
			}
			const_random_access_iterator operator+(difference_type n){
				return const_random_access_iterator(this->ptr+n);
			}
			const_random_access_iterator operator-(difference_type n){
				return const_random_access_iterator(this->ptr-n);
			}
			const_random_access_iterator& operator+=(difference_type n){
				ptr+=n;
				return *this;
			}
			const_random_access_iterator& operator-=(difference_type n){
				ptr-=n;
				return *this;
			}
			bool operator==(const const_random_access_iterator& other)const{
				return this->ptr==other.ptr;
			}
			bool operator!=(const const_random_access_iterator& other)const{
				return this->ptr!=other.ptr;
			}
			bool operator<=(const const_random_access_iterator& other)const{
				return this->ptr<=other.ptr;
			}
			bool operator>=(const const_random_access_iterator& other)const{
				return this->ptr>=other.ptr;
			}
			bool operator<(const const_random_access_iterator& other)const{
				return this->ptr<other.ptr;
			}
			bool operator>(const const_random_access_iterator& other)const{
				return this->ptr>other.ptr;
			}
	};
}
#if defined(_ATOMIC_) || defined(_GLIBCXX_ATOMIC)

#endif
#include"def_alloc_macros.hpp"
#include"Classes/Wrapper.hpp"
#include"Classes/EmptyClassOptimizations.hpp"
#include"Classes/Iterables.hpp"
#include"Classes/Optional.hpp"
#include"Classes/Stringsv2.hpp"
#include"Classes/List.hpp"
#include"Classes/Vectorv2.hpp"
//#include"Classes/ArrayListv2.hpp"
#include"Classes/Stacks_Queues.hpp"
#include"Classes/AutoPtrv2/PointerBase.hpp"
#include"Classes/Red-Black Treev2.hpp"
#include"Classes/Map.hpp"
#include"Classes/Set.hpp"
#include"Classes/BitList.hpp"
#include"Classes/Array.hpp"
#include"Classes/Matrix.hpp"
#include"Classes/BigInt.hpp"
#include"undef_alloc_macros.hpp"
