#pragma once
#include "cocos2d.h"
#include "zombie/Zombie.h"

class LawnMower : public cocos2d::Node {
public:
    static LawnMower* create(int row);
    virtual bool init(int row);
    virtual void update(float dt) override;

private:
    int _row;
    bool _isActivated = false; // 是否已激活
    float _speed = 400.0f;     // 冲锋速度
    cocos2d::Sprite* _sprite;
};