#include "PeaShooter.h"
#include "manager/MapMgr.h"
#include "layer/ZombieLayer.h"
#include "layer/BulletLayer.h"

bool PeaShooter::init() {
    if (!initWithPlantType(PlantType::PeaShooter)) {
        return false;
    }

    this->scheduleUpdate();
    return true;
}

void PeaShooter::update(float dt) {
    if (_currentState == PlantState::DYING) return;

    _timer += dt;

    if (_timer >= _properties.attackInterval) {
        // 1. 寻找当前场景中的 ZombieLayer
        auto scene = Director::getInstance()->getRunningScene();
        auto zombieLayer = dynamic_cast<ZombieLayer*>(scene->getChildByName("ZombieLayer"));

        if (zombieLayer) {
            // 2. 检测当前行右侧是否有僵尸
            // 只有当这一行有僵尸，且僵尸在自己右边时才开火
            if (zombieLayer->hasZombieInRow(static_cast<int>(_myMapPos.x), this->getPositionX())) {
                this->attack();
                _timer = 0.0f; // 重置攻击冷却
            }
        }
    }
}

void PeaShooter::attack() {
    //// 1. 视觉：可以给豌豆射手加一个瞬间缩放或闪烁的效果，表示“吐”子弹
    //_mainSprite->runAction(Sequence::create(
    //    ScaleTo::create(0.05f, 1.1f * NormalScale, 0.9f * NormalScale),
    //    ScaleTo::create(0.05f, NormalScale, NormalScale),
    //    nullptr
    //));

    // 2. 逻辑：在 BulletLayer 中生成子弹
    auto scene = Director::getInstance()->getRunningScene();
    auto bulletLayer = dynamic_cast<BulletLayer*>(scene->getChildByName("BulletLayer"));

    if (bulletLayer) {
        // 子弹发射位置：通常是植物的头部中心点，略微靠右
        Vec2 firePos = this->getPosition() + Vec2(40, 30);
        bulletLayer->spawnBullet(firePos, static_cast<int>(_myMapPos.x), _properties.attackPower,_properties.type);

        // 3. 音效：发射声音
        // AudioEngine::play2d("sound/throw.mp3", false, 0.6f);
    }
}