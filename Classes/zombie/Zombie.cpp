#include "Zombie.h"
#include "zombie/ZombieData.h"
#include"plant/PlantData.h"
#include "cocos2d.h"
#include "AudioEngine.h"

using namespace cocos2d;

void Zombie::setDebugRectVisible(bool visible) {
    _showDebugRect = visible;
    if (_showDebugRect) {
        if (!_debugDrawNode) {
            _debugDrawNode = DrawNode::create();
            // 将 DrawNode 添加到自身，坐标系就对齐了
            this->addChild(_debugDrawNode, 999);
        }
    }
    else if (_debugDrawNode) {
        _debugDrawNode->clear();
    }
}

void Zombie::updateDebugRect() {
    if (!_showDebugRect || !_debugDrawNode) return;
    _debugDrawNode->clear();

    // 绘制逻辑受击框 (转换回本地坐标进行绘制)
    Rect hitbox = getHitbox();
    Vec2 origin = this->convertToNodeSpace(hitbox.origin);

    _debugDrawNode->drawRect(origin,
        origin + Vec2(hitbox.size.width, hitbox.size.height),
        Color4F(0, 1, 0, 1.0f)); // 绿色代表逻辑受击区
}
void Zombie::applySlowDown(float duration, float percent)
{
    if (_state == ZombieState::DYING || _state == ZombieState::DEAD || _state == ZombieState::BOOMDIE)
        return;


    _isSlowed = true;
    _currentSpeed = _props.speed * percent; // 设置减速后的速度

    // 视觉反馈：变为蓝色
    if (_mainSprite) {
        _mainSprite->setColor(Color3B(100, 100, 255));
        // 暂停当前动画，重新以倍速运行（或者使用以下更直接的方法）
        // 关键：停止当前的 RepeatForever，重新跑一个带速度缩放的
        auto anim = AnimationCache::getInstance()->getAnimation(_mainSprite->getName());
        if (anim) {
            _mainSprite->stopAllActions(); // 停止当前动画

            // 使用 Speed 动作包装 Animate
            auto animate = Animate::create(anim);
            auto speedAction = Speed::create(RepeatForever::create(animate), percent);
            speedAction->setTag(1001); // 给动画打个标签方便找回
            _mainSprite->runAction(speedAction);

        }
    }

    //防止重复调用
    this->unschedule("recoverFromSlowDown");

    // 开启定时器，duration 秒后恢复
    this->scheduleOnce([this](float dt) {
        this->restoreFromSlowDown(dt);

        }, duration, "recoverFromSlowDown");
}


void Zombie::restoreFromSlowDown(float dt) {
    _isSlowed = false;
    _currentSpeed = _props.speed;

    if (_mainSprite) {
        _mainSprite->setColor(Color3B::WHITE);

        // 恢复 1.0 倍速动画
        auto action = _mainSprite->getActionByTag(1001);
        auto speedAction = dynamic_cast<Speed*>(action);
        if (speedAction) {
            speedAction->setSpeed(1.0f); // 直接将速度设回 1.0
        }
    }
}




bool Zombie::initWithZombieType(ZombieType type) {
    if (!Node::init()) return false;

    _props = ZombieData::getProps(type);
    _currentHealth = _props.health;
    _currentSpeed = _props.speed;
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
    if (_state != ZombieState::ATTACK) {
        // 向左走
        this->setPositionX(this->getPositionX() - _currentSpeed * dt);//修改,使用_currentSpeed
    }

    //颜色更新
    if (_isSlowed) {
        _mainSprite->setColor(Color3B(100, 100, 255));
        if (_headSprite) {
            _headSprite->setColor(Color3B(100, 100, 255));
        }
    }

    if (_showDebugRect) {
        updateDebugRect();
    }
}

Rect Zombie::getHitbox() const 
{
    if (!_mainSprite) 
        return Rect::ZERO;

    // 获取世界坐标下的位置
    Vec2 worldPos = this->convertToWorldSpace(_mainSprite->getPosition());
    Size size = _mainSprite->getContentSize() * this->getScale(); // 考虑节点缩放

    // 默认返回以当前坐标为底边中心的矩形
    return Rect(worldPos.x - size.width / 2, worldPos.y, size.width, size.height);
}

void Zombie::changeAnimation(const std::string& animName) {
    _mainSprite->stopAllActions(); // 停止当前正在播放的动画
    auto anim = cocos2d::AnimationCache::getInstance()->getAnimation(animName);
    if (anim) {
        _mainSprite->runAction(cocos2d::RepeatForever::create(cocos2d::Animate::create(anim)));
    }
}

//增加了一个参数attackPlant,标记最后一次攻击的发出者
void Zombie::takeDamage(float damage, PlantType attackPlant) {
    if (_state == ZombieState::DYING || _state == ZombieState::DEAD || _state == ZombieState::BOOMDIE)
        return;

    _currentHealth -= damage;

    //// 受击闪红反馈
    //_mainSprite->runAction(cocos2d::Sequence::create(
    //    cocos2d::TintTo::create(0.1f, 255, 0, 0),
    //    cocos2d::TintTo::create(0.1f, 255, 255, 255),
    //    nullptr));

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

    // 如果有吃植物的音效，停止它
    if (_soundID != AudioEngine::INVALID_AUDIO_ID) {
        AudioEngine::stop(_soundID);
        _soundID = AudioEngine::INVALID_AUDIO_ID;
    }

    auto specialAnims = ZombieData::getSpecialAnimMap(_props.type);

    // --- 逻辑分支：如果是被炸死 ---
    if (dieType == ZombieState::BOOMDIE) {
        if (specialAnims.count(ZombieState::BOOMDIE)) {
            auto boomData = specialAnims.at(ZombieState::BOOMDIE);
            auto anim = AnimationCache::getInstance()->getAnimation(boomData.animationName);
            if (anim) {
                _mainSprite->runAction(Sequence::create(
                    Animate::create(anim),
                    CallFunc::create([this]() {
                        // 动画播完了，现在执行真正的移除
                        // 这一步执行后，z->getParent() 就会变成 nullptr
                        this->removeFromParent();
                        }),
                    nullptr
                ));
                //this->removeFromParent();
                _state = ZombieState::DEAD;
                return;
            }
        }
        
    }

    // --- 逻辑分支：如果是普通死亡 (DYING) ---
       // 1. 掉脑袋 (Head Loss) - 这个是独立出来的，不需要管移除
    if (specialAnims.count(ZombieState::HEAD_LOSS)) {
        auto headData = specialAnims.at(ZombieState::HEAD_LOSS);
        _headSprite = Sprite::create();
        if (_isSlowed) {
            _headSprite->setColor(Color3B(100, 100, 255));
        }

        _headSprite->setPosition(this->getPosition() + Vec2(0, 60));
        this->getParent()->addChild(_headSprite, this->getLocalZOrder() + 1);
        auto anim = AnimationCache::getInstance()->getAnimation(headData.animationName);
        if (anim) {
            _headSprite->runAction(Sequence::create(Animate::create(anim), RemoveSelf::create(), nullptr));
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
                    AudioEngine::play2d("Music/zombie_falling_1.ogg", false, 1.0f);
                    this->removeFromParent();
                    }),
                nullptr
            );
            _mainSprite->runAction(seq);
        }
    }
    _state = ZombieState::DEAD;
}