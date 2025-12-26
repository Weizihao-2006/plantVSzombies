#pragma once
#include "zombie/Zombie.h"
#include "plant/Plant.h"

class GiantZombie : public Zombie {
public:
    CREATE_FUNC(GiantZombie);
    virtual bool init() override;
    virtual void update(float dt) override;
    virtual void onDie(ZombieState dieType = ZombieState::DYING) override;
    void throwImp();

    virtual Rect getHitbox() const;
protected:
    // 重写受伤逻辑，用于处理半血丢小鬼和切换行走姿态
    void handleStateSwitch();
    
private:
    bool _hasThrownImp = false; // 是否已经丢过小鬼
    float _attackCooldown = 0.0f;
};