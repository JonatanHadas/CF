#ifndef _POWERUPS_H
#define _POWERUPS_H

#include "game_data.h"

#include <map>

using namespace std;

extern map<PowerUpDescriptor, int> powerup_times;

class PowerUpSpawner{
	int counter;
	int timer;
	bool extra;
	void reset_timer();
public:
	PowerUpSpawner(bool extra);
	
	bool is_done() const;
	bool should_spawn();
};

#endif
