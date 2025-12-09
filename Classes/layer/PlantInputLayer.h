#pragma once
#include"cocos2d.h"
#include <functional>


class PlantInputLayer : public cocos2d::Layer {
public:
    CREATE_FUNC(PlantInputLayer);
    bool init() override;
    /* 外部调：CardMgr 通知“玩家选了哪张卡” */
    void onCardSelected(int plantId);
private:

    int _selectedId = -1;                    // -1 表示没选卡
    cocos2d::Sprite* _ghost = nullptr;       // 半透明预览
    cocos2d::EventListenerTouchOneByOne* _touchListener;

    //获取种植的坐标
    //根据需要修改
    cocos2d::Vec2 getRowCol(const cocos2d::Vec2& worldPos);
};