#include "game_geometry.h"

#include "../utils/geometry.h"
#include "game_logic.h"

#include <math.h>

bool check_curve_collision(
	const BoardSize& board,
	const PlayerPosition& last_position,
	PlayerPosition& position,
	const vector<PlayerPosition>& history,
	bool is_self
){
	bool at_begining = false;
	
	double radius = get_player_size(position.size) / 2;
	
	double prev_x = last_position.x;
	if(position.warping_x) prev_x -= board.w * position.warp_x;
	double prev_y = last_position.y;
	if(position.warping_y) prev_y -= board.h * position.warp_y;
	
	for(int i = history.size() - 1; i > 0; i--){
		double other_radius = get_player_size(history[i].size) / 2;
		
		if(is_self){
			if(
				distance(position.x, position.y, history[i].x, history[i].y) < radius + other_radius ||
				distance(last_position.x, last_position.y, history[i].x, history[i].y) < radius + other_radius
			){
				continue;  // Still at end of this player's curve.
			}
			
			is_self = false;
		}
		
		if(history[i].hovering) continue;
		
		double prev_direction = history[i].corner ? history[i].direction : history[i-1].direction;
		
		double other_prev_x = history[i-1].x;
		if(history[i].warping_x) other_prev_x -= board.w * history[i].warp_x;
		double other_prev_y = history[i-1].y;
		if(history[i].warping_y) other_prev_y -= board.h * history[i].warp_y;
		
		vector<double> poly_xs({
			other_prev_x + other_radius * sin(prev_direction),
			history[i].x + other_radius * sin(history[i].direction),
			history[i].x - other_radius * sin(history[i].direction),
			other_prev_x - other_radius * sin(prev_direction)
		});
		vector<double> poly_ys({
			other_prev_y - other_radius * cos(prev_direction),
			history[i].y - other_radius * cos(history[i].direction),
			history[i].y + other_radius * cos(history[i].direction),
			other_prev_y + other_radius * cos(prev_direction)
		});
		// Make sure it's convex
		if(
			(poly_xs[1] - poly_xs[0])*(history[i].x - other_prev_x) + (poly_ys[1] - poly_ys[0])*(history[i].y - other_prev_y) < 0
		){
			swap(poly_xs[0], poly_xs[1]);
			swap(poly_ys[0], poly_ys[1]);
		}
		if(
			(poly_xs[2] - poly_xs[3])*(history[i].x - other_prev_x) + (poly_ys[2] - poly_ys[3])*(history[i].y - other_prev_y) < 0
		){
			swap(poly_xs[2], poly_xs[3]);
			swap(poly_ys[2], poly_ys[3]);
		}
		
		for(int warp_x = min(position.warp_x, 0) - max(history[i].warp_x, 0); warp_x <= max(position.warp_x, 0) - min(history[i].warp_x, 0); warp_x++){
			for(int warp_y = min(position.warp_y, 0) - max(history[i].warp_y, 0); warp_y <= max(position.warp_y, 0) - min(history[i].warp_y, 0); warp_y++){
				position.x += board.w * warp_x;
				position.y += board.h * warp_y;
								
				if(find_convex_collision(
					poly_xs, poly_ys,
					prev_x + board.w * warp_x, prev_y + board.h * warp_y,
					position.x, position.y,
					radius,
					position.x, position.y,
					at_begining
				)) position.alive = false;
				
				if(!at_begining){
					if(history[i].corner && !history[i-1].hovering){
						if(find_circle_collision(
							other_prev_x, other_prev_y,
							prev_x + board.w * warp_x, prev_y + board.h * warp_y,
							position.x, position.y,
							radius + min(other_radius, get_player_size(history[i-1].size)/2),
							position.x, position.y,
							at_begining
						)) position.alive = false;
					}
				}
				
				position.x -= board.w * warp_x;
				position.y -= board.h * warp_y;
				
				if(at_begining){
					position.hovering = true;
					return false;
				}
			}
		}
	}
	
	return true;
}

bool check_border_collision(  // Return whether further checking is possible.
	const BoardSize& board,
	const PlayerPosition& last_position,
	PlayerPosition& position	
){
	if(position.warp_x || position.warp_y) return true;  // Warping means we do not collide with the border
	double radius = get_player_size(position.size) / 2;
	
	bool at_begining = false;
	
	if(leaving_box(
		radius, radius, board.w - radius, board.h-radius,
		last_position.x, last_position.y,
		position.x, position.y,
		position.x, position.y,
		at_begining
	)) position.alive = false;
	
	if(at_begining){
		position.hovering = true;
	}
	return at_begining;
}