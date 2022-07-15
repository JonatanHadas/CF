#include "game_gui.h"
#include <iostream>
GameGui::GameGui(const BoardSize& board, GameView* view, const map<PlayerInterface*, KeySet>& interfaces) :
	drawer(board, view),
	view(view),
	interfaces(interfaces) {}

void GameGui::step(){
	auto keyboard_state = SDL_GetKeyboardState(NULL);
	for(auto interface: interfaces){
		int turn_state = 0;
		if(keyboard_state[interface.second.left]) turn_state -= 1;
		if(keyboard_state[interface.second.right]) turn_state += 1;
		
		interface.first->step(view->get_round(), turn_state);
	}	
}

void GameGui::handle_event(const SDL_Event& event){}

void GameGui::draw(SDL_Renderer* renderer){
	drawer.draw(renderer);
}
