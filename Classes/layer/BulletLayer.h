#pragma once
#include "cocos2d.h"
#include"plant/PlantData.h"
#include"plant/Bullet.h"
class BulletLayer : public cocos2d::Layer {
public:
    CREATE_FUNC(BulletLayer);
    virtual bool init() override;

    // 每一帧移动子弹并检测碰撞
    virtual void update(float dt) override;

    // 供植物调用：在指定位置发射子弹
    // row 用于优化碰撞检测，damage 是伤害值
    void spawnBullet(cocos2d::Vec2 pos, int row, float damage, PlantType type);

    void pauseAllBullets();
    void resumeAllBullets();

private:
    cocos2d::Vector<Bullet*> _bullets;
    const float _bulletSpeed = 450.0f; // 子弹飞行速度
};