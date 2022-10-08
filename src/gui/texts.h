#ifndef _TEXTS_H
#define _TEXTS_H

#include "SDL.h"

enum class FontType{
	NRM,BIG,MID,SML
};
enum class Align{
	LEFT,RIGHT,CENTER
};

bool load_fonts();

class Msg{
	SDL_Texture* img;
	SDL_Renderer* rend;
	int w,h;
	
	void destroy();
public:
	Msg(const char* text, SDL_Color color, FontType type, SDL_Renderer* renderer);
	Msg(const char* text, SDL_Color color, FontType type, SDL_Renderer* renderer, SDL_Texture* texture);
	
	Msg(const Msg&) = delete;
	Msg(Msg&& msg);
	~Msg();
	Msg& operator=(const Msg&) = delete;
	Msg& operator=(Msg&& msg);
	
	void render_centered(int x, int y, Align a);
	bool exists();
	
	int get_width() const;
	int get_height() const;
};

#endif