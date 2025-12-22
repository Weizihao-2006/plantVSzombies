#include "Zombie.h"

bool Zombie::initWithZombieType(ZombieType type) {
    if (!Node::init()) return false;

    _props = ZombieData::getProps(type);
    _currentHealth = _props.health;

    // 1. 创建视觉主体 (类似 Plant.cpp)
    _mainSprite = cocos2d::Sprite::createWithSpriteFrameName(_props.spriteFrameName);
    if (_mainSprite) {
        _mainSprite->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_BOTTOM);
        this->addChild(_mainSprite);

        // 2. 播放动画 (使用 AnimationHelper 缓存好的动画)
        auto anim = cocos2d::AnimationCache::getInstance()->getAnimation(_props.animationName);
        if (anim) {
            _mainSprite->runAction(cocos2d::RepeatForever::create(cocos2d::Animate::create(anim)));
        }
    }

    this->scheduleUpdate();
    return true;
}

void Zombie::update(float dt) {
    if (_state!=ZombieState::ATTACK) {
        // 向左走
        this->setPositionX(this->getPositionX() - _props.speed * dt);
    }
}

void Zombie::takeDamage(float damage) {
    _currentHealth -= damage;

    // 受击闪红 (复用你的闪红逻辑)
    _mainSprite->runAction(cocos2d::Sequence::create(
        cocos2d::TintTo::create(0.1f, 255, 0, 0),
        cocos2d::TintTo::create(0.1f, 255, 255, 255),
        nullptr));

    if (_currentHealth <= 0) {
        this->removeFromParent();
    }
}