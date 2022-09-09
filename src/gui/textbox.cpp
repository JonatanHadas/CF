#include "textbox.h"
#include "../utils/utf8.h"

TextBox::TextBox(
	const SDL_Rect& rect, bool translucent,
	FontType font,
	int margin,
	const SDL_Color& color, const SDL_Color& back_color
) : SubView(rect, translucent),
	font(font),
	margin(margin),
	color(color), back_color(color),
	updated(true), typing(false), active(true) {}

bool TextBox::on_event(const SDL_Event& event){
	if(!active) return false;
	switch(event.type){
	case SDL_MOUSEBUTTONDOWN:
		if(!typing){
			typing=true;
			SDL_StartTextInput();
			return true;
		}
		break;
	case SDL_KEYDOWN:
		if(typing){
			switch(event.key.keysym.sym){
			case SDLK_BACKSPACE:
			case SDLK_KP_BACKSPACE:
				if(text.size()){
					utf8_pop_back(text);
					updated = true;
					return true;
				}
				break;
			case SDLK_RETURN:
			case SDLK_RETURN2:
			case SDLK_KP_ENTER:
				SDL_StopTextInput();
				last_text = text;
				updated = true;
				typing = false;
				on_set(text);
				return true;
			case SDLK_ESCAPE:
				SDL_StopTextInput();
				text = last_text;
				updated = true;
				typing = false;
				return true;
			}
		}
		break;
	case SDL_TEXTINPUT:
		if(typing){
			text += event.text.text;
			updated = true;
			return true;
		}
		break;
	}
	return false;
}

void TextBox::draw_content(SDL_Renderer* renderer){
	draw_back(renderer, typing);
	
	if(updated){
		msg = make_unique<Msg>(
			(text.size() ? text : get_default_text()).c_str(), 
			text.size() ? color : back_color,
			font,
			renderer
		);
		
		updated = false;
	}
	
	int y = get_rect().h / 2;
	msg->render_centered(margin, y, Align::LEFT);
}

void TextBox::draw_back(SDL_Renderer* renderer, bool typing){}

void TextBox::step() {}

void TextBox::lose_focus(){
	if(typing){
		SDL_StopTextInput();
		text = last_text;
		updated = true;
		typing = false;
	}
}

void TextBox::set_color(const SDL_Color& color, const SDL_Color& back_color){
	this->color = color;
	this->back_color = back_color;
	
	updated = true;
}

void TextBox::set_text(const string& text){
	last_text = text;
	if(!typing){
		if(text != this->text) updated = true;
		this->text = text;
	}
}

void TextBox::set_active(bool is_active){
	active = is_active;
	if(!active){
		lose_focus();
	}
}

int TextBox::get_text_width(){
	if(msg.get() == nullptr) return 0;
	return msg->get_width();
}

void TextBox::update(){
	updated = true;
}