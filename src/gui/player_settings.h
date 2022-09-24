#ifndef _PLAYER_SETTINGS_H
#define _PLAYER_SETTINGS_H

#include "../game/game_settings_view.h"
#include "../game/game_settings_manipulator.h"

#include <map>
#include <vector>

using namespace std;

class PlayerSettings{
	GameSettingsView* view;
	GameSettingsManipulator* manipulator;
	
	map<int, string> names;
	map<int, int> player_ids;
	vector<int> index_ids;
	
	int next_id;
public:
	PlayerSettings();
	
	void add_player();
	void remove_player(int id);
	
	int get_index(int id) const;
	const vector<int>& get_ids();
	
	const string& get_player_name(int id) const;
	void set_player_name(int id, const string& name);
	int get_player_color(int id) const;
	void set_player_color(int id, int color);
	
	const GameSettings& get_settings() const;
	
	bool is_attached() const;
	void attach(GameSettingsView* view, GameSettingsManipulator* manipulator);
	void detach();
};

#endif
