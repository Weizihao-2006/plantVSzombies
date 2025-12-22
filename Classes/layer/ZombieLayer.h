#pragma once
#include "cocos2d.h"
#include "zombie/Zombie.h"

class ZombieLayer : public cocos2d::Layer {
public:
    CREATE_FUNC(ZombieLayer);
    virtual bool init() override;

    // 添加僵尸到层级并按行管理
    void addZombie(Zombie* zombie);

    // 【关键接口】供豌豆射手调用：判断该行是否有僵尸
    bool hasZombieInRow(int row, float minX);

    // 获取所有僵尸（用于子弹碰撞检测）
    cocos2d::Vector<Zombie*>& getAllZombies() { return _allZombies; }

    // 暂停/恢复所有僵尸（处理暂停UI逻辑）
    void pauseAllZombies();
    void resumeAllZombies();

    virtual void update(float dt) override;

private:
    cocos2d::Vector<Zombie*> _allZombies;
};