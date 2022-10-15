#ifndef _TEXT_COMPLETER_H
#define _TEXT_COMPLETER_H

#include <vector>
#include <string>

using namespace std;

class TextCompleter{
	vector<string> texts;
	
	const char* filename;
	
	void load();
	void save() const;
public:
	TextCompleter(const char* filename);
	
	vector<string> get_suggestions(const string& start) const;
	void add_suggestion(const string& text);
};

#endif