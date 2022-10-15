#include "key_choice_button.h"


const char* get_key_name(SDL_Scancode key){
	switch(key){
	case SDL_SCANCODE_LEFT:
		return "\xe2\x86\x90";
	case SDL_SCANCODE_RIGHT:
		return "\xe2\x86\x92";
	case SDL_SCANCODE_UP:
		return "\xe2\x86\x91";
	case SDL_SCANCODE_DOWN:
		return "\xe2\x86\x93";
	case SDL_SCANCODE_RETURN:
		return "\xe2\x86\xb5";
	case SDL_SCANCODE_BACKSPACE:
		return "\xe2\x8c\xab";
	case SDL_SCANCODE_SPACE:
		return "\xe2\x90\xa3";
	case SDL_SCANCODE_DELETE:
		return "\xe2\x90\xa1";
	case SDL_SCANCODE_KP_0:
		return "KP 0";
	case SDL_SCANCODE_KP_00:
		return "KP 00";
	case SDL_SCANCODE_KP_000:
		return "KP 000";
	case SDL_SCANCODE_KP_1:
		return "KP 1";
	case SDL_SCANCODE_KP_2:
		return "KP 2";
	case SDL_SCANCODE_KP_3:
		return "KP 3";
	case SDL_SCANCODE_KP_4:
		return "KP 4";
	case SDL_SCANCODE_KP_5:
		return "KP 5";
	case SDL_SCANCODE_KP_6:
		return "KP 6";
	case SDL_SCANCODE_KP_7:
		return "KP 7";
	case SDL_SCANCODE_KP_8:
		return "KP 8";
	case SDL_SCANCODE_KP_9:
		return "KP 9";
	case SDL_SCANCODE_KP_A:
		return "KP A";
	case SDL_SCANCODE_KP_B:
		return "KP B";
	case SDL_SCANCODE_KP_C:
		return "KP C";
	case SDL_SCANCODE_KP_D:
		return "KP D";
	case SDL_SCANCODE_KP_E:
		return "KP E";
	case SDL_SCANCODE_KP_F:
		return "KP F";
	case SDL_SCANCODE_KP_AMPERSAND:
		return "KP &";
	case SDL_SCANCODE_KP_AT:
		return "KP @";
	case SDL_SCANCODE_KP_BACKSPACE:
		return "KP \xe2\x8c\xab";
	case SDL_SCANCODE_KP_BINARY:
		return "KP Bin";
	case SDL_SCANCODE_KP_CLEAR:
		return "KP Clear";
	case SDL_SCANCODE_KP_CLEARENTRY:
		return "KP ClearEntry";
	case SDL_SCANCODE_KP_COLON:
		return "KP :";
	case SDL_SCANCODE_KP_COMMA:
		return "KP ,";
	case SDL_SCANCODE_KP_DBLAMPERSAND:
		return "KP &&";
	case SDL_SCANCODE_KP_DBLVERTICALBAR:
		return "KP ||";
	case SDL_SCANCODE_KP_DECIMAL:
		return "KP Dec";
	case SDL_SCANCODE_KP_DIVIDE:
		return "KP /";
	case SDL_SCANCODE_KP_EQUALS:
		return "KP =";
	case SDL_SCANCODE_KP_EQUALSAS400:
		return "KP =(AS400)";
	case SDL_SCANCODE_KP_EXCLAM:
		return "KP !";
	case SDL_SCANCODE_KP_GREATER:
		return "KP >";
	case SDL_SCANCODE_KP_HASH:
		return "KP #";
	case SDL_SCANCODE_KP_HEXADECIMAL:
		return "KP Hex";
	case SDL_SCANCODE_KP_LEFTBRACE:
		return "KP {";
	case SDL_SCANCODE_KP_LEFTPAREN:
		return "KP (";
	case SDL_SCANCODE_KP_LESS:
		return "KP <";
	case SDL_SCANCODE_KP_MEMADD:
		return "KP MemAdd";
	case SDL_SCANCODE_KP_MEMCLEAR:
		return "KP MemClear";
	case SDL_SCANCODE_KP_MEMDIVIDE:
		return "KP MemDivide";
	case SDL_SCANCODE_KP_MEMMULTIPLY:
		return "KP MemMultipley";
	case SDL_SCANCODE_KP_MEMRECALL:
		return "KP MemRecall";
	case SDL_SCANCODE_KP_MEMSTORE:
		return "KP MemStore";
	case SDL_SCANCODE_KP_MEMSUBTRACT:
		return "KP MemSubtract";
	case SDL_SCANCODE_KP_MINUS:
		return "KP -";
	case SDL_SCANCODE_KP_MULTIPLY:
		return "KP *";
	case SDL_SCANCODE_KP_OCTAL:
		return "KP Oct";
	case SDL_SCANCODE_KP_PERCENT:
		return "KP %";
	case SDL_SCANCODE_KP_PERIOD:
		return "KP .";
	case SDL_SCANCODE_KP_PLUS:
		return "KP +";
	case SDL_SCANCODE_KP_PLUSMINUS:
		return "KP \xc2\xb1";
	case SDL_SCANCODE_KP_POWER:
		return "KP ^";
	case SDL_SCANCODE_KP_RIGHTBRACE:
		return "KP }";
	case SDL_SCANCODE_KP_RIGHTPAREN:
		return "KP )";
	case SDL_SCANCODE_KP_SPACE:
		return "KP \xe2\x90\xa3";
	case SDL_SCANCODE_KP_TAB:
		return "KP Tab";
	case SDL_SCANCODE_KP_VERTICALBAR:
		return "KP |";
	case SDL_SCANCODE_KP_XOR:
		return "KP XOR";
	case SDL_SCANCODE_KP_ENTER:
		return "KP \xe2\x86\xb5";
	default:
		return SDL_GetScancodeName(key);
	}
}

KeyChoiceButton::KeyChoiceButton(
	const SDL_Rect& rect, bool translucent,
	FontType font, SDL_Color color,
	SDL_Scancode key
) :
	SubView(rect, translucent),
	font(font), color(color),
	key(key), active(false), updated(true) {}
	
KeyChoiceButton::KeyChoiceButton(
	const SDL_Rect& rect, bool translucent,
	FontType font, SDL_Color color
) : KeyChoiceButton(rect, translucent, font, color, (SDL_Scancode)0) {}

void KeyChoiceButton::draw_content(SDL_Renderer* renderer) {
	draw_back(renderer, active, key == 0);
	
	if(updated){
		if(key){
			msg = make_unique<Msg>(get_key_name(key), color, font, renderer);
		}
		else msg = nullptr;
		
		updated = false;
	}
	
	int x = get_rect().w / 2;
	int y = get_rect().h / 2;
	
	if(msg.get() != nullptr) msg->render_centered(x, y, Align::CENTER);
}

void KeyChoiceButton::on_active() {}

bool KeyChoiceButton::on_event(const SDL_Event& event){
	switch(event.type){
	case SDL_MOUSEBUTTONDOWN:
		if(!active) on_active();
		active = true;
		return true;
	case SDL_KEYDOWN:
		if(!active) return false;
		
		if(on_set(event.key.keysym.scancode)){
			key = event.key.keysym.scancode;
			updated = true;
		}
		active = false;
		return true;
	default:
		return false;
	}
}

void KeyChoiceButton::lose_focus(){
	active = false;
}

void KeyChoiceButton::step(){
	
}
