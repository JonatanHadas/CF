#include "cheese_maker.h"

#include <stdlib.h>

CheeseMaker::CheeseMaker() :
	timer(0),
	state(true) {}
	

#define MIN_INTERVAL 30
#define MAX_INTERVAL 60

#define HOLE_SIZE 3
	
bool CheeseMaker::step(){
	if(timer) timer--;
	else{
		state = !state;
		if(state){
			timer = HOLE_SIZE;
		}
		else{
			timer = MIN_INTERVAL + (rand() % (MAX_INTERVAL - MIN_INTERVAL));
		}
	}
	return state;
}
