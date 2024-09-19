#pragma once
namespace mpv{
    template<typename T,size_t R,size_t C>
    class Matrix:public Array<Array<T,C>,R>{
        static constexpr size_t rows=R;
        static constexpr size_t columns=C;
        static constexpr size_t size=R*C;
        using base=Array<Array<T,C>,R>;
        using base::base;
        public:
            Matrix(const T(&m)[R][C]){
                for(size_t i=0;i<R;i++){
                    this->buffer[i]=m[i];
                }
            }
            Matrix operator+(const Matrix& other)const{
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=(*this)[i]+other[i];
                }
                return new_matrix;
            }
            Matrix operator-(const Matrix& other)const{
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=(*this)[i]+other[i];
                }
                return new_matrix;
            }
            Matrix operator-()const{
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=-(*this)[i];
                }
                return new_matrix;
            }
            Matrix operator+()const{
                Matrix new_matrix;
                for(size_t i=0;i<R;i++){
                    new_matrix[i]=+(*this)[i];
                }
                return new_matrix;
            }
            void triangulate(){
                size_t zc=0;// column of zeros counter
                for(size_t i=0;i<R-1+zc && i<C;i++){
                    if(is_zero(this->buffer[i-zc][i])){
                        bool column_of_zeros=true;
                        for(size_t j=i+1-zc;j<R;j++){
                            if(!is_zero(this->buffer[j][i])){
                                column_of_zeros=false;
                                swap(this->buffer[j],this->buffer[i-zc]);
                                break;
                            }
                        }
                        if(column_of_zeros){
                            zc++;
                            continue;
                        }
                    }
                    for(size_t j=i+1-zc;j<R;j++){
                        if(!is_zero(this->buffer[j][i])){
                            this->buffer[j]=this->buffer[j]*this->buffer[i-zc][i]-this->buffer[i-zc]*this->buffer[j][i];
                        }
                    }
                }
            }
            Matrix triangulated()const{
                Matrix new_matrix(*this);
                new_matrix.triangulate();
                return new_matrix;
            }
            template<size_t Rows,size_t Columns,typename=enable_if_t<C==Rows>>
            Matrix<T,R,Columns> operator*(const Matrix<T,Rows,Columns>& other){
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
            Matrix operator*(U scalar)const{
                Matrix new_matrix;
                for(size_t i=0;i<R;i++)
                    new_matrix[i]=this->buffer[i]*scalar;
                return new_matrix;
            }
            template<typename U,typename=enable_if_t<is_convertible_v<U,T>>>
            void operator*=(U scalar){
                for(size_t i=0;i<R;i++)
                    this->buffer[i]*=scalar;
            }
            template<size_t R1=R-1,size_t C1=C-1>
            enable_if_t<(R1>0 && C1>0),Matrix<T,R-1,C-1>> subm(const size_t I,const size_t J)const{
                Matrix<T,R1,C1> subm;
                for(size_t j=0;j<J;j++){
                    for(size_t i=0;i<I;i++){
                        subm[i][j]=this->buffer[i][j];
                    }
                }
                for(size_t j=J+1;j<C;j++){
                    for(size_t i=0;i<I;i++){
                        subm[i][j-1]=this->buffer[i][j];
                    }
                }
                for(size_t j=0;j<J;j++){
                    for(size_t i=I+1;i<R;i++){
                        subm[i-1][j]=this->buffer[i][j];
                    }
                }
                for(size_t j=J+1;j<C;j++){
                    for(size_t i=I+1;i<R;i++){
                        subm[i-1][j-1]=this->buffer[i][j];
                    }
                }
                return subm;
            }
            template<size_t N=R>
            enable_if_t<N==C && N!=1,T> determinant()const{
                if constexpr(N==2){
                    return this->buffer[0][0]*this->buffer[1][1]-this->buffer[0][1]*this->buffer[1][0];
                }
                else{
                    T result=0;
                    for(size_t k=0;k<R;k++){
                        Matrix<T,N-1,N-1> subm;
                        for(size_t j=0;j<k;j++){
                            for(size_t i=1;i<R;i++){
                                subm[i-1][j]=this->buffer[i][j];
                            }
                        }
                        for(size_t j=k+1;j<C;j++){
                            for(size_t i=1;i<R;i++){
                                subm[i-1][j-1]=this->buffer[i][j];
                            }
                        }
                        //std::cout<<this->buffer[0][k]*minus1pow(k)<<" * "<<subm.determinant()<<std::endl;
                        result+=this->buffer[0][k]*minus1pow(k)*subm.determinant();
                    }
                    return result;
                }
            }
            template<size_t N=R>
            enable_if_t<N==C && N!=1,Matrix> adj()const{
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
            Matrix<T,C,R> transposed()const{
                Matrix<T,C,R> new_matrix;
                for(size_t i=0;i<R;i++){
                    for(size_t j=0;j<C;j++){
                        new_matrix[j][i]=(*this)[i][j];
                    }
                }
                return new_matrix;
            }
            template<size_t N=R>
            enable_if_t<N==C && N!=1,Optional<Matrix>> inverse()const{
                const T det=this->determinant();
                if(det==0) return {};
                else return this->transposed().adj()*(1/det);
            }
    };
    template<typename T,size_t R,size_t C,typename U,typename=enable_if_t<is_convertible_v<U,T>,U>>
    Matrix<T,R,C> operator*(U scalar,const Matrix<T,R,C>& m){
        return m*scalar;
    }
    template<typename Out,typename T,size_t R,size_t C>
    Out& operator<<(Out& stream,const Matrix<T,R,C>& matrix){
        stream<<'(';
        for(size_t i=0;i<R;i++){
            for(size_t j=0;j<C;j++){
                stream<<matrix[i][j];
                if(j!=C-1) stream<<",\t";
            }
            if(i!=R-1) stream<<"\n";
        }
        return stream<<')';
    }
}
