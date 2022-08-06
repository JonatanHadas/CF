#ifndef _COLORS_H
#define _COLORS_H

#include <SDL.h>

#include <vector>

using namespace std;


vector<SDL_Color> player_colors({
	{0, 192, 0, 255},
	{192, 0, 0, 255},
	{0, 0, 192, 255},
	{240, 220, 0, 255},
	{255, 255, 255, 255},
	{96, 64, 0, 255},	
});

vector<SDL_Color> powerup_colors({
	{64, 128, 255, 255},
	{64, 255, 32, 255},
	{192, 64, 32, 255},
});

vector<SDL_Color> powerup_ring_colors({
	{199, 207, 255, 128},
	{207, 255, 199, 128},
	{255, 207, 199, 128},
});

#endif
