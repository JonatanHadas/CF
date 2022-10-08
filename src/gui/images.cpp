#include "images.h"

#include "SDL_image.h"

#include <map>

using namespace std;

#define DIR "data/images/"
#define POWERUPS "powerups/"
#define TEXTURES "textures/"
#define PNG ".png"

std::map<Img, SDL_Texture*> images;
std::map<TextureId, SDL_Texture*> textures;

template<typename ID>
void free_textures(map<ID, SDL_Texture*>& storage){
	for(auto entry: storage){
		SDL_DestroyTexture(entry.second);
	}
	storage.clear();
}

void free_images(){
	free_textures(images);
	free_textures(textures);
}

template<typename ID>
bool load_texture(SDL_Renderer* renderer, const char* name, ID id, map<ID, SDL_Texture*>& storage){
	SDL_Surface* surface = IMG_Load(name);
	
	if(surface == NULL) return false;
	
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	
	if(texture == NULL) return false;
	storage.insert({id, texture});
	return true;
}

bool load_texture(SDL_Renderer* renderer, const char* name, Img id){
	return load_texture(renderer, name, id, images);
}

bool load_texture(SDL_Renderer* renderer, const char* name, TextureId id){
	return load_texture(renderer, name, id, textures);
}

bool load_images(SDL_Renderer* renderer){
	atexit(free_images);
	
	if(load_texture(renderer, DIR POWERUPS "invert" PNG, Img::INVERT))
	if(load_texture(renderer, DIR POWERUPS "speed" PNG, Img::SPEED))
	if(load_texture(renderer, DIR POWERUPS "turtle" PNG, Img::TURTLE))
	if(load_texture(renderer, DIR POWERUPS "eraser" PNG, Img::ERASER))
	if(load_texture(renderer, DIR POWERUPS "thicken" PNG, Img::THICKEN))
	if(load_texture(renderer, DIR POWERUPS "narrow" PNG, Img::NARROW))
	if(load_texture(renderer, DIR POWERUPS "corners" PNG, Img::CORNER))
	if(load_texture(renderer, DIR POWERUPS "hover" PNG, Img::HOVER))
	if(load_texture(renderer, DIR POWERUPS "spawn" PNG, Img::SPAWN))
	if(load_texture(renderer, DIR POWERUPS "narrow_turn" PNG, Img::NARROW_TURN))
	if(load_texture(renderer, DIR POWERUPS "wide_turn" PNG, Img::WIDE_TURN))
	if(load_texture(renderer, DIR POWERUPS "warp" PNG, Img::WARP))
	if(load_texture(renderer, DIR "arrows" PNG, Img::ARROWS))
	if(load_texture(renderer, DIR TEXTURES "rainbow" PNG, TextureId::RAINBOW))
		return true;
	return false;
}

SDL_Texture* get_img(Img i){
	return images[i];
}

SDL_Texture* get_texture_by_id(TextureId i){
	return textures[i];
}
