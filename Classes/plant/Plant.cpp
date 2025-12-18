#include"Plant.h"
USING_NS_CC;



bool Plants::initWithPlantProperties(const PlantProperties& props) 
{
    if (!Node::init()) 
        return false;

    // 1. 数据同步
    _properties = props;
    _currentHealth = props.health;
    _attackInterval = props.attackInterval;
    _currentState = PlantState::IDLE;
    _timer = 0;

    // 2.统一创建第一帧图片(视觉主体)
    // 必须要已经加载了.plist
    _mainSprite = Sprite::createWithSpriteFrameName(props.spriteFrameName);
    if (_mainSprite) {
        this->addChild(_mainSprite);
    }
    // 3. 启动植物通用的update
    this->scheduleUpdate();

    return true;
}