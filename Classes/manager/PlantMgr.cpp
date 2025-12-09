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
    Vec2 worldPos = Vec2(200 + rowCol.x * 100 + 50,
        100 + rowCol.y * 100 + 50);
    auto plant = Sprite::create(g_cardAtlas[plantId].icon); 
    plant->setPosition(worldPos);
    Director::getInstance()->getRunningScene()->addChild(plant, 10);

    // 通知 CardMgr 开始冷却
    auto cardMgr = CardMgr::getInstance();
    for (size_t i = 0; i < LEVEL1_DECK.size(); ++i)
        if (LEVEL1_DECK[i] == plantId) { cardMgr->startCool(i); break; }
}