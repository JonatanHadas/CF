#ifndef _GAME_H
#define _GAME_H

#include "game_data.h"
#include "game_view.h"
#include "game_observer.h"
#include "game_advancer.h"
#include "player_interface.h"
#include "collision_grid.h"
#include "powerups.h"

#include "cheese_maker.h"

#include <vector>
#include <deque>
#include <set>
#include <map>
#include <memory>

using namespace std;

class Game : public GameView, public GameAdvancer{
	const BoardSize board;
	const ScoreSettings score_settings;

	int round_num;
	int starting_timer;
	int end_timer;
	
	bool tie_break_round;
	bool game_over;

	vector<vector<PlayerPosition>> histories;
	vector<PlayerState> states;
	vector<deque<int>> pending_inputs;
	vector<bool> player_active;

	map<int, PowerUp> powerups;
	set<unique_ptr<PowerUpEffect>> powerup_effects;

	set<GameObserver*> observers;

	vector<int> scores;
	vector<int> teams;
	vector<bool> alive;
	
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
	
	vector<collision_grid> collision_grids;
	
	vector<PowerUpDescriptor> allowed_powerups;
	set<unique_ptr<PowerUpSpawner>> spawners;
	int next_powerup_id;
	
	bool check_tie_break() const;
	bool check_over() const;
	void new_round();
	
	bool can_step();
	void step();
public:
	Game(
		const BoardSize& board,
		const ScoreSettings& score_settings,
		int team_num, const vector<int>& teams,
		const set<PowerUpDescriptor>& allowed_powerups,
		set<GameObserver*>&& observers
	);

	void add_observer(GameObserver* observer);
	void remove_observer(GameObserver* observer);

	PlayerInterface& get_player_interface(int player);

	const BoardSize& get_board_size() const;

	int get_round() const;
	const vector<int>& get_scores() const;

	const vector<PlayerState>& get_states() const;
	const vector<vector<PlayerPosition>>& get_histories() const;

	const map<int, PowerUp>& get_powerups() const;
	const set<unique_ptr<PowerUpEffect>>& get_powerup_effects() const;

	void advance();
	void allow_step();
	
	bool is_over() const;
	bool is_tie_break() const;
};

#endif
