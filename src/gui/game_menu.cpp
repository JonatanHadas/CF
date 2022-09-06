#include "game_menu.h"

#define PLAYERS_WIDTH_RATIO 0.25
#define CONNECTION_HEIGHT_RATIO 0.2

GameMenu::GameMenu(int w, int h, KeySetManager& key_manager, GameSettings&& settings) : settings(move(settings)), settings_menu(nullptr), peer(nullptr){
	SDL_Rect rect;
	rect.w = w * PLAYERS_WIDTH_RATIO;
	rect.x = w - rect.w;
	rect.h = h;
	rect.y = 0;
	
	settings_rect.x = 0;
	settings_rect.y = h * CONNECTION_HEIGHT_RATIO;
	settings_rect.w = w - rect.w;
	settings_rect.h = h - settings_rect.y;
	
	players = make_unique<PlayersSubView>(rect, key_manager);
	view_manager.add_view(players.get());
}

bool GameMenu::step(){
	view_manager.step();
	
	return false;
}

bool GameMenu::handle_event(const SDL_Event& event){	
	if(view_manager.handle_event(event)) return false;
	
	if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s){
		if(peer == nullptr){
			peer = settings.create_peer();
			players->attach_settings(peer, peer);
			
			settings_menu = make_unique<GameSettingsMenu>(settings_rect, peer, peer, true);
			view_manager.add_view(settings_menu.get());				
		}
		else{
			view_manager.remove_view(settings_menu.get());
			settings_menu = nullptr;

			players->detach_settings();
			settings.remove_peer(peer);
			peer = nullptr;
		}
	}
	
	return event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE;
}

void GameMenu::draw(SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer, 32, 32, 32, 0);
	SDL_RenderClear(renderer);
	
	view_manager.draw(renderer);
}
