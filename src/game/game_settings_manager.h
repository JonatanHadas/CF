#ifndef _GAME_SETTINGS_MANAGER_H
#define _GAME_SETTINGS_MANAGER_H

#include "game_settings_view.h"
#include "game_settings_manipulator.h"
#include "game_settings_observer.h"
#include "game_settings_observer_accumulator.h"

#include <memory>
#include <functional>

using namespace std;

class GameSettingsManager{
public:
	class Peer : public GameSettingsView, public GameSettingsManipulator, public GameSettingsObserverAccumulator{
		friend GameSettingsManager;
		
		GameSettingsManager& manager;
		
		vector<int> players;

		bool ready;	
	protected:
		void init_observer(GameSettingsObserver* observer);
	public:
		Peer(GameSettingsManager& manager);

		Peer(const Peer&) = delete;
		Peer(Peer&&) = delete;
		
		~Peer();
		
		Peer& operator=(const Peer&) = delete;
		Peer& operator=(Peer&&) = delete;

		// Manipulator
		void add_player();
		void remove_player(int index);
		
		void set_player_name(int index, string name);
		void set_player_color(int index, int color);
		
		void set_teams(bool using_teams);
		void add_team();
		void remove_team(int team);
		void set_player_team(int index, int team);
		
		void set_team_name(int team, string name);
		
		void set_allowed_powerup(PowerUpDescriptor desc, bool allowed);
		
		void set_win_criterion(WinCriterion criterion);
		void set_win_amount(int amount);
		void set_tie_break(int threshold);
		
		void set_ready(bool is_ready);
		void start_countdown();
		void start_game();
		
		// View
		const GameSettings& get_settings() const;
		
		const vector<int>& get_my_players() const;
		
		const vector<int>& get_host_players() const;
		set<int> get_ready() const;
		
		const bool am_i_host() const;
	};
private:
	friend GameSettingsManager::Peer;
	GameSettings settings;
	
	set<unique_ptr<GameSettingsManager::Peer>> peers;
	GameSettingsManager::Peer* host;
	
	void do_with_observers(function<void(GameSettingsObserver&)> todo);
	
	int add_player();
	void remove_player(int player);
	
	void set_player_name(int player, string name);
	void set_player_color(int player, int color);
	
	void set_teams(bool using_teams);
	void add_team();
	void remove_team(int team);
	void set_player_team(int player, int team);
	
	void set_team_name(int team, string name);
	
	void set_allowed_powerup(PowerUpDescriptor desc, bool allowed);
	
	void set_win_criterion(WinCriterion criterion);
	void set_win_amount(int amount);
	void set_tie_break(int threshold);
	
	void reset_all_ready();
	void set_player_ready(int player, bool is_ready);
	void start_countdown();

	bool check_ready();
public:
	GameSettingsManager(GameSettings&& settings);

	GameSettingsManager::Peer* create_peer();
	void remove_peer(GameSettingsManager::Peer* peer);
};

#endif
