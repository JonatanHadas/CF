#ifndef _TEXTBOX_H
#define _TEXTBOX_H

#include "texts.h"
#include "subview.h"

#include "../utils/text_completer.h"

#include <string>
#include <memory>

using namespace std;

class TextBox : public SubView {
	string text;
	string last_text;
	unique_ptr<Msg> msg, completion_msg;
	
	FontType font;
	SDL_Color color;
	SDL_Color back_color;
	
	TextCompleter* completer;
	vector<string> suggestions;
	int current_suggestion;
	
	int margin;
	
	bool updated, typing, active;

	TextBox(
		const SDL_Rect& rect, bool translucent,
		FontType font,
		int margin,
		const SDL_Color& color, const SDL_Color& back_color,
		TextCompleter* completer
	);

	void set();
protected:
	bool on_event(const SDL_Event& event);
	void draw_content(SDL_Renderer* renderer);
	
	virtual void draw_back(SDL_Renderer* renderer, bool typing);
	virtual void on_set(const string& text) = 0;
	virtual string get_default_text() = 0;
	virtual void on_active();
public:
	TextBox(
		const SDL_Rect& rect, bool translucent,
		FontType font,
		int margin,
		const SDL_Color& color, const SDL_Color& back_color
	);
	TextBox(
		const SDL_Rect& rect, bool translucent,
		FontType font,
		int margin,
		const SDL_Color& color, const SDL_Color& back_color,
		TextCompleter& completer
	);

	void step();
	void lose_focus();
	
	void set_color(const SDL_Color& color, const SDL_Color& back_color);
	void set_text(const string& text);
	
	void set_active(bool is_active);
	
	int get_text_width();
	
	void update();
};

#endif
