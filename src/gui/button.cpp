#include "button.h"

Button::Button(const SDL_Rect& rect, bool translucent) :
	SubView(rect, translucent),
	pressed(false), active(true) {}

void Button::draw_content(SDL_Renderer* renderer){
	if(!active) draw_inactive(renderer);
	else if(pressed) draw_pressed(renderer);
	else draw_released(renderer);
}

bool Button::on_event(const SDL_Event& event){
	if(!active) return false;
	
	switch(event.type){
	case SDL_MOUSEBUTTONDOWN:
		handle_button_down(event.button);
		return true;
	case SDL_MOUSEBUTTONUP:
		handle_button_up(event.button);
		return true;
	}
	return false;
}

void Button::handle_button_down(const SDL_MouseButtonEvent& event){
	pressed = true;
}

void Button::handle_button_up(const SDL_MouseButtonEvent& event){
	if(event.x >= 0 && event.y >= 0 && event.x < get_rect().w && event.y < get_rect().h){
		if(pressed) on_pressed();
	}
	
	pressed = false;
}

void Button::lose_focus(){
	pressed = false;
}

void Button::step() {}

void Button::set_active(bool active){
	this->active = active;
}
