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

void write_string(ostream& output, const string& value);

string read_string(istream& input);

template<typename T>
function<void(ostream&, const vector<T>&)> vector_write_function(function<void(ostream&, const T&)> write_function){
	return [write_function](ostream& output, const vector<T>& value){
		write_vector<T>(output, value, write_function);
	};
}

template<typename T>
function<vector<T>(istream&)> vector_read_function(function<T(istream&)> read_function){
	return [read_function](istream& input){
		return read_vector<T>(input, read_function);
	};
}

template<typename T>
void write_serializable(ostream& output, const T& value){
	value.serialize(output);
}

#endif