#include "manager/PlantMgr.h"
#include"manager/CardMgr.h" //g_cardAtlas 
#include "layer/ControlLayer.h"
#include"manager/MapMgr.h"
USING_NS_CC;

PlantMgr* PlantMgr::s_sharedPlantMgr = nullptr;


PlantMgr* PlantMgr::getInstance()
{ 
    if (s_sharedPlantMgr == nullptr) {
        s_sharedPlantMgr = new(std::nothrow) PlantMgr;
        s_sharedPlantMgr->init();
    }
    return s_sharedPlantMgr;

}
bool PlantMgr::init()
{
    auto scene=Director::getInstance()->getRunningScene();
    _plantLayer = dynamic_cast<PlantLayer*>(scene->getChildByName("PlantLayer"));
    return true;
}

// 在指定位置创建一个植物并且加入到RunningScene中
// 注意,创建的植物并非加到某一个Layer!


void PlantMgr::createPlantAt(const Vec2& rowCol, PlantType type)
{

    auto mapManager = MapManager::getInstance();

    Plants* plant;
    switch (type) {
        case PlantType::SunFlower:
            plant = SunFlower::create();
            break;
#if 0
        case PlantType::PeaShooter:
            plant = PeaShooter::create();
            break;
        case PlantType::ReaPeater:
            plant = Repeater::create();
            break;
        case PlantType::SnowPea:
            plant = SnowPea::create();
            break;
        case PlantType::WallNut:
            plant = WallNut::create();
            break;
        case PlantType::CherryBomb:
            plant = CherryBomb::create();
            break;
        default:
            CCLOG("Warning: Unknown PlantType!");
            return;
#endif
    }
    if (plant) {
        // 2. 放置
        int col = rowCol.x;
        int row = rowCol.y;
        plant->setPosition(mapManager->getPositionInMap(row, col));

        //放大
        plant->setScale(1.5f);

        _plantLayer->addPlant(plant);

        plant->scheduleUpdate();
    }
}