#ifndef _LOCAL_GAME_CREATOR_H
#define _LOCAL_GAME_CREATOR_H

#include "game_creator.h"
#include "game_settings_manager.h"
#include "game.h"

class LocalGame : public CreatedGame {
	Game game;
	vector<PlayerInterface*> interfaces;
public:
	LocalGame(
		const GameSettings& settings,
		const vector<int>& players
	);

	GameView* get_view();
	const vector<PlayerInterface*>& get_interfaces() const;
	GameAdvancer* get_advancer();
};

class LocalGameCreator : public GameCreator {
	GameSettingsManager manager;
	GameSettingsManager::Peer* peer;
public:
	LocalGameCreator();

	GameSettingsView* get_view();
	GameSettingsManipulator* get_manipulator();
	GameSettingsObserverAccumulator* get_accumulator();
	
	bool is_multi_peer() const;
	
	bool update();
	
	unique_ptr<CreatedGame> create_game();
};

#endif
