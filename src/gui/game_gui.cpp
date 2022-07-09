#include "game_gui.h"

GameGui::GameGui(const BoardSize& board, GameView* view, PlayerInterface* interface) :
	drawer(board, view),
	view(view),
	interface(interface) {}

void GameGui::step(){
	auto keyboard_state = SDL_GetKeyboardState(NULL);
	
	int turn_state = 0;
	if(keyboard_state[SDL_SCANCODE_LEFT]) turn_state -= 1;
	if(keyboard_state[SDL_SCANCODE_RIGHT]) turn_state += 1;
	
	interface->step(view->get_round(), turn_state);
}

void GameGui::handle_event(const SDL_Event& event){}

void GameGui::draw(SDL_Renderer* renderer){
	drawer.draw(renderer);
}
