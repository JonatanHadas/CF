#ifndef _GAME_SETTINGS_OBSERVER_H
#define _GAME_SETTINGS_OBSERVER_H

#include "game_settings.h"

class GameSettingsObserver{
public:
	virtual void init(const GameSettings& settings) = 0;

	virtual void add_player(int team, int color) = 0;
	virtual void remove_player(int player) = 0;
	virtual void set_player_index(int index, int player) = 0;
	
	virtual void set_player_name(int player, string name) = 0;
	virtual void set_player_color(int player, int color) = 0;
	
	virtual void set_teams(bool using_teams) = 0;
	virtual void add_team() = 0;
	virtual void remove_team(int team_num) = 0;
	virtual void set_player_team(int player, int team) = 0;
	
	virtual void set_team_name(int team, string name) = 0;
	
	virtual void set_allowed_powerup(PowerUpDescriptor desc, bool allowed) = 0;
	
	virtual void set_win_criterion(WinCriterion criterion) = 0;
	virtual void set_win_amount(int amount) = 0;
	virtual void set_tie_break(int threshold) = 0;
	
	virtual void set_host_player(int player) = 0;
	virtual void set_host() = 0;
	
	virtual void player_ready(int player, bool is_ready) = 0;
	virtual void reset_all_ready() = 0;
	virtual void start_countdown() = 0;
	virtual void start_game() = 0;
};

#endif
