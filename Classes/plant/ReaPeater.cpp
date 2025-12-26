#include "plant/ReaPeater.h"
#include "layer/BulletLayer.h"

bool RePeater::init() {
    if (!initWithPlantType(PlantType::ReaPeater)) 
        return false;
    this->scheduleUpdate();
    return true;
}

void RePeater::attack() {
    auto scene = Director::getInstance()->getRunningScene();
    auto bulletLayer = dynamic_cast<BulletLayer*>(scene->getChildByName("BulletLayer"));
    if (bulletLayer) {
        // 1. 发射第一颗子弹
        bulletLayer->spawnBullet(this->getPosition() + Vec2(30, 20),
            static_cast<int>(_myMapPos.x),
            _properties.attackPower,
            _properties.type);

        // 2. 延时 0.15 秒发射第二颗子弹，模拟双发效果
        auto delay = DelayTime::create(0.15f);
        auto shootAgain = CallFunc::create([this, bulletLayer]() {
            bulletLayer->spawnBullet(this->getPosition() + Vec2(30, 20),
                static_cast<int>(_myMapPos.x),
                _properties.attackPower,
                _properties.type);
            });
        this->runAction(Sequence::create(delay, shootAgain, nullptr));
    }
}