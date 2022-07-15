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

void BoardDrawer::draw(SDL_Renderer* renderer){
	init(renderer);
	
	Texture rect_texture(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		10, 10
	);
	Texture circle_texture(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		CIRCLE_RAD * 2, CIRCLE_RAD * 2
	);
	
	rect_texture.do_with_texture(renderer, [&](){
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
	});
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

			SDL_SetTextureColorMod(rect_texture.get(), 
				player_colors[i].r,
				player_colors[i].g,
				player_colors[i].b
			);

			SDL_SetTextureColorMod(circle_texture.get(), 
				player_colors[i].r,
				player_colors[i].g,
				player_colors[i].b
			);
			
			for(int j = 1; j < player_history.size(); j++){
				dst.h = DRAW_SCALE * distance(
					player_history[j].x, player_history[j].y,
					player_history[j-1].x, player_history[j-1].y
				);
				dst.w = DRAW_SCALE * get_player_size(player_history[j-1].size);
				dst.x = (DRAW_SCALE * (player_history[j].x + player_history[j-1].x) - dst.w)/2;
				dst.y = (DRAW_SCALE * (player_history[j].y + player_history[j-1].y) - dst.h)/2;
				double angle = RAD2ANG(atan2(
					player_history[j].x - player_history[j-1].x,
					player_history[j].y - player_history[j-1].y
				));
								
				if(!player_history[j].hovering){
					SDL_RenderCopyEx(renderer,
						rect_texture.get(),
						NULL, &dst,
						-angle, NULL, SDL_FLIP_NONE
					);
					
					if(!player_history[j-1].hovering){
						double rad = DRAW_SCALE * get_player_size(min(
							player_history[j].size,
							player_history[j-1].size
						)) / 2;
						
						dst.w = dst.h = 2*rad;
						
						dst.x = DRAW_SCALE * player_history[j-1].x - dst.w/2.0;
						dst.y = DRAW_SCALE * player_history[j-1].y - dst.h/2.0;
						
						SDL_RenderCopy(renderer,
							circle_texture.get(),
							NULL, &dst
						);
					}
				}
			}
			
			if(player_history.size()){
				double rad = DRAW_SCALE * get_player_size(player_history.back().size) / 2;
				
				dst.w = dst.h = 2*rad;
				
				dst.x = DRAW_SCALE * player_history.back().x - dst.w/2.0;
				dst.y = DRAW_SCALE * player_history.back().y - dst.h/2.0;
				
				SDL_RenderCopy(renderer,
					circle_texture.get(),
					NULL, &dst
				);
			}
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
