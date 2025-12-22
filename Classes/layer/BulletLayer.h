#pragma once
#include "cocos2d.h"

class BulletLayer : public cocos2d::Layer {
public:
    CREATE_FUNC(BulletLayer);
    virtual bool init() override;

    // 每一帧移动子弹并检测碰撞
    virtual void update(float dt) override;

    // 供植物调用：在指定位置发射子弹
    // row 用于优化碰撞检测，damage 是伤害值
    void spawnBullet(cocos2d::Vec2 pos, int row, float damage);

    void pauseAllBullets();
    void resumeAllBullets();

private:
    cocos2d::Vector<cocos2d::Sprite*> _bullets;
    const float _bulletSpeed = 450.0f; // 子弹飞行速度
};