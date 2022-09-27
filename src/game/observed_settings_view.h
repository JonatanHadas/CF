#ifndef _OBSERVED_SETTINGS_VIEW_H
#define _OBSERVED_SETTINGS_VIEW_H

#include "game_settings_observer.h"
#include "game_settings_observer_accumulator.h"
#include "game_settings_view.h"

class ObservedSettingsView : public GameSettingsView, public GameSettingsObserver, public GameSettingsObserverAccumulator {
	GameSettings settings;
	vector<int> my_players;
	vector<int> host_players;
	vector<int> ready_players;
	
	bool host;
	bool ready;
	bool counting_down;
protected:
	void init_observer(GameSettingsObserver* observer);
public:
	ObservedSettingsView();

	// View
	const GameSettings& get_settings() const;
	
	const vector<int>& get_my_players() const;
	
	const vector<int>& get_host_players() const;
	set<int> get_ready() const;
	
	bool am_i_host() const;
	bool am_i_ready() const;
	bool is_counting_down() const;

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

#endif
