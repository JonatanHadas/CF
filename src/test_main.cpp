#include "gui/game_gui.h"
#include "game/game.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>

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
	
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		cerr << "Error initializing SDL:" << endl << SDL_GetError() << endl;
		return 1;
	}
	atexit(SDL_Quit);

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

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

	
	BoardSize board_size(100, 120);
	
	Game game(board_size, PLAYER_NUM, set<GameObserver*>());
	for(int i = 2; i < PLAYER_NUM; i++) game.get_player_interface(i).set_active(false);
	
	map<PlayerInterface*, KeySet> interfaces;
	interfaces[&game.get_player_interface(0)] = {
		.left = SDL_SCANCODE_LEFT,
		.right = SDL_SCANCODE_RIGHT
	};
	interfaces[&game.get_player_interface(1)] = {
		.left = SDL_SCANCODE_Z,
		.right = SDL_SCANCODE_X
	};
	
	GameGui gui(board_size, &game, interfaces);
	
	auto next_tick = SDL_GetTicks() + TICK_LEN;
	
	while(true){
		gui.draw(renderer);

		SDL_RenderPresent(renderer);
		
		SDL_Event event;
				
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) return 0;
			gui.handle_event(event);
		}
		
		gui.step();
		
		game.advance();
		
		auto time = SDL_GetTicks();
		if(next_tick > time) SDL_Delay(next_tick - time);
		next_tick = SDL_GetTicks() + TICK_LEN;
	}
}