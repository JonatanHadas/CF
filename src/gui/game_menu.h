#ifndef _GAME_MENU_H
#define _GAME_MENU_H

#include "gui.h"
#include "subview_manager.h"

#include "players_subview.h"
#include "game_settings_subview.h"

#include "../game/game_settings_manager.h"

class GameMenu : public Gui{
	SDL_Rect settings_rect;
	
	SubViewManager view_manager;
	
	unique_ptr<PlayersSubView> players;
	unique_ptr<GameSettingsMenu> settings_menu;
	
	GameSettingsManager settings;
	GameSettingsManager::Peer* peer;
public:
	GameMenu(int w, int h, KeySetManager& key_manager, GameSettings&& settings);

	bool step();
	bool handle_event(const SDL_Event& event);
	void draw(SDL_Renderer* renderer);
};

#endif
