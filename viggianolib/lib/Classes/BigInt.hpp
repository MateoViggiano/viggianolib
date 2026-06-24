#pragma once
namespace mpv{
#define POS 0
#define NEG 1
	/*	base debe satisfacer la ecuacion 	n*n + n - 1 <= 2^64 - 1 ,con n = b - 1, osea el maximo numero de un digito de la base
											n*n + n - 2^64 <= 0
											n <= sqrt(2^64 + 1/4) - 1/2
		Esto es porque los BigInt guardan sus digitos como un array de enteros sin signo de 64 bits(2^64 valores representables) para sus operaciones,
		las cuales usan los algoritmos que se usan en primaria para sumar y multiplicar, y en caso de la multiplicacion la operacion que hace que se
		llegue al numero mas grande es n * m + s, siendo n y m dos digitos y s el sobrante de la columna anterior. En la ecuacion de arriba se supone
		que estos tienen el mayor valor posible, entonces n y m son el maximo numero representable con un digito en la base y s es ese mismo numero -1
		
		ejemplo en base 10 de como se realiza la multiplicacion
	sobrantes f2:				    1 2 2 1
	sobrantes f1:					0 0 0 0		en la implementacion solo se usa un digito para el sobrante
	sobrantes f0:					5 8 8 3 
									_________	
	multiplicando:					  5 9 9 4
	multiplicador:					  * 3 1 9
								¯¯¯¯¯¯¯¯¯¯¯
	resultado fila 0:				5 3 9 4 6
	10 * resultado fila 1:			5 9 9 4 0	notar que cada fila se multiplica por base ^ numero de fila (en la implementacion es agragar n cantidad de 0 al final de cada fila, siendo n el numero de fila)
	100 * resultado fila 2:     1 7 9 8 2 0 0	
	suma de los resultados:		1 9 1 2 0 8 6	en la implementacion no se necesita un array por cada fila, solo se usa 1 para el resultado
												y un array auxiliar en el que voy poniendo cada fila antes de sumarsela al resultado
	*/
	template<fixedsizeint::uint64_t B=1073741824>
	class BigInt{
		private:
			using uint64_t=fixedsizeint::uint64_t;
			Vector<uint64_t> list;
			bool sign;
			static bool isEqual(const Vector<uint64_t>& a,const Vector<uint64_t>& b){
				if(a.size()!=b.size())
					return false;
				else{
					for(Vector<uint64_t>::const_iterator i=a.rbegin(),j=b.rbegin();i!=a.rend();i--,j--){
						if(*i!=*j)
							return false;
					}
					return true;
				}

			}
			static bool isBigger(const Vector<uint64_t>& a,const Vector<uint64_t>& b){
				if(a.size()>b.size())
					return true;
				else if(a.size()<b.size())
					return false;
				else{
					for(Vector<uint64_t>::const_iterator i=a.rbegin(),j=b.rbegin();i!=a.rend();i--,j--){
						if(*i>*j)
							return true;
						else if(*i<*j)
							return false;
					}
					return false;
				}

			}
			static bool isSmaller(const Vector<uint64_t>& a,const Vector<uint64_t>& b){
				if(a.size()<b.size())
					return true;
				else if(a.size()>b.size())
					return false;
				else{
					for(Vector<uint64_t>::const_iterator i=a.rbegin(),j=b.rbegin();i!=a.rend();i--,j--){
						if(*i<*j)
							return true;
						else if(*i>*j)
							return false;
					}
					return false;
				}
			}
			static bool isBigger_eq(const Vector<uint64_t>& a,const Vector<uint64_t>& b){
				if(a.size()>b.size())
					return true;
				else if(a.size()<b.size())
					return false;
				else{
					for(Vector<uint64_t>::const_iterator i=a.rbegin(),j=b.rbegin();i!=a.rend();i--,j--){
						if(*i>*j)
							return true;
						else if(*i<*j)
							return false;
					}
					return true;
				}

			}
			static bool isSmaller_eq(const Vector<uint64_t>& a,const Vector<uint64_t>& b){
				if(a.size()<b.size())
					return true;
				else if(a.size()>b.size())
					return false;
				else{
					for(Vector<uint64_t>::const_iterator i=a.rbegin(),j=b.rbegin();i!=a.rend();i--,j--){
						if(*i<*j)
							return true;
						else if(*i>*j)
							return false;
					}
					return true;
				}
			}
			static Vector<uint64_t> add(const Vector<uint64_t>& a,const Vector<uint64_t>& b){
				uint64_t digit;
				bool carry=0;
				Vector<uint64_t> res;
				size_t i=0,j=0;
				while(i<a.size() and j<b.size()){
					digit=(a[i]+b[j]+carry)%B;
					carry=(a[i]+b[j]+carry)/B;
					res.append(digit);
					i++;j++;
				}
				while(i<a.size()){
					digit=(a[i]+carry)%B;
					carry=(a[i]+carry)/B;
					res.append(digit);
					i++;
				}
				while(j<b.size()){
					digit=(b[j]+carry)%B;
					carry=(b[j]+carry)/B;
					res.append(digit);
					j++;
				}
				if(carry)
					res.append(1);
				return res;
			}
			static Vector<uint64_t> sub(const Vector<uint64_t>& a,const Vector<uint64_t>& b){// a debe ser mayor a b(ni menor ni igual)
				uint64_t digit;
				bool borrow=0;
				Vector<uint64_t> res;
				size_t i=0,j=0;
				while(j<b.size()){
					if(a[i]>=b[j]+borrow){//	*i-borrow>=*j  no funcionaria porque se estan usando enteros sin signo y si i fuese 0 generaria un overflow
						digit=a[i]-borrow-b[j];//Esta resta es segura, no puede dar negativo
						borrow=0;
					}
					else{
						digit=a[i]+B-borrow-b[j];//segura
						borrow=1;
					}
					res.append(digit);
					i++;j++;
				}
				while(i<a.size()){
					if(a[i]>=borrow){//	*i-borrow>=0  tambien podria generar un overflow
						digit=a[i]-borrow;//segura
						borrow=0;
					}
					else{
						digit=a[i]+B-borrow;//segura
						borrow=1;
					}
					res.append(digit);
					i++;
				}
				while(res.back()==0 and res.size()>1)
					res.del_back();
				return res;
			}
			static void inc(Vector<uint64_t>& a){
				bool carry=1;
				for(size_t i=0;i<a.size() and carry==1;i++){
					a[i]+=carry;
					if(a[i]/B==0)
						carry=0;
					else
						a[i]=0;
				}
				if(carry)
					a.append(1);
			}
			static void dec(Vector<uint64_t>& a){
				bool borrow=1;
				for(size_t i=0;i!=a.size() and borrow==1;i++){
					if(a[i]>=1){
						a[i]--;
						borrow=0;
					}
					else{
						a[i]+=B-1;
					}
				}
				if(a.back()==0 and a.size()!=1)
					a.del_back();
			}
			static Vector<uint64_t> mul(Vector<uint64_t> a,Vector<uint64_t> b){
				uint64_t digit, carry=0;
				unsigned k=0;
				Vector<uint64_t> res,aux;
				res.append(0);
				for(size_t i=0;i<b.size();i++){
					aux.clear();
					unsigned K=k;
					while(K--){
						aux.append(0);
					}
					for(size_t j=0;j<a.size();j++){
						digit=((a[j]*b[i])+carry)%B;
						carry=((a[j]*b[i])+carry)/B;
						aux.append(digit);
					}
					if(carry){
						aux.append(carry);
						carry=0;
					}
					res=add(aux,res);
					k++;
				}
				return res;
			}
			static Vector<uint64_t> Kmul(Vector<uint64_t> a,Vector<uint64_t> c){
				size_t size=a.size()>c.size()? a.size():c.size();
				if(size==1){
					Vector<uint64_t> res;
					uint64_t digit=(a[0]*c[0])%B;
					uint64_t carry=(a[0]*c[0])/B;
					if(digit and carry){
						res.append(digit);
						res.append(carry);
					}
					else if(carry){
						res.append(0);
						res.append(carry);
					}
					else
						res.append(digit);
					return res;
				}
				else{
					
					while(a.size()<size)
						a.append(0);
					while(c.size()<size)
						c.append(0);
					Vector<uint64_t> b=a.cut(0,size/2),d=c.cut(0,size/2);
					Vector<uint64_t> ac=Kmul(a,c),bd=Kmul(b,d),ad_bc=sub(sub(Kmul(add(a,b),add(c,d)),bd),ac);

					for(size_t i=0;i<size/2;i++){
						ad_bc.insert(0,0);
					}
			/*		while(bd.size()<size)
						bd.append(0);
					bd+=ac;*/
					for(size_t i=0;i<b.size()*2;i++){
						ac.insert(0,0);
					}
					bd=add(ac,bd);
					//cout<<"ac_bd=";bd.println(true);
					

					Vector<uint64_t> res=add(bd,ad_bc);
					while(res.back()==0 and res.size()>1)
						res.del_back();
					//cout<<"res=";res.println(true);cout<<endl;
					return res;
				}
			}
			static Vector<uint64_t> div(const Vector<uint64_t>& a,const Vector<uint64_t>& b){//using namespace std;
				Vector<uint64_t> res(0),acum(a);
				//acum=add(acum,b);
				while(isBigger_eq(acum,b)){
					acum=sub(acum,b);//cout<<"acumulador: ";acum.println(true);
					inc(res);//cout<<"contador: ";res.println(true);
				}
				return res;
			}
		public:
			static constexpr uint64_t base=B;
			BigInt(long long n):sign(POS){
				if(n<0){
					n*=-1;
					sign=NEG;
				}
				do{
					list.append(n%base);
					n/=base;
				}while(n!=0);
			} 
			BigInt():sign(POS){}
			BigInt operator+(const BigInt& other)const{
				BigInt new_Int;
				if(this->sign==other.sign){
					new_Int.sign=this->sign;
					new_Int.list=add(this->list,other.list);
				}
				else if(isBigger(this->list,other.list)){
					new_Int.sign=this->sign;
					new_Int.list=sub(this->list,other.list);
				}
				else if(isSmaller(this->list,other.list)){
					new_Int.sign=other.sign;
					new_Int.list=sub(other.list,this->list);
				}
				else
					new_Int.list.append(0);
				return new_Int;
			}
			void operator+=(const BigInt& other){
				if(this->sign==other.sign){
					this->list=add(this->list,other.list);
				}
				else if(isBigger(this->list,other.list)){
					this->list=sub(this->list,other.list);
				}
				else if(isSmaller(this->list,other.list)){
					this->sign=other.sign;
					this->list=sub(other.list,this->list);
				}
				else{
					this->sign=POS;
					this->list.clear();
					this->list.append(0);
				}
			}
			BigInt& operator++(){
				if(this->sign==POS)
					inc(this->list);
				else if(this->sign==NEG and this->list.size()==1 and this->list[0]==1){
					this->list[0]=0;
					this->sign=POS;
				}
				else
					dec(this->list);
				return *this;
			}
			BigInt operator++(int){
				BigInt aux=*this;
				if(this->sign==POS)
					inc(this->list);
				else if(this->sign==NEG and this->list.size()==1 and this->list[0]==1){
					this->list[0]=0;
					this->sign=POS;
				}
				else
					dec(this->list);
				return aux;
			}
			BigInt& operator+(){
				return *this;
			}
			BigInt operator-(const BigInt& other){
				BigInt new_Int;
				if(this->sign!=other.sign){
					new_Int.sign=this->sign;
					new_Int.list=add(this->list,other.list);
				}
				else if(isBigger(this->list,other.list)){
					new_Int.sign=this->sign;
					new_Int.list=sub(this->list,other.list);
				}
				else if(isSmaller(this->list,other.list)){
					new_Int.sign=not this->sign;
					new_Int.list=sub(other.list,this->list);
				}
				else
					new_Int.list.append(0);
				return new_Int;
			}
			void operator-=(const BigInt& other){
				if(this->sign!=other.sign){
					this->list=add(this->list,other.list);
				}
				else if(isBigger(this->list,other.list)){
					this->list=sub(this->list,other.list);
				}
				else if(isSmaller(this->list,other.list)){
					this->sign=not this->sign;
					this->list=sub(other.list,this->list);
				}
				else{
					this->sign=POS;
					this->list.clear();
					this->list.append(0);
				}
			}
			BigInt& operator--(){
				if(this->sign==POS and this->list.size()==1 and this->list[0]==0){
					this->list[0]=1;
					this->sign=NEG;
				}
				else if(this->sign==POS)
					dec(this->list);
				else
					inc(this->list);
				return *this;
			}
			BigInt operator--(int){	//Deberia retornar un rvalue(no lo hace)
				BigInt aux=*this;
				if(this->sign==POS and this->list.size()==1 and this->list[0]==0){
					this->list[0]=1;
					this->sign=NEG;
				}
				else if(this->sign==POS)
					dec(this->list);
				else
					inc(this->list);
				return aux;
			}
			BigInt& operator-(){
				if(not(this->list.size()==1 and this->list[0]==0))
					this->sign=not this->sign;
				return *this;
			}
			BigInt operator*(const BigInt& other)const{
				BigInt new_Int;
				if((this->list.size()==1 and this->list[0]==0) or (other.list.size()==1 and other.list[0]==0))
					new_Int.list.append(0);
				else{
					if(this->sign!=other.sign)
						new_Int.sign=NEG;
					new_Int.list=mul(this->list,other.list);
				}
				return new_Int;
			}
			void operator*=(const BigInt& other){
				if((this->list.size()==1 and this->list[0]==0) or (other.list.size()==1 and other.list[0]==0)){
					this->list.clear();
					this->list.append(0);
					this->sign=POS;
				}
				else{
					if(this->sign!=other.sign)
						this->sign=NEG;
					else
						this->sign=POS;
					this->list=mul(this->list,other.list);
				}
			}
			BigInt operator/(const BigInt& other)const{
				if(other.list.size()==1 and other.list[0]==0){
					return 0;
				}
				BigInt new_Int;
				if(this->list.size()==1 and this->list[0]==0)
					new_Int.list.append(0);
				else if(this->sign!=other.sign)
					new_Int.sign=NEG;
				new_Int.list=div(this->list,other.list);
				return new_Int;
			}
			void operator/=(const BigInt& other){
				if(other.list.size()==1 and other.list[0]==0){
					return;
				}
				if(this->list.size()==1 and this->list[0]==0){
					this->list.clear();
					this->list.append(0);
					this->sign=POS;
				}
				else if(this->sign!=other.sign)
					this->sign=NEG;
				else
					this->sign=POS;
				this->list=div(this->list,other.list);
			}
			//#define POW
			#ifndef POW
			BigInt operator^(const BigInt& other)const{
				if(other.sign==NEG)
					return 0;
				BigInt new_Int=1;
				// if(this->sign==POS)
				// 	new_Int.sign=NEG;
				// else if(this->sign==NEG and other.even())
				// 	new_Int.sign=POS;
				// else 
				// 	new_Int.sign=NEG;
				for(BigInt i=0;i<other;i++)
					new_Int*=*this;
				return new_Int;
			}
			#else
			struct lookup{
				uint64_t cont;
				BigInt num;
				lookup(uint64_t cont,BigInt num):cont(cont),num(num){}
				lookup():cont(0),num(0){}
			};
			BigInt operator^(const BigInt& other)const{
				if(other.sign==NEG)
					return 0;
				BigInt acum=0;
				Vector<lookup> list;
				list.append(lookup(1,*this));
				while((BigInt)list.back().cont<=other){
					list.append(lookup(list.back().cont*2,list.back().num*list.back().num));
				}
				BigInt new_Int=1;
				for(auto i=list.rbegin();i!=list.rend() and acum!=other;i--){
					if(acum+i->cont<=other){
						acum+=i->cont;
						new_Int*=i->num;
					}
				}
				return new_Int;
			}
			#endif
			bool operator==(const BigInt& other)const{
				if(this->sign!=other.sign)
					return false;
				else return isEqual(this->list,other.list);
			}
			bool operator!=(const BigInt& other)const{
				if(this->sign!=other.sign)
					return true;
				else return not isEqual(this->list,other.list);
			}
			bool operator>=(const BigInt& other)const{
				if(this->sign==POS and other.sign==NEG)
					return true;
				else if(this->sign==NEG and other.sign==POS)
					return false;
				else if(this->sign==POS and other.sign==POS)
					return isBigger_eq(this->list,other.list);
				else 
					return isSmaller_eq(this->list,other.list);
			}
			bool operator>(const BigInt& other)const{
				if(this->sign==POS and other.sign==NEG)
					return true;
				else if(this->sign==NEG and other.sign==POS)
					return false;
				else if(this->sign==POS and other.sign==POS)
					return isBigger(this->list,other.list);
				else 
					return isSmaller(this->list,other.list);
			}
			bool operator<=(const BigInt& other)const{
				if(this->sign==POS and other.sign==NEG)
					return false;
				else if(this->sign==NEG and other.sign==POS)
					return true;
				else if(this->sign==POS and other.sign==POS)
					return isSmaller_eq(this->list,other.list);
				else
					return isBigger_eq(this->list,other.list);	
			}
			bool operator<(const BigInt& other)const{
				if(this->sign==POS and other.sign==NEG)
					return false;
				else if(this->sign==NEG and other.sign==POS)
					return true;
				else if(this->sign==POS and other.sign==POS)
					return isSmaller(this->list,other.list);
				else
					return isBigger(this->list,other.list);	
			}
			bool even()const{			//solo funciona si la base es par
				return this->list[0]%2==0;
			}
			bool odd()const{			//solo funciona si la base es par
				return this->list[0]%2==1;
			}
			template<typename Out,uint64_t b>
			friend Out& operator<<(Out& stream,const BigInt<b>& n);
	};
	template<typename Out,fixedsizeint::uint64_t B>
	Out& operator<<(Out& stream,const BigInt<B>& n){
		if(n.sign==NEG)
			stream<<'-';
		BigInt<10> b=B;
		BigInt<10> x;
		BigInt<10> j=0;
		for(auto i:n.list){
			x+=BigInt<10>(i)*(b^(j++));
		}
		for(Vector<fixedsizeint::uint64_t>::const_iterator i=x.list.rbegin();i!=x.list.rend();i--){
			stream<<*i;
		}
		return stream;
	}
	//template<>constexpr bool
	using Int=BigInt<>;
	// #define Y 1
	// #if Y==0
	// template<uint64_t b>
	// std::ostream& operator<<(std::ostream& stream,BigInt<b> n){
	// 	if(n.sign==NEG)
	// 		stream<<'-';
	// 	for(Vector<uint64_t>::const_iterator i=n.list.rbegin();i!=n.list.rend();i--){
	// 		stream<<*i;
	// 	}
	// 	return stream;
	// }
	// #else

	// template<uint64_t b>
	// std::ostream& operator<<(std::ostream& stream,BigInt<b> n){
	// 	if(n.sign==BigInt<b>::NEG)
	// 		stream<<'-';
	// 	Vector<uint64_t> lista;
	// 	BigInt<10> B=b;
	// 	BigInt<10> x;
	// 	BigInt<10> j=0;
	// 	for(auto i:n.list){
	// 		x+=BigInt<10>(i)*(B^(j++));
	// 	}
	// 	for(Vector<uint64_t>::const_iterator i=x.list.rbegin();i!=x.list.rend();i--){
	// 		stream<<*i;
	// 	}
	// 	return stream;
	// }
	// #endif
#undef POS
#undef NEG
}