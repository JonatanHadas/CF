#ifndef _PLAYERS_SUBVIEW_H
#define _PLAYERS_SUBVIEW_H

#include "../game/game_settings_view.h"
#include "../game/game_settings_manipulator.h"

#include "subview.h"
#include "button.h"
#include "textbox.h"
#include "key_choice_button.h"
#include "subview_manager.h"

#include "player_settings.h"

#include "keyset.h"

#include <vector>
#include <memory>

using namespace std;

class PlayerSubView;
class PlayersSubView;

class PlayerRemoveButton : public Button {
	unique_ptr<Texture> cross;
	
	PlayerSubView& view;

	void init(SDL_Renderer* renderer);
	void draw_button(SDL_Renderer* renderer);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);

	void on_pressed();
public:
	PlayerRemoveButton(const SDL_Rect& rect, PlayerSubView& view);
};

class PlayerNameBox : public TextBox {
	PlayerSubView& view;
protected:
	void draw_back(SDL_Renderer* renderer, bool typing);
	void on_set(const string& text);
	string get_default_text();
public:
	PlayerNameBox(const SDL_Rect& rect, int margin, PlayerSubView& view);
};

class PlayerKeyButton : public KeyChoiceButton {
	function<SDL_Scancode&(KeySet&)> key_choice;
	
	PlayerSubView& view;
protected:	
	bool on_set(SDL_Scancode key);

	void draw_back(SDL_Renderer* renderer, bool active);
public:
	PlayerKeyButton(
		const SDL_Rect& rect, PlayerSubView& view,
		function<SDL_Scancode&(KeySet&)> key_choice
	);
};

class PlayerColorButton : public Button {
	PlayerSubView& view;
	void draw_back(SDL_Renderer* renderer);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);

	void on_pressed();
public:
	PlayerColorButton(const SDL_Rect& rect, PlayerSubView& view);
};

class ColorButton : public Button {
	PlayerSubView& view;
	int color;
	unique_ptr<Texture> cross;

	void init(SDL_Renderer* renderer);
	void draw_back(SDL_Renderer* renderer);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);

	void on_pressed();
public:
	ColorButton(const SDL_Rect& rect, int color, PlayerSubView& view);
};

class ColorMenu : public SubView {
	PlayerSubView& view;
	
	SubViewManager manager;
	vector<unique_ptr<ColorButton>> buttons;
protected:
	bool on_event(const SDL_Event& event);
	void draw_content(SDL_Renderer* renderer);
public:
	ColorMenu(const SDL_Rect& first_rect, int width, PlayerSubView& view);

	void lose_focus();
	void step();
};

class PlayerSubView : public SubView {
	int id;
	PlayerSettings& settings;
	KeySetManager& key_manager;
	
	int keys_id;
	
	unique_ptr<PlayerRemoveButton> remove_button;
	unique_ptr<PlayerNameBox> name_box;
	
	unique_ptr<PlayerKeyButton> left_key, right_key;
	
	unique_ptr<PlayerColorButton> color_button;
	unique_ptr<ColorMenu> color_menu;
	bool showing_color, showing_color_menu;
	
	SubViewManager view_manager;
protected:
	bool on_event(const SDL_Event& event);
	void draw_content(SDL_Renderer* renderer);
public:
	PlayerSubView(const SDL_Rect& rect, int id, PlayerSettings& settings, KeySetManager& key_manager);
	
	PlayerSubView(PlayerSubView&&) = delete;
	PlayerSubView(const PlayerSubView&) = delete;
	
	~PlayerSubView();

	PlayerSubView& operator=(PlayerSubView&&) = delete;
	PlayerSubView& operator=(const PlayerSubView&) = delete;
	
	void lose_focus();
	void step();
	
	void remove();

	int get_id() const;
	int get_index() const;
	const string& get_name() const;
	int get_color() const;
	const KeySet& get_keys() const;

	void set_name(const string& new_name);
	void set_color(int new_color);
	void set_keys(const KeySet& keys);
	
	const PlayerSettings& get_settings() const;
	
	void set_color_menu_open(bool open);
};

class PlayerAddButton : public Button {
	unique_ptr<Texture> cross;
	
	PlayersSubView& owner;
	
	void init(SDL_Renderer* renderer);
	void draw_button(SDL_Renderer* renderer, SDL_Color color);
protected:
	void draw_pressed(SDL_Renderer* renderer);
	void draw_released(SDL_Renderer* renderer);
	void draw_inactive(SDL_Renderer* renderer);
	
	void on_pressed();
public:
	PlayerAddButton(const SDL_Rect& rect, PlayersSubView& owner);
};

class PlayersSubView : public SubView {	
	SubViewManager view_manager;
	vector<unique_ptr<PlayerSubView>> views;
	unique_ptr<PlayerAddButton> add_button;
	
	KeySetManager& key_manager;
	
	PlayerSettings settings;
protected:
	bool on_event(const SDL_Event& event);
	void draw_content(SDL_Renderer* renderer);
	
	void reorganize_views();
public:
	PlayersSubView(const SDL_Rect& rect, KeySetManager& key_manager);
	
	~PlayersSubView();
	
	PlayersSubView(const PlayersSubView&) = delete;
	PlayersSubView(PlayersSubView&&) = delete;
	
	PlayersSubView& operator=(const PlayersSubView&) = delete;
	PlayersSubView& operator=(PlayersSubView&&) = delete;
	
	void lose_focus();
	void step();
	
	void attach_settings(GameSettingsView* view, GameSettingsManipulator* manipulator);
	void detach_settings();
	
	void add_player();
	
	vector<KeySet> get_keys() const;
};

#endif
