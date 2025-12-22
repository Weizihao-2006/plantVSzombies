#pragma once
#include "cocos2d.h"
#include "zombie/ZombieData.h"
#include "zombie/CommonZombie.h"

class ZombieMgr {
public:
    static ZombieMgr* getInstance();

    // 初始化逻辑，获取 ZombieLayer 引用
    bool init();

    // 在特定行生成指定类型的僵尸
    void spawnZombie(ZombieType type, int row);

    void reset();

private:
    ZombieMgr() = default;
    static ZombieMgr* s_instance;
};