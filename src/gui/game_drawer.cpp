#include "game_drawer.h"

#include "colors.h"

#include "../utils/geometry.h"
#include "../game/game_logic.h"

BoardDrawer::BoardDrawer(const BoardSize& board, GameView* view) :
	board(board),
	view(view),
	texture(nullptr) {}
	
#define DRAW_SCALE 8.0

void BoardDrawer::init(SDL_Renderer* renderer){
	if(nullptr == texture.get()){
		texture = make_unique<Texture>(renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			board.w * DRAW_SCALE, board.h * DRAW_SCALE
		);
	}
}

#define CIRCLE_RAD 15
#include <iostream>
void BoardDrawer::draw(SDL_Renderer* renderer){
	init(renderer);
	
	Texture circle_texture(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		CIRCLE_RAD * 2, CIRCLE_RAD * 2
	);
	
	circle_texture.do_with_texture(renderer, [&](){
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		draw_circle(renderer, CIRCLE_RAD, CIRCLE_RAD, CIRCLE_RAD);
	});
	SDL_SetTextureBlendMode(circle_texture.get(), SDL_BLENDMODE_BLEND);
	
	SDL_Rect dst;

	texture->do_with_texture(renderer, [&](){
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		
		for(int i = 0; i < view->get_histories().size(); i++){
			auto player_history = view->get_histories()[i];
			
			SDL_SetRenderDrawColor(renderer, 
				player_colors[i].r,
				player_colors[i].g,
				player_colors[i].b,
				player_colors[i].a
			);

			SDL_SetTextureColorMod(circle_texture.get(), 
				player_colors[i].r,
				player_colors[i].g,
				player_colors[i].b
			);
			
			vector<SDL_Vertex> vertices;
			vector<int> indices;
			SDL_Vertex vertex = {
				.color = player_colors[i]
			};
			
			for(int j = 1; j < player_history.size(); j++){
				if(player_history[j].hovering) continue;
				
				double width = get_player_size(player_history[j].size);
				double prev_width = min(width, get_player_size(player_history[j].size));

				double prev_x = player_history[j-1].x;
				if(player_history[j].warping_x) prev_x -= board.w * player_history[j].warp_x;
				double prev_y = player_history[j-1].y;
				if(player_history[j].warping_y) prev_y -= board.h * player_history[j].warp_y;
								
				double prev_direction = player_history[j - 1].direction;
				if(player_history[j].corner) {
					prev_direction = player_history[j].direction;
					
					// Draw circle
					dst.w = dst.h = DRAW_SCALE * prev_width;
					dst.x = DRAW_SCALE * prev_x - dst.w / 2.0;
					dst.y = DRAW_SCALE * prev_y - dst.h / 2.0;
					SDL_RenderCopy(renderer, circle_texture.get(), NULL, &dst);
				}
				
				for(int warp_x = min(-player_history[j].warp_x, 0); warp_x <= max(player_history[j].warp_x, 0); warp_x += 1){
					for(int warp_y = min(-player_history[j].warp_y, 0); warp_y <= max(player_history[j].warp_y, 0); warp_y += 1){
						int index = vertices.size();
						vertex.position = {
							.x = (float)(DRAW_SCALE * (prev_x - sin(prev_direction)*(width/2) + warp_x * board.w)),
							.y = (float)(DRAW_SCALE * (prev_y + cos(prev_direction)*(width/2) + warp_y * board.h)),
						};
						vertices.push_back(vertex);
						vertex.position = {
							.x = (float)(DRAW_SCALE * (prev_x + sin(prev_direction)*(width/2) + warp_x * board.w)),
							.y = (float)(DRAW_SCALE * (prev_y - cos(prev_direction)*(width/2) + warp_y * board.h)),
						};
						vertices.push_back(vertex);
						vertex.position = {
							.x = (float)(DRAW_SCALE * (player_history[j].x - sin(player_history[j].direction)*(width/2) + warp_x * board.w)),
							.y = (float)(DRAW_SCALE * (player_history[j].y + cos(player_history[j].direction)*(width/2) + warp_y * board.h)),
						};
						vertices.push_back(vertex);
						vertex.position = {
							.x = (float)(DRAW_SCALE * (player_history[j].x + sin(player_history[j].direction)*(width/2) + warp_x * board.w)),
							.y = (float)(DRAW_SCALE * (player_history[j].y - cos(player_history[j].direction)*(width/2) + warp_y * board.h)),
						};
						vertices.push_back(vertex);

						indices.push_back(index);
						indices.push_back(index+1);
						indices.push_back(index+2);
						indices.push_back(index+1);
						indices.push_back(index+2);
						indices.push_back(index+3);
					}
				}
			}

			SDL_RenderGeometry(renderer, NULL, vertices.data(), vertices.size(), indices.data(), indices.size());

			double width = get_player_size(player_history.back().size);
			dst.w = dst.h = DRAW_SCALE * width;
			dst.x = DRAW_SCALE * player_history.back().x - dst.w / 2.0;
			dst.y = DRAW_SCALE * player_history.back().y - dst.h / 2.0;
			SDL_RenderCopy(renderer, circle_texture.get(), NULL, &dst);
		}
	});
}

Texture& BoardDrawer::get_texture() const{
	return *texture;
}


GameDrawer::GameDrawer(const BoardSize& board, GameView* view) :
	view(view),
	board(board),
	board_drawer(board, view),
	is_initialized(false) {}

void GameDrawer::init(SDL_Renderer* renderer){
	if(!is_initialized){
		int output_w, output_h;
		SDL_GetRendererOutputSize(renderer, &output_w, &output_h);
		int scaled_w = output_h * board.w / board.h;
		int scaled_h = output_w * board.h / board.w;
		screen_width = max(output_w, scaled_w);
		screen_height = max(output_h, scaled_h);
						
		SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);
		
		is_initialized = true;
	}
}

void GameDrawer::draw(SDL_Renderer* renderer){
	init(renderer);
	
	SDL_SetRenderDrawColor(renderer, 32, 32, 32, 0);
	SDL_RenderClear(renderer);

	board_drawer.draw(renderer);
	
	SDL_RenderCopy(renderer, board_drawer.get_texture().get(), NULL, NULL);
}