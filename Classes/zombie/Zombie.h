#pragma once
#include "cocos2d.h"
#include "zombie/ZombieData.h"
#include"plant/PlantData.h"



class Zombie : public cocos2d::Node {
public:
    CREATE_FUNC(Zombie);
    virtual bool initWithZombieType(ZombieType type);
    virtual void update(float dt) override;
    void takeDamage(float damage, PlantType attackPlant);
    CC_SYNTHESIZE(int, _row, Row);

    // 增加切换动画的方法
    void changeAnimation(const std::string& animName);
    // 增加获取当前状态的方法
    ZombieState getState() const { return _state; }
    bool isDead()const { return _state == ZombieState::DEAD; }
    // 增加参数，默认为普通死亡 (DYING)
    virtual void onDie(ZombieState dieType = ZombieState::DYING);

protected: // 必须是 protected 子类才能访问 _state
    ZombieProperties _props;
    ZombieState _state = ZombieState::WALK;
    cocos2d::Sprite* _mainSprite = nullptr;//指的是正常行走的状态
    float _currentHealth;
};