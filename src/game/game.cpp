#include "game.h"

#include "game_logic.h"
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
	game.player_active[player] = active;
}


Game::Game(const BoardSize& board, int player_num, set<GameObserver*>&& observers) :
	board(board),
	round_num(-1),
	histories(player_num, vector<PlayerPosition>()),
	states(player_num, PlayerState(0, 0)),
	pending_inputs(player_num, deque<int>()),
	player_active(player_num, true),
	cheese_makers(player_num, CheeseMaker()),
	observers(observers) {
		
	for(int i = 0; i < player_num; i++) interfaces.push_back(GamePlayerInterface(*this, i));

	new_round();
}

#define MARGIN 20.0

double random_number(double start, double end){
	return start + (rand() * (end - start) / RAND_MAX);
}

void Game::new_round(){
	round_num += 1;
	
	starting_timer = 0;
		
	powerups.clear();
	powerup_effects.clear();
	
	vector<PlayerPosition> positions;
	
	for(auto& player_history: histories){
		player_history.clear();
		player_history.push_back(PlayerPosition(
			random_number(MARGIN, board.w - MARGIN),  // x
			random_number(MARGIN, board.h - MARGIN),  // y
			0,  // size
			true,  // hovering
			true  // alive
		));
		positions.push_back(player_history.back());
	}
	
	for(auto& state: states){
		state.turn_state = 0;
		state.direction = random_number(0, 2 * M_PI);
	}
	
	for(auto observer: observers){
		observer->new_round(round_num);
		observer->update(positions, states);
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
	
	observer->init(histories);
	
	observers.insert(observer);
}
void Game::remove_observer(GameObserver* observer){
	observers.erase(observer);
}

PlayerInterface& Game::get_player_interface(int player){
	return interfaces[player];
}

int Game::get_round() const{
	return round_num;
}
const vector<int>& Game::get_scores() const{
	return scores;
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

bool Game::can_step(){
	for(int i = 0; i < player_active.size(); i++){
		if(player_active[i] && pending_inputs[i].empty()) return false;
	}
	
	return true;
}

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
			histories[player].back(),
			states[player], turn_state,
			powerup_effects
		));
		
		histories[player].back().hovering |= cheese_makers[player].step();
	}
	
	// Check for collisions
	for(int player = 0; player < states.size(); player++){
		if(!histories[player].back().alive) continue;  // Do not check the dead.
		if(histories[player].back().hovering) continue;  // Cannot collide wile hovering.

		double radius = get_player_size(histories[player].back().size) / 2;
		double x = histories[player][histories[player].size()-2].x;
		double y = histories[player][histories[player].size()-2].y;
		
		
		bool at_begining = false;
		if(leaving_box(
			radius, radius, board.w - radius, board.h-radius,
			x, y,
			histories[player].back().x, histories[player].back().y,
			histories[player].back().x, histories[player].back().y,
			at_begining
		)) histories[player].back().alive = false;
				
		for(int other_player = 0; other_player < states.size(); other_player++){
			bool start_colliding = other_player != player;  // If same player we need a special case for the end of the history.
			for(int i = histories[other_player].size() - 1; i > 0; i--){
				double other_width = get_player_size(histories[other_player][i].size);
				
				if(!start_colliding){  // Still at end of this player's curve.
					if(distance(
						histories[player].back().x, histories[player].back().y,
						histories[other_player][i].x, histories[other_player][i].y
					) < radius + other_width/2 ||
					distance(
						x, y, histories[other_player][i].x, histories[other_player][i].y
					) < radius + other_width/2){
						continue;
					}
					
					start_colliding = true;
				}
				
				if(histories[other_player][i].hovering) continue;
				
				if(find_line_collision(
					histories[other_player][i].x, histories[other_player][i].y,
					histories[other_player][i-1].x, histories[other_player][i-1].y,
					other_width,
					x, y,
					histories[player].back().x, histories[player].back().y,
					radius,
					histories[player].back().x, histories[player].back().y,
					at_begining
				)) histories[player].back().alive = false;

				if(at_begining) break;
				
				if(i != histories[other_player].size() - 1 && !histories[other_player][i + 1].hovering){
					other_width = get_player_size(min(
						histories[other_player][i].size,
						histories[other_player][i+1].size
					));
					
					if(find_circle_collision(
						histories[other_player][i].x, histories[other_player][i].y,
						x, y,
						histories[player].back().x, histories[player].back().y,
						radius + other_width/2,
						histories[player].back().x, histories[player].back().y,
						at_begining
					)) histories[player].back().alive = false;
					
					if(at_begining) break;
				}
			}
			if(at_begining) break;
		}
		if(at_begining) histories[player].back().hovering = true;
	}

	// Update observers
	vector<PlayerPosition> positions;
	for(const auto& player_history: histories) positions.push_back(player_history.back());
	
	for(auto observer: observers) observer->update(positions, states);

	// Update timer
	starting_timer++;
	
}
