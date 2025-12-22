#pragma once
#include"plant/Plant.h"



class WallNut : public Plants {
public:
    CREATE_FUNC(WallNut);

    virtual bool init() override;

    // 重写关键接口：根据血量更新视觉状态
    virtual void updateVisualByHealth() override;


private:
    // 记录当前处于哪种破损阶段，避免重复切换动画造成性能浪费
    NutState _nutState;
};