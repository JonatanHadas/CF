#include "number_button.h"

#include "colors.h"

#include <string>

NumberButtonChange::NumberButtonChange(
	const SDL_Rect& rect,
	const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive,
	bool direction,
	NumberButton& button
) : Button(rect, false),
	pressed(pressed), released(released), inactive(inactive),
	direction(direction),
	button(button) {}

void NumberButtonChange::draw_arrow(SDL_Renderer* renderer, const SDL_Color& color){
	SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	SDL_RenderClear(renderer);
	
	SDL_Vertex vertices[3] = {
		{
			.position = {
				.x = (float)(0.5 * get_rect().w),
				.y = (float)(direction ? 0 : get_rect().h),
			},
			.color = color,
		},
		{
			.position = {
				.x = (float)(get_rect().w),
				.y = (float)(direction ? get_rect().h : 0),
			},
			.color = color,
		},
		{
			.position = {
				.x = 0.0f,
				.y = (float)(direction ? get_rect().h : 0),
			},
			.color = color,
		},
	};
	
	SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
}

void NumberButtonChange::draw_pressed(SDL_Renderer* renderer){
	draw_arrow(renderer, pressed);
}

void NumberButtonChange::draw_released(SDL_Renderer* renderer){
	draw_arrow(renderer, released);
}

void NumberButtonChange::draw_inactive(SDL_Renderer* renderer){
	draw_arrow(renderer, inactive);
}

void NumberButtonChange::on_pressed(){
	button.set_number(button.get_value() + (direction ? 1 : -1));
}

NumberButton::NumberButton(
	const SDL_Rect& rect,
	int button_width, int button_height,
	FontType font,
	const SDL_Color& text_color_active, const SDL_Color& text_color_inactive,
	const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive,
	int lower_limit, int upper_limit,
	bool use_lower_limit, bool use_upper_limit
) : SubView(rect, true),
	lower_limit(lower_limit), upper_limit(upper_limit),
	use_lower_limit(use_lower_limit), use_upper_limit(use_upper_limit),
	font(font),
	text_color_active(text_color_active), text_color_inactive(text_color_inactive),
	active(true)  {
		
	SDL_Rect button_rect;
	button_rect.w = button_width;
	button_rect.h = button_height;
	button_rect.x = (rect.w - button_rect.w) / 2;
	
	button_rect.y = 0;
	inc = make_unique<NumberButtonChange>(
		button_rect,
		pressed, released, inactive,
		true, *this
	);
	
	button_rect.y = rect.h - button_rect.h;
	dec = make_unique<NumberButtonChange>(
		button_rect,
		pressed, released, inactive,
		false, *this
	);
	
	view_manager.add_view(inc.get());
	view_manager.add_view(dec.get());
}

NumberButton::NumberButton(const SDL_Rect& rect,
	int button_width, int button_height,
	FontType font,
	const SDL_Color& text_color_active, const SDL_Color& text_color_inactive,
	const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive
) : NumberButton(
		rect,
		button_width, button_height,
		font,
		text_color_active, text_color_inactive,
		pressed, released, inactive,
		0, 0, false, false
	) {}

NumberButton::NumberButton(const SDL_Rect& rect,
	int button_width, int button_height,
	FontType font,
	const SDL_Color& text_color_active, const SDL_Color& text_color_inactive,
	const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive,
	int lower_limit
) : NumberButton(
		rect,
		button_width, button_height,
		font,
		text_color_active, text_color_inactive,
		pressed, released, inactive,
		lower_limit, 0, true, false
	) {}

NumberButton::NumberButton(const SDL_Rect& rect,
	int button_width, int button_height,
	FontType font,
	const SDL_Color& text_color_active, const SDL_Color& text_color_inactive,
	const SDL_Color& pressed, const SDL_Color& released, const SDL_Color& inactive,
	int lower_limit, int upper_limit
) : NumberButton(
		rect,
		button_width, button_height,
		font,
		text_color_active, text_color_inactive,
		pressed, released, inactive,
		lower_limit, upper_limit, true, true
	) {}

void NumberButton::sync_display(){
	inc->set_active(active && ((get_value() < upper_limit) || !use_upper_limit));
	dec->set_active(active && ((get_value() > lower_limit) || !use_lower_limit));
}

void NumberButton::draw_content(SDL_Renderer* renderer){
	sync_display();

	SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	SDL_RenderClear(renderer);
	
	if(msg.get() == nullptr || get_value() != number) msg = make_unique<Msg>(
		to_string(get_value()).c_str(),
		active ? text_color_active : text_color_inactive,
		font,
		renderer
	);
	number = get_value();
	
	view_manager.draw(renderer);

	msg->render_centered(get_rect().w / 2, get_rect().h / 2, Align::CENTER);	
}

bool NumberButton::on_event(const SDL_Event& event){
	sync_display();
	
	if(view_manager.handle_event(event)) return true;
	
	if(event.type != SDL_MOUSEWHEEL) return false;
	
	set_number(get_value() + event.wheel.y);
	return true;
}

void NumberButton::lose_focus(){
	sync_display();
	
	view_manager.lose_focus();
}

void NumberButton::step(){
	sync_display();
	
	view_manager.step();
}

void NumberButton::set_number(int num){
	if(use_lower_limit && num < lower_limit) return;
	if(use_upper_limit && num > upper_limit) return;
	on_set(num);
}

void NumberButton::set_active(bool is_active){
	if(active != is_active) msg = nullptr;
	active = is_active;

	sync_display();
}
