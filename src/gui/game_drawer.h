#ifndef _GAME_DRAWER
#define _GAME_DRAWER

#include "gui_utils.h"
#include "../game/game_view.h"

#include <memory>
#include <SDL.h>

using namespace std;

class BoardDrawer{
	GameView* view;
	const BoardSize board;
	
	unique_ptr<Texture> texture;

	void init(SDL_Renderer* renderer);
public:
	BoardDrawer(const BoardSize& board, GameView* view);
	
	void draw(SDL_Renderer* renderer);
	
	Texture& get_texture() const;
};

class GameDrawer{
	GameView* view;
	const BoardSize board;
	
	BoardDrawer board_drawer;

	bool is_initialized;
	int screen_width, screen_height;
	
	void init(SDL_Renderer* renderer);
public:
	GameDrawer(const BoardSize& board, GameView* view);
	
	void draw(SDL_Renderer* renderer);
};

#endif
