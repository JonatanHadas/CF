#ifndef _REMOTE_GAME_CREATOR_H
#define _REMOTE_GAME_CREATOR_H

#include "game_settings_client.h"
#include "game_client.h"

#include "../game/game_creator.h"
#include "../game/observed_settings_view.h"
#include "../game/game_extrapolator.h"
#include "../game/composite_player_interface.h"

#include "../network/client.h"

class RemoteGameAdvancer : public GameAdvancer{
	GameClient& client;
	GameView& game;
	GameAdvancer& advancer;
public:
	RemoteGameAdvancer(
		GameClient& client,
		GameView& game,
		GameAdvancer& advancer
	);
	
	void advance();
	void allow_step();
};

class RemoteGame : public CreatedGame {
	GameExtrapolator game;
	GameClient client;
	RemoteGameAdvancer advancer;
	
	vector<CompositePlayerInterface> interfaces;
	vector<PlayerInterface*> interface_ptrs;
public:
	RemoteGame(
		Client& client,
		const GameSettingsView& settings,
		GameSettingsClient& settings_client
		);
	
	GameView* get_view();
	const vector<PlayerInterface*>& get_interfaces() const;
	GameAdvancer* get_advancer();
	GameEventListenerAccumulator* get_accumulator();
};

class RemoteGameCreator : public GameCreator {
	Client client;
	GameSettingsClient settings_client;
	ObservedSettingsView settings;
public:
	RemoteGameCreator(Client&& client);
	~RemoteGameCreator();

	GameSettingsView* get_view();
	GameSettingsManipulator* get_manipulator();
	GameSettingsObserverAccumulator* get_accumulator();
	
	bool is_multi_peer() const;
	
	bool update();
	
	unique_ptr<CreatedGame> create_game();
};

#endif
