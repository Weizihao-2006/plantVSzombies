#pragma once
#include "plant/Plant.h"

class CherryBomb : public Plants 
{
public:
    CREATE_FUNC(CherryBomb);

    virtual bool init() override;

private:
    // 爆炸触发逻辑
    void explode();

    // 播放爆炸特效并销毁
    void playExplosionEffect();
};