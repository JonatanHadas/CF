#ifndef _GAME_LOGIC_H
#define _GAME_LOGIC_H

#include "game_data.h"

#include <set>
#include <functional>
#include <memory>

using namespace std;

#define POWERUP_RADIUS 3.5

void applying_to_player(const set<unique_ptr<PowerUpEffect>>& effects, int player, function<void(const PowerUpEffect&)> todo);

void apply_to_players(PowerUpAffects affects, int player, int player_num, function<void(int)> todo);

int count_powerups(int player, PowerUpType type, const set<unique_ptr<PowerUpEffect>>& effects);

int count_self_powerups(int player, PowerUpType type, const set<unique_ptr<PowerUpEffect>>& effects);

int count_others_powerups(int player, PowerUpType type, const set<unique_ptr<PowerUpEffect>>& effects);

double apply_multiplier(double start, double multiplier, int amount);

PlayerPosition advance_player(
	int player, const BoardSize& board, int starting_timer,
	const PlayerPosition& position,
	PlayerState& state, int turn_state,
	const set<unique_ptr<PowerUpEffect>>& effects
);

double get_player_size(int size);

void count_down_powerups(set<unique_ptr<PowerUpEffect>>& effects);

#endif
