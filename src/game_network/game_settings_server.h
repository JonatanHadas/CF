#ifndef _GAME_SETTINGS_SERVER_H
#define _GAME_SETTINGS_SERVER_H

#include "../network/server.h"

#include "../game/game_settings_manager.h"

#include <map>
#include <memory>

using namespace std;

class ServerSettingsObserver : public GameSettingsObserver{
	Peer& peer;
	
	void send(string message);
public:
	ServerSettingsObserver(Peer& peer);
	
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

class GameSettingsServer{
	GameSettingsManager settings;
	Server server;

	map<Peer*, GameSettingsManager::Peer*> peers;
	map<Peer*, unique_ptr<ServerSettingsObserver>> peer_observers;
	
	void handle_connection(const NetEvent& event);
	void handle_disconnection(const NetEvent& event);
	void handle_message(const NetEvent& event);

	static void add_player(GameSettingsManipulator* maipulator, istream& input);
	static void remove_player(GameSettingsManipulator* maipulator, istream& input);
	static void set_player_name(GameSettingsManipulator* maipulator, istream& input);
	static void set_player_color(GameSettingsManipulator* maipulator, istream& input);
	static void set_teams(GameSettingsManipulator* maipulator, istream& input);
	static void add_team(GameSettingsManipulator* maipulator, istream& input);
	static void remove_team(GameSettingsManipulator* maipulator, istream& input);
	static void set_player_team(GameSettingsManipulator* maipulator, istream& input);
	static void set_team_name(GameSettingsManipulator* maipulator, istream& input);
	static void set_allowed_powerup(GameSettingsManipulator* maipulator, istream& input);
	static void set_win_criterion(GameSettingsManipulator* maipulator, istream& input);
	static void set_win_amount(GameSettingsManipulator* maipulator, istream& input);
	static void set_tie_break(GameSettingsManipulator* maipulator, istream& input);
	static void set_ready(GameSettingsManipulator* maipulator, istream& input);
	static void start_countdown(GameSettingsManipulator* maipulator, istream& input);
	static void start_game(GameSettingsManipulator* maipulator, istream& input);
public:
	GameSettingsServer(const GameSettings& settings);
	
	void serve(int interval);
};

#endif
