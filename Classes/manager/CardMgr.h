#pragma once

#include <vector>
#include <functional>
#include "util/Global.h"

/*
流程说明：
展示卡牌-> 等待点击 (in CardBarLayer:: onCardClicked) -> CardMgr::onCardSelected ->
PlantInputLayer 处理预览 -> 移动鼠标 -> onTouchEnded ->
PlantMgr::createPlantAt (创建植物播放) + CardMgr::onPlantConfirmed (启动冷却)
*/

struct CardDef {
    int   id;
    const char* icon;
    int   sunCost;
    float coolTime;
};

const std::vector<CardDef> g_cardAtlas = {
    {0, "plantCard/SunFlower.png",    50, 7.5f},
    {1, "plantCard/CherryBomb.png",  150, 30.0f},
    {2, "plantCard/PeaShooter.png",  100, 7.5f},
    {3, "plantCard/Repeater.png",    200, 7.5f},
    {4, "plantCard/SnowPea.png",     175, 7.5f},
    {5, "plantCard/Wallnut.png",      50, 35.0f},
    {6, "plantCard/Wallnut.png",      50, 35.0f},
    {7, "plantCard/Wallnut.png",      50, 35.0f}
};

// 初始化逻辑中的卡组，如需修改
class CardMgr {
public:
    static CardMgr* getInstance();

    // 初始化卡组
    void initWithDeck(const std::vector<int>& deck);

    // 每帧更新冷却
    void update(float dt);

    // 检查是否能种植（阳光 & 冷却）
    bool canPlant(int idx) const;

    // 启动冷却计时
    void startCool(int idx);

    // 获取冷却进度 0~100
    float getCoolPercent(int idx) const;

    // 获取冷却时间（剩余秒数）
    float getCoolTimeLeft(int idx) const;

    // 判断是否在冷却中
    bool isInCoolDown(int idx) const {
        return _rt[idx].inCD;
    }

    // 检查阳光是否足够
    bool canAfford(int idx) const {
        if (idx < 0 || idx >= _deck.size()) return false;
        return Global::getInstance()->getSun() >= g_cardAtlas[_deck[idx]].sunCost;
    }

    // 植物确认种植后调用（扣阳光 + 启动冷却）
    void CardMgr::onPlantConfirmed(int idx) {
        if (idx < 0 || idx >= _deck.size()) return;

        const auto& cardDef = g_cardAtlas[_deck[idx]];
        auto* global = Global::getInstance();

        // 检查并消耗阳光（更安全的做法）
        if (global->consumeSun(cardDef.sunCost)) {
            // 阳光足够，启动冷却
            startCool(idx);
            CCLOG("CardMgr:  Plant [%d] confirmed, cooldown started", idx);
        }
        else {
            // 阳光不足（这种情况不应该发生，因为 canPlant 已经检查过）
            CCLOG("CardMgr:  ERROR - Not enough sun for plant [%d]", idx);
        }
    }

    // 当卡牌被点击时调用
    // 由 CardBarLayer::onCardClicked 调用
    // 参数 idx:  卡牌在卡组中的索引
    std::function<void(int)> onCardSelected;

    // 获取卡牌定义
    const CardDef& getCardDef(int idx) const {
        return g_cardAtlas[_deck[idx]];
    }

    // 获取卡组大小
    size_t getDeckSize() const {
        return _deck.size();
    }

private:
    std::vector<int> _deck;  // 卡组（存储卡牌ID）

    struct Runtime {
        bool inCD = false;      // 是否在冷却中
        float cdLeft = 0.f;     // 剩余冷却时间
    };

    std::vector<Runtime> _rt;  // 运行时状态（与 _deck 对应）
};