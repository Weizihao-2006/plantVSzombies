#pragma once
#include "zombie/CommonZombie.h"

// ImpZombie.h
class ImpZombie : public CommonZombie {
public:
    CREATE_FUNC(ImpZombie);
    virtual bool init() override;
    virtual ZombieType getType() override { return ZombieType::Imp; }
    virtual void update(float dt) override;
    // ´¦ÀíÅ×³öÂß¼­
    void flyTo(float targetX);
protected:
    bool _isFlying = false;
};

