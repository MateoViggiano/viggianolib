#pragma once
namespace mpv{
    template<typename T,typename Alloc=allocator<T>,typename Cmp=less<T>>
    class Set:public Tree<T,Alloc,Cmp>{
        public:
        	using base=Tree<T,Alloc,Cmp>;
        	using base::base;
    };
    
}