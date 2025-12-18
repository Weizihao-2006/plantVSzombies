#include "manager/PlantMgr.h"
#include"manager/CardMgr.h" //g_cardAtlas 
#include "layer/ControlLayer.h"
#include "MapCoordinate.h"
USING_NS_CC;

PlantMgr* PlantMgr::getInstance() { static PlantMgr mgr; return &mgr; }


// 在指定位置创建一个植物并且加入到RunningScene中
// 注意,创建的植物并非加到某一个Layer!

//实际上判断是否可以种植的逻辑可以分到GameMapManager中
//plantId
void PlantMgr::createPlantAt(const Vec2& rowCol, int plantId)
{

    /*std::string filename = _CardPreview[plantId];
    auto plant = Sprite::create(filename);
    int col = rowCol.x;
    int row = rowCol.y;
    plant->setPosition(MapCoordinate[row][col]);
    plant->setScale(1.5f);
    Director::getInstance()->getRunningScene()->addChild(plant, 10);*/


    Plants* plant = nullptr;
    PlantProperties props;
    PlantType plant_type = CardMgr::getInstance()->getPlantType(plantId);

#if 0
    switch (plant_type) {
        case PlantType::SunFlower:
            plant = SunFlower::create();
            // 阳光花费, 冷却时间, 血量, 攻击力, 类型, 名字
            props = { 50, 7.5f, 300, 0, PlantType::SunFlower, "SunFlower" };
            break;

        case PlantType::PeaShooter:
            plant = PeaShooter::create();
            props = { 100, 7.5f, 300, 20, PlantType::PeaShooter, "PeaShooter" };
            break;

        case PlantType::ReaPeater:
            plant = Repeater::create();
            props = { 200, 7.5f, 300, 20, PlantType::ReaPeater, "Repeater" };
            break;

        case PlantType::SnowPea:
            plant = SnowPea::create();
            props = { 175, 7.5f, 300, 20, PlantType::SnowPea, "SnowPea" };
            break;

        case PlantType::WallNut:
            plant = WallNut::create();
            props = { 50, 30.0f, 4000, 0, PlantType::WallNut, "WallNut" };
            break;

        case PlantType::CherryBomb:
            plant = CherryBomb::create();
            props = { 150, 50.0f, 300, 1800, PlantType::CherryBomb, "CherryBomb" };
            break;

        default:
            CCLOG("Warning: Unknown PlantType!");
            return;
    }
    if (plant) {
        // 2. 放置
        plant->setPosition(MapCoordinate[static_cast<int>(rowCol.y)][static_cast<int>(rowCol.x)]);
        _plantLayer->addChild(plant);
        // 3. 登记
        _allPlants.push_back(plant);
        // 4. 启动update的schedule,具体见plant::update()
        plant->scheduleUpdate();
    }
#endif
}