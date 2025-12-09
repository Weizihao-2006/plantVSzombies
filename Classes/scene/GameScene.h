#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "cocos2d.h"

class BgLayer;
class CardBarLayer;
class SunLayer;
class PlantLayer;
class ZombieLayer;
class BulletLayer;
class GameUILayer;

class GameScene :public cocos2d::Scene {

private:
    int _levelID = 1;
    BgLayer* _bgLayer = nullptr;
    CardBarLayer* _cardBarLayer = nullptr;
    SunLayer* _sunLayer = nullptr;
    PlantLayer* _plantLayer = nullptr;
    ZombieLayer* _zombieLayer = nullptr;
    BulletLayer* _bulletLayer = nullptr;
    GameUILayer* _uiLayer = nullptr;

public:

    static GameScene* createWithLevel(int level_id);
    bool initWithLevel(int level_id);


    void onZombieEnterHouse();   // ½©Ê¬½øÎÝ -> Ê§°Ü
    void onAllZombieClear();     // ½©Ê¬Ë¢Íê -> Ê¤Àû
    void onPause(bool pause);    // ÔÝÍ£/¼ÌÐø
    void onSpeedChanged(float s);// 0/0.5/1/2 ±¶ËÙ
    int  getLevelID() const { return _levelID; }

private:
    void createLayers();        
    void bindLayerSignals();    

};



#endif // !GAME_SCENE_H

