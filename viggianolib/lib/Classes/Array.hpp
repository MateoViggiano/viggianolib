#pragma once
#include "../arrays.hpp"
namespace mpv{
template<typename T,size_t size>
    class Array COUNT_IT{
        static_assert(size!=0,"array size cannot be 0");
        protected:
            T buffer[size];
        public:
            static constexpr size_t len(){
                return size;
            }
            Array()=default;
            Array(const Array&)=default;
            Array(Array&&)=default;
            Array& operator=(const Array&)=default;
            Array& operator=(Array&&)=default;
            Array(const T(&list)[size]){
                for(size_t i=0;i<size;i++)
                    buffer[i]=list[i];
            }
//            template<typename... Args>
//            Array(Args&&... args):buffer{static_cast<T>(static_cast<Args&&>(args))...}{}
            const T& operator[](size_t index)const{
                return buffer[index];
            }
            T& operator[](size_t index){
                return buffer[index];
            }
            void sort(){
                dflt::insertion_sort(buffer,size);
            }
            void sort_reverse(){
                dflt::insertion_sort_reverse(buffer,size);
            }
            template<typename D>
            void sort(D(*func)(const T&)){
                dflt::insertion_sort(buffer,size,func);
            }
            template<typename Cmp>
            void sort(Cmp&& cmp=Cmp{}){
                dflt::insertion_sort<T,Cmp>(buffer,size,static_cast<Cmp&&>(cmp));
            }
            Array operator+(const Array& other)const{
                Array new_array;
                for(size_t i=0;i<size;i++){
                    new_array[i]=(*this)[i]+other[i];
                }
                return new_array;
            }
            Array operator-(const Array& other)const{
                Array new_array;
                for(size_t i=0;i<size;i++){
                    new_array[i]=(*this)[i]-other[i];
                }
                return new_array;
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            Array operator*(U scalar)const{
                Array new_array;
                for(size_t i=0;i<size;i++){
                    new_array[i]=(*this)[i]*scalar;
                }
                return new_array;
            }
            Array operator-()const{
                Array new_array;
                for(size_t i=0;i<size;i++){
                    new_array[i]=-(*this)[i];
                }
                return new_array;
            }
            Array operator+()const{
                Array new_array;
                for(size_t i=0;i<size;i++){
                    new_array[i]=+(*this)[i];
                }
                return new_array;
            }
            T operator*(const Array& other)const{
                T n=0;
                for(size_t i=0;i<size;i++){
                    n+=(*this)[i]*other[i];
                }
                return n;
            }
            void operator+=(const Array& other){
                for(size_t i=0;i<size;i++){
                    (*this)[i]+=other[i];
                }
            }
            void operator-=(const Array& other){
                for(size_t i=0;i<size;i++){
                    (*this)[i]-=other[i];
                }
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            void operator*=(U scalar){
                for(size_t i=0;i<size;i++){
                    (*this)[i]*=scalar;
                }
            }
            double norm()const{
                double sum=0;
                for(size_t i=0;i<size;i++){
                    sum+=(*this)[i]*(*this)[i];
                }
                return mpv::sqrt(sum);
            }
            Array<double,size> make_versor(){
                Array<double,size> new_vec;
                double n=1/(*this).norm();
                for(size_t i=0;i<size;i++){
                    new_vec[i]=(*this)[i]*n;
                }
                return new_vec;
            }
            Array<double,size> make_neg_versor(){
                Array<double,size> new_vec;
                double n=1/(*this).norm();
                for(size_t i=0;i<size;i++){
                    new_vec[i]=-(*this)[i]*n;
                }
                return new_vec;
            }
            bool operator==(const Array& other)const{
                for(size_t i=0;i<size;i++)
                    if((*this)[i]!=other[i]) return false;
                return true;
            }
            bool operator!=(const Array& other)const{
                for(size_t i=0;i<size;i++)
                    if((*this)[i]!=other[i]) return true;
                return false;
            }
            template<size_t s=size,typename=enable_if_t<s==3>>
            Array<T,3> operator%(const Array<T,3>& other)const{
                Array<T,3> new_array;
                new_array[0]=(*this)[1]*other[2]-other[1]*(*this)[2];
                new_array[1]=other[0]*(*this)[2]-(*this)[0]*other[2];
                new_array[2]=(*this)[0]*other[1]-other[0]*(*this)[1];
                return new_array;
            }
            template<size_t s=size,typename=enable_if_t<s==3>>
            void operator%=(const Array<T,3>& other){
                Array<T,3> aux=*this;
                (*this)[0]=aux[1]*other[2]-other[1]*aux[2];
                (*this)[1]=other[0]*aux[2]-aux[0]*other[2];
                (*this)[2]=aux[0]*other[1]-other[0]*aux[1];
            }
    };
    template<typename T,size_t size,typename U,typename=enable_if_t<is_convertible_v<U,T>,U>>
    Array<T,size> operator*(U scalar,const Array<T,size>& vec){
        return vec*scalar;
    }
#if __cplusplus>=201703
    template<typename T,typename... Args> Array(T,Args...)->Array<enable_if_t<are_same_v<T,Args...>,T>,sizeof...(Args)+1>;
#endif
    template<typename Out,typename T,size_t size>
    Out& operator<<(Out& stream,const Array<T,size>& array){
        stream<<'(';
        for(size_t i=0;i<size;i++){
            stream<<array[i];
            if(i<size-1)
                stream<<", ";		
        }
        return stream<<')';
    }
}