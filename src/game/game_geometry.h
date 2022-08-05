#ifndef _GAME_GEOMETRY_H
#define _GAME_GEOMETRY_H

#include "game_data.h"
#include "collision_grid.h"

collision_grid create_collision_grid(const BoardSize& board);
void clear_collision_grid(collision_grid& grid);

void add_to_grid(
	const BoardSize& board,
	const PlayerPosition& last_position,
	const PlayerPosition& position,
	collision_grid& grid,
	int index
);

bool check_curve_collision(  // Return whether further checking is possible.
	const BoardSize& board,
	const PlayerPosition& last_position,
	PlayerPosition& position,
	const vector<PlayerPosition>& history,
	const collision_grid& grid,
	bool is_self
);

bool check_border_collision(  // Return whether further checking is possible.
	const BoardSize& board,
	const PlayerPosition& last_position,
	PlayerPosition& position	
);

bool check_powerup_collision(
	const BoardSize& board,
	const PlayerPosition& last_position,
	const PlayerPosition& position,
	const PowerUp& powerup
);

#endif
