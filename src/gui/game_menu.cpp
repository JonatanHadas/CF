#include "game_menu.h"

#include "images.h"
#include "colors.h"

MusicButton::MusicButton(const SDL_Rect& rect) :
	Button(rect, true), music_on(true) {}

void MusicButton::draw(SDL_Renderer* renderer, const SDL_Color color){
	fill_back(renderer, clear_color);
	
	auto image = get_img(music_on ? Img::UNMUTED : Img::MUTED);
	
	SDL_SetTextureBlendMode(image, SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(image, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(image, color.a);
	
	SDL_RenderCopy(renderer, image, NULL, NULL);
}

void MusicButton::draw_inactive(SDL_Renderer* renderer){
	draw(renderer, text_color);
}

void MusicButton::draw_pressed(SDL_Renderer* renderer){
	draw(renderer, active_color);
}

void MusicButton::draw_released(SDL_Renderer* renderer){
	draw(renderer, text_color);
}

void MusicButton::on_pressed(){
	music_on = !music_on;
}

bool MusicButton::is_music_on() const{
	return music_on;
}

#define PLAYERS_WIDTH_RATIO 0.25
#define CONNECTION_HEIGHT_RATIO 0.2
#define MUSIC_H 0.05
#define MUSIC_MARGIN 0.02


GameMenu::GameMenu(
	int w, int h,
	TextCompleter& host_completer, TextCompleter& name_completer,
	KeySetManager& key_manager
) : settings_menu(nullptr), w(w), h(h), name_completer(name_completer){
	SDL_Rect rect;
	int margin = h * MUSIC_MARGIN;
	rect.w = rect.h = MUSIC_H * h;
	rect.x = w - (rect.w + margin);
	rect.y = h - (rect.h + margin);
	
	music = make_unique<MusicButton>(rect);
	view_manager.add_view(music.get());
	
	rect.h = h - (rect.h + 2 * margin);
	rect.y = 0;
	rect.w = w * PLAYERS_WIDTH_RATIO;
	rect.x = w - rect.w;

	settings_rect.x = 0;
	settings_rect.y = h * CONNECTION_HEIGHT_RATIO;
	settings_rect.w = w - rect.w;
	settings_rect.h = h - settings_rect.y;
	
	players = make_unique<PlayersSubView>(rect, name_completer, key_manager);
	view_manager.add_view(players.get());
	
	rect.y = rect.x = 0;
	rect.w = settings_rect.w;
	rect.h = settings_rect.y;
	startup = make_unique<GameStartupMenu>(rect, host_completer);
	view_manager.add_view(startup.get());
}

GameMenu::~GameMenu(){
	settings_menu = nullptr;
	players->detach_settings();
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
				name_completer,
				game_creator->is_multi_peer()
			);
			view_manager.add_view(settings_menu.get());
		}
	}
	
	if(game.get() != nullptr && game_gui.get() == nullptr){
		map<int, KeySet> keys_map;
		map<int, PlayerInterface*> interfaces_map;
		
		vector<KeySet> keys = players->get_keys();
		for(int i = 0; i < keys.size(); i++){
			auto player = game_creator->get_view()->get_my_players()[i];
			keys_map[player] = keys[i];
			interfaces_map[player] = game->get_interfaces()[i];
		}

		game_gui = make_unique<GameGui>(
			game->get_view(),
			game->get_advancer(),
			game->get_accumulator(),
			game_creator->get_view()->get_settings(),
			interfaces_map,
			keys_map,
			music->is_music_on()
		);
	}
}

bool GameMenu::step(){
	sync_display();
	if(game_gui.get() != nullptr){
		if(game.get() == nullptr) return false;

		if(game_gui->step()){
			winner_display = game_gui->get_score_display();
			game_gui = nullptr;
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
			game_gui = nullptr;
			game = nullptr;
			startup->set_creator(nullptr);
			return false;
		}

		if(game_gui->handle_event(event)){
			winner_display = game_gui->get_score_display();
			game_gui = nullptr;
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
		game_gui->draw(renderer);
		return;
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
