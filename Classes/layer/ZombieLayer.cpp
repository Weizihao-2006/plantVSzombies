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
    // 清理已经死亡从父节点移除的僵尸
    for (int i = _allZombies.size() - 1; i >= 0; --i) {
        auto z = _allZombies.at(i);
        if (z->getParent() == nullptr) {
            _allZombies.erase(i);
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