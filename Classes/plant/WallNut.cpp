#include"plant/WallNut.h"

USING_NS_CC;




bool WallNut::init() 
{
    if (!initWithPlantType(PlantType::WallNut)) 
    {
        return false;
    }
    _currentState = PlantState::IDLE; // 默认为健康状态

    return true;
}

void WallNut::updateVisualByHealth() 
{
    // 1. 计算当前血量百分比
    float healthPercent = static_cast<float>(_currentHealth / _properties.health);

    auto nextState = _currentState;

    // 2. 根据阈值判断应该进入哪个状态
    if (healthPercent > 0.7f) {
        nextState = PlantState::IDLE;
    }
    else if (healthPercent > 0.3f) {
        nextState = PlantState::DAMAGED;
    }
    else {
        nextState = PlantState::CRITICAL;
    }

    // 3. 如果状态改变，则切换动画（避免每帧重复调用）
    if (nextState != _currentState)
    {
        auto props = PlantData::getProps(PlantType::WallNut);
        auto specialAnimProps = PlantData::getSpecialAnimMap(PlantType::WallNut);

        _currentState = nextState;

        std::string animName = "";
        animName = specialAnimProps.at(_currentState).animationName;

        // 切换动画
        auto animation = AnimationCache::getInstance()->getAnimation(animName);
        if (animation) {
            _mainSprite->stopAllActions(); // 停止旧状态的循环动画
            _mainSprite->runAction(RepeatForever::create(Animate::create(animation)));
        }
    }
}



