#include "game_gui.h"

#define START_DELAY 30

GameGui::GameGui(
	GameView* view,
	GameAdvancer* advancer,
	const map<PlayerInterface*, KeySet>& interfaces
) :
	last_round(-1),
	starting_timer(0),
	paused(false),
	drawer(view),
	view(view),
	advancer(advancer),
	interfaces(interfaces) {}

bool GameGui::step(){
	if(!paused){
		if(last_round != view->get_round()){
			if(view->is_over()) return true;
			
			last_round = view->get_round();
			starting_timer = START_DELAY;
		}

		if(starting_timer) starting_timer--;
		else{
			auto keyboard_state = SDL_GetKeyboardState(NULL);
			for(auto interface: interfaces){
				int turn_state = 0;
				if(keyboard_state[interface.second.left]) turn_state -= 1;
				if(keyboard_state[interface.second.right]) turn_state += 1;
				
				interface.first->step(view->get_round(), turn_state);
			}
		}
	}
	
	advancer->advance();	
	return false;
}

bool GameGui::handle_event(const SDL_Event& event){
	switch(event.type){
	case SDL_KEYDOWN:
		switch(event.key.keysym.scancode){
		case SDL_SCANCODE_ESCAPE:
			return true;
		case SDL_SCANCODE_RETURN:
			paused = !paused;
			break;
		}
		break;
	}
	return false;
}

void GameGui::draw(SDL_Renderer* renderer){
	drawer.draw(renderer);
}
