#include "keyset.h"

#include "../utils/serialization.h"
#include "../utils/utils.h"

#include <fstream>

void KeySet::serialize(ostream& output) const{
	write_raw(output, left);
	write_raw(output, right);
}

KeySet KeySet::deserialize(istream& input){
	KeySet value;
	
	value.left = read_raw<SDL_Scancode>(input);
	value.right = read_raw<SDL_Scancode>(input);
	
	return value;
}

KeySetManager::KeySetManager(const char* filename) :
	filename(filename) {
		
	load();
}

void KeySetManager::load(){
	ifstream file;
	file.open(filename, ios::in | ios::binary);
	if(!file.is_open()) return;

	keysets = read_vector<KeySet>(file, KeySet::deserialize);
	
	file.close();
}

void KeySetManager::save() const {
	ofstream file;
	file.open(filename, ios::out | ios::binary);
	
	write_vector<KeySet>(file, keysets, [](ostream& output, const KeySet& keyset){
		keyset.serialize(output);
	});
	
	file << fflush;
	
	file.close();
}

int KeySetManager::get_index(int id) const {
	for(int i = 0; i < ids.size(); i++){
		if(ids[i] == id) return i;
	}
	return -1;
}

const KeySet& KeySetManager::get_keys(int id) const {
	return keysets[get_index(id)];
}

void KeySetManager::set_keys(int id, const KeySet& keys){
	keysets[get_index(id)] = keys;
	
	save();
}

int KeySetManager::get_new(){
	ids.push_back(next_id++);
	if(ids.size() > keysets.size()){
		keysets.push_back(KeySet());
	}
	
	return ids.back();
}

void KeySetManager::remove(int id){
	int index = get_index(id);
	
	KeySet keys = pop_index(keysets, index);
	remove_index(ids, index);
	
	keysets.insert(keysets.begin() + ids.size(), keys);
	
	save();
}
