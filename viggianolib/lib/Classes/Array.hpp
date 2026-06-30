#pragma once
#include "../arrays.hpp"
namespace mpv{
template<typename T,size_t len>
    class Array COUNT_IT{
        static_assert(len!=0,"array length cannot be 0");
        protected:
            T elems[len];
        public:
            using value_type=T;
            using size_type=size_t;
            using difference_type=ptrdiff_t;
            using pointer=T*;
            using const_pointer=const T*;
            using reference=T&;
            using const_reference=const T&;
            using iterator=T*;
            using const_iterator=const T*;
            static constexpr size_t length=len;
            static constexpr size_t size(){
                return length;
            }
            constexpr Array(const T (&list)[length]){
                for(size_t i=0;i<length;i++)
                    elems[i]=list[i];
            }
            template<typename... Args,enable_if_t<conjunction_v<is_constructible<T,Args>...>>* = nullptr>
            constexpr Array(Args&&... args):elems{static_cast<Args&&>(args)...}{}
            //Array(std::initializer_list<T> ilist):elems(ilist){}
            constexpr const T& operator[](size_t index)const{
                return elems[index];
            }
            constexpr T& operator[](size_t index){
                return elems[index];
            }
			constexpr bool operator==(const Array& other)const{
				return equal(this->begin(),this->end(),other.begin());
			}
			constexpr bool operator!=(const Array& other)const{
				return !equal(this->begin(),this->end(),other.begin());
			}
            constexpr bool operator<(const Array& other)const{
                return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool operator>(const Array& other)const{
                return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator<=(const Array& other)const{
                return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator>=(const Array& other)const{
                return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            void sort(){
                dflt::insertion_sort(elems,length);
            }
            void sort_reverse(){
                dflt::insertion_sort_reverse(elems,length);
            }
            template<typename D>
            void sort(D(*func)(const T&)){
                dflt::insertion_sort(elems,length,func);
            }
            template<typename Cmp>
            void sort(Cmp&& cmp=Cmp{}){
                dflt::insertion_sort<T,Cmp>(elems,length,static_cast<Cmp&&>(cmp));
            }
            constexpr Array operator+(const Array& other)const{
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=(*this)[i]+other[i];
                }
                return new_array;
            }
            constexpr Array operator-(const Array& other)const{
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=(*this)[i]-other[i];
                }
                return new_array;
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            constexpr Array operator*(U scalar)const{
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=(*this)[i]*scalar;
                }
                return new_array;
            }
            constexpr Array operator-()const{
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=-(*this)[i];
                }
                return new_array;
            }
            constexpr Array operator+()const{
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=+(*this)[i];
                }
                return new_array;
            }
            constexpr T operator*(const Array& other)const{
                T n=0;
                for(size_t i=0;i<length;i++){
                    n+=(*this)[i]*other[i];
                }
                return n;
            }
            constexpr void operator+=(const Array& other){
                for(size_t i=0;i<length;i++){
                    (*this)[i]+=other[i];
                }
            }
            constexpr void operator-=(const Array& other){
                for(size_t i=0;i<length;i++){
                    (*this)[i]-=other[i];
                }
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            constexpr void operator*=(U scalar){
                for(size_t i=0;i<length;i++){
                    (*this)[i]*=scalar;
                }
            }
            constexpr double norm()const{
                double sum=0;
                for(size_t i=0;i<length;i++){
                    sum+=(*this)[i]*(*this)[i];
                }
                return mpv::sqrt(sum);
            }
            constexpr double distance(const Array& other)const{
                T sum=0;
                for(size_t i=0;i<length;i++){
                    T res=other[i]-(*this)[i];
                    sum+=res*res;
                }
                return sqrt(sum);
            }
            constexpr Array<double,length> make_versor(){
                Array<double,length> new_vec;
                double n=1/(*this).norm();
                for(size_t i=0;i<length;i++){
                    new_vec[i]=(*this)[i]*n;
                }
                return new_vec;
            }
            constexpr Array<double,length> make_neg_versor(){
                Array<double,length> new_vec;
                double n=1/(*this).norm();
                for(size_t i=0;i<length;i++){
                    new_vec[i]=-(*this)[i]*n;
                }
                return new_vec;
            }
            template<size_t s=length,typename=enable_if_t<s==3>>
            constexpr Array<T,3> operator%(const Array<T,3>& other)const{
                Array<T,3> new_array;
                new_array[0]=(*this)[1]*other[2]-other[1]*(*this)[2];
                new_array[1]=other[0]*(*this)[2]-(*this)[0]*other[2];
                new_array[2]=(*this)[0]*other[1]-other[0]*(*this)[1];
                return new_array;
            }
            template<size_t s=length,typename=enable_if_t<s==3>>
            constexpr void operator%=(const Array<T,3>& other){
                Array<T,3> aux=*this;
                (*this)[0]=aux[1]*other[2]-other[1]*aux[2];
                (*this)[1]=other[0]*aux[2]-aux[0]*other[2];
                (*this)[2]=aux[0]*other[1]-other[0]*aux[1];
            }
            constexpr iterator begin(){
                return elems;
            }
            constexpr iterator end(){
                return elems+length;
            }
            constexpr const_iterator begin()const{
                return elems;
            }
            constexpr const_iterator end()const{
                return elems+length;
            }
    };
    template<typename T,size_t length,typename U,typename=enable_if_t<is_convertible_v<U,T>,U>>
    constexpr Array<T,length> operator*(U scalar,const Array<T,length>& vec){
        return vec*scalar;
    }
#if __cplusplus>=201703
    template<typename T,typename... Args> Array(T,Args...)->Array<enable_if_t<are_same_v<T,Args...>,T>,sizeof...(Args)+1>;
#endif
    template<typename Out,typename T,size_t length>
    Out& operator<<(Out& stream,const Array<T,length>& array){
        stream<<'(';
        for(size_t i=0;i<length;i++){
            stream<<array[i];
            if(i<length-1)
                stream<<", ";		
        }
        return stream<<')';
    }
}