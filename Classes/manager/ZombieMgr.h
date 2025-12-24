#pragma once
#include "cocos2d.h"
#include "zombie/ZombieData.h"
#include "zombie/CommonZombie.h"
#include "zombie/ConeZombie.h"
#include "zombie/BucketZombie.h"

class ZombieMgr : public cocos2d::Ref {
public:
    static ZombieMgr* getInstance();

    // 每一帧由 GameScene 调用，驱动波次逻辑
    void update(float dt);

    // 启动关卡
    void startLevel();

    // 核心刷怪函数
    void spawnZombie(ZombieType type, int row);

    // 获取当前存活的僵尸数量
    int getAliveZombieCount();

    // 重置manager
    void reset();
private:
    ZombieMgr();
    static ZombieMgr* _instance;

    // 波次管理逻辑
    void generateNextWave(int waveIndex);
    void spawnHugeWave(int waveIndex);
    ZombieType getRandomZombieTypeByWave(int waveIndex);

    // 成员变量
    int _currentWave = 0;
    float _waveTimer = 0.0f;
    float _nextWaveInterval = 15.0f; // 第一波前的准备时间
    bool _isLevelStarted = false;
    bool _isSpawningWave;
    float _waveDelay; // 每一波的渲染时间
};