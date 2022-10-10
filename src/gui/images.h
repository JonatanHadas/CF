#ifndef _IMAGES_H
#define _IMAGES_H

#include "SDL.h"

enum class Img{
	INVERT,
	SPEED,
	TURTLE,
	ERASER,
	THICKEN,
	NARROW,
	CORNER,
	HOVER,
	SPAWN,
	NARROW_TURN,
	WIDE_TURN,
	WARP,
	ARROWS,
	MUTED,
	UNMUTED,
};

enum class TextureId{
	RAINBOW,
	RED_STRIPES,
	TRAFFIC_LIGHT,
};

bool load_images(SDL_Renderer* rend);

SDL_Texture* get_img(Img i);

SDL_Texture* get_texture_by_id(TextureId i);

#endif