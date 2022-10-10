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

class Music{
public:
	Music();

	Music(const Music&) = delete;
	Music(Music&&) = delete;

	~Music();

	Music& operator=(const Music&) = delete;
	Music& operator=(Music&&) = delete;

	void play();
	void halt();

	void pause();
	void resume();
	
	bool is_playing();
	bool is_paused();
};

#endif