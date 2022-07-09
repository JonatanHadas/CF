#ifndef _SERIALIZATION_H
#define _SERIALIZATION_H

#include <iostream>
#include <ostream>
#include <istream>
#include <functional>
#include <vector>

using namespace std;

template<typename T>
void write_raw(ostream& output, const T& value){
	output.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
T read_raw(istream& input){
	T value;
	input.read(reinterpret_cast<char*>(&value), sizeof(T));
	return value;
}

template<typename T>
void write_vector(ostream& output, const vector<T>& value, function<void(ostream&, const T&)> write_function){
	unsigned int length = value.size();
	write_raw(output, length);
	for(unsigned int i = 0; i < length; i++){
		write_function(output, value[i]);
	}
}

template<typename T>
vector<T> read_vector(istream& input, function<T(istream&)> read_function){
	auto length = read_raw<unsigned int>(input);
	
	vector<T> value;
	for(unsigned int i = 0; i < length; i++){
		value.push_back(read_function(input));
	}
	
	return value;
}

#endif