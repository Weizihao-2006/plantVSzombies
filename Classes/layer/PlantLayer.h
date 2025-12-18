#pragma once

#include <vector>
#include "cocos2d.h"
#include "plant\Plant.h"

USING_NS_CC;
class PlantLayer : public cocos2d::Layer {
public:
    CREATE_FUNC(PlantLayer);
    virtual bool init() override;

    // 添加植物
    void addPlant(Plants* plant);

    // 移除植物
    void removePlant(Plants* plant);

    // 获取所有植物
    std::vector<Plants*> getAllPlants() const { return _plants; }

    // 根据网格位置获取植物
    Plants* getPlantAtGrid(int row, int col) const;

    // 更新所有植物
    void updatePlants(float deltaTime);

private:
    std::vector<Plants*> _plants;
};