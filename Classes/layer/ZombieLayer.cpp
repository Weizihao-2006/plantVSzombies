#include "layer/ZombieLayer.h"

USING_NS_CC;

bool ZombieLayer::init() {
    if (!Layer::init()) return false;
    this->scheduleUpdate();
    return true;
}

void ZombieLayer::addZombie(Zombie* zombie) {
    if (!zombie) return;
    this->addChild(zombie);
    _allZombies.pushBack(zombie);
}

//row:通常是植物所在行,minX通常是植物的X坐标
bool ZombieLayer::hasZombieInRow(int row, float minX) {
    bool isHit = false;
    for (auto z : _allZombies) {

        if (z->getRow() == row) {
            isHit = true;
            break;
        }
    }
    return isHit;
}

void ZombieLayer::update(float dt) {
    // 记录是否已经触发过失败，防止一帧内多次触发
    bool houseReached = false;

    for (int i = _allZombies.size() - 1; i >= 0; --i) {
        auto z = _allZombies.at(i);

        // 1. 如果僵尸已被移除，安全删除索引并继续下一个循环
        if (z->getParent() == nullptr) {
            _allZombies.erase(i);
            continue; // 必须 continue，跳过后续对 z 的操作
        }

        // 2. 判定失败逻辑
        if (!houseReached && z->getPositionX() < 500.0f && !z->isDead()) {
            houseReached = true;
            if (onZombieReachHouse) {
                onZombieReachHouse();
            }
        }
    }
}

void ZombieLayer::pauseAllZombies() {
    this->pause();
    for (auto z : _allZombies) {
        z->pause();
        for (auto c : z->getChildren()) c->pause(); // 递归暂停动画精灵
    }
}

void ZombieLayer::resumeAllZombies() {
    this->resume();
    for (auto z : _allZombies) {
        z->resume();
        for (auto c : z->getChildren()) c->resume();
    }
}