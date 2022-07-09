#include "game_drawer.h"

#include "colors.h"

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

void BoardDrawer::draw(SDL_Renderer* renderer){
	init(renderer);
	
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
			
			for(int j = 1; j < player_history.size(); j++){
				if(!player_history[j-1].hovering){
					SDL_RenderDrawLine(renderer,
						DRAW_SCALE * player_history[j].x,
						DRAW_SCALE * player_history[j].y,
						DRAW_SCALE * player_history[j-1].x,
						DRAW_SCALE * player_history[j-1].y
					);				
				}
			}
			
			if(player_history.size() && player_history.back().hovering){
				SDL_RenderDrawPoint(renderer,
					DRAW_SCALE * player_history.back().x,
					DRAW_SCALE * player_history.back().y
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
	
	board_drawer.draw(renderer);
	
	SDL_RenderCopy(renderer, board_drawer.get_texture().get(), NULL, NULL);
}
