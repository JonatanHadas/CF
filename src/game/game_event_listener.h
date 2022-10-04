#ifndef _GAME_EVENT_LISTENER_H
#define _GAME_EVENT_LISTENER_H

#include "game_data.h"

class GameEventListener{
public:
	virtual void kill_player(int player) = 0;

	virtual void spawn_powerup(const PowerUp& power_up) = 0;
	virtual void activate_powerup(const PowerUp& power_up) = 0;
};

#endif
