#ifndef _GAME_GUI_H
#define _GAME_GUI_H

#include "gui.h"

#include "winner_display.h"

#include "game_drawer.h"
#include "keyset.h"
#include "../game/player_interface.h"
#include "../game/game_advancer.h"
#include "../game/game_event_listener_accumulator.h"

#include "button.h"
#include "subview_manager.h"

#include "sounds.h"

#include <map>
#include <memory>

using namespace std;

class GameSoundManager : public GameEventListener {
public:
	GameSoundManager();

	GameSoundManager(GameSoundManager&&) = delete;
	GameSoundManager(const GameSoundManager&) = delete;

	~GameSoundManager();
	
	GameSoundManager& operator=(GameSoundManager&&) = delete;
	GameSoundManager& operator=(const GameSoundManager&) = delete;

	void kill_player(int player);

	void spawn_powerup(const PowerUp& power_up);
	void activate_powerup(const PowerUp& power_up);
};

class GameMusicButton : public Button {
	Music music;
	void draw(SDL_Renderer* renderer, const SDL_Color color);
protected:
	void draw_inactive(SDL_Renderer* renderer);
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	
	void on_pressed();
public:
	GameMusicButton(const SDL_Rect& rect, bool music_on);
	
	bool is_music_on();
};

class GameGui : public Gui{
	const GameSettings& settings;
	GameDrawer drawer;
	GameSoundManager sounds;
	
	unique_ptr<GameMusicButton> music_button;
	SubViewManager view_manager;
	
	GameView* view;
	GameAdvancer* advancer;
	GameEventListenerAccumulator* accumulator;
	map<int, PlayerInterface*> interfaces;
	map<int, KeySet> keysets;
	
	bool paused;
	int last_round;
	int starting_timer;
	
	bool music_on;
public:
	GameGui(
		GameView* view,
		GameAdvancer* advancer,
		GameEventListenerAccumulator* accumulator,
		const GameSettings& settings,
		const map<int, PlayerInterface*>& interfaces,
		const map<int, KeySet>& keysets,
		bool music_on
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
