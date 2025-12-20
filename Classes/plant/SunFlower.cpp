#include"plant/SunFlower.h"
#include"scene/GameScene.h"

bool SunFlower::init() 
{
    // 调用我们重构后的基类初始化，自动加载配置、设置动画、启动update
    if (!initWithPlantType(PlantType::SunFlower)) {
        return false;
    }
    // 5. 启动更新逻辑
    this->scheduleUpdate();

    // 向日葵刚种下时，通常会给一个初始计时随机偏移，避免所有向日葵同时产阳光
    _timer = CCRANDOM_0_1() * 3.0f;
    return true;
}

void SunFlower::update(float dt) {
    // 只有在非死亡状态下才生产
    if (_currentState == PlantState::DYING) 
        return;

    _timer += dt;
    if (_timer >= _properties.attackInterval) {
        this->produceResource();
        _timer = 0; // 重置计时器
    }
}

void SunFlower::produceResource() 
{
    // 1. 播放生产特效（比如向日葵变亮一下）
    playProduceEffect();

    // 2. 逻辑产出：通知阳光层在当前位置生成一个阳光
    // 这里的 getPosition() 是植物在地图上的中心点
    // 我们通常让阳光从植物位置“弹跳”出来
    auto scene = Director::getInstance()->getRunningScene();
    auto sunLayer = dynamic_cast<SunLayer*>(scene->getChildByName("SunLayer"));
    if (sunLayer) {
        sunLayer->spawnSunFromPlant(this->getPosition());
    }
}

void SunFlower::playProduceEffect()
{
    // 简单的视觉反馈：放大缩小一下，并变亮
    auto scaleUp = ScaleTo::create(0.2f, 1.3f);
    auto scaleBack = ScaleTo::create(0.2f, 1.0f);
    auto tint = TintTo::create(0.2f, Color3B::YELLOW);
    auto tintBack = TintTo::create(0.2f, Color3B::WHITE);

    _mainSprite->runAction(Sequence::create(
        Spawn::create(scaleUp, tint, nullptr),
        Spawn::create(scaleBack, tintBack, nullptr),
        nullptr
    ));
}