#include "cocos2d.h"
#include "Peashooter.h"

USING_NS_CC;

bool Peashooter::init()
{
	if (!Node::init()) {
		return false;
	}

	plant = Sprite::create();
	plant->setTexture("peashooter.gif");
	plant->setPosition(Vec2(200, 200));
	this->addChild(plant);
}