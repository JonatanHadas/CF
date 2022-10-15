#include "colors.h"

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

SDL_Color bg_color = {32, 32, 32, 255};
SDL_Color line_color = {0, 0, 0, 255};
SDL_Color line_color2 = {255, 255, 255, 255};
SDL_Color active_color = {64, 64, 64, 255};

SDL_Color clear_color = {0, 0, 0, 0};

SDL_Color text_color = {96, 96, 96, 255};
SDL_Color text_completion_color = {96, 96, 96, 128};
