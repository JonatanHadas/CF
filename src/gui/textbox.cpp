#include "textbox.h"
#include "../utils/utf8.h"

TextBox::TextBox(
	const SDL_Rect& rect, bool translucent,
	FontType font,
	int margin,
	const SDL_Color& color, const SDL_Color& back_color,
	TextCompleter* completer
) : SubView(rect, translucent),
	font(font),
	margin(margin),
	color(color), back_color(back_color),
	updated(true), typing(false), active(true),
	completer(completer) {}
	
TextBox::TextBox(
	const SDL_Rect& rect, bool translucent,
	FontType font,
	int margin,
	const SDL_Color& color, const SDL_Color& back_color
) : TextBox(rect, translucent, font, margin, color, back_color, nullptr) {}

TextBox::TextBox(
	const SDL_Rect& rect, bool translucent,
	FontType font,
	int margin,
	const SDL_Color& color, const SDL_Color& back_color,
	TextCompleter& completer
) : TextBox(rect, translucent, font, margin, color, back_color, &completer) {}
	
void TextBox::set(){
	if(typing){
		SDL_StopTextInput();

		if(current_suggestion < suggestions.size()) text = suggestions[current_suggestion];		
		last_text = text;
		updated = true;
		typing = false;
		
		on_set(text);
	}
}

bool TextBox::on_event(const SDL_Event& event){
	if(!active) return false;
	switch(event.type){
	case SDL_MOUSEBUTTONDOWN:
		if(!typing){
			on_active();
			typing=true;
			SDL_StartTextInput();
			update();
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
					update();
					return true;
				}
				break;
			case SDLK_TAB:
				if(current_suggestion >= suggestions.size()) break;
				text = suggestions[current_suggestion];
				update();
				return true;
			case SDLK_RETURN:
			case SDLK_RETURN2:
			case SDLK_KP_ENTER:
				set();
				return true;
			case SDLK_UP:
				if(current_suggestion > 0){
					current_suggestion--;
					updated = true;
				}
				return true;
			case SDLK_DOWN:
				if(current_suggestion < suggestions.size()){
					current_suggestion++;
					updated = true;
				}
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
			update();
			return true;
		}
		break;
	}
	return false;
}

void TextBox::draw_content(SDL_Renderer* renderer){
	draw_back(renderer, typing);
	
	if(updated){
		if(text.size()){
			msg = make_unique<Msg>(
				text.c_str(), 
				color,
				font,
				renderer
			);
		}
		else if(!typing || current_suggestion >= suggestions.size()){
			msg = make_unique<Msg>(
				get_default_text().c_str(), 
				typing ? back_color : color,
				font,
				renderer
			);
		}
		else{
			msg = nullptr;
		}
		
		updated = false;
		
		if(typing && current_suggestion < suggestions.size()){
			completion_msg = make_unique<Msg>(
				suggestions[current_suggestion].substr(text.size()).c_str(),
				back_color,
				font,
				renderer
			);
		}
		else{
			completion_msg = nullptr;
		}
	}
	
	int y = get_rect().h / 2;
	int x = margin;
	if(msg.get() != nullptr){
		msg->render_centered(x, y, Align::LEFT);
		x += msg->get_width();
	}
	if(completion_msg.get() != nullptr){
		completion_msg->render_centered(x, y, Align::LEFT);
	}
}

void TextBox::on_active() {}

void TextBox::draw_back(SDL_Renderer* renderer, bool typing){}

void TextBox::step() {}

void TextBox::lose_focus(){
	set();
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
	int width = 0;
	if(msg.get() != nullptr) width += msg->get_width();
	if(completion_msg != nullptr) width += completion_msg->get_width();
	return width;
}

void TextBox::update(){
	updated = true;

	if(completer != nullptr) suggestions = completer->get_suggestions(text);
	current_suggestion = 0;	
}

void TextBox::add_last_as_suggestion(){
	if(completer != nullptr && last_text.size()){
		completer->add_suggestion(last_text);
	
		update();
	}
}
