#include "game_extrapolator.h"

#include "game_logic.h"

GameExtrapolator::ExtrapolatorPlayerInterface::ExtrapolatorPlayerInterface(GameExtrapolator& game, int player) :
	game(game), player(player) {}

void GameExtrapolator::ExtrapolatorPlayerInterface::step(int round, int turn_state){
	if(game.round_num != round) return;
	if(turn_state > 0) turn_state = 1;
	if(turn_state < 0) turn_state = -1;
	game.pending_inputs[player].push_back(turn_state);
}

void GameExtrapolator::ExtrapolatorPlayerInterface::set_active(bool active){
	game.player_active[player] = active;
}

GameExtrapolator::GameExtrapolator(
	const BoardSize& board,
	const ScoreSettings& score_settings,
	int player_num, int team_num
) : 
	board(board),
	score_settings(score_settings),
	round_num(-1),
	round_timer(-1),
	scores(team_num, 0),
	game_over(false), tie_break_round(false), round_over(false),
	states(player_num, PlayerState(0)),
	extrapolated_states(player_num, PlayerState(0)),
	histories(player_num, vector<PlayerPosition>()),
	extrapolated_size(0),
	extrapolation_limit(0),
	history_exists(false),
	pending_inputs(player_num, deque<int>()),
	player_active(player_num, true)
	{

	for(int i = 0; i < player_num; i++) interfaces.push_back(ExtrapolatorPlayerInterface(*this, i));
}	

bool GameExtrapolator::can_step(){
	if(game_over) return false;
	if(!history_exists) return false;
	if(extrapolated_size >= extrapolation_limit) return false;
	
	for(int i = 0; i < player_active.size(); i++){
		if(player_active[i] && pending_inputs[i].size() <= extrapolated_size) return false;
	}
	
	return true;
}

void GameExtrapolator::step(){
	// Advance players
	for(int player = 0; player < states.size(); player++){
		int turn_state = 0;
		if(pending_inputs[player].size() > extrapolated_size) {
			turn_state = pending_inputs[player][extrapolated_size];
		}
		
		if(!histories[player].back().alive) continue;  // Do not advance the dead.
		
		histories[player].push_back(advance_player(
			player, board, round_timer + extrapolated_size,
			histories[player].back(),
			states[player], turn_state,
			extrapolated_powerup_effects
		));
	}

	count_down_powerups(extrapolated_powerup_effects);

	// Update timer
	extrapolated_size++;
}

void GameExtrapolator::clear_extrapolation(){
	for(auto& history: histories){
		for(int i = 0; i < extrapolated_size; i++){
			if(history.back().alive) history.pop_back();
		}
	}
	
	extrapolated_states = states;
	extrapolated_powerup_effects.clear();
	for(const auto& effect: powerup_effects){
		extrapolated_powerup_effects.insert(make_unique<PowerUpEffect>(*effect));
	}
	
	extrapolated_size = 0;
}

bool GameExtrapolator::check_over() const{
	if(scores.size() <= 1) return false;

	return check_end_condition(score_settings, scores, round_num) && !check_tie(score_settings, scores);
}

bool GameExtrapolator::check_tie_break() const{
	if(scores.size() <= 1) return false;

	return check_end_condition(score_settings, scores, round_num) && check_tie(score_settings, scores);
}

PlayerInterface& GameExtrapolator::get_player_interface(int player){
	return interfaces[player];
}

// view
const BoardSize& GameExtrapolator::get_board_size() const {
	return board;
}

int GameExtrapolator::get_round() const {
	return round_num;
}

const vector<int>& GameExtrapolator::get_scores() const {
	return scores;
}

bool GameExtrapolator::is_over() const {
	return game_over;
}

bool GameExtrapolator::is_tie_break() const {
	return tie_break_round;
}

bool GameExtrapolator::is_round_over() const {
	return round_over;
}

const vector<int>& GameExtrapolator::get_round_winners() const {
	return round_winners;
}

const vector<PlayerState>& GameExtrapolator::get_states() const {
	return extrapolated_states;
}

const vector<vector<PlayerPosition>>& GameExtrapolator::get_histories() const {
	return histories;
}

const map<int, PowerUp>& GameExtrapolator::get_powerups() const {
	return powerups;
}

const set<unique_ptr<PowerUpEffect>>& GameExtrapolator::get_powerup_effects() const {
	return extrapolated_powerup_effects;
}

// Observer
void GameExtrapolator::init(const vector<vector<PlayerPosition>>& histories, int round_timer){
	extrapolated_size = 0;
	extrapolation_limit = 0;
	this->histories = histories;
	this->round_timer = round_timer;
}

void GameExtrapolator::new_round(int round){
	round_num = round;
	round_timer = 0;
	
	powerups.clear();
	powerup_effects.clear();
	extrapolated_powerup_effects.clear();
	
	for(auto& history: histories) history.clear();
	extrapolated_size = 0;
	extrapolation_limit = 0;
	history_exists = false;
	
	for(auto& inputs: pending_inputs) inputs.clear();
	
	tie_break_round |= check_tie_break();
	game_over |= check_over();
	
	round_over = false;
}

void GameExtrapolator::update_scores(const vector<int>& addition){
	scores = addition;
}

void GameExtrapolator::set_winners(const vector<int>& winners){
	round_over = true;
	round_winners = winners;
}

void GameExtrapolator::update(const vector<PlayerPosition>& positions, const vector<PlayerState>& states){
	count_down_powerups(powerup_effects);

	clear_extrapolation();
	
	for(int i = 0; i < positions.size(); i++){
		histories[i].push_back(positions[i]);
	}
	this->states = states;
	history_exists = true;
	
	extrapolation_limit--;
	
	for(auto& inputs: pending_inputs){
		if(inputs.size()) inputs.pop_front();
	}
	
	round_timer++;
}

void GameExtrapolator::spawn_powerup(int id, const PowerUp& power_up){
	powerups.insert({id, power_up});
}

void GameExtrapolator::activate_powerup(int id, const PowerUpEffect& effect){
	powerups.erase(id);
	
	powerup_effects.insert(make_unique<PowerUpEffect>(effect));
	clear_extrapolation();
	
	switch(effect.desc.type){
	case PowerUpType::ERASER:
		apply_to_players(effect.desc.affects, effect.player, states.size(), [&](int player){
			histories[player].clear();
		});
		history_exists = false;
		break;
	}
}

// Advancer
void GameExtrapolator::advance(){
	while(can_step()) step();
}

void GameExtrapolator::allow_step(){
	extrapolation_limit++;
}
