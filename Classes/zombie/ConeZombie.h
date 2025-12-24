#pragma once
#include "zombie/Zombie.h"

// 继承自 Zombie 基类
class ConeheadZombie : public Zombie {
public:
    CREATE_FUNC(ConeheadZombie);

    // 重写 init
    virtual bool init() override;

    // 重写 update 以处理啃咬逻辑（逻辑与 CommonZombie 一致）
    virtual void update(float dt) override;

protected:
    // 啃咬逻辑
    void eatPlant(float dt, Plants* plant);
    void convertToNormal();

private:
    float _eatTimer = 0.0f;
    bool _isConeBroken;
};