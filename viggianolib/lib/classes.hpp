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
		byte()=default;
		constexpr byte(unsigned char b)noexcept:uval(b){}
		constexpr void operator=(unsigned char new_val)noexcept{
			uval=new_val;
		}
		constexpr bool get(unsigned char index)const noexcept{//from left
			unsigned char v=uval<<index;
			v>>=7;
			v<<=7-index;
			return v;
		}
		template<unsigned char index>
		constexpr bool get()const noexcept{
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
		template<unsigned char index,unsigned char size>
		constexpr unsigned char get_num()const noexcept{
			static_assert(index<8,"index should be <8");
			unsigned char v=uval<<index;
			return v>>(8-size);
		}
		constexpr unsigned char get_num(unsigned char index,unsigned char size)const noexcept{
			unsigned char v=uval<<index;
			return v>>(8-size);
		}
		constexpr void write(bool value,unsigned char index)noexcept{
			unsigned char v=0b10000000;
			v>>=index;
			if(value) val|=v;
			else val&=~v;
		}
		template<unsigned char index>
		constexpr void write(bool value)noexcept{
			unsigned char v=0b10000000;
			v>>=index;
			if(value) val|=v;
			else val&=~v;
		}
		template<unsigned char index,unsigned char size>
		constexpr void write_num(unsigned char num)noexcept{
			static_assert(index<8,"index should be <8");
			uval&=~byte(0b11111111).get_num<index,size>();
			num<<=8-size;
			uval|=num>>index;
		}
		constexpr void write_num(unsigned char num,unsigned char index,unsigned char size)noexcept{
			uval&=~byte(0b11111111).get_num(index,size);
			num<<=8-size;
			uval|=num>>index;
		}
		constexpr operator unsigned char()const noexcept{
			return uval;
		}
		template<unsigned char size>
		constexpr unsigned char get_right_bits()const noexcept{
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
		constexpr unsigned char get_left_bits()const noexcept{
			static_assert(size<=8,"size should be <=8");
			return uval>>(8-size);
		}
		template<unsigned char size>
		constexpr void write_right_bits(unsigned char new_val)noexcept{
			static_assert(size<=8,"size should be <=8");
			uval>>=size;
			uval<<=size;
			new_val<<=8-size;
			new_val>>=8-size;
			uval|=new_val;
		}
		template<unsigned char size>
		constexpr void write_left_bits(unsigned char new_val)noexcept{
			static_assert(size<=8,"size should be <=8");
			uval<<=size;
			uval>>=size;
			new_val<<=8-size;
			uval|=new_val;
		}
	};
    template<typename Out>
    Out& operator<<(Out& stream,const byte b){
		for(unsigned char i=0;i<8;i++){
			stream<<b.get(i);
		}
        return stream;
    }
    template<typename T,T P1,T P2> struct params{
        static constexpr T p1=P1;
        static constexpr T p2=P2;
    };
	struct reserve_tag{};
	struct setSize{};
	//	^^^^^^ tags
	template<typename T> struct Wrap{T val;};
	template<typename T1,typename T2>
	struct Pair COUNT_IT{
		T1 x1;
		T2 x2;
		Pair()=default;
		template<typename T,typename D>
		constexpr Pair(T&& data1,D&& data2):x1((T&&)data1),x2((D&&)data2){}
		constexpr bool operator==(const Pair& other)const{
			return this->x1==other.x1 && this->x2==other.x2;
		}
		constexpr bool operator!=(const Pair& other)const{
			return this->x1!=other.x1 || this->x2!=other.x2;
		}
	};
	template<typename Out,typename T1,typename T2>
	Out& operator<<(Out& stream,const Pair<T1,T2>& pair){
		stream<<'('<<pair.x1<<','<<pair.x2<<')';
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
}

#include"Classes/Wrapper.hpp"
#include"Classes/EmptyClassOptimizations.hpp"
#include"Classes/TemporaryOwners.hpp"
#include"Classes/Iterators.hpp"
#include"Classes/Iterables.hpp"
#include"Classes/Optional.hpp"
#include"Classes/AutoPtrv2/PointerBase.hpp"
#include"Classes/Stringsv2.hpp"
#include"Classes/List.hpp"
#include"Classes/Vectorv2.hpp"
#include"Classes/Stacks_Queues.hpp"
#include"Classes/Red-Black Treev4.hpp"
#include"Classes/Map.hpp"
#include"Classes/Set.hpp"
//#include"Classes/BitList.hpp"
#include"Classes/Array.hpp"
#include"Classes/Matrix.hpp"
#include"Classes/StackBasedStateMachine.hpp"
//#include"Classes/BigInt.hpp"
