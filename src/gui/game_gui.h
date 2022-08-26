#ifndef _GAME_GUI_H
#define _GAME_GUI_H

#include "gui.h"

#include "game_drawer.h"
#include "keyset.h"
#include "../game/player_interface.h"
#include "../game/game_advancer.h"

#include <map>

using namespace std;

class GameGui : public Gui{
	GameDrawer drawer;
	
	GameView* view;
	GameAdvancer* advancer;
	map<PlayerInterface*, KeySet> interfaces;
	
	bool paused;
	int last_round;
	int starting_timer;
public:
	GameGui(
		const BoardSize& board,
		GameView* view,
		GameAdvancer* advancer,
		const map<PlayerInterface*, KeySet>& interfaces
	);
	
	bool step();
	bool handle_event(const SDL_Event& event);
	void draw(SDL_Renderer* renderer);
};

#endif
