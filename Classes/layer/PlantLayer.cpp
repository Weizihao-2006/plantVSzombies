#include"layer/PlantLayer.h"
USING_NS_CC;


//初始化
bool PlantLayer::init()
{
	if (!Layer::init())
		return false;
    return true;
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

void PlantLayer::pauseAllPlants() {
    this->pause();
    for (auto& plant : _plants) {
        // 1. 暂停植物容器
        plant->pause();

        // 2. 暂停所有子节点（暂停预置得animation）
        for (auto& child : plant->getChildren()) {
            child->pause();
        }
    }
}

void PlantLayer::resumeAllPlants() {
    this->resume();
    for (auto& plant : _plants) {
        plant->resume();
        for (auto& child : plant->getChildren()) {
            child->resume();
        }
    }
}