#ifndef _GAME_OBSERVER_H
#define _GAME_OBSERVER_H

#include "game_data.h"

#include <vector>

using namespace std;

class GameObserver{
public:
	virtual void init(const vector<vector<PlayerPosition>>& histories, int round_timer) = 0;
	
	virtual void new_round(int round) = 0;
	virtual void update_scores(const vector<int>& addition) = 0;

	virtual void update(const vector<PlayerPosition>& positions, const vector<PlayerState>& states) = 0;
	
	virtual void spawn_powerup(int id, const PowerUp& power_up) = 0;
	virtual void activate_powerup(int id, const PowerUpEffect& effect) = 0;
};

#endif
