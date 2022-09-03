#ifndef _KEY_CHOICE_BUTTON_H
#define _KEY_CHOICE_BUTTON_H

#include "subview.h"
#include "texts.h"

#include <memory>

using namespace std;

class KeyChoiceButton : public SubView {
	bool active;
	SDL_Scancode key;
	FontType font;
	SDL_Color color;
	
	unique_ptr<Msg> msg;
	bool updated;
protected:
	virtual bool on_set(SDL_Scancode key) = 0;

	virtual void draw_back(SDL_Renderer* renderer, bool active) = 0;
	
	void draw_content(SDL_Renderer* renderer);
	bool on_event(const SDL_Event& event);
public:
	KeyChoiceButton(
		const SDL_Rect& rect, bool translucent,
		FontType font, SDL_Color color
	);

	KeyChoiceButton(
		const SDL_Rect& rect, bool translucent,
		FontType font, SDL_Color color,
		SDL_Scancode key
	);
	
	void lose_focus();
	void step();
};

#endif
