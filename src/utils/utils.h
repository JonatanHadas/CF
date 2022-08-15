#ifndef _UTILS_H
#define _UTILS_H

#include <vector>

using namespace std;

template<typename T>
void remove_index(vector<T>& container, int index){
	container.erase(container.begin() + index);
}

template<typename T>
T pop_index(vector<T>& container, int index){
	T value = move(container[index]);
	remove_index(container, index);
	return value;
}

#endif
