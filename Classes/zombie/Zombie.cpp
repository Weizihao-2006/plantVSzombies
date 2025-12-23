#include "Zombie.h"
#include "zombie/ZombieData.h"
#include"plant/PlantData.h"
#include "cocos2d.h"

using namespace cocos2d;

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

void Zombie::changeAnimation(const std::string& animName) {
    _mainSprite->stopAllActions(); // 停止当前正在播放的动画
    auto anim = cocos2d::AnimationCache::getInstance()->getAnimation(animName);
    if (anim) {
        _mainSprite->runAction(cocos2d::RepeatForever::create(cocos2d::Animate::create(anim)));
    }
}

//增加了一个参数attackPlant,标记最后一次攻击的发出者
void Zombie::takeDamage(float damage,PlantType attackPlant) {
    if (_state == ZombieState::DYING || _state == ZombieState::DEAD||_state==ZombieState::BOOMDIE) 
        return;

    _currentHealth -= damage;

    // 受击闪红反馈
    _mainSprite->runAction(cocos2d::Sequence::create(
        cocos2d::TintTo::create(0.1f, 255, 0, 0),
        cocos2d::TintTo::create(0.1f, 255, 255, 255),
        nullptr));

    if (_currentHealth <= 0) {

        ZombieState dieType = ZombieState::DYING;
        switch (attackPlant) {

            case PlantType::CherryBomb:
                dieType = ZombieState::BOOMDIE;
                break;
            default:
                break;

        }
        onDie(dieType); // 触发死亡流程
    }
}

void Zombie::onDie(ZombieState dieType) 
{
    if (_state == ZombieState::DEAD) 
        return;
    _state = dieType; // 设置为 DYING 或 BOOMDIE

    _mainSprite->stopAllActions();
    this->unscheduleUpdate();

    auto specialAnims = ZombieData::getSpecialAnimMap(_props.type);

    // --- 逻辑分支：如果是被炸死 ---
    if (dieType == ZombieState::BOOMDIE) {
        if (specialAnims.count(ZombieState::BOOMDIE)) {
            auto boomData = specialAnims.at(ZombieState::BOOMDIE);
            auto anim = AnimationCache::getInstance()->getAnimation(boomData.animationName);
            if (anim) {
                _mainSprite->runAction(Sequence::create(
                    Animate::create(anim),
                    RemoveSelf::create(),
                    nullptr
                ));
            }
        }
        //this->removeFromParent();
        _state = ZombieState::DEAD;
        return;
    }

    // --- 逻辑分支：如果是普通死亡 (DYING) ---
    // 1. 掉脑袋逻辑 (HEAD_LOSS)
    if (specialAnims.count(ZombieState::HEAD_LOSS)) {
        auto headData = specialAnims.at(ZombieState::HEAD_LOSS);
        auto head = Sprite::create();
        head->setPosition(this->getPosition() + Vec2(0, 60));
        this->getParent()->addChild(head, this->getLocalZOrder() + 1);

        auto anim = AnimationCache::getInstance()->getAnimation(headData.animationName);
        if (anim) {
            head->runAction(Sequence::create(Animate::create(anim), RemoveSelf::create(), nullptr));
        }
    }

    // 2. 播放对应的身体倒地动画
    if (specialAnims.count(dieType)) {
        auto dieData = specialAnims.at(dieType);
        auto anim = AnimationCache::getInstance()->getAnimation(dieData.animationName);
        if (anim) {
            _mainSprite->runAction(Sequence::create(
                Animate::create(anim),
                DelayTime::create(0.5f),//这个参数
                RemoveSelf::create(),
                nullptr
            ));
        }
    }
    _state = ZombieState::DEAD;
}