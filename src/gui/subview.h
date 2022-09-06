#ifndef _SUBVIEW_H
#define _SUBVIEW_H

#include "gui_utils.h"

#include <memory>

using namespace std;

class SubView{
	unique_ptr<Texture> texture;
	SDL_Rect rect;
	bool translucent;
	
	int mouse_x, mouse_y;
protected:
	virtual bool on_event(const SDL_Event& event) = 0;
	virtual void draw_content(SDL_Renderer* renderer) = 0;
public:
	SubView(const SDL_Rect& rect, bool translucent);
	
	bool handle_event(SDL_Event event);
	void draw(SDL_Renderer* renderer);
	
	virtual void step() = 0;
	virtual void lose_focus() = 0;
	
	bool check_inside(int x, int y);

	const SDL_Rect& get_rect() const;
	void move(int x, int y);

	void fill_back(SDL_Renderer* renderer, const SDL_Color& color) const;
	void draw_frame(SDL_Renderer* renderer, const SDL_Color& color) const;
};

#endif
