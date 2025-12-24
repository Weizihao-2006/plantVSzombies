#pragma once
#include "Plant.h"


class SnowPea : public Plants 
{
public:
    CREATE_FUNC(SnowPea);
    virtual bool init() override;
    virtual void update(float dt) override;
protected:
    virtual void attack() override;

    float _slowPercent = 0.5f;   // 减速到原速的 50%
    float _slowDuration = 3.0f; // 持续 3 秒
};