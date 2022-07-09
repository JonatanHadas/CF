#ifndef _GAME_VIEW_H
#define _GAME_VIEW_H

#include "game_data.h"

#include <vector>
#include <set>
#include <map>
#include <memory>


class GameView{
public:
	virtual int get_round() const = 0;
	virtual const vector<int>& get_scores() const = 0;

	virtual const vector<PlayerState>& get_states() const = 0;
	virtual const vector<vector<PlayerPosition>>& get_histories() const = 0;

	virtual const map<int, PowerUp>& get_powerups() const = 0;
	virtual const set<unique_ptr<PowerUpEffect>>& get_powerup_effects() const = 0;
};

#endif
