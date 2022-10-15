#include "game.h"

#include "game_logic.h"
#include "game_geometry.h"
#include "../utils/geometry.h"

#include <stdlib.h>

Game::GamePlayerInterface::GamePlayerInterface(Game& game, int player) :
	game(game),
	player(player) {}

void Game::GamePlayerInterface::step(int round, int turn_state){
	if(game.round_num != round) return;
	if(turn_state > 0) turn_state = 1;
	if(turn_state < 0) turn_state = -1;
	game.pending_inputs[player].push_back(turn_state);
}
void Game::GamePlayerInterface::set_active(bool active){
	game.states[player].active = active;
}

Game::Game(
	const BoardSize& board,
	const ScoreSettings& score_settings,
	int team_num, const vector<int>& teams,
	const set<PowerUpDescriptor>& allowed_powerups,
	set<GameObserver*>&& observers
) :
	board(board),
	score_settings(score_settings),
	round_num(-1),
	tie_break_round(false),
	game_over(false),
	histories(teams.size(), vector<PlayerPosition>()),
	states(teams.size(), PlayerState(0, 0, true)),
	pending_inputs(teams.size(), deque<int>()),
	cheese_makers(teams.size(), CheeseMaker()),
	collision_grids(teams.size(), create_collision_grid(board)),
	teams(teams),
	scores(team_num, 0),
	alive(team_num, false),
	allowed_powerups(allowed_powerups.begin(), allowed_powerups.end()),
	observers(observers) {
		
	for(int i = 0; i < teams.size(); i++) interfaces.push_back(GamePlayerInterface(*this, i));

	new_round();
}

#define MARGIN 20.0

double random_number(double start, double end){
	return start + (rand() * (end - start) / RAND_MAX);
}

void Game::new_round(){
	round_num += 1;
	
	starting_timer = 0;
	end_timer = -1;
		
	powerups.clear();
	powerup_effects.clear();
	
	spawners.clear();
	spawners.insert(make_unique<PowerUpSpawner>(false));
	
	vector<PlayerPosition> positions;
	
	alive = vector<bool>(true, scores.size());
	
	for(auto& player_history: histories){
		player_history.clear();
		player_history.push_back(PlayerPosition(
			random_number(MARGIN, board.w - MARGIN),  // x
			random_number(MARGIN, board.h - MARGIN),  // y
			random_number(0, 2 * M_PI)  // direction
		));
		positions.push_back(player_history.back());
	}
	for(auto& grid: collision_grids){
		clear_collision_grid(grid);
	}
	
	for(auto& state: states){
		state.turn_state = 0;
	}
	for(auto& inputs: pending_inputs){
		inputs.clear();
	}
	
	tie_break_round |= check_tie_break();
	game_over |= check_over();
	
	for(auto observer: observers){
		observer->new_round(round_num);
		if(!game_over) observer->update(positions, states);
	}
}

void Game::add_observer(GameObserver* observer){
	observer->new_round(round_num);
	observer->update_scores(scores);
	
	vector<PlayerPosition> positions;
	for(auto player_history: histories) positions.push_back(player_history.back());
	
	observer->update(positions, states);
	
	for(auto powerup: powerups) observer->spawn_powerup(powerup.first, powerup.second);
	for(auto& effect: powerup_effects) observer->activate_powerup(-1, *effect);
	
	observer->init(histories, starting_timer);
	
	observers.insert(observer);
}
void Game::remove_observer(GameObserver* observer){
	observers.erase(observer);
}

PlayerInterface& Game::get_player_interface(int player){
	return interfaces[player];
}

const BoardSize& Game::get_board_size() const {
	return board;
}

int Game::get_round() const{
	return round_num;
}
const vector<int>& Game::get_scores() const{
	return scores;
}

bool Game::is_round_over() const{
	return end_timer >= 0;
}

const vector<int>& Game::get_round_winners() const {
	return round_winners;
}

const vector<PlayerState>& Game::get_states() const{
	return states;
}
const vector<vector<PlayerPosition>>& Game::get_histories() const{
	return histories;
}

const map<int, PowerUp>& Game::get_powerups() const{
	return powerups;
}
const set<unique_ptr<PowerUpEffect>>& Game::get_powerup_effects() const{
	return powerup_effects;
}

void Game::advance(){
	while(can_step()) step();
}

void Game::allow_step(){}

bool Game::can_step(){
	if(game_over) return false;

	for(int i = 0; i < states.size(); i++){
		if(states[i].active && pending_inputs[i].empty()) return false;
	}
	
	return true;
}

#define END_TIMER 300

void Game::step(){
	// Advance players
	for(int player = 0; player < states.size(); player++){
		int turn_state = 0;
		if(!pending_inputs[player].empty()) {
			turn_state = pending_inputs[player].front();
			pending_inputs[player].pop_front();
		}
		
		if(!histories[player].back().alive) continue;  // Do not advance the dead.
		
		histories[player].push_back(advance_player(
			player, board, starting_timer,
			teams,
			histories[player].back(),
			states[player], turn_state,
			powerup_effects
		));
		
		histories[player].back().hovering |= cheese_makers[player].step();
		add_to_grid(board, histories[player][histories[player].size() - 2], histories[player].back(), collision_grids[player], histories[player].size() - 1);
	}
	
	// Check for collisions
	for(int player = 0; player < states.size(); player++){
		if(!histories[player].back().alive) continue;  // Do not check the dead.
		if(histories[player].back().hovering) continue;  // Cannot collide wile hovering.		
		
		if(count_powerups(player, teams, PowerUpType::WARP_AROUND, powerup_effects) == 0){
			if(check_border_collision(
				board,
				histories[player][histories[player].size()-2],
				histories[player].back()
			)) continue;
		}
		
		for(int other_player = 0; other_player < states.size(); other_player++){
			if(!check_curve_collision(
				board,
				histories[player][histories[player].size()-2],
				histories[player].back(),
				histories[other_player],
				collision_grids[other_player],
				other_player == player
			)) break;
		}
		
		if(!histories[player].back().alive){
			for(auto listener: get_listeners()) listener->kill_player(player);
		}
	}
	
	// Update scores
	vector<bool> new_alive(scores.size(), false);
	for(int player = 0; player < teams.size(); player++){
		if(histories[player].back().alive){
			new_alive[teams[player]] = true;
		}
	}
	
	vector<int> newly_dead;
	for(int team = 0; team < scores.size(); team++){
		if(alive[team] && !new_alive[team]) newly_dead.push_back(team);
		alive[team] = new_alive[team];
	}
	if(newly_dead.size()){
		vector<int> living;
		for(int team = 0; team < scores.size(); team++){
			if(alive[team]){
				scores[team] += newly_dead.size();
				living.push_back(team);
			}
		}

		if(living.size() <= 1 && end_timer < 0){
			if(living.size()) round_winners = living;
			else round_winners = newly_dead;
			end_timer = END_TIMER;
			
			for(auto observer: observers) observer->set_winners(round_winners);
		}
	}

	for(auto observer: observers) observer->update_scores(scores);
	
	set<int> erased;
	
	// Take powerups
	for(int player = 0; player < histories.size(); player++){
		if(!histories[player].back().alive) continue;  // Do not check the dead.
		for(auto it = powerups.begin(); it != powerups.end();){
			if(check_powerup_collision(board, histories[player][histories[player].size() - 2], histories[player].back(), it->second)){
				auto effect = make_unique<PowerUpEffect>(
					powerup_times[it->second.desc],
					it->second.desc,
					player
				);
				
				switch(it->second.desc.type){
					case PowerUpType::ERASER:
						apply_to_players(it->second.desc.affects, player, teams, [&](int player){
							erased.insert(player);
						});
						break;
					case PowerUpType::SPAWN_POWERUPS:
						spawners.insert(make_unique<PowerUpSpawner>(true));
						break;
				}
				
				for(auto observer: observers) observer->activate_powerup(it->first, *effect);
				for(auto listener: get_listeners()) listener->activate_powerup(it->second);
				powerup_effects.insert(move(effect));
				
				powerups.erase(it++);
			}
			else ++it;
		}
	}
	
	// Erase
	for(auto player: erased){
		auto position = histories[player].back();
		histories[player].clear();
		histories[player].push_back(position);
		
		clear_collision_grid(collision_grids[player]);
	}

	count_down_powerups(powerup_effects);
	
	// Spawn powerups
	if(allowed_powerups.size()){
		for(auto it = spawners.begin(); it != spawners.end();){
			if((*it)->should_spawn()){
				int id = next_powerup_id++;
				PowerUp powerup(
					allowed_powerups[rand() % allowed_powerups.size()],
					random_number(POWERUP_RADIUS, board.w - POWERUP_RADIUS),
					random_number(POWERUP_RADIUS, board.h - POWERUP_RADIUS)
				);

				powerups.insert({id, powerup});
				for(auto observer: observers) observer->spawn_powerup(id, powerup);
				for(auto listener: get_listeners()) listener->spawn_powerup(powerup);
			}
			
			if((*it)->is_done()) spawners.erase(it++);
			else ++it;
		}
	}
	
	// Update observers
	vector<PlayerPosition> positions;
	for(const auto& player_history: histories) positions.push_back(player_history.back());
	
	for(auto observer: observers) observer->update(positions, states);

	// Update timer
	starting_timer++;

	if(end_timer > 0){
		end_timer--;
	}
	else if(end_timer == 0){
		new_round();
	}
}

bool Game::check_over() const {
	if(scores.size() <= 1) return false;

	return check_end_condition(score_settings, scores, round_num) && !check_tie(score_settings, scores);
}

bool Game::is_over() const {
	return game_over;
}

bool Game::check_tie_break() const {
	if(scores.size() <= 1) return false;

	return check_end_condition(score_settings, scores, round_num) && check_tie(score_settings, scores);
}

bool Game::is_tie_break() const {
	return tie_break_round;
}
