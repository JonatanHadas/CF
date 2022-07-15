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

#define MARGIN 0.5

#define STARTING_HOVER 60

PlayerPosition advance_player(
	int player, const BoardSize& board, int starting_timer,
	const PlayerPosition& position,
	PlayerState& state, int turn_state,
	const set<unique_ptr<PowerUpEffect>>& effects
){
	int real_turn_state = turn_state;
	if(count_powerups(player, PowerUpType::INVERT, effects) % 2) real_turn_state = -turn_state;

	if(count_powerups(player, PowerUpType::RIGHT_TURN, effects)){
		if(turn_state && !state.turn_state) state.direction += M_PI_2 * real_turn_state;
	}
	else{
		int turn_size = count_powerups(player, PowerUpType::NARROW_TURN, effects) - 
					    count_powerups(player, PowerUpType::WIDE_TURN, effects);

		state.direction += real_turn_state * apply_multiplier(TURN, TURN_MULTIPLIER, turn_size);
	}
	
	while(state.direction < 0) state.direction += (2 * M_PI);
	while(state.direction > (2 * M_PI)) state.direction -= (2 * M_PI);
	
	state.turn_state = turn_state;
	
	int speed_size = count_powerups(player, PowerUpType::SPEED_UP, effects) - 
					 count_powerups(player, PowerUpType::SLOW_DOWN, effects);
	double speed = apply_multiplier(SPEED, SPEED_MULTIPLIER, speed_size);
	
	int size = count_powerups(player, PowerUpType::THICKEN, effects) -
			   count_powerups(player, PowerUpType::NARROW, effects);
			   
	double x = position.x + (speed * cos(state.direction));
	double y = position.y + (speed * sin(state.direction));
	
	bool warp = false;
	if(count_powerups(player, PowerUpType::WARP_AROUND, effects)){
		if(x < -MARGIN){
			x += board.w + 2*MARGIN;
			warp = true;
		}
		if(x > board.w + MARGIN){
			x -= board.w + 2*MARGIN;
			warp = true;
		}

		if(y < -MARGIN){
			y += board.h + 2*MARGIN;
			warp = true;
		}
		if(y > board.h + MARGIN){
			y -= board.h + 2*MARGIN;
			warp = true;
		}
	}
	
	return PlayerPosition(
		x, y,
		size,
		(count_powerups(player, PowerUpType::HOVER, effects) || warp) && (starting_timer > STARTING_HOVER),
		position.alive
	);
}


#define WIDTH 1.2
#define WIDTH_MULTIPLIER 2.0

double get_player_size(int size){
	return apply_multiplier(WIDTH, WIDTH_MULTIPLIER, size);
}
