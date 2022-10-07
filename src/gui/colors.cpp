#include "colors.h"

vector<SDL_Color> player_colors({
	{0, 255, 0, 255},  // Green
	{255, 0, 0, 255},  // Red
	{0, 0, 255, 255},  // Blue
	{255, 255, 0, 255},  // Yellow
	{0, 255, 255, 255},  // Cyan
	{255, 0, 255, 255},  // Magenta
	{255, 128, 0, 255},  // Orange
	{128, 0, 255, 255},  // Purple
	{0, 128, 0, 255},  // Dark Green
	{0, 0, 128, 255},  // Dark Blue
	{255, 255, 255, 255},  // White
	{128, 128, 128, 255},  // Gray
	{255, 128, 128, 255},  // Pink
	{96, 64, 0, 255},  // Brown
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

SDL_Color bg_color = {32, 32, 32, 255};
SDL_Color line_color = {0, 0, 0, 255};
SDL_Color line_color2 = {255, 255, 255, 255};
SDL_Color active_color = {64, 64, 64, 255};

SDL_Color clear_color = {0, 0, 0, 0};

SDL_Color text_color = {96, 96, 96, 255};
