#ifndef _GAME_SETTINGS_SUBVIEW
#define _GAME_SETTINGS_SUBVIEW

#include "subview.h"
#include "tab_view.h"
#include "button.h"
#include "options_menu.h"
#include "number_button.h"

#include "texts.h"

#include "../game/game_settings_view.h"
#include "../game/game_settings_manipulator.h"

class GameSettingsView;

class WinCriterionMenu : public HorizontalOptionsMenu {
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
protected:
	void on_set(int option);
public:
	WinCriterionMenu(
		const SDL_Rect& rect, int x_margin,
		GameSettingsView* view, GameSettingsManipulator* manipulator
	);
	
	int get_option();
};

class WinAmountButton : public NumberButton {
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
protected:
	void on_set(int num);
public:
	WinAmountButton(
		const SDL_Rect& rect,
		GameSettingsView* view, GameSettingsManipulator* manipulator
	);
	
	int get_value();
};

class TieBreakButton : public NumberButton {
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
protected:
	void on_set(int num);
public:
	TieBreakButton(
		const SDL_Rect& rect,
		GameSettingsView* view, GameSettingsManipulator* manipulator
	);
	
	int get_value();
};

class PowerUpButton : public Button {
	unique_ptr<Texture> circle;

	PowerUpDescriptor descriptor;
	
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;	
	
	void draw_powerup(SDL_Renderer* renderer, bool pressed);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);
	
	void on_pressed();
public:
	PowerUpButton(
		const SDL_Rect& rect,
		PowerUpDescriptor descriptor,
		GameSettingsView* view, GameSettingsManipulator* manipulator
	);
};

class GameSettingsSubView : public SubView {
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
	
	SubViewManager view_manager;
	
	unique_ptr<Msg> scores_label;
	unique_ptr<WinCriterionMenu> win_criterion;

	bool using_score_numbers;
	unique_ptr<Msg> score_label, round_label;
	unique_ptr<WinAmountButton> win_amount_button;
	
	unique_ptr<Msg> tie_break_label;
	unique_ptr<TieBreakButton> tie_break_button;
	
	unique_ptr<Msg> powerup_label;
	vector<unique_ptr<PowerUpButton>> powerups;
	
	void sync_display();
protected:
	bool on_event(const SDL_Event& event);
	void draw_content(SDL_Renderer* renderer);
public:
	GameSettingsSubView(
		const SDL_Rect& rect,
		GameSettingsView* view, GameSettingsManipulator* manipulator
	);

	void lose_focus();
	void step();
};

class PlayersView : public SubView {
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
	bool multi_peer;
protected:
	bool on_event(const SDL_Event& event);
	void draw_content(SDL_Renderer* renderer);
public:
	PlayersView(
		const SDL_Rect& rect,
		GameSettingsView* view, GameSettingsManipulator* manipulator,
		bool multi_peer
	);

	void lose_focus();
	void step();
};

class GameSettingsMenu : public TabView {
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
	bool multi_peer;

	unique_ptr<GameSettingsSubView> settings;
	unique_ptr<PlayersView> players;
	
protected:
	vector<TabView::ViewDescriptor> init_subviews(const SDL_Rect& rect);
	void draw_button_back(SDL_Renderer* renderer, const SubView& view, TabView::State state);

public:
	GameSettingsMenu(
		const SDL_Rect& rect,
		GameSettingsView* view, GameSettingsManipulator* manipulator,
		bool multi_peer
	);
};

#endif
