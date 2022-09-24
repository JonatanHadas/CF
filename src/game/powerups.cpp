#include "powerups.h"

#include "stdlib.h"

map<PowerUpDescriptor, int> powerup_times({
	{ PowerUpDescriptor(PowerUpType::SPEED_UP, PowerUpAffects::YOU), 240 },
	{ PowerUpDescriptor(PowerUpType::SPEED_UP, PowerUpAffects::OTHERS), 180 },
	{ PowerUpDescriptor(PowerUpType::SLOW_DOWN, PowerUpAffects::YOU), 420 },
	{ PowerUpDescriptor(PowerUpType::SLOW_DOWN, PowerUpAffects::OTHERS), 330 },
	{ PowerUpDescriptor(PowerUpType::NARROW, PowerUpAffects::YOU), 390 },
	{ PowerUpDescriptor(PowerUpType::THICKEN, PowerUpAffects::OTHERS), 330 },
	{ PowerUpDescriptor(PowerUpType::NARROW_TURN, PowerUpAffects::YOU), 300 },
	{ PowerUpDescriptor(PowerUpType::WIDE_TURN, PowerUpAffects::OTHERS), 300 },
	{ PowerUpDescriptor(PowerUpType::RIGHT_TURN, PowerUpAffects::YOU), 720 },
	{ PowerUpDescriptor(PowerUpType::INVERT, PowerUpAffects::OTHERS), 300 },
	{ PowerUpDescriptor(PowerUpType::WARP_AROUND, PowerUpAffects::YOU), 420 },
	{ PowerUpDescriptor(PowerUpType::WARP_AROUND, PowerUpAffects::ALL), 420 },
	{ PowerUpDescriptor(PowerUpType::ERASER, PowerUpAffects::YOU), 1 },
	{ PowerUpDescriptor(PowerUpType::ERASER, PowerUpAffects::ALL), 1 },
	{ PowerUpDescriptor(PowerUpType::HOVER, PowerUpAffects::YOU), 360 },
	{ PowerUpDescriptor(PowerUpType::SPAWN_POWERUPS, PowerUpAffects::ALL), 1 },
});

#define MIN_REGULAR_TIME 300
#define MAX_REGULAR_TIME 600

#define EXTRA_SPAWN_COUNT 3

#define MIN_EXTRA_TIME 10
#define MAX_EXTRA_TIME 20

PowerUpSpawner::PowerUpSpawner(bool extra) :
	extra(extra),
	timer(-1),
	counter(extra ? EXTRA_SPAWN_COUNT : -1) {
	
	reset_timer();
}

void PowerUpSpawner::reset_timer(){
	int min = extra ? MIN_EXTRA_TIME : MIN_REGULAR_TIME;
	int max = extra ? MAX_EXTRA_TIME : MAX_REGULAR_TIME;
	
	timer = min + rand() % (max - min);
}

bool PowerUpSpawner::is_done() const{
	return extra && !counter;
}

bool PowerUpSpawner::should_spawn(){
	if(timer) {
		timer--;
		return false;
	}
	else{
		reset_timer();
		if(extra) counter--;
		return true;
	}
}
