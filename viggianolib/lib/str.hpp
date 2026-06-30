#pragma once
namespace mpv{
	template<typename Char>
	constexpr bool is_alpha(Char c)noexcept{
		return (c>=65 && c<=90) || (c>=97 && c<=122);
	}
	template<typename Char>
	constexpr bool is_alnum(Char c)noexcept{
		return (c>=65 && c<=90) || (c>=97 && c<=122) || (c>=48 && c<=57);
	}
	template<typename Char>
	constexpr bool is_special(Char c)noexcept{
		return !((c>=65 && c<=90) || (c>=97 && c<=122) || (c>=48 && c<=57));
	}
	template<typename Char>
	constexpr bool is_numeric(Char c)noexcept{
		return c>=48 && c<=57;
	}
	template<typename Char>
	constexpr bool is_alnum_us(Char c)noexcept{// true if c is alphanumeric or an underscore
		return (c>=65 && c<=90) || (c>=97 && c<=122) || (c>=48 && c<=57) || c=='_';
	}
	template<typename Char>
	constexpr Char upper(const Char c)noexcept{
		return (c>=97 && c<=122)? c-32 : c;
	}
	template<typename Char>
	constexpr Char lower(const Char c)noexcept{
		return (c>=65 && c<=90)? c+32 : c;
	}
	template<typename Char>
	constexpr size_t parseUint(const Char* s){
		size_t num=0;
		size_t i=0;
		while(s[i]==' ' || s[i]=='\n' || s[i]=='\r' || s[i]=='\t')i++;
		for(;s[i]>='0' && s[i]<='9';i++){
			num*=10;
			num+=s[i]-48;// 'n'- 48 == n
		}
		return num;
	}
	template<typename Char>
	constexpr ssize_t parseInt(const Char* s){
		ssize_t num=0;
		size_t i=0;
		bool neg=false;
		while(s[i]==' ' || s[i]=='\n' || s[i]=='\r' || s[i]=='\t')i++;
		if(s[i]=='-'){
			neg=true;
			i++;
		}else if(s[i]=='+')i++;
		for(;s[i]>='0' && s[i]<='9';i++){
			num*=10;
			num+=s[i]-48;
		}
		return neg ? -num : num;
	}
	template<typename Char>
	constexpr bool is_float_convertible(const Char* s){
		if(*s=='\0') return false;
		if(*s=='+' || *s=='-') s++;
		if(*s=='e' || *s=='E') return false;
		const Char* e_pos=nullptr;
		bool dot_found=false;
		while(*s!='\0'){
			if(!is_numeric(*s)){
				if(*s=='.'){
					if(dot_found || e_pos!=nullptr) return false;
					else dot_found=true;
				}
				else if(*s=='e' || *s=='E'){
					if(e_pos!=nullptr) return false;
					e_pos=s++;
					if(*s=='\0') return false;
					if(*s=='+' || *s=='-'){
						e_pos=s;
					}
				}
				else return false;
			}
			s++;
		}
		return s-1!=e_pos;	//Checks if the dot is not the last character
	}
	template<typename Char>
	constexpr long double parseFloat(const Char* s){
		long double num=0;
		size_t i=0;
		bool neg=false;
		while(s[i]==' ' || s[i]=='\n' || s[i]=='\r' || s[i]=='\t')i++;
		if(s[i]=='-'){
			neg=true;
			i++;
		}else if(s[i]=='+')i++;
		for(;s[i]>='0' && s[i]<='9';i++){
			num*=10;
			num+=s[i]-48;
		}
		if(s[i]=='.'){i++;
			for(long double div=10;s[i]>='0' && s[i]<='9';i++,div*=10){
				num+=((s[i]-48)/div);
			}		
		}
		if(s[i]=='e' || s[i]=='E'){i++;
			long double e=10;
			unsigned int e_num=0;
			if(s[i]=='-'){
				e=0.1;
				i++;
			}else if(s[i]=='+')i++;
			for(;s[i]>='0' && s[i]<='9';i++){
				e_num*=10;
				e_num+=s[i]-48;
			}
			for(unsigned short j=0;j<e_num;j++){
				num*=e;
			}
		}
		return neg ? -num : num;
	}
	inline constexpr char to_char(int digit){
		return digit<10? digit+48:digit+55;
	}
}