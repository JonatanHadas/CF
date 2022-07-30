#include "game_logic.h"

#define _USE_MATH_DEFINES
#include <math.h>


void applying_to_player(const set<unique_ptr<PowerUpEffect>>& effects, int player, function<void(const PowerUpEffect&)> todo){
	for(const unique_ptr<PowerUpEffect>& effect: effects){
		if((player != effect->player) ^ effect->affects_taker){
			todo(*effect);
		}
	}
}

int count_powerups(int player, PowerUpType type, const set<unique_ptr<PowerUpEffect>>& effects) {
	int count = 0;
	applying_to_player(effects, player, [type, &count](const PowerUpEffect& effect) {
		if(effect.type == type) count++;
	});
	
	return count;
}

double apply_multiplier(double start, double multiplier, int amount){
	for(; amount > 0; --amount) start *= multiplier;
	for(; amount < 0; ++amount) start /= multiplier;
	return start;
}


#define SPEED (0.3)
#define SPEED_MULTIPLIER 1.5

#define TURN (M_PI / 60)
#define TURN_MULTIPLIER 1.5

#define STARTING_HOVER 60

PlayerPosition advance_player(
	int player, const BoardSize& board, int starting_timer,
	const PlayerPosition& position,
	PlayerState& state, int turn_state,
	const set<unique_ptr<PowerUpEffect>>& effects
){
	int real_turn_state = turn_state;
	if(count_powerups(player, PowerUpType::INVERT, effects) % 2) real_turn_state = -turn_state;

	double turn = 0;
	bool corner = false;
	double direction = position.direction;
	if(count_powerups(player, PowerUpType::RIGHT_TURN, effects)){
		if(turn_state && !state.turn_state){
			direction += M_PI_2 * real_turn_state;
			corner = true;
		}
	}
	else{
		int turn_size = count_powerups(player, PowerUpType::NARROW_TURN, effects) - 
					    count_powerups(player, PowerUpType::WIDE_TURN, effects);
						
		turn = real_turn_state * apply_multiplier(TURN, TURN_MULTIPLIER, turn_size) / 2;

		direction += turn;
	}
	
	while(direction < -M_PI) direction += (2 * M_PI);
	while(direction > M_PI) direction -= (2 * M_PI);
	
	state.turn_state = turn_state;
	
	int speed_size = count_powerups(player, PowerUpType::SPEED_UP, effects) - 
					 count_powerups(player, PowerUpType::SLOW_DOWN, effects);
	double speed = apply_multiplier(SPEED, SPEED_MULTIPLIER, speed_size);
	
	int size = count_powerups(player, PowerUpType::THICKEN, effects) -
			   count_powerups(player, PowerUpType::NARROW, effects);
	double radius = get_player_size(size) / 2;
			   
	double x = position.x + (speed * cos(direction));
	double y = position.y + (speed * sin(direction));
	
	direction += turn;
	
	int warp_x = 0, warp_y = 0;
	bool warping_x = false, warping_y = false;
	if(count_powerups(player, PowerUpType::WARP_AROUND, effects)){
		if(x < radius){
			warp_x = 1;
		}
		else if(x > board.w - radius){
			warp_x = -1;
		}
		if(x < 0 || x > board.w){
			x += board.w * warp_x;
			warp_x = -warp_x;
			warping_x = true;
		}

		if(y < radius){
			warp_y = 1;
		}
		else if(y > board.h - radius){
			warp_y = -1;
		}
		if(y < 0 || y > board.h){
			y += board.h * warp_y;
			warp_y = -warp_y;
			warping_y = true;
		}
	}
	
	return PlayerPosition(
		x, y, direction,
		size,
		warp_x, warp_y,
		warping_x, warping_y,
		corner,
		count_powerups(player, PowerUpType::HOVER, effects) || (starting_timer < STARTING_HOVER),
		position.alive
	);
}


#define WIDTH 1.2
#define WIDTH_MULTIPLIER 2.0

double get_player_size(int size){
	return apply_multiplier(WIDTH, WIDTH_MULTIPLIER, size);
}

void count_down_powerups(set<unique_ptr<PowerUpEffect>>& effects){
	for(auto it = effects.begin(); it != effects.end();){
		(*it)->timer--;
		if((*it)->timer == 0) effects.erase(it++);
		else ++it;
	}
}