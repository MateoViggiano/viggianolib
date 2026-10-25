#pragma once
namespace mpv{
template<typename T,size_t len>
    class Array{
        public:
            T elems[len];
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
            constexpr const T& operator[](size_t index)const noexcept{
                return elems[index];
            }
            constexpr T& operator[](size_t index)noexcept{
                return elems[index];
            }
			constexpr bool operator==(const Array& other)const noexcept(noexcept(equal(this->begin(),this->end(),other.begin()))){
				return equal(this->begin(),this->end(),other.begin());
			}
			constexpr bool operator!=(const Array& other)const noexcept(noexcept(equal(this->begin(),this->end(),other.begin()))){
				return !equal(this->begin(),this->end(),other.begin());
			}
            constexpr bool operator<(const Array& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            constexpr bool operator>(const Array& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator<=(const Array& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return !lexicographical_compare(other.begin(),other.end(),this->begin(),this->end());
            }
            constexpr bool operator>=(const Array& other)const noexcept(noexcept(fake_copy_init<bool>(*this->begin()<*this->begin()))){
                return !lexicographical_compare(this->begin(),this->end(),other.begin(),other.end());
            }
            template<typename Pred=less<>>
            constexpr void sort(Pred pred=Pred{})noexcept(noexcept(insertion_sort(elems,elems+length,pred))){
                insertion_sort(elems,elems+length,pred);
            }
            constexpr Array operator+(const Array& other)const noexcept(is_nothrow_move_constructible_v<Array> && is_nothrow_default_constructible_v<T> && noexcept(mpv::declval<Array&>()[0]=(*this)[0]+other[0])){
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=(*this)[i]+other[i];
                }
                return new_array;
            }
            constexpr Array operator-(const Array& other)const noexcept(is_nothrow_move_constructible_v<Array> && is_nothrow_default_constructible_v<T> && noexcept(mpv::declval<Array&>()[0]=(*this)[0]-other[0])){
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=(*this)[i]-other[i];
                }
                return new_array;
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            constexpr Array operator*(U scalar)const noexcept(is_nothrow_move_constructible_v<Array> && is_nothrow_default_constructible_v<T> && noexcept(mpv::declval<Array&>()[0]=(*this)[0]*scalar)){
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=(*this)[i]*scalar;
                }
                return new_array;
            }
            constexpr Array operator-()const noexcept(is_nothrow_move_constructible_v<Array> && is_nothrow_default_constructible_v<T> && noexcept(mpv::declval<Array&>()[0]=-(*this)[0])){
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=-(*this)[i];
                }
                return new_array;
            }
            constexpr Array operator+()const noexcept(is_nothrow_move_constructible_v<Array> && is_nothrow_default_constructible_v<T> && noexcept(mpv::declval<Array&>()[0]=+(*this)[0])){
                Array new_array;
                for(size_t i=0;i<length;i++){
                    new_array[i]=+(*this)[i];
                }
                return new_array;
            }
            constexpr T operator*(const Array& other)const noexcept(noexcept(T(0)) && noexcept(mpv::declval<T&>()+=(*this)[0]*other[0])){
                T n(0);
                for(size_t i=0;i<length;i++){
                    n+=(*this)[i]*other[i];
                }
                return n;
            }
            constexpr void operator+=(const Array& other)noexcept(noexcept((*this)[0]+=other[0])){
                for(size_t i=0;i<length;i++){
                    (*this)[i]+=other[i];
                }
            }
            constexpr void operator-=(const Array& other)noexcept(noexcept((*this)[0]-=other[0])){
                for(size_t i=0;i<length;i++){
                    (*this)[i]-=other[i];
                }
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            constexpr void operator*=(U scalar)noexcept(noexcept((*this)[0]*=scalar)){
                for(size_t i=0;i<length;i++){
                    (*this)[i]*=scalar;
                }
            }
            constexpr double norm()const noexcept(noexcept(fake_copy_init<double>((*this)[0]*(*this)[0]))){
                double sum=0;
                for(size_t i=0;i<length;i++){
                    sum+=(*this)[i]*(*this)[i];
                }
                return mpv::sqrt(sum);
            }
            constexpr double distance(const Array& other)const noexcept(noexcept(T(0)) && noexcept(T(other[0]-(*this)[0])) && noexcept(mpv::declval<T&>()+=mpv::declval<T&>()*mpv::declval<T&>())){
                T sum(0);
                for(size_t i=0;i<length;i++){
                    T res=other[i]-(*this)[i];
                    sum+=res*res;
                }
                return sqrt(sum);
            }
            constexpr Array<double,length> make_versor()const noexcept(noexcept(double(1/(*this).norm())) && noexcept(mpv::declval<double&>()=(*this)[0]*1.)){
                Array<double,length> new_vec;
                double n=1/(*this).norm();
                for(size_t i=0;i<length;i++){
                    new_vec[i]=(*this)[i]*n;
                }
                return new_vec;
            }
            constexpr Array<double,length> make_neg_versor()const noexcept(noexcept(double(1/(*this).norm())) && noexcept(mpv::declval<double&>()=-(*this)[0]*1.)){
                Array<double,length> new_vec;
                double n=1/(*this).norm();
                for(size_t i=0;i<length;i++){
                    new_vec[i]=-(*this)[i]*n;
                }
                return new_vec;
            }
            template<size_t s=length,typename=enable_if_t<s==3 && s==length>>
            constexpr Array<T,3> operator%(const Array<T,3>& other)const noexcept(is_nothrow_move_constructible_v<Array<T,3>> && noexcept(mpv::declval<Array&>()[0]=other[1]*other[2]-other[1]*other[2])){//producto vectorial
                Array<T,3> new_array;
                new_array[0]=(*this)[1]*other[2]-other[1]*(*this)[2];
                new_array[1]=other[0]*(*this)[2]-(*this)[0]*other[2];
                new_array[2]=(*this)[0]*other[1]-other[0]*(*this)[1];
                return new_array;
            }
            template<size_t s=length,typename=enable_if_t<s==3>>
            constexpr void operator%=(const Array<T,3>& other)noexcept(is_nothrow_move_constructible_v<Array<T,3>> && noexcept(mpv::declval<Array&>()[0]=other[1]*other[2]-other[1]*other[2])){
                Array<T,3> aux;
                aux[0]=(*this)[1]*other[2]-other[1]*(*this)[2];
                aux[1]=other[0]*(*this)[2]-(*this)[0]*other[2];
                aux[2]=(*this)[0]*other[1]-other[0]*(*this)[1];
                *this=static_cast<Array<T,3>&&>(aux);
            }
            constexpr iterator begin()noexcept{
                return elems;
            }
            constexpr iterator end()noexcept{
                return elems+length;
            }
            constexpr const_iterator begin()const noexcept{
                return elems;
            }
            constexpr const_iterator end()const noexcept{
                return elems+length;
            }
    };
    template<typename T,size_t length,typename U,typename=enable_if_t<is_convertible_v<U,T>,U>>
    constexpr Array<T,length> operator*(U scalar,const Array<T,length>& vec)noexcept(noexcept(fake_copy_init<Array<T,length>>(vec*scalar))){
        return vec*scalar;
    }
#if __cplusplus>=201703
    template<typename T,typename... Args> Array(T,Args...)->Array<enable_if_t<are_same_v<T,Args...>,T>,sizeof...(Args)+1>;
#endif
    template<typename Out,typename T,size_t length>
    Out& operator<<(Out& stream,const Array<T,length>& array){
        stream<<"(";
        for(size_t i=0;i<length;i++){
            stream<<""<<array[i];
            if(i<length-1)
                stream<<", ";		
        }
        stream<<")";
        return stream;
    }
}