#pragma once
#include "cocos2d.h"
#include "plant/PlantData.h"
#include"plant/BulletData.h"
class Bullet : public cocos2d::Sprite {
public:
    // 创建函数：传入初始位置、行号、伤害和来源植物类型
    static Bullet* createWithConfig(int row, float damage, PlantType type);

    virtual bool init() override;

    // 子弹自身的逻辑更新（位移等）
    void updateLogic(float dt, float speed);

    // 触发碰撞后的表现（替换图片并消失）
    void onHit();

    // Getter
    int getRow() const { return _row; }
    float getDamage() const { return _damage; }
    PlantType getPlantType() const { return _originType; }
    bool isExploding() const { return _isExploding; }
    float getSlowPercent()const { return _slowPercent; }
    float getSlowDuration() const { return _slowDuration; }
    std::string getHitTexturePath() { return _props.hitTexturePath; }

private:
    int _row;
    float _damage;
    PlantType _originType;

    float _slowPercent=0.5f;   // 减速百分比
    float _slowDuration=3.0f; // 持续时间

    bool _isExploding = false; // 标记是否正在播放消失动画，防止重复触发伤害
    BulletProperties _props;
};