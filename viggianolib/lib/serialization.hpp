#pragma once
namespace mpv{
    namespace serialization{
        class Bytes;
        template<typename T> constexpr enable_if_t<is_trivially_copyable_v<T>,Bytes> to_bytes(const T& val);
        template<typename T,typename U> Bytes to_bytes(const MapPair<T,U>& pair);
        template<typename Container> enable_if_t<!is_trivially_copyable_v<Container> && is_trivially_serializable_container_v<Container>,Bytes> to_bytes(const Container& cont);
        template<typename Container> enable_if_t<!is_trivially_copyable_v<Container> && !is_trivially_serializable_container_v<Container>,Bytes> to_bytes(const Container& cont);
        
        template<typename T>
        struct NullTerminatedArray{
            static_assert(is_trivially_copyable_v<T>,"T mus be trivially copyable");
            const T* ptr;
            size_t size;
            NullTerminatedArray(const T* ptr,size_t size):ptr(ptr),size(size){}
        };
        template<typename T>
        union Serialized{
            static_assert(is_trivially_copyable_v<T>,"T must be trivially copyable");
            T value;
            unsigned char bytearray[sizeof(T)];
            Serialized(const T& val):value(val){}
        };
        template<typename Container,typename Uint>
        struct Wrapper:public Container{
            static_assert(is_iterable_v<Container>,"Container must be iterable");
            static_assert(is_unsigned_v<Uint> && !is_bool_v<Uint>,"Uint must be an unsigned integer");
            using Base=Container;
            using Container::Container;
            using Container::operator=;
            Wrapper(const Base& b):Base(b){}
            Wrapper(Base&& b):Base(static_cast<Base&&>(b)){}
            using typename Container::value_type;
            using typename Container::reference;
            using typename Container::pointer;
            constexpr Uint size()const noexcept{
                return static_cast<Uint>(Container::size());
            }
        };
        template<typename T>
        struct unwrapped{
            using type=T;
        };
        template<typename T> using unwrapped_t=typename unwrapped<T>::type;
        
        template<template<typename...> class Container,typename... Args>
        struct unwrapped<Container<Args...>>{
            using type=Container<unwrapped_t<Args>...>;
        };
        template<template<typename...> class Container,typename Uint,typename... Args>
        struct unwrapped<Wrapper<Container<Args...>,Uint>>{
            using type=Container<unwrapped_t<Args>...>;
        };
        class Bytes:public Vector<unsigned char>{
        public:
            using Base = Vector<unsigned char>;
            using Base::Base;
            template<typename T>
            Bytes(const Serialized<T> val):Base(val.bytearray,val.bytearray+sizeof(val)){}
            Bytes(const Vector<unsigned char>& vec):Base(vec){}
            Bytes operator+(const Bytes& other)const&{
                return this->Base::operator+(other);
            }
            Bytes operator+(Bytes&& other)const&{
                return this->Base::operator+(static_cast<Bytes&&>(other));
            }
            Bytes operator+(const Bytes& other)&&{
                return static_cast<Bytes&&>(*this).Base::operator+(other);
            }
            Bytes operator+(Bytes&& other)&&{
                return static_cast<Bytes&&>(*this).Base::operator+(static_cast<Bytes&&>(other));
            }
            Bytes& operator+=(const Bytes& other){
                this->Base::operator+=(other);
                return *this;
            }
            Bytes& operator+=(Bytes&& other){
                this->Base::operator+=(static_cast<Bytes&&>(other));
                return *this;
            }
            template<typename T>
            Bytes operator+(const Serialized<T> trivial)const{
                Bytes res(sizeof(trivial)+this->size());
                mpv::copy(res.begin(),this->begin(),this->end());
                *reinterpret_cast<Serialized<T>*>(res.get_array()+this->size())=trivial;
                return res;
            }
            template<typename T>
            Bytes& operator+=(Serialized<T> trivial){
                this->insert(this->end(),mpv::begin(trivial.bytearray),mpv::end(trivial.bytearray));
                return *this;
            }
        };
        template<typename T>
        Bytes operator+(Serialized<T> fst,const Bytes& scnd){
            Bytes res(sizeof(fst)+scnd.size());
            *reinterpret_cast<Serialized<T>*>(res.get_array())=fst;
            mpv::copy(res.begin()+sizeof(fst),scnd.begin(),scnd.end());
            return res;
        }
        template<typename T>
        constexpr enable_if_t<is_trivially_copyable_v<T>,Bytes> to_bytes(const T& val){
            return Serialized<T>(val);
        }
        template<typename T,typename U>
        Bytes to_bytes(const MapPair<T,U>& pair){
            return to_bytes(pair.key)+to_bytes(pair.val);
        }
        template<typename Container>
        enable_if_t<!is_trivially_copyable_v<Container> && is_trivially_serializable_container_v<Container>,Bytes> to_bytes(const Container& cont){
            Vector<unsigned char> data(sizeof(decltype(cont.size())) + cont.size() * sizeof(typename Container::value_type));
            using size_type=decltype(cont.size());
            using value_type=typename Container::value_type;
            unsigned char* ptr=data.get_array();
            *reinterpret_cast<size_type*>(ptr)=cont.size();
            ptr+=sizeof(size_type);
            copy_n(reinterpret_cast<value_type*>(ptr),cont.begin(),cont.size());
            return Bytes(data);
        }
        template<typename Container>
        enable_if_t<!is_trivially_copyable_v<Container> && !is_trivially_serializable_container_v<Container>,Bytes> to_bytes(const Container& cont){
            Vector<unsigned char> data(sizeof(decltype(cont.size())));
            using size_type=decltype(cont.size());
            *reinterpret_cast<size_type*>(data.get_array()) = cont.size();
            for(const auto& val:cont){
                data+=to_bytes(val);
            }
            return Bytes(data);
        }
        template<typename T>
        Bytes to_bytes(NullTerminatedArray<T> array){
            return Bytes(reinterpret_cast<const unsigned char*>(array.ptr),reinterpret_cast<const unsigned char*>(array.ptr)+(array.size+1)*sizeof(T));
        }
        template<typename,typename=void> struct Deserialize{};
        template<typename T> 
        struct Deserialize<T,enable_if_t<is_trivially_copyable_v<T>>>{
            template<typename except_t, bool runtime_check=true>
            static constexpr T from_bytes(const unsigned char*& data,const unsigned char* end)noexcept(runtime_check==false){
                if constexpr(runtime_check){
                    if(data+sizeof(T)>end) throw except_t();
                }
                T val=*reinterpret_cast<const T*>(data);
                data+=sizeof(T);
                return val;
            }
        };
        template<typename K,typename V>
        struct Deserialize<MapPair<K,V>>{
        	template<typename except_t,bool runtime_check=true>
            static constexpr MapPair<unwrapped_t<K>,unwrapped_t<V>> from_bytes(const unsigned char*& data,const unsigned char* end){
                unwrapped_t<K> key=Deserialize<K>::template from_bytes<except_t,runtime_check>(data,end);
                return MapPair<unwrapped_t<K>,unwrapped_t<V>>(static_cast<unwrapped_t<K>&&>(key),Deserialize<V>::template from_bytes<except_t,runtime_check>(data,end));
            }
        };
        template<typename Sequence>
        struct Deserialize<Sequence,enable_if_t<!is_trivially_copyable_v<Sequence> && is_trivially_serializable_container_v<Sequence>>>{
            template<typename except_t,bool runtime_check=true>
            static constexpr unwrapped_t<Sequence> from_bytes(const unsigned char*& data,const unsigned char* end){
                using size_type=decltype(mpv::declval<Sequence>().size());
                using value_type=typename Sequence::value_type;
                if constexpr(runtime_check){
                    if(data+sizeof(size_type)>end) throw except_t();
                }
                size_type size=*reinterpret_cast<const size_type*>(data);
                data+=sizeof(size_type);
                if constexpr(runtime_check){
                    if(data+size*sizeof(value_type)>end) throw except_t();
                }
                const value_type* rebound=reinterpret_cast<const value_type*>(data);
                data+=size*sizeof(value_type);
                return unwrapped_t<Sequence>(rebound,rebound+size);
            }            
        };

        template<typename Sequence>
        struct Deserialize<Sequence,enable_if_t<has_push_back_v<Sequence> && is_iterable_v<Sequence> && has_size_v<Sequence> && !is_trivially_copyable_v<Sequence> && !is_trivially_serializable_container_v<Sequence>>>{
            template<typename except_t,bool runtime_check=true>
            static constexpr unwrapped_t<Sequence> from_bytes(const unsigned char*& data,const unsigned char* end){
                using size_type=decltype(mpv::declval<Sequence>().size());
                if constexpr(runtime_check){
                    if(data+sizeof(size_type)>end) throw except_t();
                }
                size_type size=*reinterpret_cast<const size_type*>(data);
                unwrapped_t<Sequence> cont;
                if constexpr(has_reserve_v<Sequence>){
                    cont.reserve(size);
                }
                
                data+=sizeof(size_type);
                for(size_type i=0;i<size;i++)
                    cont.push_back(Deserialize<typename Sequence::value_type>::template from_bytes<except_t,runtime_check>(data,end));
                return cont;
            }
        };
        template<typename Tree>
        struct Deserialize<Tree,enable_if_t<has_get_root_v<Tree> && is_iterable_v<Tree> && has_size_v<Tree> && !is_trivially_copyable_v<Tree> && !is_trivially_serializable_container_v<Tree>>>{
            template<typename except_t,bool runtime_check=true>
            static constexpr unwrapped_t<Tree> from_bytes(const unsigned char*& data,const unsigned char* end){
                using size_type=decltype(mpv::declval<Tree>().size());
                if constexpr(runtime_check){
                    if(data+sizeof(size_type)>end) throw except_t();
                }
                size_type size=*reinterpret_cast<const size_type*>(data);
                unwrapped_t<Tree> cont;
                data+=sizeof(size_type);
                for(size_type i=0;i<size;i++)
                    cont.insert(Deserialize<typename Tree::value_type>::template from_bytes<except_t,runtime_check>(data,end));
                return cont;
            }
        };
        template<typename except_t=int,bool runtime_check=true>
        class Deserializer{
            const unsigned char* data,* end;
        public:
            Deserializer(const void* data,size_t size):data(reinterpret_cast<const unsigned char*>(data)),end(this->data+size){}
            template<typename T>
            unwrapped_t<T> read(){
                return Deserialize<T>::template from_bytes<except_t,runtime_check>(data,end);
            }
            template<typename T>
            Str<T> read_null_terminated_array(){
                Str<T> str;
                if constexpr(runtime_check){
                    if(data+sizeof(T)>end) throw except_t();
                }
                while(*reinterpret_cast<const T*>(data)!=T(0)){
                    str+=*reinterpret_cast<const T*>(data);
                    data+=sizeof(T);
                    if constexpr(runtime_check){
                        if(data+sizeof(T)>end) throw except_t();
                    }
                }
                data+=sizeof(T);
                return str;
            }
            constexpr size_t get_remaining_bytes()const noexcept{
                return end-data;
            }
        };
    }
}