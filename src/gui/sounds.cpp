#include "sounds.h"

#include <stdlib.h>

#include "SDL_mixer.h"
#include <map>

#define DIR "data/sounds/"
#define WAV ".wav"


std::map<Sound, Mix_Chunk*> sounds;

void free_sounds(){
	for(auto entry: sounds){
		Mix_FreeChunk(entry.second);
	}
	
	sounds.clear();
}

bool load_sound(Sound id, const char* path, double volume){
	Mix_Chunk* chunk = Mix_LoadWAV(path);
	if(!chunk) return false;
	
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME * volume);
	
	sounds[id] = chunk;
	return true;
}

bool load_sounds(){
	atexit(free_sounds);
	
	if(load_sound(Sound::SPAWN_POWERUP, DIR "spawn_powerup" WAV, 1.0))
	if(load_sound(Sound::TAKE_POWERUP, DIR "take_powerup" WAV, 1.0))
	if(load_sound(Sound::COLLISION, DIR "collision" WAV, 1.0))
	if(load_sound(Sound::DING_LOW, DIR "ding_low" WAV, 0.05))
	if(load_sound(Sound::DING_HIGH, DIR "ding_high" WAV, 0.05))
	if(load_sound(Sound::CLICK, DIR "click" WAV, 1.0))
	if(load_sound(Sound::GAME_END, DIR "game_end" WAV, 1.0))
		return true;
	return false;
}

void play(Sound snd){
	Mix_PlayChannel(-1, sounds[snd], 0);
}

void silence(){
	Mix_HaltChannel(-1);
}
