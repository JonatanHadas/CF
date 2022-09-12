#ifndef _GAME_SETTINGS_SUBVIEW
#define _GAME_SETTINGS_SUBVIEW

#include "subview.h"
#include "tab_view.h"
#include "button.h"
#include "textbox.h"
#include "options_menu.h"
#include "number_button.h"

#include "texts.h"

#include "../game/game_settings_view.h"
#include "../game/game_settings_manipulator.h"
#include "../game/game_settings_observer.h"
#include "../game/game_settings_observer_accumulator.h"

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

class PlayerView;

class TeamChangeButton : public Button {
	bool direction;
	
	PlayerView& view;

	void draw_arrow(SDL_Renderer* renderer, const SDL_Color& color);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);
	
	void on_pressed();
public:
	TeamChangeButton(
		const SDL_Rect& rect,
		bool direction,
		PlayerView& view
	);
};

class PlayerView : public SubView {
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
	
	bool multi_peer;
	
	int color;
	string name;
	int player;
	unique_ptr<Msg> name_label;

	unique_ptr<Msg> host_label;
	unique_ptr<Msg> ready_label;
	
	unique_ptr<TeamChangeButton> inc, dec;
	
	SubViewManager view_manager;
	
	void sync_display();
protected:
	void draw_content(SDL_Renderer* renderer);
	bool on_event(const SDL_Event& event);
public:
	PlayerView(
		const SDL_Rect& rect,
		int player,
		GameSettingsView* view, GameSettingsManipulator* manipulator,
		bool multi_peer
	);
	
	void lose_focus();
	void step();
	
	int get_player();
	void set_player(int player);
	
	int get_team();
	void set_team(int team);
};

class TeamSeparator;

class TeamNameBox : public TextBox {
	TeamSeparator& view;
protected:
	void draw_back(SDL_Renderer* renderer, bool typing);
	void on_set(const string& text);
	string get_default_text();
public:
	TeamNameBox(
		const SDL_Rect& rect, int margin,
		TeamSeparator& view
	);
};

class TeamRemoveButton : public Button {
	unique_ptr<Texture> cross;
	
	TeamSeparator& view;
	void init(SDL_Renderer* renderer);
	void draw_cross(SDL_Renderer* renderer);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);
	
	void on_pressed();
public:
	TeamRemoveButton(const SDL_Rect& rect, TeamSeparator& view);
};

class TeamSeparator : public SubView {
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
	
	unique_ptr<TeamNameBox> name;
	unique_ptr<TeamRemoveButton> remove_button;
	
	int team;
	
	SubViewManager view_manager;
	
	void sync_display();
protected:
	void draw_content(SDL_Renderer* renderer);
	bool on_event(const SDL_Event& event);
public:
	TeamSeparator(
		const SDL_Rect& rect,
		int team,
		GameSettingsView* view, GameSettingsManipulator* manipulator
	);
	
	void lose_focus();
	void step();
	
	int get_team();
	void set_team(int team);
	
	void set_name(string name);
	void remove();
};

class TeamsButton : public Button {
	unique_ptr<Msg> label;
	bool state;
	
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
	
	void draw_button(SDL_Renderer* renderer, const SDL_Color& color);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);
	
	void on_pressed();
public:
	TeamsButton(
		const SDL_Rect& rect,
		GameSettingsView* view, GameSettingsManipulator* manipulator
	);
};

class TeamAddButton : public Button {
	unique_ptr<Msg> label;
	
	GameSettingsManipulator* manipulator;
	
	void draw_button(SDL_Renderer* renderer, const SDL_Color& color);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);
	
	void on_pressed();
public:
	TeamAddButton(
		const SDL_Rect& rect,
		GameSettingsManipulator* manipulator
	); 
};

class PlayersView : public SubView, public GameSettingsObserver {
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
	
	bool multi_peer;
	bool using_teams;
	
	vector<unique_ptr<PlayerView>> players;
	vector<unique_ptr<TeamSeparator>> teams;
	
	vector<unique_ptr<PlayerView>> removed_players;
	vector<unique_ptr<TeamSeparator>> removed_teams;
	
	unique_ptr<TeamsButton> using_teams_button;
	unique_ptr<TeamAddButton> add_team_button;
	
	SubViewManager view_manager;
	
	void sync_displays();
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
	
	// Observer
	void init(const GameSettings& settings);

	void add_player(int team, int color);
	void remove_player(int player);
	void set_player_index(int index, int player);
	
	void set_player_name(int player, string name);
	void set_player_color(int player, int color);
	
	void set_teams(bool using_teams);
	void add_team();
	void remove_team(int team_num);
	void set_player_team(int player, int team);
	
	void set_team_name(int team, string name);
	
	void set_allowed_powerup(PowerUpDescriptor desc, bool allowed);
	
	void set_win_criterion(WinCriterion criterion);
	void set_win_amount(int amount);
	void set_tie_break(int threshold);
	
	void set_host_player(int player);
	void set_host();
	
	void player_ready(int player, bool is_ready);
	void reset_all_ready();
	void start_countdown();
	void start_game();
};

class GameSettingsMenu : public TabView {
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
	GameSettingsObserverAccumulator* accumulator;
	bool multi_peer;

	unique_ptr<GameSettingsSubView> settings;
	unique_ptr<PlayersView> players;	
protected:
	vector<TabView::ViewDescriptor> init_subviews(const SDL_Rect& rect);
	void draw_button_back(SDL_Renderer* renderer, const SubView& view, TabView::State state);

public:
	GameSettingsMenu(
		const SDL_Rect& rect,
		GameSettingsView* view, GameSettingsManipulator* manipulator, GameSettingsObserverAccumulator* accumulator,
		bool multi_peer
	);
	
	GameSettingsMenu(const GameSettingsMenu&) = delete;
	GameSettingsMenu(GameSettingsMenu&&) = delete;

	~GameSettingsMenu();

	GameSettingsMenu& operator=(const GameSettingsMenu&) = delete;
	GameSettingsMenu& operator=(GameSettingsMenu&&) = delete;
};

#endif
