#ifndef _GAME_GUI_H
#define _GAME_GUI_H

#include "game_drawer.h"
#include "../game/player_interface.h"

#include <SDL.h>

class GameGui{
	GameDrawer drawer;
	
	GameView* view;
	PlayerInterface* interface;
public:
	GameGui(const BoardSize& board, GameView* view, PlayerInterface* interface);
	
	void step();
	void handle_event(const SDL_Event& event);
	void draw(SDL_Renderer* renderer);
};

#endif
