#pragma once
#include"plant/Plant.h"
#include"plant/PlantData.h"

class WallNut : public Plants {
public:
    CREATE_FUNC(WallNut);

    virtual bool init() override;

    // 重写关键接口：根据血量更新视觉状态
    virtual void updateVisualByHealth() override;

    virtual void update(float dt) override;

};