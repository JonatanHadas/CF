#ifndef _GAME_GUI_H
#define _GAME_GUI_H

#include "gui.h"

#include "winner_display.h"

#include "game_drawer.h"
#include "keyset.h"
#include "../game/player_interface.h"
#include "../game/game_advancer.h"
#include "../game/game_event_listener_accumulator.h"

#include <map>

using namespace std;

class GameSoundManager : public GameEventListener {
public:
	GameSoundManager(GameSoundManager&&) = delete;
	GameSoundManager(const GameSoundManager&) = delete;

	~GameSoundManager();
	
	GameSoundManager& operator=(GameSoundManager&&) = delete;
	GameSoundManager& operator=(const GameSoundManager&) = delete;

	void kill_player(int player);

	void spawn_powerup(const PowerUp& power_up);
	void activate_powerup(const PowerUp& power_up);
};

class GameGui : public Gui{
	const GameSettings& settings;
	GameDrawer drawer;
	GameSoundManager sounds;
	
	GameView* view;
	GameAdvancer* advancer;
	GameEventListenerAccumulator* accumulator;
	map<PlayerInterface*, KeySet> interfaces;
	
	bool paused;
	int last_round;
	int starting_timer;
public:
	GameGui(
		GameView* view,
		GameAdvancer* advancer,
		GameEventListenerAccumulator* accumulator,
		const GameSettings& settings,
		const map<PlayerInterface*, KeySet>& interfaces
	);
	
	GameGui(GameGui&&) = delete;
	GameGui(const GameGui&) = delete;
	
	~GameGui();
	
	GameGui& operator=(GameGui&&) = delete;
	GameGui& operator=(const GameGui&) = delete;
	
	bool step();
	bool handle_event(const SDL_Event& event);
	void draw(SDL_Renderer* renderer);
	
	unique_ptr<WinnerDisplay> get_score_display();
};

#endif
