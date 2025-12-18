#include"plant/SunFlower.h"
#include"scene/GameScene.h"


void SunFlower::produceResource() {
    // 1. 视觉反馈：向日葵闪烁一下（暗示要产阳光了）
    auto tint = Sequence::create(
        TintTo::create(0.2f, 255, 255, 100),
        TintTo::create(0.2f, 255, 255, 255),
        nullptr
    );

    // 2. 产生阳光：通过 GameScene 获取 SunLayer
    auto callback = CallFunc::create([this]() {
        auto scene = dynamic_cast<GameScene*>(this->getScene());
        if (scene && scene->getSunLayer()) {
            // 在向日葵当前位置产生一个阳光
            // 这个阳光会有一个“跳出来”的抛物线动画
            scene->getSunLayer()->spawnSunFromPlant(this->getPosition());
        }
        });

    _mainSprite->runAction(Sequence::create(tint, callback, nullptr));
}