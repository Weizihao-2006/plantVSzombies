#include "plant/CherryBomb.h"
#include "zombie/Zombie.h"
#include "layer/ZombieLayer.h"
#include "AudioEngine.h"
#include "manager/MapMgr.h"

bool CherryBomb::init() 
{
    if (!initWithPlantType(PlantType::CherryBomb)) {
        return false;
    }

    // 樱桃炸弹不需要每帧更新逻辑 (scheduleUpdate)
    // 它在动画结束后触发

    auto animation = AnimationCache::getInstance()->getAnimation(_properties.animationName);
    if (animation) {
        // 1. 停止基类默认的 RepeatForever
        _mainSprite->stopAllActions();

        // 2. 播放一次准备动画，结束后调用 explode
        auto animate = Animate::create(animation);
        auto callback = CallFunc::create(CC_CALLBACK_0(CherryBomb::explode, this));
        _mainSprite->runAction(Sequence::create(animate, callback, nullptr));
    }

    return true;
}

void CherryBomb::explode() {
    auto scene = Director::getInstance()->getRunningScene();
    auto zombieLayer = dynamic_cast<ZombieLayer*>(scene->getChildByName("ZombieLayer"));

    if (zombieLayer) {
        auto& allZombies = zombieLayer->getAllZombies();

        // convertToWorldSpace(Vec2::ZERO) 获取当前节点原点在屏幕上的绝对位置
        Vec2 myWorldPos = this->convertToWorldSpace(Vec2::ZERO);

        float explosionRadius = 290.0f;

        for (int i = allZombies.size() - 1; i >= 0; --i) {
            auto zombie = allZombies.at(i);

            // --- 修改点 2：获取僵尸的世界坐标 ---
            // 同样将僵尸的局部位置转为世界位置
            Vec2 zombieWorldPos = zombie->getParent()->convertToWorldSpace(zombie->getPosition());

            // --- 修改点 3：使用世界坐标计算距离 ---
            float dist = myWorldPos.distance(zombieWorldPos);

            if (dist <= explosionRadius) {
                zombie->takeDamage(_properties.attackPower, _properties.type);
            }
        }
    }

    AudioEngine::play2d("Music/cherrybomb.ogg", false, 1.0f);
    handleDeath();
}