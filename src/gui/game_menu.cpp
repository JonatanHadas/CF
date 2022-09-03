#include "game_menu.h"

#define PLAYERS_WIDTH_RATIO 4

GameMenu::GameMenu(int w, int h, KeySetManager& key_manager){
	SDL_Rect rect;
	rect.w = w / PLAYERS_WIDTH_RATIO;
	rect.x = w - rect.w;
	rect.h = h;
	rect.y = 0;
	
	players = make_unique<PlayersSubView>(rect, key_manager);
	view_manager.add_view(players.get());
}

bool GameMenu::step(){
	view_manager.step();
	
	return false;
}

bool GameMenu::handle_event(const SDL_Event& event){	
	if(view_manager.handle_event(event)) return false;
	
	return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE;
}

void GameMenu::draw(SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer, 32, 32, 32, 0);
	SDL_RenderClear(renderer);
	
	view_manager.draw(renderer);
}
