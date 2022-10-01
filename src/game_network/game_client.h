#ifndef _GAME_CLIENT_H
#define _GAME_CLIENT_H

#include "../network/client.h"

#include "../game/game_observer.h"
#include "../game/player_interface.h"

#include <vector>
#include <functional>

using namespace std;

class GameClientPlayer : public PlayerInterface {
	Client& client;
	int index;

	void send(const string& message);	
public:
	GameClientPlayer(Client& client, int index);
	
	void step(int round, int turn_state);
	void set_active(bool is_active);
};

class GameClient{
	Client& client;
	GameObserver& observer;
	
	function<void(const string&)> handle_settings_message;
	
	vector<GameClientPlayer> players;
	vector<PlayerInterface*> player_ptrs;

	bool handle_message(const string& message);

    static void init(GameObserver& observer, istream& input);

    static void new_round(GameObserver& observer, istream& input);
    static void update_scores(GameObserver& observer, istream& input);
	static void set_winners(GameObserver& observer, istream& input);

    static void update(GameObserver& observer, istream& input);

    static void spawn_powerup(GameObserver& observer, istream& input);
    static void activate_powerup(GameObserver& observer, istream& input);
public:
	GameClient(
		Client& client,
		GameObserver& observer,
		int players,
		function<void(const string&)> handle_settings_message
		);
	
	const vector<PlayerInterface*>& get_players();

	bool handle_messages();
};

#endif
