#ifndef _GUI_UTILS_H
#define _GUI_UTILS_H

#include <SDL.h>
#include <functional>
#include <memory>

using namespace std;

class Texture{
	SDL_Texture* texture;
public:
	Texture(SDL_Renderer* renderer,
		unsigned int format, int access,
		int width, int height
		);
	
	Texture(const Texture& inst) = delete;
	Texture(Texture&& inst);
	
	Texture& operator=(const Texture& inst) = delete;
	Texture& operator=(Texture&& inst);
	
	~Texture();
	
	SDL_Texture* get();
	
	void do_with_texture(SDL_Renderer* renderer, function<void()> todo);
};

void draw_circle(SDL_Renderer* renderer, int x, int y, double radius);

unique_ptr<Texture> make_cross(SDL_Renderer* renderer, int size, double ratio);

#endif
