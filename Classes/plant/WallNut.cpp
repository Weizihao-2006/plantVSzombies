#include"plant/WallNut.h"

USING_NS_CC;

bool WallNut::init() 
{
    if (!initWithPlantType(PlantType::WallNut)) 
    {
        return false;
    }

    _nutState = NutState::Health; // 默认为健康状态
    return true;
}

void WallNut::updateVisualByHealth() 
{
    // 1. 计算当前血量百分比
    float healthPercent = static_cast<float>(_currentHealth) / _properties.health;

    NutState nextState = _nutState;

    // 2. 根据阈值判断应该进入哪个状态
    if (healthPercent > 0.7f) {
        nextState = NutState::Health;
    }
    else if (healthPercent > 0.3f) {
        nextState = NutState::Damaged;
    }
    else {
        nextState = NutState::Critical;
    }

    // 3. 如果状态改变，则切换动画（避免每帧重复调用）
    if (nextState != _nutState) 
    {
        _nutState = nextState;

        std::string animName = "";
        switch (_nutState) {
            case NutState::Health:  animName = "WallNut_Idle"; break;
            case NutState::Damaged: animName = "WallNut_Cracked1"; break;
            case NutState::Critical:animName = "WallNut_Cracked2"; break;
        }

        // 切换动画
        auto animation = AnimationCache::getInstance()->getAnimation(animName);
        if (animation) {
            _mainSprite->stopAllActions(); // 停止旧状态的循环动画
            _mainSprite->runAction(RepeatForever::create(Animate::create(animation)));
        }
    }
}

