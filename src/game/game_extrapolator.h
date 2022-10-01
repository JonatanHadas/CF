#ifndef _GAME_EXTRAPOLATOR_H
#define _GAME_EXTRAPOLATOR_H

#include "game_observer.h"
#include "game_view.h"
#include "game_advancer.h"

#include "player_interface.h"

#include <vector>
#include <set>
#include <map>
#include <deque>
#include <memory>

using namespace std;

class GameExtrapolator : public GameView, public GameObserver, public GameAdvancer {
	const BoardSize board;
	const ScoreSettings score_settings;
	
	int round_num;
	vector<int> scores;
	
	bool game_over;
	bool tie_break_round;
	
	vector<PlayerState> states, extrapolated_states;
	vector<vector<PlayerPosition>> histories;
	int extrapolated_size, extrapolation_limit;
	vector<deque<int>> pending_inputs;
	vector<bool> player_active;
	bool history_exists;

	map<int, PowerUp> powerups;
	set<unique_ptr<PowerUpEffect>> powerup_effects;
	set<unique_ptr<PowerUpEffect>> extrapolated_powerup_effects;
	
	bool round_over;
	vector<int> round_winners;

	int round_timer;
	
	class ExtrapolatorPlayerInterface : public PlayerInterface{
		GameExtrapolator& game;
		int player;
	public:
		explicit ExtrapolatorPlayerInterface(GameExtrapolator& game, int player);

		void step(int round, int turn_state);
		void set_active(bool active);
	};

	vector<ExtrapolatorPlayerInterface> interfaces;
	
	bool can_step();
	void step();
	
	void clear_extrapolation();

	bool check_tie_break() const;
	bool check_over() const;
public:
	GameExtrapolator(
		const BoardSize& board,
		const ScoreSettings& score_settings,
		int player_num, int team_num
	);

	PlayerInterface& get_player_interface(int player);

	// view
	const BoardSize& get_board_size() const;

	int get_round() const;
	const vector<int>& get_scores() const;
	bool is_over() const;
	bool is_tie_break() const;

	bool is_round_over() const;
	const vector<int>& get_round_winners() const;

	const vector<PlayerState>& get_states() const;
	const vector<vector<PlayerPosition>>& get_histories() const;

	const map<int, PowerUp>& get_powerups() const;
	const set<unique_ptr<PowerUpEffect>>& get_powerup_effects() const;

	// Observer
	void init(const vector<vector<PlayerPosition>>& histories, int round_timer);
	
	void new_round(int round);
	void update_scores(const vector<int>& addition);
	void set_winners(const vector<int>& winners);

	void update(const vector<PlayerPosition>& positions, const vector<PlayerState>& states);
	
	void spawn_powerup(int id, const PowerUp& power_up);
	void activate_powerup(int id, const PowerUpEffect& effect);
	
	// Advancer
	void advance();
	void allow_step();
};

#endif
