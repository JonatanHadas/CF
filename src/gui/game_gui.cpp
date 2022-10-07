#include "game_gui.h"

#include "sounds.h"

GameSoundManager::GameSoundManager(){}

GameSoundManager::~GameSoundManager(){
	silence();
}

void GameSoundManager::kill_player(int player){
	play(Sound::COLLISION);
}

void GameSoundManager::spawn_powerup(const PowerUp& power_up){
	play(Sound::SPAWN_POWERUP);
}

void GameSoundManager::activate_powerup(const PowerUp& power_up){
	play(Sound::TAKE_POWERUP);
}


#define START_DELAY 30

GameGui::GameGui(
	GameView* view,
	GameAdvancer* advancer,
	GameEventListenerAccumulator* accumulator,
	const GameSettings& settings,
	const map<PlayerInterface*, KeySet>& interfaces
) :
	settings(settings),
	last_round(-1),
	starting_timer(0),
	paused(false),
	drawer(view, settings),
	view(view),
	advancer(advancer),
	accumulator(accumulator),
	interfaces(interfaces) {
		
	accumulator->add_listener(&sounds);
}

GameGui::~GameGui(){
	accumulator->remove_listener(&sounds);
}

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
	
	advancer->allow_step();
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

unique_ptr<WinnerDisplay> GameGui::get_score_display(){
	if(!view->is_over()) return nullptr;
	return make_unique<WinnerDisplay>(settings, view);
}
