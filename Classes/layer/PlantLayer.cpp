#include"layer/PlantLayer.h"
USING_NS_CC;


//初始化
bool PlantLayer::init()
{
	if (!Layer::init())
		return false;
}

void PlantLayer::addPlant(Plants* plant)
{
	this->addChild(plant);
	_plants.pushBack(plant);
}

//更新植物状态
void PlantLayer::update(float dt)
{

}