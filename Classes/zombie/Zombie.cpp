#include "Zombie.h"
#include "zombie/ZombieData.h"
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

// 修改 takeDamage 以支持死亡动画
void Zombie::takeDamage(float damage) {
    if (_state == ZombieState::DYING || _state == ZombieState::DEAD) return;

    _currentHealth -= damage;

    // 受击闪红反馈
    _mainSprite->runAction(cocos2d::Sequence::create(
        cocos2d::TintTo::create(0.1f, 255, 0, 0),
        cocos2d::TintTo::create(0.1f, 255, 255, 255),
        nullptr));

    if (_currentHealth <= 0) {
        onDie(); // 触发死亡流程
    }
}

void Zombie::onDie(ZombieState dieType) {
    if (_state == ZombieState::DEAD) return;
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
        else {
            this->removeFromParent(); // 如果没配炸死动画，直接消失
        }
        return; // 炸死通常不掉头，直接结束
    }

    // --- 逻辑分支：如果是普通死亡 (DYING) ---
    // 1. 掉脑袋 (Head Loss) - 这个是独立出来的，不需要管移除
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

    // 2. 处理身体倒地逻辑
    bool hasDieAnimation = false;
    if (specialAnims.count(dieType)) {
        auto dieData = specialAnims.at(dieType);
        auto anim = AnimationCache::getInstance()->getAnimation(dieData.animationName);
        if (anim) {
            hasDieAnimation = true;
            // 核心修改点：把 removeFromParent 包装进 Sequence
            auto seq = Sequence::create(
                Animate::create(anim),
                DelayTime::create(0.5f), // 尸体停留一会儿
                CallFunc::create([this]() {
                    // 动画播完了，现在执行真正的移除
                    // 这一步执行后，z->getParent() 就会变成 nullptr
                    this->removeFromParent();
                    }),
                nullptr
            );
            _mainSprite->runAction(seq);
        }
    }

    // 3. 兜底逻辑：如果没有找到倒地动画，则立即执行移除
    if (!hasDieAnimation) {
        this->removeFromParent();
    }
}