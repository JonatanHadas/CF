#ifndef _SOUNDS_H
#define _SOUNDS_H

#define CHANNELS 4

bool load_sounds();

enum class Sound{
	SPAWN_POWERUP,
	TAKE_POWERUP,
	COLLISION,
	DING_LOW,
	DING_HIGH,
	CLICK,
	GAME_END,
};

void play(Sound snd);

void silence();

#endif