#include "manager/PlantMgr.h"
#include"manager/CardMgr.h" //g_cardAtlas 
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

//在
//在指定位置创建一个植物并且加入到RunningScene中
//注意,创建的植物并非加到某一个Layer!
//可能要修改,坐标不一定对,
void PlantMgr::createPlantAt(const Vec2& rowCol, int plantId)
{
    // 根据地图网格计算世界坐标（适配ControlLayer中的地图尺寸）
    float cellWidth = (1900.0f - 490.0f) / 9;  // 地图宽度/列数
    float cellHeight = (1150.0f - 255.0f) / 5; // 地图高度/行数

    Vec2 worldPos = Vec2(
        490.0f + rowCol.x * cellWidth + cellWidth / 2,  // 列坐标转X
        255.0f + rowCol.y * cellHeight + cellHeight / 2 // 行坐标转Y
    );

    //// 创建植物精灵（使用正确的图片路径）
    //std::string imgPath = StringUtils::format("plant_%d.png", plantId);
    auto plant = Sprite::create("plant/Peashooter_0.png");
    plant->setPosition(worldPos);
    Director::getInstance()->getRunningScene()->addChild(plant, 10);
}