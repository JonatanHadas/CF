#include "options_menu.h"

#include "colors.h"

OptionButton::OptionButton(
	const SDL_Rect& rect,
	int option,
	Msg&& msg,
	const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive,
	OptionsMenu& menu
) : Button(rect, true),
	option(option),
	msg(std::move(msg)), texture(nullptr),
	pressed(pressed), released(released), inactive(inactive),
	menu(menu) {}
	

void OptionButton::draw_text(SDL_Renderer* renderer, SDL_Color color){
	if(texture.get() == nullptr){
		texture = make_unique<Texture>(
			renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			get_rect().w, get_rect().h
		);
		
		texture->do_with_texture(renderer, [&](){
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
			SDL_RenderClear(renderer);
			
			msg.render_centered(get_rect().w / 2, get_rect().h / 2, Align::CENTER);
		});	
	}
	
	fill_back(renderer, clear_color);
	
	SDL_SetTextureColorMod(texture->get(), color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(texture->get(), color.a);
	
	SDL_RenderCopy(renderer, texture->get(), NULL, NULL);
}

void OptionButton::draw_pressed(SDL_Renderer* renderer){
	draw_text(renderer, pressed);
}

void OptionButton::draw_released(SDL_Renderer* renderer){
	draw_text(renderer, released);
}

void OptionButton::draw_inactive(SDL_Renderer* renderer){
	draw_text(renderer, inactive);
}

void OptionButton::on_pressed(){
	menu.set_option(option);
}

OptionsMenu::OptionsMenu(
	const SDL_Rect& rect,
	vector<string>&& texts,
	const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive,
	FontType font
) : SubView(rect, true),
	texts(std::move(texts)),
	pressed(pressed), released(released), inactive(inactive),
	font(font),
	active(true) {}

void OptionsMenu::init(SDL_Renderer* renderer){
	if(buttons.size()) return;
	
	vector<Msg> msgs;

	for(const auto& text: texts){
		msgs.push_back(Msg(text.c_str(), {255, 255, 255, 255}, font, renderer));
	}
	
	auto rects = get_locations(msgs);
	
	for(int i = 0; i < msgs.size(); i++){
		buttons.push_back(make_unique<OptionButton>(
			rects[i], i,
			std::move(msgs[i]), pressed, released, inactive,
			*this
		));
		
		buttons.back()->set_active(active);
		manager.add_view(buttons.back().get());
	}
}

bool OptionsMenu::on_event(const SDL_Event& event){
	return manager.handle_event(event);
}

void OptionsMenu::draw_content(SDL_Renderer* renderer){
	init(renderer);
	
	fill_back(renderer, clear_color);
	
	SDL_Rect rect = buttons[get_option()]->get_rect();
	SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b, line_color.a);
	SDL_RenderDrawRect(renderer, &rect);
	
	manager.draw(renderer);
}


void OptionsMenu::lose_focus(){
	manager.lose_focus();
}

void OptionsMenu::step(){
	manager.step();
}

void OptionsMenu::set_option(int opt){
	on_set(opt);
}

void OptionsMenu::set_active(bool is_active){
	active = is_active;
	for(auto& button: buttons) button->set_active(active);
}

VerticalOptionsMenu::VerticalOptionsMenu(
	const SDL_Rect& rect,
	vector<string>&& texts,
	SDL_Color pressed, SDL_Color released, SDL_Color inactive,
	FontType font
) : OptionsMenu(
		{ .x = rect.x, .y = rect.y, .w = rect.w, .h = rect.h * (int)texts.size() },
		std::move(texts),
		pressed, released, inactive,
		font
	),
	option_height(rect.h) {}
	

vector<SDL_Rect> VerticalOptionsMenu::get_locations(const vector<Msg>& msgs){
	vector<SDL_Rect> locations;
	
	for(int i = 0; i < msgs.size(); i++){
		locations.push_back({
			.x = 0,
			.y = option_height * i,
			.w = get_rect().w,
			.h = option_height,
		});
	}
	
	return locations;
}

HorizontalOptionsMenu::HorizontalOptionsMenu(
	const SDL_Rect& rect,
	vector<string>&& texts, int x_margin,
	SDL_Color pressed, SDL_Color released, SDL_Color inactive,
	FontType font
) : OptionsMenu(
		{ .x = rect.x, .y = rect.y, .w = rect.w, .h = rect.h },
		std::move(texts),
		pressed, released, inactive,
		font
	),
	x_margin(x_margin) {}

vector<SDL_Rect> HorizontalOptionsMenu::get_locations(const vector<Msg>& msgs){
	SDL_Rect rect;
	rect.y = rect.x = 0;
	rect.h = get_rect().h;
	
	vector<SDL_Rect> locations;
	
	for(const auto& msg: msgs){
		rect.w = 2 * x_margin + msg.get_width();
		locations.push_back(rect);
		rect.x += rect.w;
	}
	
	return locations;
}
