#pragma once
#if (defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)) || defined(_INC_STDIO) && defined(SHOWREMAININGOBJECTS)
#define SETCOUNTERS		int remainingQueues=0;\
						int remainingStacks=0;\
						int remainingLists=0;\
						int remainingStrings=0;\
						int remainingControlBlocks=0;\
						int remainingStrongPointers=0;\
						int remainingWeakPointers=0;\
						int remainingUniquePointers=0;\
						int remainingTrees=0;\
						int remainingListNodes=0;
						
#define INCQUEUES 					remainingQueues++;
#define DECQUEUES					remainingQueues--;
#define REMAININGQUEUES 			printf("\nREMAINING QUEUES: %d\n",remainingQueues);
#define INCSTACKS 					remainingStacks++;
#define DECSTACKS 					remainingStacks--;
#define REMAININGSTACKS	 			printf("\nREMAINING STACKS: %d\n",remainingStacks);
#define INCLISTS  					remainingLists++;
#define DECLISTS  					remainingLists--;
#define REMAININGLISTS 				printf("\nREMAINING LISTS: %d\n",remainingLists);
#define INCSTRINGS 					remainingStrings++;
#define DECSTRINGS					remainingStrings--;
#define REMAININGSTRINGS 			printf("\nREMAINING STRINGS: %d\n",remainingStrings);
#define INCCTRLBLOCKS				remainingControlBlocks++;
#define DECCTRLBLOCKS				remainingControlBlocks--;
#define REMAININGCTRLBLOCKS 		printf("\nREMAINING CONTROL BLOCKS: %d\n",remainingControlBlocks);
#define INCSTRONGPOINTERS 			remainingStrongPointers++;
#define DECSTRONGPOINTERS 			remainingStrongPointers--;
#define REMAININGSTRONGPOINTERS 	printf("\nREMAINING STRONG POINTERS: %d\n",remainingStrongPointers);
#define INCWEAKPOINTERS				remainingWeakPointers++;
#define DECWEAKPOINTERS				remainingWeakPointers--;
#define REMAININGWEAKPOINTERS		printf("\nREMAINING WEAK POINTERS: %d\n",remainingWeakPointers);
#define INCUNIQUEPOINTERS			remainingUniquePointers++;
#define DECUNIQUEPOINTERS			remainingUniquePointers--;
#define REMAININGUNIQUEPOINTERS		printf("\nREMAINING UNIQUE POINTERS: %d\n",remainingUniquePointers);
#define INCTREES					remainingTrees++;
#define DECTREES					remainingTrees--;
#define REMAININGTREES				printf("\nREMAINING TREES: %d\n",remainingTrees);
#define INCLNODES					remainingListNodes++;
#define DECLNODES					remainingListNodes--;
#define REMAININGLNODES				printf("\nREMAINING LIST NODES: %d\n",remainingListNodes);
#else
#define SETCOUNTERS
#define INCQUEUES
#define DECQUEUES
#define REMAININGQUEUES
#define INCSTACKS
#define DECSTACKS
#define REMAININGSTACKS
#define INCLISTS
#define DECLISTS
#define REMAININGLISTS
#define INCSTRINGS
#define DECSTRINGS
#define REMAININGSTRINGS
#define INCCTRLBLOCKS
#define DECCTRLBLOCKS
#define REMAININGCTRLBLOCKS
#define INCSTRONGPOINTERS
#define DECSTRONGPOINTERS
#define REMAININGSTRONGPOINTERS
#define INCWEAKPOINTERS
#define DECWEAKPOINTERS
#define REMAININGWEAKPOINTERS
#define INCUNIQUEPOINTERS
#define DECUNIQUEPOINTERS
#define REMAININGUNIQUEPOINTERS
#define INCTREES
#define DECTREES
#define REMAININGTREES
#define INCLNODES
#define DECLNODES
#define REMAININGLNODES
#endif
SETCOUNTERS
namespace mpv{
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
#include"Classes/EmptyClassOptimizations.hpp"
#include"Classes/Iterables.hpp"
#include"Classes/Optional.hpp"
#include"Classes/Stringsv2.hpp"
#include"Classes/Lists.hpp"
#include"Classes/ArrayListsv2.hpp"
#include"Classes/Stacks_Queues.hpp"
#include"Classes/AutoPtrv2/PointerBase.hpp"
#include"Classes/Red-Black Treev2.hpp"
#include"Classes/Map.hpp"
#include"Classes/BitList.hpp"
#include"Classes/Array.hpp"
#include"Classes/Matrix.hpp"
