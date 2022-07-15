#ifndef _GAME_GUI_H
#define _GAME_GUI_H

#include "game_drawer.h"
#include "keyset.h"
#include "../game/player_interface.h"

#include <SDL.h>

#include <map>

using namespace std;

class GameGui{
	GameDrawer drawer;
	
	GameView* view;
	map<PlayerInterface*, KeySet> interfaces;
public:
	GameGui(const BoardSize& board, GameView* view, const map<PlayerInterface*, KeySet>& interfaces);
	
	void step();
	void handle_event(const SDL_Event& event);
	void draw(SDL_Renderer* renderer);
};

#endif
