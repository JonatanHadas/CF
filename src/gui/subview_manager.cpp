#include "subview_manager.h"

SubViewManager::SubViewManager() : focus(nullptr) {}

void SubViewManager::add_view(SubView* view){
	subviews.insert({view, true});
}

void SubViewManager::remove_view(SubView* view){
	if(focus == view) lose_focus();
	
	subviews.erase(view);
}

void SubViewManager::set_active(SubView* view, bool active){
	if(view == focus && !active) lose_focus();
	
	subviews[view] = active;
}

SubView* SubViewManager::get_position(int x, int y){
	for(auto& entry: subviews){
		if(entry.second && entry.first->check_inside(x, y)){
			return entry.first;
		}
	}
	return nullptr;
}

void SubViewManager::lose_focus(){
	SubView* old_focus = focus;
	focus = nullptr;
	
	if(old_focus != nullptr) old_focus->lose_focus();	
}

SubView* SubViewManager::get_focus(){
	return focus;
}

void SubViewManager::draw(SDL_Renderer* renderer){
	for(auto& entry: subviews){
		entry.first->draw(renderer);
	}
}

bool SubViewManager::handle_event(const SDL_Event& event){
	if(event.type == SDL_MOUSEBUTTONDOWN){
		SubView* new_focus = get_position(event.button.x, event.button.y);
		
		if(new_focus != focus) lose_focus();
		focus = new_focus;
		
	}
	if(focus != nullptr) return focus->handle_event(event);
	
	return false;
}

void SubViewManager::step(){
	for(auto& entry: subviews){
		entry.first->step();
	}
}