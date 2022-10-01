#ifndef _GAME_VIEW_H
#define _GAME_VIEW_H

#include "game_data.h"

#include <vector>
#include <set>
#include <map>
#include <memory>


class GameView{
public:
	virtual const BoardSize& get_board_size() const = 0;

	virtual int get_round() const = 0;
	virtual const vector<int>& get_scores() const = 0;
	virtual bool is_over() const = 0;
	virtual bool is_tie_break() const = 0;
	
	virtual bool is_round_over() const = 0;
	virtual const vector<int>& get_round_winners() const = 0;

	virtual const vector<PlayerState>& get_states() const = 0;
	virtual const vector<vector<PlayerPosition>>& get_histories() const = 0;

	virtual const map<int, PowerUp>& get_powerups() const = 0;
	virtual const set<unique_ptr<PowerUpEffect>>& get_powerup_effects() const = 0;
};

#endif
