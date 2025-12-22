#pragma once
#include "Plant.h"

class PeaShooter : public Plants {
public:
    CREATE_FUNC(PeaShooter);
    virtual bool init() override;

    // 每帧检测逻辑
    virtual void update(float dt) override;

protected:
    // 执行具体的射击动作
    virtual void attack() override;

private:
    
};