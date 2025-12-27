#pragma once
#include "cocos2d.h"
#include "zombie/ZombieData.h"
#include"plant/PlantData.h"
#include "plant/Plant.h"



class Zombie : public cocos2d::Node {
public:
    CREATE_FUNC(Zombie);
    virtual bool initWithZombieType(ZombieType type);
    virtual void update(float dt) override;
    void takeDamage(float damage, PlantType attackPlant);
    CC_SYNTHESIZE(int, _row, Row);

    // 增加切换动画的方法
    void changeAnimation(const std::string& animName);
    // 增加获取当前状态的方法
    ZombieState getState() const { return _state; }
    bool isDead()const { return _state == ZombieState::DEAD; }
    // 增加参数，默认为普通死亡 (DYING)
    virtual void onDie(ZombieState dieType = ZombieState::DYING);
    virtual ZombieType getType() { return ZombieType::Error; }
    //新增
    // 施加减速效果：duration 为持续时间，percent 为速度缩放比例（默认0.5倍速）
    void applySlowDown(float duration, float percent = 0.5f);
    void restoreFromSlowDown(float dt);

    virtual cocos2d::Rect getHitbox() const;
    // 开启/关闭外轮廓显示
    void setDebugRectVisible(bool visible);
    // 更新外轮廓
    void updateDebugRect();

    int getSoundID() { return _soundID; }

protected: // 必须是 protected 子类才能访问 _state
    ZombieProperties _props;
    ZombieState _state = ZombieState::WALK;
    cocos2d::Sprite* _mainSprite = nullptr;//指的是正常行走的状态
    cocos2d::Sprite* _headSprite = nullptr;//特指脑袋动画
    float _currentHealth;

    //速度相关
    float _currentSpeed;
    bool _isSlowed = false;

    // 音效
    int _soundID;

    cocos2d::DrawNode* _debugDrawNode = nullptr;
    bool _showDebugRect = false;
};