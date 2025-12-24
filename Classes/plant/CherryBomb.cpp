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
    // 1. 查找场景中的僵尸层
    auto scene = Director::getInstance()->getRunningScene();
    auto zombieLayer = dynamic_cast<ZombieLayer*>(scene->getChildByName("ZombieLayer"));

    if (zombieLayer) {
        auto& allZombies = zombieLayer->getAllZombies();
        Vec2 myPos = this->getPosition();

        // 2. 遍历所有僵尸，进行范围检测 3x3区域
        float explosionRadius = 450.0f;

        for (int i = allZombies.size() - 1; i >= 0; --i) {
            auto zombie = allZombies.at(i);
            float dist = myPos.distance(zombie->getPosition());

            if (dist <= explosionRadius) {
                // 造成巨额伤害（配置表中的 attackPower）
                zombie->takeDamage(_properties.attackPower,_properties.type);
            }
        }
    }

    AudioEngine::play2d("Music/cherrybomb.ogg", false, 1.0f);
    _mainSprite->setVisible(false);

    // 从MapMgr当中移除
    auto MapMgr = MapManager::getInstance();
    MapMgr->setMapCellStatus(static_cast<int>(_myMapPos.x), static_cast<int>(_myMapPos.y), PlantType::Error);
}

void CherryBomb::playExplosionEffect() {
    // 隐藏植物主体
    
}