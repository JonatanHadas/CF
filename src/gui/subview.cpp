#include "subview.h"

SubView::SubView(const SDL_Rect& rect, bool translucent) :
	rect(rect),
	translucent(translucent),
	texture(nullptr) {}

bool SubView::handle_event(SDL_Event event){
	switch(event.type){
	case SDL_MOUSEMOTION:
		event.motion.x -= rect.x;
		event.motion.y -= rect.y;
		mouse_x = event.motion.x;
		mouse_y = event.motion.y;
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		event.button.x -= rect.x;
		event.button.y -= rect.y;
		mouse_x = event.button.x;
		mouse_y = event.button.y;
		break;
	}
	
	return on_event(event);
}

void SubView::draw(SDL_Renderer* renderer){
	if(texture.get() == nullptr){
		texture = make_unique<Texture>(renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			rect.w, rect.h
		);
		
		if(translucent) SDL_SetTextureBlendMode(texture->get(), SDL_BLENDMODE_BLEND);
	}
	
	texture->do_with_texture(renderer, [&](){
		draw_content(renderer);
	});
	
	SDL_RenderCopy(renderer, texture->get(), NULL, &rect);
}

bool SubView::check_inside(int x, int y){
	return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
}

const SDL_Rect& SubView::get_rect() const{
	return rect;
}

void SubView::move(int x, int y){
	rect.x = x;
	rect.y = y;
}

void SubView::fill_back(SDL_Renderer* renderer, const SDL_Color& color) const {
	SDL_Rect rect = get_rect();
	rect.x = rect.y = 0;
	
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

void SubView::draw_frame(SDL_Renderer* renderer, const SDL_Color& color) const {
	SDL_Rect rect = get_rect();
	rect.x = rect.y = 0;
	
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(renderer, &rect);
}
