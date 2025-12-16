#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "cocos2d.h"

class BgLayer;
class CardBarLayer;
class SunLayer;
class ControlLayer;
class ZombieLayer;
class BulletLayer;
class GameUILayer;

class GameScene : public cocos2d::Scene {

private:
    int _levelID = 1;
    BgLayer* _bgLayer = nullptr;
    CardBarLayer* _cardBarLayer = nullptr;
    SunLayer* _sunLayer = nullptr;
    ControlLayer* _controlLayer = nullptr;
    ZombieLayer* _zombieLayer = nullptr;
    BulletLayer* _bulletLayer = nullptr;
    GameUILayer* _uiLayer = nullptr;

public:
    static GameScene* createWithLevel(int level_id = 1);
    bool initWithLevel(int level_id);

    void update(float dt) override;  // 每帧更新

    void onZombieEnterHouse();       // 僵尸进入 -> 失败
    void onAllZombieClear();         // 僵尸清空 -> 胜利
    void onPause(bool pause);        // 暂停/继续
    void onSpeedChanged(float s);    // 0/0.5/1/2 倍速
    int getLevelID() const { return _levelID; }

private:
    void createLayers();
    void bindLayerSignals();
};

#endif // !GAME_SCENE_H