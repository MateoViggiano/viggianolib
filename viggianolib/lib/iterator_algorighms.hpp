#pragma once
#include"Classes/Optional.hpp"
#include"Classes/TemporaryOwners.hpp"
#include"../viggiano.hpp"
namespace mpv{
	template<typename It>
	constexpr typename iterator_traits<It>::difference_type distance(It first,It last){
		if constexpr(is_random_access_iterator_v<It>) return last-first;
		else{
			typename iterator_traits<It>::difference_type dist=0;
			while(first!=last){
				++dist;
				++first;
			}
			return dist;
		}
	}
	template<typename It,typename Diff>
	constexpr void advance(It& it,Diff offset){
		if constexpr(is_random_access_iterator_v<It>){
			it+=offset;
		}
		else{
			if constexpr(is_signed_v<Diff> && is_bidirectional_iterator_v<It>){
				while(offset<0){
					--it;
					++offset;
				}
			}
			while(offset>0){
				++it;
				--offset;
			}
		}
	}
	template<typename T,typename U>
	constexpr void copy_trivial(T* dest,const U* source,size_t size){// If it does overlap, source should be >= dest
		if constexpr(USE_MEMCPY){
			MEMMOVE(dest,source,size*sizeof(U));
		}else{
			while(size--)
				*(dest++)=*(source++);
		}
	}
	template<typename T,typename U>
	constexpr void copy_backward_trivial(T* destlast,const U* sourcelast,size_t size){// If it does overlap, source should be <= dest
		if constexpr(USE_MEMCPY){
			MEMMOVE(destlast-size,sourcelast-size,size*sizeof(U));
		}else{
			while(size--)
				*(--destlast)=*(--sourcelast);
		}
	}
	template<typename T,typename U>
	constexpr void reverse_copy_trivial(T* dest,const U* source,size_t size){// If it does overlap, source should be <= dest
		if constexpr(USE_MEMCPY){
			MEMMOVE(dest,source,size*sizeof(U));
		}else{
			dest+=size;source+=size;
			while(size--)
				*(--dest)=*(--source);
		}
	}
	template<typename T,typename U>
	constexpr void copy_trivial_no_overlap(T* dest,const U* source,size_t size){// If it does overlap, source should be >= dest
		if constexpr(USE_MEMCPY){
			MEMCPY(dest,source,size*sizeof(U));
		}else{
			while(size--)
				*(dest++)=*(source++);
		}
	}
	template<typename T,typename U>
	constexpr void copy_trivial_overlap(T* dest,const U* source,size_t size){
		if constexpr(USE_MEMCPY){
			MEMMOVE(dest,source,size*sizeof(U));
		}else{
			if(source>=dest){
				while(size--)
					*(dest++)=*(source++);				
			}
			else{
				dest+=size;source+=size;
				while(size--)
					*(--dest)=*(--source);
			}
		}
	}
	template<typename T,typename V>
	constexpr void fill_trivial(T* dest,size_t size,const V val){
		if constexpr(USE_MEMCPY){
			if constexpr(fill_memset_safe_iter<T*,V>){
				MEMSET(dest,val,size*sizeof(T));
				return;
			}
			else if constexpr(fill_zero_memset_safe_iter<T*,V>){
				if(is_all_bits_zero(val)){
					MEMSET(dest,0,size*sizeof(T));
					return;
				}
			}
		}
		while(size--)
			*(dest++)=val;
	}
	template<typename T,typename U>
	constexpr int same_size_equal_trivial(const T* a,const U* b,size_t size){
		if constexpr(USE_MEMCPY){
			return 0==MEMCMP(a,b,size*sizeof(U));
		}else{
			while(size--){
				if(*(a++)!=*(b++)) return false;
			}
			return true;
		}
	}
	template<typename T,typename U>
	constexpr int same_size_lex_compare_trivial(const T* a,const U* b,size_t size){
		if constexpr(USE_MEMCPY){
			return MEMCMP(a,b,size*sizeof(U));
		}
		else{
			while(size--){
				if(*a!=*b) return *a<*b ? -1 : 1;
				a++;
				b++;
			}
			return 0;			
		}
	}
	template<typename OutIt,typename InIt>
	constexpr void copy(OutIt dest,InIt first,InIt last){
		if constexpr(!is_bitcopy_assignable_iter_v<InIt,OutIt>){
			while(first!=last)
				*(dest++)=*(first++);
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename BidIt1,typename BidIt2>
	constexpr void copy_backward(BidIt1 destlast,BidIt2 first,BidIt2 last){
		if constexpr(!is_bitcopy_assignable_iter_v<BidIt2,BidIt1>){
			while(last!=first)
				*(--destlast)=*(--last);
		}
		else{
			copy_backward_trivial(unfancy(destlast),unfancy(last),last-first);
		}
	}
	template<typename BidIt1,typename BidIt2>
	constexpr void copy_reverse(BidIt1 dest,BidIt2 first,BidIt2 last){
		if constexpr(!is_bitcopy_assignable_iter_v<BidIt2,BidIt1>){
			advance(dest,distance(first,last));
			while(last!=first)
				*(--dest)=*(--last);
		}
		else{
			reverse_copy_trivial(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename RandIt1,typename RandIt2>
	constexpr void copy_overlap(RandIt1 dest,RandIt2 first,RandIt2 last){
		if constexpr(!is_bitcopy_assignable_iter_v<RandIt2,RandIt1>){
			if(first>=dest){
				while(first!=last)
					*(dest++)=*(first++);
			}
			else{
				advance(dest,distance(first,last));
				while(last!=first)
					*(--dest)=*(--last);
			}
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename OutIt,typename InIt>
	constexpr void move(OutIt dest,InIt first,InIt last){
		if constexpr(!is_bitcopy_move_assignable_iter_v<InIt,OutIt>){
			while(first!=last)
				*(dest++)=mpv::move(*(first++));
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename BidIt1,typename BidIt2>
	constexpr void move_backward(BidIt1 destlast,BidIt2 first,BidIt2 last){
		if constexpr(!is_bitcopy_move_assignable_iter_v<BidIt2,BidIt1>){
			while(last!=first)
				*(--destlast)=mpv::move(*(--last));
		}
		else{
			copy_backward_trivial(unfancy(destlast),unfancy(last),last-first);
		}
	}
	template<typename BidIt1,typename BidIt2>
	constexpr void move_reverse(BidIt1 dest,BidIt2 first,BidIt2 last){
		if constexpr(!is_bitcopy_move_assignable_iter_v<BidIt2,BidIt1>){
			advance(dest,distance(first,last));
			while(last!=first)
				*(--dest)=mpv::move(*(--last));
		}
		else{
			reverse_copy_trivial(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename FwdIt,typename T>
	constexpr void fill(FwdIt first,FwdIt last,const T& val){
		if constexpr(!fill_memset_safe_iter<FwdIt,T> && !fill_zero_memset_safe_iter<FwdIt,T>){
			while(first!=last)
				*(first++)=val;
		}
		else{
			fill_trivial(unfancy(first),last-first,val);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt>
	constexpr void copy_construct(Alloc& alloc,OutIt dest,InIt first,InIt last){
		if constexpr(!is_bitcopy_constructible_iter_v<InIt,OutIt>){
			DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
			while(first!=last){
				allocator_traits<Alloc>::construct(alloc,unfancy(dest++),*(first++));
				++guard.count;
			}
			guard.count=0;
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt>
	constexpr void move_construct(Alloc& alloc,OutIt dest,InIt first,InIt last){
		if constexpr(!is_bitcopy_move_constructible_iter_v<InIt,OutIt>){
			DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
			while(first!=last){
				allocator_traits<Alloc>::construct(alloc,unfancy(dest++),mpv::move(*(first++)));
				++guard.count;
			}
			guard.count=0;
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt>
	constexpr void move_construct_if_nt(Alloc& alloc,OutIt dest,InIt first,InIt last){
		if constexpr(!is_bitcopy_move_constructible_iter_v<InIt,OutIt>){
			DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
			while(first!=last){
				allocator_traits<Alloc>::construct(alloc,unfancy(dest++),move_if_noexcept(*(first++)));
				++guard.count;
			}
			guard.count=0;
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),last-first);
		}
	}
	template<typename Alloc,typename FwdIt,typename T>
	constexpr void fill_construct(Alloc& alloc,FwdIt first,FwdIt last,const T& val){
		if constexpr(!fill_construct_memset_safe_iter<FwdIt,T> && !fill_zero_construct_memset_safe_iter<FwdIt,T>){
			DestroySequenceGuard<Alloc,FwdIt> guard(alloc,first);
			while(first!=last){
				allocator_traits<Alloc>::construct(alloc,unfancy(first++),val);
				++guard.count;
			}
			guard.count=0;
		}
		else{
			fill_trivial(unfancy(first),last-first,val);
		}
	}
	template<typename Alloc,typename FwdIt>
	constexpr void default_construct(Alloc& alloc,FwdIt first,FwdIt last){
		if constexpr(!is_trivially_default_constructible_v<iter_value_t<FwdIt>> || !Uses_default_construct_v<Alloc,decltype(unfancy(first++))>){
			DestroySequenceGuard<Alloc,FwdIt> guard(alloc,first);
			while(first!=last){
				allocator_traits<Alloc>::construct(alloc,unfancy(first++));
				++guard.count;
			}
			guard.count=0;
		}
	}
	template<typename Alloc,typename FwdIt>
	constexpr void destroy(Alloc& alloc,FwdIt first,FwdIt last)noexcept(noexcept(first++)){
		if constexpr(!is_trivially_destructible_v<iter_value_t<FwdIt>> || !Uses_default_destroy_v<Alloc,decltype(unfancy(first++))>){
			while(first!=last)
				allocator_traits<Alloc>::destroy(alloc,unfancy(first++));
		}
	}
	template<typename OutIt,typename InIt,typename Size>
	constexpr void copy_n(OutIt dest,InIt first,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<InIt,OutIt>){
			if(size>0){
				while(true){// Avoids extra increment whith single pass imput iterators. This is not an issue in copy_backward_n and copy_reverse_n because they only accept bidirectional iterators
					*(dest++)=*(first);
					if(--size==0) break;
					++first;
				}
			}
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename BidIt1,typename BidIt2,typename Size>
	constexpr void copy_backward_n(BidIt1 destlast,BidIt2 last,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<BidIt2,BidIt1>){
			while(size--)
				*(--destlast)=*(--last);
		}
		else{
			copy_backward_trivial(unfancy(destlast),unfancy(last),size);
		}
	}
	template<typename BidIt1,typename BidIt2,typename Size>
	constexpr void copy_reverse_n(BidIt1 dest,BidIt2 first,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<BidIt2,BidIt1>){
			advance(dest,size);
			advance(first,size);
			while(size--)
				*(--dest)=*(--first);
		}
		else{
			reverse_copy_trivial(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename RandIt1,typename RandIt2,typename Size>
	constexpr void copy_overlap_n(RandIt1 dest,RandIt2 first,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<RandIt2,RandIt1>){
			if(first>=dest){
				while(size--)
					*(dest++)=*(first++);
			}
			else{
				advance(dest,size);
				advance(first,size);
				while(size--)
					*(--dest)=*(--first);
			}
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename OutIt,typename InIt,typename Size>
	constexpr void move_n(OutIt dest,InIt first,Size size){
		if constexpr(!is_bitcopy_move_assignable_iter_v<InIt,OutIt>){
			if(size>0){
				while(true){// Avoids extra increment whith single pass imput iterators. This is not an issue in move_backward_n and move_reverse_n because they only accept bidirectional iterators
					*(dest++)=mpv::move(*(first));
					if(--size==0) break;
					++first;
				}
			}
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename BidIt1,typename BidIt2,typename Size>
	constexpr void move_backward_n(BidIt1 destlast,BidIt2 last,Size size){
		if constexpr(!is_bitcopy_move_assignable_iter_v<BidIt2,BidIt1>){
			while(size--)
				*(--destlast)=mpv::move(*(--last));
		}
		else{
			copy_backward_trivial(unfancy(destlast),unfancy(last),size);
		}
	}
	template<typename BidIt1,typename BidIt2,typename Size>
	constexpr void move_reverse_n(BidIt1 dest,BidIt2 first,Size size){
		if constexpr(!is_bitcopy_move_assignable_iter_v<BidIt2,BidIt1>){
			advance(dest,size);
			advance(first,size);
			while(size--)
				*(--dest)=mpv::move(*(--first));
		}
		else{
			reverse_copy_trivial(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename FwdIt,typename Size,typename T>
	constexpr void fill_n(FwdIt first,Size size,const T& val){
		if constexpr(!fill_memset_safe_iter<FwdIt,T> && !fill_zero_memset_safe_iter<FwdIt,T>){
			while(size--)
				*(first++)=val;
		}
		else{
			fill_trivial(unfancy(first),size,val);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt,typename Size>
	constexpr void copy_construct_n(Alloc& alloc,OutIt dest,InIt first,Size size){
		if constexpr(!is_bitcopy_constructible_iter_v<InIt,OutIt>){
			if(size>0){
				DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
				while(true){
					allocator_traits<Alloc>::construct(alloc,unfancy(dest++),*(first));
					++guard.count;
					if(--size==0) break;
					++first;
				}
				guard.count=0;				
			}
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt,typename Size>
	constexpr void move_construct_n(Alloc& alloc,OutIt dest,InIt first,Size size){
		if constexpr(!is_bitcopy_move_constructible_iter_v<InIt,OutIt>){
			if(size>0){
				DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
				while(true){
					allocator_traits<Alloc>::construct(alloc,unfancy(dest++),mpv::move(*(first)));
					++guard.count;
					if(--size==0) break;
					++first;
				}
				guard.count=0;				
			}
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename Alloc,typename OutIt,typename InIt,typename Size>
	constexpr void move_construct_if_nt_n(Alloc& alloc,OutIt dest,InIt first,Size size){
		if constexpr(!is_bitcopy_move_constructible_iter_v<InIt,OutIt>){
			if(size>0){
				DestroySequenceGuard<Alloc,OutIt> guard(alloc,dest);
				while(true){
					allocator_traits<Alloc>::construct(alloc,unfancy(dest++),move_if_noexcept(*(first)));
					++guard.count;
					if(--size==0) break;
					++first;
				}
				guard.count=0;				
			}
		}
		else{
			copy_trivial_no_overlap(unfancy(dest),unfancy(first),size);
		}
	}
	template<typename Alloc,typename FwdIt,typename T,typename Size>
	constexpr void fill_construct_n(Alloc& alloc,FwdIt first,Size size,const T& val){
		if constexpr(!fill_construct_memset_safe_iter<FwdIt,T> && !fill_zero_construct_memset_safe_iter<FwdIt,T>){
			DestroySequenceGuard<Alloc,FwdIt> guard(alloc,first);
			while(size--){
				allocator_traits<Alloc>::construct(alloc,unfancy(first++),val);
				++guard.count;
			}
			guard.count=0;
		}
		else{
			fill_trivial(unfancy(first),size,val);
		}
	}
	template<typename Alloc,typename FwdIt,typename Size>
	constexpr void default_construct_n(Alloc& alloc,FwdIt first,Size size){
		if constexpr(!is_trivially_default_constructible_v<iter_value_t<FwdIt>> || !Uses_default_construct_v<Alloc,decltype(unfancy(first++))>){
			DestroySequenceGuard<Alloc,FwdIt> guard(alloc,first);
			while(size--){
				allocator_traits<Alloc>::construct(alloc,unfancy(first++));
				++guard.count;
			}
			guard.count=0;
		}
	}
	template<typename Alloc,typename FwdIt,typename Size>
	constexpr void destroy_n(Alloc& alloc,FwdIt first,Size size)noexcept(noexcept(first++)){
		if constexpr(!is_trivially_destructible_v<iter_value_t<FwdIt>> || !Uses_default_destroy_v<Alloc,decltype(unfancy(first++))>){
			while(size--)
				allocator_traits<Alloc>::destroy(alloc,unfancy(first++));
		}
	}
	template<typename InIt1,typename InIt2,typename Pred=equal_to<>>
	constexpr bool equal(InIt1 first,InIt1 last,InIt2 first2,Pred pred=Pred{}){
		if constexpr(!equal_memcmp_is_safe_iter<InIt1,InIt2,Pred>){
			while(first!=last){
				if(!pred(*first++,*first2++)) return false;
			}
			return true;
		}
		else{
			return same_size_equal_trivial(unfancy(first),unfancy(first2),last-first);
		}
	}
	template<typename InIt1,typename InIt2,typename Pred=equal_to<>>
	constexpr bool equal(InIt1 first1,InIt1 last1,InIt2 first2,InIt2 last2,Pred pred=Pred{}){
		if constexpr(is_random_access_iterator_v<InIt1> && is_random_access_iterator_v<InIt2>){
			if(last1-first1!=last2-first2) return false;
			else return equal(first1,last1,first2,pass_func(pred));
		}
		else{
			while(true){
				if(first1==last1) return first2==last2;
				else if(first2==last2) return false;
				else if(!pred(*first1,*first2)) return false;
				++first1;++first2;
			}
		}
	}
	template<typename InIt1,typename InIt2,typename Size,typename Pred=equal_to<>>
	constexpr bool equal_n(InIt1 first,InIt2 first2,Size size,Pred pred=Pred{}){
		if constexpr(!equal_memcmp_is_safe_iter<InIt1,InIt2,Pred>){
			while(size--){
				if(!pred(*first++,*first2++)) return false;
			}
			return true;
		}
		else{
			return same_size_equal_trivial(unfancy(first),unfancy(first2),size);
		}
	}
	template<typename InIt1,typename InIt2,typename Pred=less<>>
	constexpr bool lexicographical_compare(InIt1 first1,InIt1 last1,InIt2 first2,InIt2 last2,Pred pred=Pred{}){
		using MemcmpPred=lex_compare_memcmp_classify<InIt1,InIt2,Pred>;
		if constexpr(!is_void_v<MemcmpPred>){
			const size_t size1=last1-first1,size2=last2-first2;
			const int res=same_size_lex_compare_trivial(unfancy(first1),unfancy(first2),size1<size2 ? size1:size2);
			return MemcmpPred{}(res,0) || (res==0 && size1<size2);
		}
		else{
			while(true){
				if(first1==last1) return first2!=last2;
				else if(first2==last2) return false;
				else if(pred(*first1,*first2)) return true;
				else if(pred(*first2,*first1)) return false;
				++first1;++first2;
			}
		}
	}
	template<typename BidIt>
	constexpr void reverse(BidIt first,BidIt last){
		while(first!=last && first!=--last){
			swap(*(first++),*last);
		}
	}
	template<typename FwdIt,typename T>
	constexpr FwdIt find(FwdIt first,FwdIt last,const T& val){
		while(first!=last){
			if(*first==val) break;
			first++;
		}
		return first;
	}
	template<typename FwdIt,typename T,typename Size=size_t>
	constexpr Size count(FwdIt first,FwdIt last,const T& val){
		Size found=0;
		while(first!=last){
			if(*first==val) ++found;
			first++;
		}
		return found;
	}
	template<typename FwdIt,typename T>
	constexpr bool contains(FwdIt first,FwdIt last,const T& val){
		while(first!=last){
			if(*first==val) return true;
			first++;
		}
		return false;
	}
	template<typename RanIt,typename T,typename Cmp=less<T>>
	constexpr RanIt binary_search(RanIt first,RanIt last,const T& val,Cmp&& cmp=Cmp{}){
		RanIt end=last-1;
		while(end>=first){
			RanIt index=first+(end-first)/2;
			if(cmp(*index,val))
				first=index+1;
			else if(cmp(val,*index))
				end=index-1;
			else
				return index;
		}
		return last;
	}
	template<typename FwdIt,typename T,typename Size=size_t>
	constexpr Size index_of(FwdIt first,FwdIt last,const T& val){ 
		Size index=0;
		while(first!=last){
			if(*first==val) return index;
			first++;
			index++;
		}
		return index;//One past the last element if val is not found
	}
	template<typename FwdIt>
	constexpr void rotate(FwdIt first,FwdIt mid,FwdIt last){
		if(mid==last) return;
		FwdIt next=mid;
		while(first!=next){
			swap(*first++,*next++);
			if(next==last) next=mid;
			else if(first==mid) mid=next;
		}
	}

	template<typename OutIt,typename InIt,typename Size>
	constexpr void advance_copy_n(OutIt dest,InIt& first,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<InIt,OutIt>){
			while(size--)
				*(dest++)=*(first++);
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),size);
			first+=size;
		}
	}
	template<typename OutIt,typename InIt,typename Size>
	constexpr void advance_move_n(OutIt dest,InIt& first,Size size){
		if constexpr(!is_bitcopy_assignable_iter_v<InIt,OutIt>){
			while(size--)
				*(dest++)=mpv::move(*(first++));
		}
		else{
			copy_trivial(unfancy(dest),unfancy(first),size);
			first+=size;
		}
	}
	template<typename FwdIt,typename Pred=less<>>
	bool is_sorted(FwdIt first,FwdIt last,Pred pred=Pred{}){
		if(first==last) return true;
		FwdIt next=first;
		++next;
		while(next!=last){
			if(pred(*next++,*first++)) return false;
		}
		return true;
	}
	template<typename BidIt,typename Pred=less<>>
	void insertion_sort(const BidIt first,const BidIt last,Pred pred=Pred{}){
		if(first==last) return;
		BidIt current_i=first,current_j=first;
		for(++current_i;current_i!=last;++current_i){
			iter_value_t<BidIt> aux=mpv::move(*current_i);
			for(current_j=current_i;current_j!=first && pred(aux,*(--BidIt(current_j)));--current_j){
				*current_j=mpv::move(*(--BidIt(current_j)));
			}
			*current_j=mpv::move(aux);
		}
	}
}

