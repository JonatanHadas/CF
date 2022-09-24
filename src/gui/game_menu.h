#ifndef _GAME_MENU_H
#define _GAME_MENU_H

#include "gui.h"
#include "subview_manager.h"

#include "players_subview.h"
#include "game_settings_subview.h"
#include "game_startup_menu.h"

#include "game_gui.h"

#include "../game/game_settings_manager.h"

class GameMenu : public Gui, public GameSettingsObserver{
	int w, h;
	SDL_Rect settings_rect;
	
	SubViewManager view_manager;
	
	unique_ptr<PlayersSubView> players;
	unique_ptr<GameSettingsMenu> settings_menu;
	unique_ptr<GameStartupMenu> startup;
	
	shared_ptr<GameCreator> game_creator;
	unique_ptr<CreatedGame> game;
	
	unique_ptr<GameGui> game_gui;
	
	void sync_display();
public:
	GameMenu(int w, int h, KeySetManager& key_manager);

	bool step();
	bool handle_event(const SDL_Event& event);
	void draw(SDL_Renderer* renderer);

	// Observer
	void init(const GameSettings& settings);

	void add_player(int team, int color);
	void remove_player(int player);
	void set_player_index(int index, int player);
	
	void set_player_name(int player, string name);
	void set_player_color(int player, int color);
	
	void set_teams(bool using_teams);
	void add_team();
	void remove_team(int team_num);
	void set_player_team(int player, int team);
	
	void set_team_name(int team, string name);
	
	void set_allowed_powerup(PowerUpDescriptor desc, bool allowed);
	
	void set_win_criterion(WinCriterion criterion);
	void set_win_amount(int amount);
	void set_tie_break(int threshold);
	
	void set_host_player(int player);
	void set_host();
	
	void player_ready(int player, bool is_ready);
	void reset_all_ready();
	void start_countdown();
	void start_game();
};

#endif
