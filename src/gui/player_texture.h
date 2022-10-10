#ifndef _PLAYER_TEXTURE_H
#define _PLAYER_TEXTURE_H

#include "images.h"

#include <SDL.h>

#include <vector>

using namespace std;

class PlayerTexture {
	SDL_Color color;
	bool using_texture;
	TextureId image;
	int length;
	bool lateral;
	
public:
	PlayerTexture(const SDL_Color& color);
	PlayerTexture(TextureId image, int length, bool lateral=false);

	const SDL_Color& get_color() const;
	SDL_Texture* get_texture() const;
	int get_length() const;
	bool is_lateral() const;
};

extern vector<PlayerTexture> player_textures;

#endif
