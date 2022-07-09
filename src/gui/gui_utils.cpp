#include "gui_utils.h"
#include <math.h>

Texture::Texture(SDL_Renderer* renderer,
	unsigned int format, int access,
	int width, int height
	) : 
	texture(SDL_CreateTexture(renderer, format, access, width, height)){}

Texture::Texture(Texture&& inst) :
	texture(inst.texture) {
		
	inst.texture = NULL;
}

Texture& Texture::operator=(Texture&& inst){
	texture = inst.texture;
	inst.texture = NULL;
	
	return *this;
}

Texture::~Texture(){
	if(NULL != texture) SDL_DestroyTexture(texture);
}

SDL_Texture* Texture::get(){
	return texture;
}

void Texture::do_with_texture(SDL_Renderer* renderer, function<void()> todo){
	SDL_Texture* old_target = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, texture);
	
	todo();
	
	SDL_SetRenderTarget(renderer, old_target);
}


void draw_circle(SDL_Renderer* renderer, int x, int y, double radius){
	for(int dx = 1; dx < radius; dx++){
		double dy = sqrt(radius*radius - dx*dx);
		
		SDL_Rect rect;
		
		rect.x = x - dx;
		rect.y = y - dy;
		rect.w = dx * 2;
		rect.h = dy * 2;		
		SDL_RenderFillRect(renderer, &rect);

		rect.x = x - dy;
		rect.y = y - dx;
		rect.w = dy * 2;
		rect.h = dx * 2;		
		SDL_RenderFillRect(renderer, &rect);
	}
}
