#pragma once
#include"cocos2d.h"
#include"layer/PlantLayer.h"
#include"manager/CardMgr.h"
#include"plant/Plant.h"
#include"plant/PlantData.h"

#include"plant/SunFlower.h"
#include"plant/WallNut.h"



class PlantMgr : public cocos2d::Node {

protected:
    PlantLayer* _plantLayer = nullptr;
    static PlantMgr* s_sharedPlantMgr;
public:
    void setPlantLayer(const PlantLayer* layer) {
    }

    // 移除植物
    bool removePlantAt(const cocos2d::Vec2& gridPos);

    // 获取植物
    Plants* getPlantAt(const cocos2d::Vec2& gridPos);

    // 更新所有植物

   
    static PlantMgr* getInstance();

    bool init() override;

    void createPlantAt(const cocos2d::Vec2& rowCol, PlantType type);

    void reset();
};

