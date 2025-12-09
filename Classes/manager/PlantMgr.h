#pragma once
#include"cocos2d.h"


class PlantMgr : public cocos2d::Node {
public:
    static PlantMgr* getInstance();
    void createPlantAt(const cocos2d::Vec2& rowCol, int plantId);
};