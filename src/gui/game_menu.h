#ifndef _GAME_MENU_H
#define _GAME_MENU_H

#include "gui.h"
#include "subview_manager.h"

#include "players_subview.h"

class GameMenu : public Gui{
	SubViewManager view_manager;
	
	unique_ptr<PlayersSubView> players;
public:
	GameMenu(int w, int h, KeySetManager& key_manager);

	bool step();
	bool handle_event(const SDL_Event& event);
	void draw(SDL_Renderer* renderer);
};

#endif
