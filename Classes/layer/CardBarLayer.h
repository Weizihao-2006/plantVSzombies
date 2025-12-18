#pragma once
#include <vector>
#include"cocos2d.h"
class CardBarLayer : public cocos2d::Layer
{
public:
    static CardBarLayer* create();
    bool init();

    // 更新冷却条显示
    void updateCoolDown(int idx, float percent);

    // 更新卡牌按钮状态（禁用/启用）
    void updateCardState(int idx, bool canUse);

private:
    // 创建卡牌槽背景
    void createCardSlotBg();

    // 处理卡牌点击
    void onCardClicked(int idx);

    cocos2d::Menu* _menu = nullptr;
    std::vector<cocos2d::ProgressTimer*> _cdBars;
    std::vector<cocos2d::Node*> _cardSlots;  // 存储卡牌槽节点
    cocos2d::Node* _cardBarBg = nullptr;     // 卡牌栏背景
};