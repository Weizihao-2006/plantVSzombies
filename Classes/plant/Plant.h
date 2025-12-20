#pragma once
#include<string>
#include"cocos2d.h"
#include"zombie/Zombie.h"
#include"plant/PlantData.h"
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



    // 受伤状态切换：基类什么都不做
    virtual void updateVisualByHealth() {}

    //植物受伤
    void takeDamage(float damage) {
        _currentHealth -= damage;

        // 视觉反馈：通用的受击闪烁
        _mainSprite->runAction(Sequence::create(
            TintTo::create(0.1f, Color3B::RED),
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

        //播放死亡动画，动画结束后再移除
        playDeathAnimation();
    }

    //基类提供默认死亡动画与移除逻辑
    virtual void playDeathAnimation() {
        auto fade = FadeOut::create(0.5f);
        auto callback = CallFunc::create([this]() {
            this->removeFromParent(); //真正移除
            });
        _mainSprite->runAction(Sequence::create(fade, callback, nullptr));
    }

    //植物状态管理
    void setState(const PlantState state) {
        _currentState = state;
    }
    PlantState getState() const {
        return _currentState;
    }

protected:
    PlantProperties _properties;  // 统一管理属性
    PlantState _currentState;     // 当前状态
    Sprite* _mainSprite = nullptr; // 植物的图片/动画主体
    float _currentHealth;         // 当前生命值
    float _attackInterval; // 攻击间隔
    float _timer=0;          // 攻击计时器
};