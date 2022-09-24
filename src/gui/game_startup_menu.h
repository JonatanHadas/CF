#ifndef _GAME_STARTUP_MENU_H
#define _GAME_STARTUP_MENU_H

#include "subview.h"
#include "button.h"
#include "subview_manager.h"

#include "texts.h"

#include "../game/game_creator.h"

#include <memory>
#include <functional>

using namespace std;

class SimpleButton : public Button {
	string text;
	unique_ptr<Msg> label;
	
	void draw_button(SDL_Renderer* renderer, const SDL_Color& color);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);	
public:
	SimpleButton(const SDL_Rect& rect, string text);
};

class GameStartupMenu;

class LocalGameButton : public SimpleButton {
	GameStartupMenu& menu;
protected:
	void on_pressed();
public:
	LocalGameButton(const SDL_Rect& rect, GameStartupMenu& menu);
};

class LeaveGameButton : public SimpleButton {
	GameStartupMenu& menu;
protected:
	void on_pressed();
public:
	LeaveGameButton(const SDL_Rect& rect, GameStartupMenu& menu);	
};

#define COUNTDOWN_LENGTH 4

class ReadyButton : public Button {
	unique_ptr<Msg> ready, not_ready, start, waiting;
	unique_ptr<Msg> countdown[COUNTDOWN_LENGTH];
	
	function<int()> get_countdown;
	
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;

	void draw_button(SDL_Renderer* renderer, const SDL_Color& color);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);	

	void on_pressed();
public:
	ReadyButton(
		const SDL_Rect& rect,
		GameSettingsView* view,
		GameSettingsManipulator* manipulator,
		function<int()> get_countdown
	);
};

class GameStartupMenu : public SubView {
	unique_ptr<LocalGameButton> local_game;
	
	unique_ptr<LeaveGameButton> leave_game;
	unique_ptr<ReadyButton> ready;
	
	shared_ptr<GameCreator> game_creator;
	
	int countdown;
	
	SubViewManager view_manager;
	
	void sync_display();
protected:
	void draw_content(SDL_Renderer* renderer);
	bool on_event(const SDL_Event& event);
public:
	GameStartupMenu(const SDL_Rect& rect);
	
	shared_ptr<GameCreator> get_creator();
	void set_creator(shared_ptr<GameCreator> creator);

	void lose_focus();
	void step();
};

#endif
