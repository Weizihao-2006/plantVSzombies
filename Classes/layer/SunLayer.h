#ifndef SUN_LAYER_H
#define SUN_LAYER_H

#include "cocos2d.h"
#include<string>
USING_NS_CC;

// 包含阳光和阳光管理层


// 阳光个体类
class SunSprite : public Sprite {
public:
    static const int exitSeqTag;

    static SunSprite* createSun(int amount = 25);

    //玩家点击了阳光时调用,操作阳光本身向destPos飞去
    void collect(Vec2 destPos);

    int getAmount() const { return _amount; }
private:
    int _amount = 25;
    static const float SUN_SCALE;
};


// 阳光管理层
class SunLayer : public Layer {
public:
    CREATE_FUNC(SunLayer);
    virtual bool init() override;

    // 外部产出接口
    //这个地方要设置坐标
    void spawnSkySun();             // 天空随机掉落
    void spawnSunFromPlant(Vec2 pos);      // 植物（向日葵）产出

    // 点击判定,只有worldPos(点击坐标)在_suns中的某一个阳光的范围时才会调用sun->collect()
    bool containsAndCollectSun(const Vec2& worldPos);

    void pauseAllSuns();
    void resumeAllSuns();

    void startSunLogic();

private:
    void update(float dt) override;

    // 使用 cocos2d::Vector 自动管理内存
    Vector<SunSprite*> _suns;

    float _dropTimer = 9.0f;//用于记录时间掉落阳光的时间间隔
    float _dropInterval = 4.0f; // 10秒掉落一次

    float _dropTimeCost = 4.0f;//阳光在空中移动的时间
    float _clearTimer = 0.0f;
};

#endif