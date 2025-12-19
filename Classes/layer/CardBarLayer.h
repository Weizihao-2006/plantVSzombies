#pragma once
#include <vector>
#include"plant/PlantData.h"
#include"manager/CardMgr.h"
#include"cocos2d.h"

/// <summary>
/// update 2025/12/19
/// 重新划分职责
/// 初始化：遍历 CardMgr 中的卡组（PlantType 列表），从 PlantData 获取图标。
/// 更新：每帧（或者通过事件）向 CardMgr 询问每个卡槽的冷却百分比。
/// 交互：点击卡牌时，直接调用 CardMgr::onCardSelected(type)
/// </summary>

const std::vector<PlantType> CardDeck = {
    PlantType::SunFlower, PlantType::PeaShooter, PlantType::ReaPeater,
    PlantType::CherryBomb, PlantType::SnowPea,PlantType::WallNut

};

class CardBarLayer : public cocos2d::Layer
{
public:
    static CardBarLayer* create();
    bool init();
    void update(float dt) override;

private:
    // 创建卡牌槽背景
    void createCardSlotBg();
    

    CardMgr* _cardMgr;//卡片管理者
    cocos2d::Menu* _menu = nullptr;
    std::vector<cocos2d::ProgressTimer*> _cdBars;
    std::vector<cocos2d::Node*> _cardSlots;  // 存储卡牌槽节点
    cocos2d::Node* _cardBarBg = nullptr;     // 卡牌栏背景
};