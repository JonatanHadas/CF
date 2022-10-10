#include "sounds.h"

#include <stdlib.h>

#include "SDL_mixer.h"
#include <map>

#define DIR "data/sounds/"
#define MUSIC "music/"
#define WAV ".wav"
#define MP3 ".mp3"


std::map<Sound, Mix_Chunk*> sounds;

Mix_Music* music = NULL;

void free_sounds(){
	for(auto entry: sounds){
		Mix_FreeChunk(entry.second);
	}
	
	sounds.clear();
	
	if(music != NULL) Mix_FreeMusic(music);
	music = NULL;
}

bool load_sound(Sound id, const char* path, double volume){
	Mix_Chunk* chunk = Mix_LoadWAV(path);
	if(!chunk) return false;
	
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME * volume);
	
	sounds[id] = chunk;
	return true;
}

bool load_music(const char* path){
	if(music != NULL) Mix_FreeMusic(music);
	music = Mix_LoadMUS(path);
	
	return music != NULL;
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
	if(load_music(DIR MUSIC "astro_race" MP3))
		return true;
	return false;
}

void play(Sound snd){
	Mix_PlayChannel(-1, sounds[snd], 0);
}

void silence(){
	Mix_HaltChannel(-1);
}

Music::Music(){}

Music::~Music(){
	halt();
}

void Music::play(){
	if(music != NULL && !is_playing()) Mix_PlayMusic(music, -1);
}

void Music::halt(){
	if(is_playing()) Mix_HaltMusic();
}

void Music::pause(){
	if(is_playing() && is_paused()) Mix_ResumeMusic();
}

void Music::resume(){
	if(is_playing() && !is_paused()) Mix_PauseMusic();
}

bool Music::is_playing(){
	return Mix_PlayingMusic();
}

bool Music::is_paused(){
	return Mix_PausedMusic();
}
