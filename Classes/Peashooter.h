#ifndef __Plant_H__
#define __Plant_H__

#include "Plant.h"
#include "cocos2d.h"

class Peashooter :public Plant {
public:
	virtual bool init();
	CREATE_FUNC(Peashooter);
};

#endif // __Plant_H__