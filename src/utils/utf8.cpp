#include "utf8.h"

#define FIRST_B_MASK 0b11000000
#define NON_FIRST_B  0b10000000

void utf8_pop_back(std::string& str){
	while((str.back() & FIRST_B_MASK) == NON_FIRST_B){
		str.pop_back();
	}
	str.pop_back();
}

void reverse_utf8(std::string& str){
	char tmp[5];
	std::string str1 = str, str2 = str;
	str.clear();
	while(str1.length()>0){
		utf8_pop_back(str2);
		
		size_t len = str1.copy(tmp,4,str2.length());
		tmp[len]='\0';
		
		str += tmp;
		
		utf8_pop_back(str1);
	}
}