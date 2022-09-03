#ifndef _BUTTON_H
#define _BUTTON_H

#include "subview.h"

class Button : public SubView {
	bool pressed;
	bool active;
	
	void handle_button_down(const SDL_MouseButtonEvent& event);
	void handle_button_up(const SDL_MouseButtonEvent& event);
protected:
	virtual void draw_inactive(SDL_Renderer* renderer) = 0;
	virtual void draw_pressed(SDL_Renderer* renderer) = 0;
	virtual void draw_released(SDL_Renderer* renderer) = 0;
	
	virtual void on_pressed() = 0;
	
	void draw_content(SDL_Renderer* renderer);
	bool on_event(const SDL_Event& event);
public:
	Button(const SDL_Rect& rect, bool translucent);
	
	void lose_focus();
	void step();
	
	void set_active(bool active);
};

#endif
