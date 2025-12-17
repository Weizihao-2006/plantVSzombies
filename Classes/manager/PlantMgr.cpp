#include "manager/PlantMgr.h"
#include"manager/CardMgr.h" //g_cardAtlas 
#include "layer/ControlLayer.h"
#include "MapCoordinate.h"
USING_NS_CC;

const std::vector<int> LEVEL1_DECK = {
    0,   // PeaShooter
    1,   // SunFlower
    2,   // WallNut
    3,   // PotatoMine
    4,   // SnowPea
    5,   // CherryBomb
    6,   // Repeater
    7    // Torchwood
};

PlantMgr* PlantMgr::getInstance() { static PlantMgr mgr; return &mgr; }


// 在指定位置创建一个植物并且加入到RunningScene中
// 注意,创建的植物并非加到某一个Layer!
void PlantMgr::createPlantAt(const Vec2& rowCol, int plantId)
{
    // 创建植物精灵（使用正确的图片路径）
    // TODO：animation的操作，这里先用图片代替
    std::string filename = _CardPreview[plantId];
    auto plant = Sprite::create(filename);
    int col = rowCol.x;
    int row = rowCol.y;
    plant->setPosition(MapCoordinate[row][col]);
    plant->setScale(1.5f);
    Director::getInstance()->getRunningScene()->addChild(plant, 10);
}