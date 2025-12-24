#include "zombie/CommonZombie.h"
#include "manager/MapMgr.h"
#include "layer/PlantLayer.h"
#include "manager/MapMgr.h"
#include "AudioEngine.h"

bool CommonZombie::init() {
    // 1. 调用基类的初始化，传入类型，自动加载配置和播放行走动画
    if (!Zombie::initWithZombieType(ZombieType::Normal)) {
        return false;
    }

    // 2. 初始状态为行走
    _state = ZombieState::WALK;

    this->scheduleUpdate();
    return true;
}

void CommonZombie::update(float dt) {
    if (_state == ZombieState::DYING || _state == ZombieState::DEAD) return;

    auto scene = Director::getInstance()->getRunningScene();
    auto plantLayer = dynamic_cast<PlantLayer*>(scene->getChildByName("PlantLayer"));

    bool findPlant = false;
    if (plantLayer) {
        auto& allPlants = plantLayer->getAllPlants();
        for (auto plant : allPlants) {
            float dist = this->getPositionX() - plant->getPositionX();
            auto mapPos = MapManager::getInstance()->getPositionInMap(this->getRow(), 0);
            if (abs(mapPos.y - plant->getPositionY()) < 50.0f) {
                if (dist > 0 && dist < 60.0f) {
                    findPlant = true;
                    if (_state != ZombieState::ATTACK) {
                        _state = ZombieState::ATTACK;
                        // 切换到啃咬动画
                        auto anims = ZombieData::getSpecialAnimMap(_props.type);
                        if (anims.count(ZombieState::ATTACK)) {
                            this->changeAnimation(anims.at(ZombieState::ATTACK).animationName);
                        }
                    }
                    this->eatPlant(dt, plant); // 传入植物对象以便扣血
                    break;
                }
            }
        }
    }

    if (!findPlant && _state == ZombieState::ATTACK) {
        _state = ZombieState::WALK;
        this->changeAnimation(_props.animationName); // 恢复行走动画
    }

    if (_state == ZombieState::WALK) {
        Zombie::update(dt);
    }
}

void CommonZombie::eatPlant(float dt, Plants* plant) {
    _eatTimer += dt;
    if (_eatTimer >= _props.attackInterval) {
        _eatTimer = 0.0f;
        // 假设你的植物基类 Plants 有 takeDamage 方法
        plant->takeDamage(_props.attackPower);

        // 可以在这里播放一个“咔嚓”的声音特效
        // AudioEngine::play2d("Music/chompsoft.ogg", true, 1.0f);
    }
}