#pragma once
namespace mpv{
	bool is_number(char c){
		return c>=48 and c<=57;
	}
	bool is_letter(char c){
		return (c>=65 and c<=90) or (c>=97 and c<=122);
	}
	bool is_special(char c){
		return not(c>=48 and c<=57) and not((c>=65 and c<=90) or (c>=97 and c<=122));
	}
	size_t parseUint(const char* buffer){
		size_t num=0;
		size_t i=0;
		while(buffer[i]==' ' or buffer[i]=='\n' or buffer[i]=='\r' or buffer[i]=='\t')i++;
		for(;buffer[i]>='0' and buffer[i]<='9';i++){
			num*=10;
			num+=buffer[i]-48;// 'n'- 48 == n
		}
		return num;
	}
	ssize_t parseInt(const char* buffer){
		ssize_t num=0;
		size_t i=0;
		bool neg=false;
		while(buffer[i]==' ' or buffer[i]=='\n' or buffer[i]=='\r' or buffer[i]=='\t')i++;
		if(buffer[i]=='-'){
			neg=true;
			i++;
		}else if(buffer[i]=='+')i++;
		for(;buffer[i]>='0' and buffer[i]<='9';i++){
			num*=10;
			num+=buffer[i]-48;
		}
		return neg ? -num : num;
	}
	long double parseFloat(const char* buffer){
		long double num=0;
		size_t i=0;
		bool neg=false;
		while(buffer[i]==' ' or buffer[i]=='\n' or buffer[i]=='\r' or buffer[i]=='\t')i++;
		if(buffer[i]=='-'){
			neg=true;
			i++;
		}else if(buffer[i]=='+')i++;
		for(;buffer[i]>='0' and buffer[i]<='9';i++){
			num*=10;
			num+=buffer[i]-48;
		}
		if(buffer[i]=='.'){i++;
			for(long double div=10;buffer[i]>='0' and buffer[i]<='9';i++,div*=10){
				num+=((buffer[i]-48)/div);
			}		
		}
		if(buffer[i]=='e' or buffer[i]=='E'){i++;
			long double e=10;
			unsigned int e_num=0;
			if(buffer[i]=='-'){
				e=0.1;
				i++;
			}else if(buffer[i]=='+')i++;
			for(;buffer[i]>='0' and buffer[i]<='9';i++){
				e_num*=10;
				e_num+=buffer[i]-48;
			}
			for(unsigned short j=0;j<e_num;j++){
				num*=e;
			}
		}
		return neg ? -num : num;
	}
	inline char to_char(char digit){
		return digit<10? digit+48:digit+55;
	}
	template<typename Int,typename String_type=String,short base=10>
	String_type to_str(Int num){
		String_type new_str,aux;
		if(num<0){
			num=-num;
			new_str='-';
		}
		do{
			aux+=to_char(num%base);
			num/=base;
		}while(num);
		for(auto i=aux.len();i>0;i--)
			new_str+=aux[i-1];
		return new_str;
	}
}