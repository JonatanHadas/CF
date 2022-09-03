#ifndef _SUBVIEW_MANAGER_H
#define _SUBVIEW_MANAGER_H

#include "subview.h"

#include <map>

using namespace std;

class SubViewManager{
	map<SubView*, bool> subviews;
	SubView* focus;
public:
	SubViewManager();
	
	void add_view(SubView* view);
	void remove_view(SubView* view);
	void set_active(SubView* view, bool active);
	
	SubView* get_position(int x, int y);
	
	void lose_focus();
	SubView* get_focus();
	
	void draw(SDL_Renderer* renderer);
	bool handle_event(const SDL_Event& event);
	void step();
};

#endif
