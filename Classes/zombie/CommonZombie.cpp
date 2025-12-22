#include "zombie/CommonZombie.h"
#include "manager/MapMgr.h"
#include "layer/PlantLayer.h"

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
    if (_state == ZombieState::DEAD)
        return;

    // 1. 检测前方是否有植物
    auto scene = Director::getInstance()->getRunningScene();
    auto plantLayer = dynamic_cast<PlantLayer*>(scene->getChildByName("PlantLayer"));

    bool findPlant = false;
    if (plantLayer) {
        // 获取所有植物进行范围检测 (简单的矩形碰撞)
        auto& allPlants = plantLayer->getAllPlants();
        for (auto plant : allPlants) {
            // 必须在同一行，且植物在僵尸左侧的很近范围内
            float dist = this->getPositionX() - plant->getPositionX();
            // 获取僵尸行号 (基类定义的 _row)
            // 这里假设你的植物类也能通过坐标拿到 row，或者在种植时已经存了 row
            // 简单处理：判断 Y 坐标是否接近
            if (abs(this->getPositionY() - plant->getPositionY()) < 50.0f) {
                if (dist > 0 && dist < 60.0f) { // 触碰距离
                    findPlant = true;
                    if (_state != ZombieState::ATTACK) {
                        _state = ZombieState::ATTACK;
                        // 这里可以切换到“啃咬动画”，如果你的 ZombieData 配了的话
                        // _mainSprite->runAction(...)
                    }
                    this->eatPlant(dt); // 执行啃咬逻辑
                    break;
                }
            }
        }
    }

    // 2. 如果没发现植物，继续行走
    if (!findPlant) {
        if (_state == ZombieState::ATTACK) {
            _state = ZombieState::WALK;
            // 恢复行走动画逻辑（此处可根据需要重新播放行走动画）
        }
        // 调用基类的移动逻辑
        Zombie::update(dt);
    }
}

void CommonZombie::eatPlant(float dt) {
    _eatTimer += dt;
    if (_eatTimer >= _props.attackInterval) {
        // 执行伤害
        // 注意：这里需要根据坐标通过 MapManager 找到对应的植物对象并调用 takeDamage
        // 为了演示简单，我们在 update 循环里已经找到了植物
        _eatTimer = 0.0f;
        // CCLOG("Zombie is eating plant!");
    }
}