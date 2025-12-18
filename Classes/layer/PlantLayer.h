#pragma once

#include "cocos2d.h"
#include "plant\Plant.h"

USING_NS_CC;

//所有植物从PlantMgr中种下,加入到PlantLayer的Vector中管理

class PlantLayer : public Layer {
public:
    CREATE_FUNC(PlantLayer);
    virtual bool init() override;

    // 添加植物
    void addPlant(Plants* plant);

    // 移除植物
    void removePlant(Plants* plant);

    // 获取所有植物
    Vector<Plants*>& getAllPlants(){ return _plants; }

    // 根据网格位置获取植物
    Plants* getPlantAtGrid(int row, int col) const;

    // 更新所有植物
    virtual void update(float dt) override;

private:
    Vector<Plants*> _plants;
};