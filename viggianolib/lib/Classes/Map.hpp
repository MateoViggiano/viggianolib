#pragma once
namespace mpv{
	template<typename keyT,typename valT>
    struct MapPair COUNT_IT{
        valT val;
        const keyT key;
        MapPair()=delete;
        template<typename kT,typename vT,enable_if_t<is_convertible_v<kT,keyT> && is_convertible_v<vT,valT>>* = nullptr>
        MapPair(kT&& key,vT&& val):val(static_cast<vT&&>(val)),key(static_cast<kT&&>(key)){}
    };
    template<typename T,typename D,typename Cmp>
    struct key_cmp:public Cmp{
        bool operator()(const MapPair<T,D>& a,const MapPair<T,D>& b)const{
            return Cmp::operator()(a.key,b.key);
        }
        bool operator()(const T& a,const MapPair<T,D>& b)const{
            return Cmp::operator()(a,b.key);
        }
        bool operator()(const MapPair<T,D>& a,const T& b)const{
            return Cmp::operator()(a.key,b);
        }
    };
    template<typename T,typename D,typename Alloc=allocator<MapPair<T,D>>,typename Cmp=less<T>>
    class Map:public Tree<MapPair<T,D>,Alloc,key_cmp<T,D,Cmp>>{
        public:
        	using base=Tree<MapPair<T,D>,Alloc,key_cmp<T,D,Cmp>>;
        	using base::base;
            const D& operator[](const T& key)const{
                return this->get(key)->val;
            }
            D& operator[](const T& key){
                return this->get(key)->val;
            }
            template<typename kT>
            enable_if_t<is_convertible_v<kT,T>,D&> push(kT&& key,const D& val){
                MapPair<T,D>* p=this->emplace(static_cast<kT&&>(key),val);
            	return p->val;
            }
            Optional<D> at(const T& key)const{
                const MapPair<T,D>* p=this->get(key);
                return p!=nullptr ? p->val : Optional<D>();
            }
    };
    template<typename Out,typename T,typename D>
    Out& operator<<(Out& stream,const MapPair<T,D>& pair){
        stream<<'{'<<pair.key<<" : "<<pair.val<<'}';
        return stream;
    }
}