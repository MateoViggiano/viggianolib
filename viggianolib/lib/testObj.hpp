#pragma once
namespace mpv{
#if ((defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)) || defined(_INC_STDIO)) && defined(SHOWCOUNT)
	struct testObj COUNT_IT {
		static long long objCount;
		int a;
		int* array;
		int get(){return a;}
		testObj() :a(99),array(new int[10]){
			objCount++;
	        puts("\ndefault constructor");
		}
		testObj(int a):a(a),array(new int[10]){
			for(int i=0;i<10;i++)
				array[i]=a;
			objCount++;
			puts("\nassignment constructor");
		}
		testObj(const testObj& other):a(other.a),array(new int[10]){
			for(int i=0;i<10;i++)
				array[i]=a;
			objCount++;
			puts("\ncopy constructor");
		}
		testObj(testObj&& other):a(other.a){
			this->array=other.array;
			other.array=nullptr;
			objCount++;
			puts("\nmovement constructor");
		}
		void operator=(const testObj& other){
			delete[] this->array;
			this->array=new int[10];
			for(int i=0;i<10;i++)
				this->array[i]=other.array[i];
			this->a=other.a;
			puts("\ncopy operator");
		}
		void operator=(testObj&& other){
			delete[] this->array;
			this->array=other.array;
			other.array=nullptr;
			this->a=other.a;
			puts("\nmovement operator");
		}
		~testObj(){
			delete[] this->array;
			//if(--objCount==0)
				printf("\nOBJECT COUNTER=%lli\n",--objCount);
		}
		bool operator==(const testObj& other)const{
			return this->a==other.a;
		}
		bool operator!=(const testObj& other)const{
			return this->a!=other.a;
		}
		bool operator<=(const testObj& other)const{
			return this->a<=other.a;
		}
		bool operator>=(const testObj& other)const{
			return this->a>=other.a;
		}
		bool operator<(const testObj& other)const{
			return this->a<other.a;
		}
		bool operator>(const testObj& other)const{
			return this->a>other.a;
		}
		operator int(){
			return a;
		}
	};
#else
	struct testObj COUNT_IT {
	    static long long objCount;
		int a;
		int* array;
		int get(){return a;}
		testObj():a(99),array(new int[10]){objCount++;}
		testObj(int a):a(a),array(new int[10]){objCount++;
			for(int i=0;i<10;i++)
				array[i]=a;
		}
		testObj(const testObj& other):a(other.a),array(new int[10]){
			for(int i=0;i<10;i++)
				array[i]=a;
			objCount++;
		}
		testObj(testObj&& other):a(other.a){
			this->array=other.array;
			other.array=nullptr;
			objCount++;
		}
		void operator=(const testObj& other){
			delete[] this->array;
			this->array=new int[10];
			for(int i=0;i<10;i++)
				this->array[i]=other.array[i];
			this->a=other.a;
		}
		void operator=(testObj&& other){
			delete[] this->array;
			this->array=other.array;
			other.array=nullptr;
			this->a=other.a;
		}
		~testObj(){
			delete[] this->array;
			--objCount;
		}
		bool operator==(const testObj& other)const{
			return this->a==other.a;
		}
		bool operator!=(const testObj& other)const{
			return this->a!=other.a;
		}
		bool operator<=(const testObj& other)const{
			return this->a<=other.a;
		}
		bool operator>=(const testObj& other)const{
			return this->a>=other.a;
		}
		bool operator<(const testObj& other)const{
			return this->a<other.a;
		}
		bool operator>(const testObj& other)const{
			return this->a>other.a;
		}
		operator int(){
			return a;
		}
	};
#endif
	long long testObj::objCount=0;
	template<typename Out>
	Out& operator<<(Out& stream,const testObj& obj){
		stream<<obj.a;
		return stream;
	}
}