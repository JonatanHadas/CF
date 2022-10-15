#ifndef _GAME_STARTUP_MENU_H
#define _GAME_STARTUP_MENU_H

#include "subview.h"
#include "button.h"
#include "textbox.h"
#include "subview_manager.h"

#include "texts.h"

#include "../game/game_creator.h"

#include "../network/client.h"

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

#define COUNTDOWN_LENGTH 3

class StartButton : public Button {
	unique_ptr<Msg> start, waiting;
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
	StartButton(
		const SDL_Rect& rect,
		GameSettingsView* view,
		GameSettingsManipulator* manipulator,
		function<int()> get_countdown
	);
};

class ConnectionState{
	int text_x, text_y;
	
	int error_x, exiting_error_x, connecting_x;
	
	int counter;
	
	unique_ptr<Client> client;
	
	bool has_error;
	unique_ptr<string> error;
	unique_ptr<Msg> error_msg, exiting_error;

	unique_ptr<Msg> connecting_msg;
public:
	ConnectionState(int text_x, int text_y);

	void connect(const string& hostname);
	
	bool is_active();
	bool is_connected();
	
	void draw(SDL_Renderer* renderer);
	void step();
	
	void set_error(const string& message);
	void clear_error();
	
	unique_ptr<Client>&& get_client();
};

class HostTextBox : public TextBox {
	ConnectionState& connection;
protected:
	void draw_back(SDL_Renderer* renderer, bool typing);
	void on_set(const string& text);
	string get_default_text();
	void on_active();
public:
	HostTextBox(
		const SDL_Rect& rect, int margin,
		TextCompleter& completer,
		ConnectionState& connection
	);
};

class GameStartupMenu : public SubView {
	unique_ptr<LocalGameButton> local_game;
	
	unique_ptr<HostTextBox> host_box;
	ConnectionState connection;
	
	unique_ptr<LeaveGameButton> leave_game;
	unique_ptr<StartButton> start;
	
	shared_ptr<GameCreator> game_creator;
	
	int countdown;
	
	SubViewManager view_manager;
	
	void sync_display();
protected:
	void draw_content(SDL_Renderer* renderer);
	bool on_event(const SDL_Event& event);
public:
	GameStartupMenu(const SDL_Rect& rect, TextCompleter& host_completer);
	
	shared_ptr<GameCreator> get_creator();
	void set_creator(shared_ptr<GameCreator> creator);

	void lose_focus();
	void step();
};

#endif
