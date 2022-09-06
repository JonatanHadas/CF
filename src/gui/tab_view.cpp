#include "tab_view.h"

TabButton::TabButton(
	const SDL_Rect& rect,
	Msg&& msg,
	int tab,
	TabView& view
) : Button(rect, false),
	msg(std::move(msg)),
	tab(tab),
	view(view) {}

void TabButton::draw_button(SDL_Renderer* renderer){
	msg.render_centered(get_rect().w / 2 , get_rect().h / 2, Align::CENTER);
}

void TabButton::draw_pressed(SDL_Renderer* renderer){
	view.draw_button_back(renderer, *this, TabView::State::PRESSED);
	draw_button(renderer);
}

void TabButton::draw_released(SDL_Renderer* renderer){
	view.draw_button_back(renderer, *this, TabView::State::RELEASED);
	draw_button(renderer);
}

void TabButton::draw_inactive(SDL_Renderer* renderer){
	view.draw_button_back(renderer, *this, TabView::State::CURRENT);
	draw_button(renderer);
}

void TabButton::on_pressed(){
	view.set_current(tab);
}

TabMenu::TabMenu(
	const SDL_Rect& rect,
	vector<string>&& names, FontType font, SDL_Color text_color,
	int x_margin,
	TabView& view
) : SubView(rect, false),
	names(std::move(names)),
	font(font), text_color(text_color),
	x_margin(x_margin),
	view(view) {}
	

void TabMenu::init(SDL_Renderer* renderer){
	if(buttons.size()) return;

	SDL_Rect rect = get_rect();
	rect.x = rect.y = 0;
	for(int i = 0; i < names.size(); i++){
		Msg msg(names[i].c_str(), text_color, font, renderer);
		rect.w = 2 * x_margin + msg.get_width();
		
		buttons.push_back(make_unique<TabButton>(
			rect,
			std::move(msg),
			i, view
		));
		
		view_manager.add_view(buttons.back().get());
		
		rect.x += rect.w;
	}
	
	set_tab(0);
}

bool TabMenu::on_event(const SDL_Event& event){
	return view_manager.handle_event(event);
}

void TabMenu::draw_content(SDL_Renderer* renderer){
	view.draw_button_back(renderer, *this, TabView::State::BACK);
	
	init(renderer);
	view_manager.draw(renderer);
}

void TabMenu::step(){
	view_manager.step();
}

void TabMenu::lose_focus(){
	view_manager.lose_focus();
}

void TabMenu::set_tab(int tab){
	for(int i = 0; i < buttons.size(); i++){
		buttons[i]->set_active(tab != i);
	}
}

TabView::TabView(
	const SDL_Rect& rect, int menu_height, bool translucent,
	FontType font, SDL_Color text_color,
	int x_margin
) : SubView(rect, translucent),
	menu_height(menu_height), x_margin(x_margin),
	font(font), text_color(text_color),
	current(0) {}

void TabView::init(){
	if(subviews.size()) return;
	
	SDL_Rect rect;
	rect.x = 0;
	rect.y = menu_height;
	rect.w = get_rect().w;
	rect.h = get_rect().h - menu_height;

	auto descs = init_subviews(rect);
	
	vector<string> names;
	for(const auto& desc: descs){
		names.push_back(desc.name);
		subviews.push_back(desc.view);
	}
	
	rect.y = 0;
	rect.h = menu_height;
	
	menu = make_unique<TabMenu>(
		rect,
		std::move(names), font, text_color,
		x_margin,
		*this
	);
}
	
void TabView::set_current(int tab){
	init();
	if(tab != current){
		subviews[current]->lose_focus();
	}
	
	current = tab;
	menu->set_tab(tab);
}

bool TabView::on_event(const SDL_Event& event){
	init();
	bool menu_handled = menu->handle_event(event);
	bool view_handled = subviews[current]->handle_event(event);
	return menu_handled || view_handled;
}

void TabView::draw_content(SDL_Renderer* renderer){
	init();
	menu->draw(renderer);
	subviews[current]->draw(renderer);
}

void TabView::step(){
	init();
	menu->step();
	subviews[current]->step();
}

void TabView::lose_focus(){
	init();
	menu->lose_focus();
	subviews[current]->lose_focus();
}
