#include "game_menu.h"

#define PLAYERS_WIDTH_RATIO 0.25
#define CONNECTION_HEIGHT_RATIO 0.2

GameMenu::GameMenu(int w, int h, KeySetManager& key_manager) : settings_menu(nullptr), w(w), h(h){
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
	
	rect.y = rect.x = 0;
	rect.w = settings_rect.w;
	rect.h = settings_rect.y;
	startup = make_unique<GameStartupMenu>(rect);
	view_manager.add_view(startup.get());
}

GameMenu::~GameMenu(){
	game_gui = nullptr;
	game = nullptr;
	game_creator = nullptr;
}

void GameMenu::sync_display(){
	auto new_creator = startup->get_creator();
	if(game_creator != new_creator){
		if(game_creator.get() != nullptr){
			game_creator->get_accumulator()->remove_observer(this);
			players->detach_settings();
		}
		if(settings_menu.get() != nullptr){
			view_manager.remove_view(settings_menu.get());
			settings_menu = nullptr;
		}
		
		game_creator = new_creator;
		
		if(game_creator.get() != nullptr){
			game_creator->get_accumulator()->add_observer(this);

			players->attach_settings(
				game_creator->get_view(),
				game_creator->get_manipulator()
			);
			
			settings_menu = make_unique<GameSettingsMenu>(
				settings_rect,
				game_creator->get_view(), game_creator->get_manipulator(), game_creator->get_accumulator(),
				game_creator->is_multi_peer()
			);
			view_manager.add_view(settings_menu.get());
		}
	}
	
	if(game.get() != nullptr && game_gui.get() == nullptr){
		map<PlayerInterface*, KeySet> keys_map;
		vector<KeySet> keys = players->get_keys();
		for(int i = 0; i < keys.size(); i++){
			keys_map[game->get_interfaces()[i]] = keys[i];
		}

		game_gui = make_unique<GameGui>(
			game->get_view(),
			game->get_advancer(),
			game_creator->get_view()->get_settings(),
			keys_map
		);
	}
}

bool GameMenu::step(){
	sync_display();
	if(game_gui.get() != nullptr){
		if(game.get() == nullptr) return false;

		if(game_gui->step()){
			winner_display = game_gui->get_score_display();
			game = nullptr;
		}
		
		return false;
	}	
	if(game_creator.get() != nullptr) game_creator->update();

	if(winner_display.get() != nullptr){
		if(winner_display->step()) winner_display = nullptr;
		
		return false;
	}

	view_manager.step();

	return false;
}

bool GameMenu::handle_event(const SDL_Event& event){
	sync_display();
	
	bool exit_key = event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE;

	if(game_gui.get() != nullptr){		
		if(game.get() == nullptr) return exit_key;
		else if(exit_key){
			game = nullptr;
			startup->set_creator(nullptr);
			return false;
		}

		if(game_gui->handle_event(event)){
			winner_display = game_gui->get_score_display();
			game = nullptr;
		}
		return false;
	}
	if(winner_display.get() != nullptr){
		if(winner_display->handle_event(event)) winner_display = nullptr;
		return false;
	}
	
	if(view_manager.handle_event(event)) return false;
	
	return exit_key;
}

void GameMenu::draw(SDL_Renderer* renderer){
	sync_display();
	
	if(game_gui.get() != nullptr){
		if(game.get() == nullptr){
			SDL_RenderSetLogicalSize(renderer, w, h);
			game_gui = nullptr;
		}
		else{			
			game_gui->draw(renderer);
			return;
		}
	}
	if(winner_display.get() != nullptr){
		winner_display->draw(renderer);
		return;
	}
	
	SDL_SetRenderDrawColor(renderer, 32, 32, 32, 0);
	SDL_RenderClear(renderer);
	
	view_manager.draw(renderer);
}

void GameMenu::start_game(){
	game = game_creator->create_game();
	game_gui = nullptr;
}

void GameMenu::init(const GameSettings& settings){}
void GameMenu::add_player(int team, int color){}
void GameMenu::remove_player(int player){}
void GameMenu::set_player_index(int index, int player){}
void GameMenu::set_player_name(int player, string name){}
void GameMenu::set_player_color(int player, int color){}
void GameMenu::set_teams(bool using_teams){}
void GameMenu::add_team(){}
void GameMenu::remove_team(int team_num){}
void GameMenu::set_player_team(int player, int team){}
void GameMenu::set_team_name(int team, string name){}
void GameMenu::set_allowed_powerup(PowerUpDescriptor desc, bool allowed){}
void GameMenu::set_win_criterion(WinCriterion criterion){}
void GameMenu::set_win_amount(int amount){}
void GameMenu::set_tie_break(int threshold){}
void GameMenu::set_host_player(int player){}
void GameMenu::set_host(){}
void GameMenu::player_ready(int player, bool is_ready){}
void GameMenu::reset_all_ready(){}
void GameMenu::start_countdown(){}
