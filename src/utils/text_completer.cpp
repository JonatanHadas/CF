#include "text_completer.h"

#include "serialization.h"
#include "utils.h"

#include <fstream>

TextCompleter::TextCompleter(const char* filename) :
	filename(filename) {

	load();
}

void TextCompleter::load(){
	ifstream file;
	file.open(filename, ios::in | ios::binary);
	if(!file.is_open()) return;

	texts = read_vector<string>(file, read_string);
	
	file.close();
}

void TextCompleter::save() const {
	ofstream file;
	file.open(filename, ios::out | ios::binary);
	
	write_vector<string>(file, texts, write_string);
	
	file << fflush;
	
	file.close();
}

bool is_prefix(const string& haystack, const string& needle){
	return mismatch(needle.begin(), needle.end(), haystack.begin()).first == needle.end();
}

vector<string> TextCompleter::get_suggestions(const string& start) const {
	vector<string> suggestions;
	
	for(int i = texts.size() - 1; i >= 0; i--){
		if(is_prefix(texts[i], start)) suggestions.push_back(texts[i]);
	}
	
	return suggestions;
}

void TextCompleter::add_suggestion(const string& text){
	erase(texts, text);
	texts.push_back(text);
	
	save();
}
