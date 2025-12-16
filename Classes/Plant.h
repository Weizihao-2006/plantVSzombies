#pragma once

#include "cocos2d.h"


class Plant :public cocos2d::Node {
public:
	virtual bool init() = 0;


protected:
	cocos2d::Sprite* plant;

};