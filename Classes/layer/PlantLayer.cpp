#include"layer/PlantLayer.h"
#include "layer/ControlLayer.h"
USING_NS_CC;


//初始化
bool PlantLayer::init()
{
	if (!Layer::init())
		return false;
    this->scheduleUpdate();
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
    for (int i = _plants.size() - 1; i >= 0; --i) {
        auto z = _plants.at(i);
        if (z->getParent() == nullptr) {

            _plants.erase(i);
        }
    }
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

Plants* PlantLayer::getPlantAtGrid(int row, int col) const {
    // 遍历当前层级所有的植物
    for (auto plant : _plants) {

        if (plant->getRow() == row && plant->getCol() == col) {
            return plant;
        }
    }
    return nullptr;
}

bool PlantLayer::removePlant(Plants* plant)
{
    if (!plant)
        return false;

    if (plant->getParent() == this) {
        plant->handleDeath();
        _plants.eraseObject(plant);
        return true;
    }
    return false;
}

bool PlantLayer::removePlant(int row, int col)
{
    auto plant = getPlantAtGrid(row, col);
    return removePlant(plant);
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
