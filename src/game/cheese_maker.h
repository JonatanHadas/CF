#ifndef _CHEESE_MAKER_H
#define _CHEESE_MAKER_H

class CheeseMaker{
	unsigned int timer;
	bool state;
	
public:
	CheeseMaker();
	
	bool step();
};

#endif
