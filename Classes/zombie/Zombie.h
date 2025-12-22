#pragma once
#include "cocos2d.h"
#include "zombie/ZombieData.h"

// 确保枚举在基类可见
enum class ZombieState { WALK, ATTACK, DIE };

class Zombie : public cocos2d::Node {
public:
    CREATE_FUNC(Zombie);
    virtual bool initWithZombieType(ZombieType type);
    virtual void update(float dt) override;
    void takeDamage(float damage);
    CC_SYNTHESIZE(int, _row, Row);

protected: // 必须是 protected 子类才能访问 _state
    ZombieProperties _props;
    ZombieState _state = ZombieState::WALK;
    cocos2d::Sprite* _mainSprite = nullptr;
    float _currentHealth;
};