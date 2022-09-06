#ifndef _NUMBER_BUTTON_H
#define _NUMBER_BUTTON_H

#include "subview.h"
#include "button.h"
#include "subview_manager.h"

#include "texts.h"

class NumberButton;

class NumberButtonChange : public Button {
	bool direction;
	
	SDL_Color pressed, released, inactive;
	
	NumberButton& button;
	
	void draw_arrow(SDL_Renderer* renderer, const SDL_Color& color);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);
	
	void on_pressed();
public:
	NumberButtonChange(
		const SDL_Rect& rect,
		const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive,
		bool direction,
		NumberButton& button
	);
};

class NumberButton : public SubView {
	int number;
	
	int lower_limit, upper_limit;
	bool use_lower_limit, use_upper_limit;
	
	bool active;
	
	unique_ptr<Msg> msg;
	
	FontType font;
	SDL_Color text_color_active, text_color_inactive;
	
	SubViewManager view_manager;
	
	unique_ptr<NumberButtonChange> inc, dec;

	NumberButton(
		const SDL_Rect& rect,
		int button_width, int button_height,
		FontType font,
		const SDL_Color& text_color_active, const SDL_Color& text_color_inactive,
		const SDL_Color& active, const SDL_Color& released, const SDL_Color& inactive,
		int lower_limit, int upper_limit,
		bool use_lower_limit, bool use_upper_limit
	);
	
	void sync_display();
protected:
	virtual void on_set(int) = 0;
	
	void draw_content(SDL_Renderer* renderer);
	bool on_event(const SDL_Event& event);

public:
	NumberButton(const SDL_Rect& rect,
		int button_width, int button_height,
		FontType font,
		const SDL_Color& text_color_active, const SDL_Color& text_color_inactive,
		const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive
	);

	NumberButton(const SDL_Rect& rect,
		int button_width, int button_height,
		FontType font,
		const SDL_Color& text_color_active, const SDL_Color& text_color_inactive,
		const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive,
		int lower_limit
	);

	NumberButton(const SDL_Rect& rect,
		int button_width, int button_height,
		FontType font,
		const SDL_Color& text_color_active, const SDL_Color& text_color_inactive,
		const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive,
		int lower_limit, int upper_limit
	);
	
	void lose_focus();
	void step();
	
	virtual int get_value() = 0;
	
	void set_number(int num);
	
	void set_active(bool is_active);
};

#endif
