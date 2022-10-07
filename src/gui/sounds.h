#ifndef _SOUNDS_H
#define _SOUNDS_H

#define CHANNELS 4

bool load_sounds();

enum class Sound{
	SPAWN_POWERUP,
	TAKE_POWERUP,
	COLLISION,
};

void play(Sound snd);

void silence();

#endif