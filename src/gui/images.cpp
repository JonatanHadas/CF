#include "images.h"

#include "SDL_image.h"

#include <map>

using namespace std;

#define DIR "data/images/"
#define POWERUPS "powerups/"
#define PNG ".png"

std::map<Img, SDL_Texture*> textures;

void free_images(){
	for(auto entry: textures){
		SDL_DestroyTexture(entry.second);
	}
}

bool load_texture(SDL_Renderer* renderer, const char* name, Img id){
	SDL_Surface* surface = IMG_Load(name);
	
	if(surface == NULL) return false;
	
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	
	if(texture == NULL) return false;
	textures.insert({id, texture});
	return true;
}

bool load_images(SDL_Renderer* renderer){
	atexit(free_images);
	
	if(load_texture(renderer, DIR POWERUPS "invert" PNG, Img::INVERT))
		return true;
	return false;
}

SDL_Texture* get_img(Img i){
	return textures[i];
}