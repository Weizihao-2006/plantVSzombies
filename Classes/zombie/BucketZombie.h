#pragma once
#include "zombie/Zombie.h"

class BucketheadZombie : public Zombie {
public:
    CREATE_FUNC(BucketheadZombie);

    virtual bool init() override;
    virtual void update(float dt) override;

protected:
    void eatPlant(float dt, Plants* plant);
    void convertToNormal(); // 铁桶掉落后转为普通僵尸
    virtual ZombieType getType() { return ZombieType::Buckethead; }
private:
    float _eatTimer = 0.0f;
    bool _isArmorBroken = false;
};