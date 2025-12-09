#pragma once

#include <vector>
#include <functional>
#include"util/Global.h"

/*
展示卡牌->点击卡牌 → CardBarLayer::onCardClicked → CardMgr::onCardSelected →
PlantInputLayer 进入预览 → 移动鼠标 → onTouchEnded →
PlantMgr::createPlantAt → 真正生成植物 + CardMgr::startCool
*/

struct CardDef {
    int   id;
    const char* icon;
    int   sunCost;
    float coolTime;
};
const std::vector<CardDef> g_cardAtlas = {
    {0, "icon_pea.png",   100, 7.5f},
    {1, "icon_sun.png",    50, 7.5f},
    {2, "icon_wall.png",   50, 30.0f},
    {3, "icon_potato.png", 25, 30.0f},
    {4, "icon_snow.png",  150, 7.5f},
    {5, "icon_cherry.png",150, 35.0f},
    {6, "icon_repeat.png",200, 7.5f},
    {7, "icon_torch.png", 175, 7.5f}
};

//初始化逻辑有点问题,需要修改
class CardMgr {
public:
    static CardMgr* getInstance();
    void initWithDeck(const std::vector<int>& deck);  // 仅限本关
    void update(float dt);                            // 每帧减冷却
    bool canPlant(int idx) const;                     // 阳光 & 冷却
    void startCool(int idx);                          // 种下即调用
    float getCoolPercent(int idx) const;              // 0~100

    //判断阳光是否足够,不看冷却时间
    bool canAfford(int idx) const {
        return Global::getInstance()->getSun() >= g_cardAtlas[_deck[idx]].sunCost;
    }

    //开始冷却+减少阳光总量
    void onPlantConfirmed(int idx) 
    {     // 真正种下才调用
        startCool(idx);                  // 这里才进冷却
        Global::getInstance()->getSun() -= g_cardAtlas[_deck[idx]].sunCost;
    }
    
    //这个会被赋值为PlantInput中的void onCardSelected(int plantId)
    //auto* cardMgr = CardMgr::getInstance();
    //cardMgr->initWithDeck(LEVEL1_DECK);
    //cardMgr->onCardSelected = [this](int plantId) {
    //   _plantInput->onCardSelected(plantId);
    //   };
    //在CardBarLayer中用来通知PlantInputLayer进行种植
    std::function<void(int)> onCardSelected;
private:
    std::vector<int> _deck;
    struct Runtime { bool inCD = false; float cdLeft = 0.f; };
    std::vector<Runtime> _rt;
    int _sun = 0;
};