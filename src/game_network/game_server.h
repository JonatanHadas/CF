#ifndef _GAME_SERVER_H
#define _GAME_SERVER_H

#include "../network/server.h"

#include "../game/game.h"
#include "../game/game_observer.h"
#include "../game/game_settings_view.h"

#include <map>
#include <vector>
#include <memory>
#include <functional>

using namespace std;

class GameServerObserver : public GameObserver {
	Peer& peer;

	void send(string message);
public:
	GameServerObserver(Peer& peer);

	void init(const vector<vector<PlayerPosition>>& histories, int round_timer);
	
	void new_round(int round);
	void update_scores(const vector<int>& addition);
	void set_winners(const vector<int>& winners);

	void update(const vector<PlayerPosition>& positions, const vector<PlayerState>& states);
	
	void spawn_powerup(int id, const PowerUp& power_up);
	void activate_powerup(int id, const PowerUpEffect& effect);
};

class GameServer{
	Game game;
	Server& server;
	function<void(const NetEvent&)> on_connect, on_disconnect;
	
	map<Peer*, vector<PlayerInterface*>> peer_players;
	map<Peer*, unique_ptr<GameServerObserver>> peer_observers;

	void handle_connection(const NetEvent& event);
	void handle_disconnection(const NetEvent& event);
	void handle_message(const NetEvent& event);
	
	void step(PlayerInterface* player, istream& input);
	void set_active(PlayerInterface* player, istream& input);
public:
	GameServer(
		const GameSettings& settings,
		Server& server,
		const map<Peer*, GameSettingsView*>& peers,
		function<void(const NetEvent&)> on_connect,
		function<void(const NetEvent&)> on_disconnect
	);
	
	void serve(int interval);
};

#endif
