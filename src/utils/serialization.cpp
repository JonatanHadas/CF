#include "serialization.h"

void write_string(ostream& output, const string& value){
	unsigned int length = value.size();
	write_raw(output, length);
	output.write(value.data(), length);
}

string read_string(istream& input){
	auto length = read_raw<unsigned int>(input);
	
	string value(length, '\0');
	input.read(&value[0], length);
	
	return value;
}
