#ifndef _TAB_VIEW_H
#define _TAB_VIEW_H

#include "subview.h"
#include "subview_manager.h"
#include "button.h"

#include "texts.h"

#include <vector>
#include <utility>
#include <string>
#include <memory>

using namespace std;

class TabView;

class TabButton : public Button {
	TabView& view;
	int tab;
	
	Msg msg;
	
	void draw_button(SDL_Renderer* renderer);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);
	
	void on_pressed();
public:
	TabButton(
		const SDL_Rect& rect,
		Msg&& msg,
		int tab,
		TabView& view
	);
};

class TabMenu : public SubView {
	TabView& view;
	
	vector<string> names;
	FontType font;
	SDL_Color text_color;
	
	int x_margin;
	
	vector<unique_ptr<TabButton>> buttons;
	SubViewManager view_manager;

	void init(SDL_Renderer* renderer);
protected:
	bool on_event(const SDL_Event& event);
	void draw_content(SDL_Renderer* renderer);	
public:
	TabMenu(
		const SDL_Rect& rect,
		vector<string>&& names, FontType font, SDL_Color text_color,
		int x_margin,
		TabView& view
	);

	void step();
	void lose_focus();
	
	void set_tab(int tab);
};

class TabView : public SubView {
	int menu_height;
	int x_margin;
	FontType font;
	SDL_Color text_color;
	
	unique_ptr<TabMenu> menu;
	vector<SubView*> subviews;
	
	int current;
	
	friend TabButton;
	friend	TabMenu;
	void set_current(int tab);
	
	void init();
protected:
	enum class State{
		CURRENT,
		PRESSED,
		RELEASED,
		BACK,
	};
	
	struct ViewDescriptor{
		SubView* view;
		string name;
	};

	virtual vector<ViewDescriptor> init_subviews(const SDL_Rect& rect) = 0;
	virtual void draw_button_back(SDL_Renderer* renderer, const SubView& view, TabView::State state) = 0;
	virtual void on_changed();
	
	bool on_event(const SDL_Event& event);
	void draw_content(SDL_Renderer* renderer);	
public:
	TabView(
		const SDL_Rect& rect, int menu_height, bool translucent,
		FontType font, SDL_Color text_color,
		int x_margin
	);

	void step();
	void lose_focus();	
};

#endif
