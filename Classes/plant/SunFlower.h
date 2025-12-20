#pragma once
#include"cocos2d.h"
#include"plant/Plant.h"

USING_NS_CC;

class SunFlower : public Plants {
public:
    CREATE_FUNC(SunFlower);

    virtual bool init() override;
    virtual void update(float dt) override;

    // 重写资源生产接口
    virtual void produceResource() override;

private:
    // 可以添加向日葵特有的视觉效果，比如生产阳光时的“闪光”
    void playProduceEffect();
};