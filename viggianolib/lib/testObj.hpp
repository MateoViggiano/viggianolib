#pragma once
namespace mpv{
	struct testObj COUNT_IT {
		static long long objCount;
		int a;
		int* array;
		int get(){return a;}
		testObj() :a(-99),array(new int[10]){
			objCount++;
	        PUTS("\ndefault constructor");
		}
		testObj(int a):a(a),array(new int[10]){objCount++;
			for(int i=0;i<10;i++)
				array[i]=a;
			PUTS("\nassignment constructor");
		}
		testObj(const testObj& other):a(other.a),array(new int[10]){
			//if(objCount==8) throw "objCount==8";
			objCount++;
			for(int i=0;i<10;i++)
				array[i]=other.array[i];
			PUTS("\ncopy constructor");
		}
		testObj(testObj&& other)noexcept:a(other.a){objCount++;
			this->array=other.array;
			other.a=-100;
			other.array=nullptr;
			PUTS("\nmovement constructor");
		}
		void operator=(const testObj& other){
			delete[] this->array;
			this->array=new int[10];
			for(int i=0;i<10;i++)
				this->array[i]=other.array[i];
			this->a=other.a;
			PUTS("\ncopy operator");
		}
		void operator=(testObj&& other)noexcept{
			if(this==&other)return;
			if(array) delete[] this->array;
			this->array=other.array;
			other.array=nullptr;
			this->a=other.a;
			other.a=-100;
			PUTS("\nmovement operator");
		}
		~testObj(){
			if(array) delete[] this->array;
			//if(--objCount==0)
				PRINTF("\nOBJECT COUNTER=%lli\n",--objCount);
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
		explicit operator int()const{
			return a;
		}
	};
	inline long long testObj::objCount=0;
	template<typename Out>
	Out& operator<<(Out& stream,const testObj& obj){
		stream<<obj.a;
		return stream;
	}
}