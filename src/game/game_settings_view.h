#ifndef _GAME_SETTINGS_VIEW_H
#define _GAME_SETTINGS_VIEW_H

#include "game_settings.h"

#include <vector>
#include <set>

class GameSettingsView{
public:
	virtual const GameSettings& get_settings() const = 0;
	
	virtual const vector<int>& get_my_players() const = 0;
	
	virtual const vector<int>& get_host_players() const = 0;
	virtual set<int> get_ready() const = 0;
	
	virtual bool am_i_host() const = 0;
	virtual bool am_i_ready() const = 0;
	virtual bool is_counting_down() const = 0;
};

#endif
