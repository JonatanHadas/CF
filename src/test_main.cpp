#include "gui/game_gui.h"
#include "gui/texts.h"
#include "gui/images.h"
#include "game/game.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "SDL_ttf.h"
#include "SDL_image.h"

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

	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
		cerr << "Error initializing SDL_image:" << endl << IMG_GetError() << endl;
		return 1;
	}
	atexit(IMG_Quit);

	if(TTF_Init() < 0){
		cerr << "Error initializing SDL_ttf:" << endl << TTF_GetError() << endl;
	}
	atexit(TTF_Quit);

	if(!load_fonts()){
		cerr << "Error while loading fonts" << endl << SDL_GetError() << TTF_GetError() << endl;
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

	set<PowerUpDescriptor> allowed_powerups({
		PowerUpDescriptor(PowerUpType::INVERT, PowerUpAffects::OTHERS),
		PowerUpDescriptor(PowerUpType::SPEED_UP, PowerUpAffects::YOU),
		PowerUpDescriptor(PowerUpType::SPEED_UP, PowerUpAffects::OTHERS),
		PowerUpDescriptor(PowerUpType::SLOW_DOWN, PowerUpAffects::YOU),
		PowerUpDescriptor(PowerUpType::SLOW_DOWN, PowerUpAffects::OTHERS),
		PowerUpDescriptor(PowerUpType::NARROW, PowerUpAffects::YOU),
		PowerUpDescriptor(PowerUpType::THICKEN, PowerUpAffects::OTHERS),
		PowerUpDescriptor(PowerUpType::NARROW_TURN, PowerUpAffects::YOU),
		PowerUpDescriptor(PowerUpType::WIDE_TURN, PowerUpAffects::OTHERS),
//		PowerUpDescriptor(PowerUpType::ERASER, PowerUpAffects::YOU),
		PowerUpDescriptor(PowerUpType::ERASER, PowerUpAffects::ALL),
		PowerUpDescriptor(PowerUpType::RIGHT_TURN, PowerUpAffects::YOU),
		PowerUpDescriptor(PowerUpType::HOVER, PowerUpAffects::YOU),
		PowerUpDescriptor(PowerUpType::SPAWN_POWERUPS, PowerUpAffects::ALL),
//		PowerUpDescriptor(PowerUpType::WARP_AROUND, PowerUpAffects::YOU),
		PowerUpDescriptor(PowerUpType::WARP_AROUND, PowerUpAffects::ALL),
	});
	
	BoardSize board_size(100, 120);
	
	vector<int> teams;
	for(int i = 0; i < PLAYER_NUM; i++){
		teams.push_back(i);
	}
	Game game(board_size, PLAYER_NUM, teams, allowed_powerups, set<GameObserver*>());
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
	
	int last_round = -1;
	int starting_timer = 0;
	
	bool paused = false;
	
	while(true){
		gui.draw(renderer);

		SDL_RenderPresent(renderer);
		
		SDL_Event event;
				
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) return 0;
			if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN) paused = !paused;
			gui.handle_event(event);
		}
		
		if(!paused){
			if(last_round != game.get_round()){
				last_round = game.get_round();
				starting_timer = 30;
			}
			if(starting_timer) starting_timer--;
			else gui.step();
		}
		
		game.advance();
		
		auto time = SDL_GetTicks();
		if(next_tick > time) SDL_Delay(next_tick - time);
		next_tick = SDL_GetTicks() + TICK_LEN;
	}
}