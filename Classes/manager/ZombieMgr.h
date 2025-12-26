#pragma once
#include "cocos2d.h"
#include "zombie/ZombieData.h"
#include "zombie/CommonZombie.h"
#include "zombie/ConeZombie.h"
#include "zombie/BucketZombie.h"
#include"zombie/GiantZombie.h"
#include"layer/ZombieLayer.h"
#include "util/LevelConfig.h"

struct PendingZombie {
    ZombieType type;
    int row;
    float delay; // 剩余等待生成的秒数
};

class ZombieMgr : public cocos2d::Ref {
public:
    static ZombieMgr* getInstance();

    void addZombie(Zombie* zombie) {
        if (!_zombieLayer) {
            auto scene = Director::getInstance()->getRunningScene();
            _instance->_zombieLayer = dynamic_cast<ZombieLayer*>(scene->getChildByName("ZombieLayer"));
        }
       
        _zombieLayer->addChild(zombie);
        _zombieLayer->getAllZombies().pushBack(zombie);
        
    }

    // 每一帧由 GameScene 调用，驱动波次逻辑
    void update(float dt);

    // 启动关卡
    void startLevel(GameMode mode, int levelId);

    // 核心刷怪函数
    void spawnZombie(ZombieType type, int row);

    // 获取当前存活的僵尸数量
    int getAliveZombieCount();

    // 重置manager
    void reset();

    // 设置是否出怪
    void setIsLevelStarted(bool flag) { _isLevelStarted = flag; }

private:
    ZombieMgr();
    static ZombieMgr* _instance;

    // 波次管理逻辑
    void generateNextWave(int waveIndex);
    void EndlessNextWave(int waveIndex);

    void spawnHugeWave(int waveIndex);
    ZombieType getRandomZombieTypeByWave(int waveIndex);

    // 成员变量
    ZombieLayer* _zombieLayer = nullptr;

    int _currentWave = 0;
    float _waveTimer = 0.0f;
    float _nextWaveInterval = 15.0f; // 第一波前的准备时间
    bool _isLevelStarted = false;
    float _waveDelay; // 一波的渲染时间

    GameMode _currentMode;
    LevelData _currentLevelData; // 直接存储当前关卡的结构体副本

    std::vector<PendingZombie> _pendingZombies; // 待生成队列
};