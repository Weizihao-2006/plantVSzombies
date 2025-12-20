#include"Plant.h"
USING_NS_CC;



bool Plants::initWithPlantType(PlantType type)
{
    if (!Node::init())
        return false;

    // 1. 直接从配置表拉取属性
    _properties = PlantData::getProps(type);

    // 2. 数据初始化
    _currentHealth = _properties.health;
    _attackInterval = _properties.attackInterval;
    _currentState = PlantState::IDLE;
    _timer = 0;

    // 3. 创建视觉主体
    // 使用配置表中的预览图或第一帧名
    _mainSprite = Sprite::createWithSpriteFrameName(_properties.spriteFrameName);
    if (_mainSprite) {
        this->addChild(_mainSprite);

        // 4. 绑定预加载动画
        auto animation = AnimationCache::getInstance()->getAnimation(_properties.animationName);
        if (animation) {
            _mainSprite->runAction(RepeatForever::create(Animate::create(animation)));
        }
    }

    

    return true;
}