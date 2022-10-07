#include "gui/texts.h"
#include "gui/images.h"
#include "gui/sounds.h"

#include "gui/game_menu.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

using namespace std;

SDL_Window*  screen;
SDL_Renderer* renderer;

void close_rend(){
	SDL_DestroyRenderer(renderer);
}
void close_window(){
	SDL_DestroyWindow(screen);
}

#define TICK_LEN (1000/60)

#define PLAYER_NUM 2

int main(int argc, char** argv){
	srand(time(NULL));
	
	if(enet_initialize() != 0){
		cerr << "Error while initializing enet" << endl;
		return 4;
	}
	atexit(enet_deinitialize);

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		cerr << "Error initializing SDL:" << endl << SDL_GetError() << endl;
		return 1;
	}
	atexit(SDL_Quit);

	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
		cerr << "Error initializing SDL_image:" << endl << IMG_GetError() << endl;
		return 1;
	}
	atexit(IMG_Quit);

	if(TTF_Init() < 0){
		cerr << "Error initializing SDL_ttf:" << endl << TTF_GetError() << endl;
	}
	atexit(TTF_Quit);
	
	if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, CHANNELS, 2048 ) < 0){
		cerr << "Error initializing SDL_mixer:" << endl << Mix_GetError() << endl;
	}
	atexit(Mix_Quit);


	if(!load_fonts()){
		cerr << "Error while loading fonts" << endl << SDL_GetError() << TTF_GetError() << endl;
		return 1;
	}
	
	if(!load_sounds()){
		cerr << "Error while loading sounds" << endl << Mix_GetError() << endl;
		return 1;
	}

	screen = SDL_CreateWindow(
										"Curve Fever",
										SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
										//960, 640, 0);
										0, 0,
										SDL_WINDOW_FULLSCREEN_DESKTOP);

	if(screen == NULL){
		cerr << "Error while opening window:" << endl << SDL_GetError() << endl;
		return 2;
	}
	atexit(close_window);

	renderer = SDL_CreateRenderer(screen, -1,SDL_RENDERER_TARGETTEXTURE);
	
	if(renderer == NULL){
		cerr << "Error while opening renderer:" << endl << SDL_GetError() << endl;
		return 3;
	}
	atexit(close_rend);
	
	if(!load_images(renderer)){
		cerr << "Error while loading images" << endl << SDL_GetError() << IMG_GetError() << endl;
		return 0;
	}
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

	KeySetManager key_manager("game_data/keys");
	
	int screen_w, screen_h;
	SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);
	SDL_RenderSetLogicalSize(renderer, screen_w, screen_h);

	GameMenu gui(screen_w, screen_h, key_manager);
	
	mainloop(gui, renderer);
	
	return 0;
}