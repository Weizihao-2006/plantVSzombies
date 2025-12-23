#pragma once
#include<string>
#include"cocos2d.h"
#include"zombie/Zombie.h"
#include"plant/PlantData.h"
#include "layer/ControlLayer.h"

USING_NS_CC;

//植物属性结构体


//基类不提供create(),需要在子类中使用initWithPlantProperties
class Plants : public Node {
public:
    virtual bool initWithPlantType(PlantType type);


    virtual void update(float dt) override {} //执行攻击/生产逻辑检测
    //攻击逻辑（攻击型植物实现）
    virtual void attack() {};

        //生产资源（资源型植物实现）
    virtual void produceResource() {};

    bool isDead() { return _currentHealth <= 0; }

    // 受伤状态切换：基类什么都不做
    virtual void updateVisualByHealth() {}

    //植物受伤
    void takeDamage(float damage) 
    {
        if (_currentHealth < 0) {
            return;
        }
        _currentHealth -= damage;

        // 视觉反馈：通用的受击闪烁
        _mainSprite->runAction(Sequence::create(
            TintTo::create(0.1f, Color3B(100,0,0)),
            TintTo::create(0.1f, Color3B::WHITE),
            nullptr));

        // 受伤状态切换：由各子类决定是否需要根据血量切换图片
        updateVisualByHealth();

        if (_currentHealth <= 0 && _currentState != PlantState::DYING) {
            handleDeath();//让植物死亡
        }
    }

    //调用之后植物进入死亡状态,停止所有攻击,播放死亡动画,在子类中重写playDeathAnimation()以播放死亡动画
    void handleDeath() {
        if (_currentState == PlantState::DYING) 
            return;
        this->setState(PlantState::DYING);
        this->unscheduleUpdate(); //立即停止逻辑更新（如停止产阳光、停止射击）
        
        // 从ControlLayer里删除植物（因为mapMgr和ControlLayer）挂钩
        auto scene = cocos2d::Director::getInstance()->getRunningScene();
        auto controlLayer = dynamic_cast<ControlLayer*>(scene->getChildByName("ControlLayer"));
        controlLayer->removePlantFromMap(static_cast<int>(_myMapPos.x), static_cast<int>(_myMapPos.y), PlantType::Error);

        this->removeFromParent();
    }

    //植物状态管理
    void setState(const PlantState state) {
        _currentState = state;
    }
    PlantState getState() const {
        return _currentState;
    }

    // 设置坐标
    void setPos(Vec2 MapPos) {
        _myMapPos = MapPos;
    }

protected:
    Vec2 _myMapPos; // 记录植物在地图中的坐标
    PlantProperties _properties;  // 统一管理属性
    PlantState _currentState;     // 当前状态
    Sprite* _mainSprite = nullptr; // 植物的图片/动画主体
    float _currentHealth;         // 当前生命值
    float _attackInterval; // 攻击间隔
    float _timer=0;          // 攻击计时器
};