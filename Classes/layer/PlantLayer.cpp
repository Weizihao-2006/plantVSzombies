#include"layer/PlantLayer.h"
#include "layer/ControlLayer.h"
#include"manager/MapMgr.h"
#include"tool/LawnMower.h"
USING_NS_CC;


//初始化
bool PlantLayer::init()
{
	if (!Layer::init())
		return false;


    createLawnMowers();

    this->scheduleUpdate();
    return true;
}

void PlantLayer::createLawnMowers() {
  
    // 假设你的地图有 5 行
    int row = MapManager::getInstance()->getRowNumbers();

    for (int i = 0; i < row; ++i) {
        auto mower = LawnMower::create(i);
        mower->setScale(1.6f);
        // 1. 获取该行第 0 列的中心坐标
        Vec2 rowPos = MapManager::getInstance()->getPositionInMap(i, 0);

        // 2. 设置初始位置：X 设在草坪左侧边缘（比如 70），Y 与该行中心对齐
        // 注意：setPosition 是相对于父节点 PlantLayer 的坐标
        mower->setPosition(Vec2(rowPos.x-110.0f, rowPos.y));

        // 3. 添加到 PlantLayer，ZOrder 设为 5
        this->addChild(mower, 5);

        CCLOG("LawnMower created at Row %d, Pos(%.1f, %.1f)", i, mower->getPositionX(), mower->getPositionY());
    }
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
