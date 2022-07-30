#ifndef _IMAGES_H
#define _IMAGES_H

#include "SDL.h"

enum class Img{
	INVERT,
};

bool load_images(SDL_Renderer* rend);

SDL_Texture* get_img(Img i);

#endif