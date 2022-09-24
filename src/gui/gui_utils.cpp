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

unique_ptr<Texture> make_cross(SDL_Renderer* renderer, int size, double ratio){
	auto texture = make_unique<Texture>(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		size, size
	);
	
	SDL_SetTextureBlendMode(texture->get(), SDL_BLENDMODE_BLEND);

	texture->do_with_texture(renderer, [&](){
		SDL_SetRenderDrawColor(renderer, 0, 0, 0 ,0);

		SDL_Rect rect;
		rect.w = rect.h = size;
		rect.x = rect.y = 0;

		SDL_RenderFillRect(renderer, &rect);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		
		rect.w = size * ratio; rect.x = (size - rect.w) / 2;
		rect.h = size; rect.y = 0;
		SDL_RenderFillRect(renderer, &rect);

		rect.h = size * ratio; rect.y = (size - rect.h) / 2;
		rect.w = size; rect.x = 0;
		SDL_RenderFillRect(renderer, &rect);
	});

	return texture;
}
