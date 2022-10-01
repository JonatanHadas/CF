#ifndef _GAME_SETTINGS_CLIENT_H
#define _GAME_SETTINGS_CLIENT_H

#include "../game/game_settings_observer_accumulator.h"
#include "../game/game_settings_manipulator.h"

#include "../network/client.h"

class GameSettingsClient : public GameSettingsManipulator{
	Client& client;
	GameSettingsObserver& observer;
	
	void send(const string& message);

    static void init(GameSettingsObserver& observer, istream& input);

    static void add_player(GameSettingsObserver& observer, istream& input);
    static void remove_player(GameSettingsObserver& observer, istream& input);
    static void set_player_index(GameSettingsObserver& observer, istream& input);

    static void set_player_name(GameSettingsObserver& observer, istream& input);
    static void set_player_color(GameSettingsObserver& observer, istream& input);

    static void set_teams(GameSettingsObserver& observer, istream& input);
    static void add_team(GameSettingsObserver& observer, istream& input);
    static void remove_team(GameSettingsObserver& observer, istream& input);
    static void set_player_team(GameSettingsObserver& observer, istream& input);

    static void set_team_name(GameSettingsObserver& observer, istream& input);

    static void set_allowed_powerup(GameSettingsObserver& observer, istream& input);

    static void set_win_criterion(GameSettingsObserver& observer, istream& input);
    static void set_win_amount(GameSettingsObserver& observer, istream& input);
    static void set_tie_break(GameSettingsObserver& observer, istream& input);

    static void set_host_player(GameSettingsObserver& observer, istream& input);
    static void set_host(GameSettingsObserver& observer, istream& input);

    static void player_ready(GameSettingsObserver& observer, istream& input);
    static void reset_all_ready(GameSettingsObserver& observer, istream& input);
    static void start_countdown(GameSettingsObserver& observer, istream& input);
    static void start_game(GameSettingsObserver& observer, istream& input);
public:
	GameSettingsClient(Client& client, GameSettingsObserver& observer);
	
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
	
	void handle_messages();

	bool handle_message(const string& message);
};

#endif
