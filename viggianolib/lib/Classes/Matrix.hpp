#pragma once
namespace mpv{
    template<typename T,size_t R,size_t C>
    class Matrix:public Array<Array<T,C>,R>{
        public:
            using base=Array<Array<T,C>,R>;
            using value_type=T;
            using size_type=size_t;
            using difference_type=ptrdiff_t;
            using pointer=T*;
            using const_pointer=const T*;
            using reference=T&;
            using const_reference=const T&;
            using iterator=typename base::iterator;
            using const_iterator=typename base::const_iterator;
            static constexpr size_t rows=R;
            static constexpr size_t columns=C;
            static constexpr size_t length=R*C;
            Matrix()noexcept(is_nothrow_default_constructible_v<T>)=default;
            Matrix(std::initializer_list<Array<T,C>> ilist)noexcept(is_nothrow_default_constructible_v<Matrix> && noexcept(mpv::copy(this->elems,ilist.begin(),ilist.end()))){
            	mpv::copy(this->elems,ilist.begin(),ilist.end());
            }
            constexpr size_t size()noexcept{
                return rows;
            }
            constexpr void fill(T val)noexcept(noexcept((*this)[0][0]=val)){
                for(size_t i=0;i<R;i++){
                    for(size_t j=0;j<C;j++){
                        (*this)[i][j]=val;
                    }
                }
            }
            constexpr Matrix operator+(const Matrix& other)const noexcept(is_nothrow_default_constructible_v<Matrix> && is_nothrow_move_constructible_v<Matrix> && noexcept(declval<Array<T,C>&>()=(*this)[0]+other[0])){
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=(*this)[i]+other[i];
                }
                return new_matrix;
            }
            constexpr Matrix operator-(const Matrix& other)const noexcept(is_nothrow_default_constructible_v<Matrix> && is_nothrow_move_constructible_v<Matrix> && noexcept(declval<Array<T,C>&>()=(*this)[0]-other[0])){
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=(*this)[i]-other[i];
                }
                return new_matrix;
            }
            constexpr Matrix operator-()const noexcept(is_nothrow_default_constructible_v<Matrix> && is_nothrow_move_constructible_v<Matrix> && noexcept(declval<Array<T,C>&>()=-(*this)[0])){
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=-(*this)[i];
                }
                return new_matrix;
            }
            constexpr Matrix operator+()const noexcept(is_nothrow_default_constructible_v<Matrix> && is_nothrow_move_constructible_v<Matrix> && noexcept(declval<Array<T,C>&>()=+(*this)[0])){
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=+(*this)[i];
                }
                return new_matrix;
            }
            constexpr void triangulate()noexcept(noexcept(is_zero(this->elems[0][0])) && noexcept(swap(this->elems[0],this->elems[0])) && noexcept(this->elems[0]=this->elems[0]*this->elems[0][0]-this->elems[0]*this->elems[0][0])){
                size_t zc=0;// column of zeros counter
                for(size_t i=0;i<R-1+zc && i<C;i++){
                    if(is_zero(this->elems[i-zc][i])){
                        bool column_of_zeros=true;
                        for(size_t j=i+1-zc;j<R;j++){
                            if(!is_zero(this->elems[j][i])){
                                column_of_zeros=false;
                                swap(this->elems[j],this->elems[i-zc]);
                                break;
                            }
                        }
                        if(column_of_zeros){
                            zc++;
                            continue;
                        }
                    }
                    for(size_t j=i+1-zc;j<R;j++){
                        if(!is_zero(this->elems[j][i])){
                            this->elems[j]=this->elems[j]*this->elems[i-zc][i]-this->elems[i-zc]*this->elems[j][i];
                        }
                    }
                }
            }
            constexpr Matrix triangulated()const noexcept(is_nothrow_copy_constructible_v<Matrix> && is_nothrow_move_constructible_v<Matrix> && noexcept(declval<Matrix&>().triangulate())){
                Matrix new_matrix(*this);
                new_matrix.triangulate();
                return new_matrix;
            }
            template<size_t Rows,size_t Columns,typename=enable_if_t<C==Rows>>
            constexpr Matrix<T,R,Columns> operator*(const Matrix<T,Rows,Columns>& other)const noexcept(is_nothrow_default_constructible_v<Matrix<T,R,Columns>> && is_nothrow_move_constructible_v<Matrix<T,R,Columns>> && noexcept(mpv::declval<T&>()=(*this)[0][0]*other[0][0])){
                Matrix<T,R,Columns> new_matrix;
                for(size_t i=0;i<R;i++){
                    for(size_t j=0;j<Columns;j++){
                        new_matrix[i][j]=0;
                        for(size_t k=0;k<C;k++){
                            new_matrix[i][j]+=(*this)[i][k]*other[k][j];
                        }
                    }
                }
                return new_matrix;
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            constexpr Matrix operator*(U scalar)const noexcept(is_nothrow_default_constructible_v<Matrix> && noexcept(mpv::declval<Matrix&>()[0]=this->elems[0]*scalar)){
                Matrix new_matrix;
                for(size_t i=0;i<R;i++)
                    new_matrix[i]=this->elems[i]*scalar;
                return new_matrix;
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            constexpr void operator*=(U scalar)noexcept(noexcept(this->elems[0]*=scalar)){
                for(size_t i=0;i<R;i++)
                    this->elems[i]*=scalar;
            }
            template<size_t R1=R-1,size_t C1=C-1>
            constexpr enable_if_t<(R1>0 && C1>0),Matrix<T,R-1,C-1>> subm(const size_t I,const size_t J)const noexcept(is_nothrow_default_constructible_v<Matrix<T,R1,C1>> && is_nothrow_move_constructible_v<Matrix<T,R1,C1>> && noexcept(mpv::declval<T&>()=this->elems[0][0])){
                Matrix<T,R1,C1> subm;
                for(size_t j=0;j<J;j++){
                    for(size_t i=0;i<I;i++){
                        subm[i][j]=this->elems[i][j];
                    }
                }
                for(size_t j=J+1;j<C;j++){
                    for(size_t i=0;i<I;i++){
                        subm[i][j-1]=this->elems[i][j];
                    }
                }
                for(size_t j=0;j<J;j++){
                    for(size_t i=I+1;i<R;i++){
                        subm[i-1][j]=this->elems[i][j];
                    }
                }
                for(size_t j=J+1;j<C;j++){
                    for(size_t i=I+1;i<R;i++){
                        subm[i-1][j-1]=this->elems[i][j];
                    }
                }
                return subm;
            }
            template<size_t N=R>
            constexpr enable_if_t<N==C,T> determinant()const noexcept(is_nothrow_constructible_v<T,int> && noexcept(mpv::declval<T&>()+=this->elems[0][0]*mpv::declval<size_t>()*mpv::declval<T>()) && noexcept(fake_copy_init<T>(this->elems[0][0]*this->elems[1][1]-this->elems[0][1]*this->elems[1][0]))){
                if constexpr(N==0){
                    return 1;
                }
                else if constexpr(N==1){
                    return this->elems[0][0];
                }
                else if constexpr(N==2){
                    return this->elems[0][0]*this->elems[1][1]-this->elems[0][1]*this->elems[1][0];
                }
                else{
                    T result=0;
                    for(size_t k=0;k<R;k++){
                        Matrix<T,N-1,N-1> subm;
                        for(size_t j=0;j<k;j++){
                            for(size_t i=1;i<R;i++){
                                subm[i-1][j]=this->elems[i][j];
                            }
                        }
                        for(size_t j=k+1;j<C;j++){
                            for(size_t i=1;i<R;i++){
                                subm[i-1][j-1]=this->elems[i][j];
                            }
                        }
                        result+=this->elems[0][k]*minus1pow(k)*subm.determinant();
                    }
                    return result;
                }
            }
            template<size_t N=R>
            constexpr enable_if_t<N==C,Matrix> cof()const noexcept([]{
                                                                        if constexpr(N==1) return noexcept(fake_copy_init<Matrix>(Matrix{{1}}));
                                                                        else return is_nothrow_default_constructible_v<Matrix> && is_nothrow_move_constructible_v<Matrix> && is_nothrow_copy_assignable_v<T> && noexcept(mpv::declval<T&>()=minus1pow(0)*declval<Matrix>().subm(0,0).determinant());
                                                                    }()){
                if constexpr(N==1) return {{1}};
                else{
                    Matrix new_matrix;
                    if constexpr(N==2){
                        new_matrix[0][0]=(*this)[1][1];
                        new_matrix[1][1]=(*this)[0][0];
                        new_matrix[1][0]=-(*this)[0][1];
                        new_matrix[0][1]=-(*this)[1][0];
                    }
                    else{
                        for(size_t i=0;i<R;i++){
                            for(size_t j=0;j<C;j++){
                                new_matrix[i][j]=minus1pow(i+j)*this->subm(i,j).determinant();
                            }
                        }
                    }
                    return new_matrix;                    
                }
            }
            constexpr Matrix<T,C,R> transposed()const noexcept(is_nothrow_default_constructible_v<Matrix<T,C,R>> && is_nothrow_move_constructible_v<Matrix<T,C,R>> && noexcept(declval<T&>()=(*this)[0][0])){
                Matrix<T,C,R> new_matrix;
                for(size_t i=0;i<R;i++){
                    for(size_t j=0;j<C;j++){
                        new_matrix[j][i]=(*this)[i][j];
                    }
                }
                return new_matrix;
            }
            template<size_t N=R>
            constexpr enable_if_t<N==C,Optional<Matrix>> inverse()const noexcept(noexcept(T(this->determinant())) && noexcept(fake_copy_init<Optional<Matrix>>(this->transposed().cof()*(1/mpv::declval<T&>())))){
                const T det=this->determinant();
                if(det==0) return {};
                else return this->transposed().cof()*(1/det);
            }
    };
    template<typename T,size_t R,size_t C,typename U,typename=enable_if_t<is_convertible_v<U,T>,U>>
    constexpr Matrix<T,R,C> operator*(U scalar,const Matrix<T,R,C>& m)noexcept(noexcept(fake_copy_init<Matrix<T,R,C>>(m*scalar))){
        return m*scalar;
    }
    template<typename Out,typename T,size_t R,size_t C>
    Out& operator<<(Out& stream,const Matrix<T,R,C>& matrix){
        stream<<"(";
        for(size_t i=0;i<R;i++){
            for(size_t j=0;j<C;j++){
                stream<<""<<matrix[i][j];
                if(j!=C-1) stream<<",\t";
            }
            if(i!=R-1) stream<<"\n";
        }
        stream<<")";
        return stream;
    }
}
