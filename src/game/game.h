#ifndef _GAME_H
#define _GAME_H

#include "game_data.h"
#include "game_view.h"
#include "game_observer.h"
#include "player_interface.h"

#include "cheese_maker.h"

#include <vector>
#include <deque>
#include <set>
#include <map>
#include <memory>

using namespace std;

class Game : public GameView{
	const BoardSize board;

	int round_num;
	int starting_timer;

	vector<vector<PlayerPosition>> histories;
	vector<PlayerState> states;
	vector<deque<int>> pending_inputs;
	vector<bool> player_active;

	map<int, PowerUp> powerups;
	set<unique_ptr<PowerUpEffect>> powerup_effects;

	set<GameObserver*> observers;

	vector<int> scores;
	
	vector<CheeseMaker> cheese_makers;

	class GamePlayerInterface : public PlayerInterface{
		Game& game;
		int player;
	public:
		explicit GamePlayerInterface(Game& game, int player);

		void step(int round, int turn_state);
		void set_active(bool active);
	};
	
	vector<GamePlayerInterface> interfaces;
	
	void new_round();
	
	bool can_step();
	void step();
public:
	Game(const BoardSize& board, int player_num, set<GameObserver*>&& observers);

	void add_observer(GameObserver* observer);
	void remove_observer(GameObserver* observer);

	PlayerInterface& get_player_interface(int player);

	int get_round() const;
	const vector<int>& get_scores() const;

	const vector<PlayerState>& get_states() const;
	const vector<vector<PlayerPosition>>& get_histories() const;

	const map<int, PowerUp>& get_powerups() const;
	const set<unique_ptr<PowerUpEffect>>& get_powerup_effects() const;

	void advance();
};

#endif
