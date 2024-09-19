#pragma once
#include"Red-Black Treev2.hpp"
namespace mpv{
    template<typename T,typename Hash_t,size_t array_size>
    class HashTable{static_assert(array_size>1,"array_size must be > 0");
        private:
            Tree<T> array[array_size];
            Hash_t hasher;
        public:
            bool insert(const T& val){
                size_t position=hash(val)%array_size;
                return array[position].insert(val);
            }
            bool insert(T&& val){
                size_t position=hash(val)%array_size;
                return array[position].insert((T&&)val);
            }
            template<typename... Args>
            bool insert(Args&& args){
                T val((Args&&)args...);
                size_t position=hash(val)%array_size;
                return array[position].insert((T&&)val);
            }
            bool del(const T& val){
                size_t position=hash(val)%array_size;
                return array[position].del(val);
            }
            bool has(c){
                size_t position=hash(val)%array_size;
                return array[position].has(val);
            }

    };
}