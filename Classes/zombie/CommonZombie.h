#pragma once
#include "zombie/Zombie.h"

class CommonZombie : public Zombie {
public:
    CREATE_FUNC(CommonZombie);
    virtual bool init() override;

    // 重写更新逻辑：处理“移动”与“啃咬”状态的切换
    virtual void update(float dt) override;

protected:
    // 啃咬逻辑
    void eatPlant(float dt);

private:
    float _eatTimer = 0.0f;
};