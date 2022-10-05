#ifndef _OPTIONS_MENU_H
#define _OPTIONS_MENU_H

#include "subview.h"
#include "subview_manager.h"
#include "button.h"

#include "texts.h"
#include "gui_utils.h"

#include <vector>
#include <string>
#include <memory>

class OptionsMenu;

class OptionButton : public Button{
	OptionsMenu& menu;
	
	Msg msg;
	unique_ptr<Texture> texture;
	
	SDL_Color pressed, released, inactive;
	
	int option;
	
	void draw_text(SDL_Renderer* renderer, SDL_Color color);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);
	
	void on_pressed();
public:
	OptionButton(
		const SDL_Rect& rect,
		int option,
		Msg&& msg,
		const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive,
		OptionsMenu& menu
	);
};

class OptionsMenu : public SubView {
	vector<string> texts;
	
	double follow_speed;
	SDL_Rect rect;
	
	SDL_Color pressed, released, inactive;
	FontType font;
	
	vector<unique_ptr<OptionButton>> buttons;
	
	bool active;
	
	SubViewManager manager;
	void init(SDL_Renderer* renderer);
protected:
	virtual vector<SDL_Rect> get_locations(const vector<Msg>& msgs) = 0;
	virtual void on_set(int option) = 0;
	
	bool on_event(const SDL_Event& event);
	void draw_content(SDL_Renderer* renderer);
public:
	OptionsMenu(
		const SDL_Rect& rect,
		vector<string>&& texts,
		double follow_speed,
		const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive,
		FontType font
	);

	void lose_focus();
	void step();

	virtual int get_option() = 0;
	void set_option(int opt);
	
	void set_active(bool is_active);
};

class VerticalOptionsMenu : public OptionsMenu {
	int option_height;
protected:
	vector<SDL_Rect> get_locations(const vector<Msg>& msgs);
public:
	VerticalOptionsMenu(
		const SDL_Rect& rect,
		vector<string>&& texts,
		double follow_speed,
		SDL_Color pressed, SDL_Color released, SDL_Color inactive,
		FontType font
	);
};

class HorizontalOptionsMenu : public OptionsMenu {
	int x_margin;
protected:
	vector<SDL_Rect> get_locations(const vector<Msg>& msgs);
public:
	HorizontalOptionsMenu(
		const SDL_Rect& rect,
		vector<string>&& texts, int x_margin,
		double follow_speed,
		SDL_Color pressed, SDL_Color released, SDL_Color inactive,
		FontType font
	);
};

#endif
