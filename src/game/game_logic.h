#ifndef _GAME_LOGIC_H
#define _GAME_LOGIC_H

#include "game_data.h"

#include <set>
#include <functional>
#include <memory>

using namespace std;

void applying_to_player(const set<unique_ptr<PowerUpEffect>>& effects, int player, function<void(const PowerUpEffect&)> todo);

int count_powerups(int player, PowerUpType type, const set<unique_ptr<PowerUpEffect>>& effects);

double apply_multiplier(double start, double multiplier, int amount);

PlayerPosition advance_player(
	int player, const BoardSize& board, int starting_timer,
	const PlayerPosition& position,
	PlayerState& state, int turn_state,
	const set<unique_ptr<PowerUpEffect>>& effects
);

#endif
