#include "texts.h"

#include "SDL_ttf.h"

#include <vector>

#define FONT_NUM 4
int sizes[FONT_NUM] = {20,72,30,14};

std::vector<TTF_Font*> fonts;

void clear_fonts(){
	while(fonts.size()>0){
		TTF_CloseFont(fonts.back());
		fonts.pop_back();
	}
}

bool load_fonts(){
	atexit(clear_fonts);
	for(int i = 0; i<FONT_NUM;i++){
		fonts.push_back(TTF_OpenFont("data/fonts/arial.ttf", sizes[i]));
		if(fonts.back() == NULL){
			fonts.pop_back();
			return false;
		}
	}

	return true;
}


int get_font_ind(FontType f){
	switch(f){
	case FontType::NRM:
		return 0;
	case FontType::BIG:
		return 1;
	case FontType::MID:
		return 2;
	case FontType::SML:
		return 3;
	}
	return -1;
}

Msg::Msg(const char* text, SDL_Color color, FontType t, SDL_Renderer* renderer) :
	rend(renderer),
	img(NULL){

	SDL_Surface* surf = TTF_RenderUTF8_Blended( fonts[get_font_ind(t)], (text && (*text))?text:" ", color);
	if(surf == NULL){img = NULL; w = -1; h = -1;}
	else{
		img = SDL_CreateTextureFromSurface(rend, surf);
		if(img == NULL){w = -1; h = -1;}
		else{ w = surf->w; h = surf->h;}
		SDL_SetTextureAlphaMod(img, color.a);
		SDL_FreeSurface(surf);
	}
}

Msg::Msg(Msg&& msg) :
	img(msg.img),
	w(msg.w),
	h(msg.h),
	rend(msg.rend){

	msg.img = NULL;
}
Msg::~Msg(){
	if(img != NULL) SDL_DestroyTexture(img);
}
Msg& Msg::operator=(Msg&& msg){
	rend = msg.rend;
	w = msg.w;
	h = msg.h;
	img = msg.img;
	msg.img = NULL;
	
	return *this;
}

void Msg::render_centered(int x, int y, Align a){
	SDL_Rect dst;
	switch(a){
	case Align::RIGHT:
		dst.x = x-w;
		break;
	case Align::LEFT:
		dst.x = x;
		break;
	case Align::CENTER:
		dst.x = x-w/2;
		break;
	}
	dst.y = y - h/2;
	dst.w = w;dst.h=h;
	SDL_RenderCopy(rend,img,NULL, &dst);
}

bool Msg::exists(){
	return img != NULL;
}

int Msg::get_width() const {
	return w;
}

int Msg::get_height() const {
	return h;
}
