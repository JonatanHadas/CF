#ifndef _GAME_SETTINGS_MANIPULATOR_H
#define _GAME_SETTINGS_MANIPULATOR_H

#include "game_data.h"

#include <string>

using namespace std;

class GameSettingsManipulator{
public:
	virtual void add_player() = 0;
	virtual void remove_player(int index) = 0;
	
	virtual void set_player_name(int index, string name) = 0;
	virtual void set_player_color(int index, int color) = 0;
	
	virtual void set_teams(bool using_teams) = 0;
	virtual void add_team() = 0;
	virtual void remove_team(int team) = 0;
	virtual void set_player_team(int index, int team) = 0;
	
	virtual void set_team_name(int team, string name) = 0;
	
	virtual void set_allowed_powerup(PowerUpDescriptor desc, bool allowed) = 0;
	
	virtual void set_win_criterion(WinCriterion criterion) = 0;
	virtual void set_win_amount(int amount) = 0;
	virtual void set_tie_break(int threshold) = 0;
	
	virtual void set_ready(bool is_ready) = 0;
	virtual void start_countdown() = 0;
	virtual void start_game() = 0;
};

#endif
