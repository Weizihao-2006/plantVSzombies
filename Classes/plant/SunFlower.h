#pragma once
#include"cocos2d.h"
#include"plant/Plant.h"

USING_NS_CC;

class SunFlower : public Plants {
public:
    //子类特有的 create 方法
    static SunFlower* create(const PlantProperties& props) 
    {
        SunFlower* ret = new (std::nothrow) SunFlower();
        if (ret && ret->initWithPlantProperties(props)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    // 实现基类的虚函数
    virtual void attack() override { /* 向日葵不攻击 */ }

    virtual void produceResource() override; // 核心逻辑：产阳光

    //暂时没写
    virtual bool checkZombieCollision(Zombie* zombie) override {
        return false;  
    }

    virtual void update(float dt) override; //重写update处理生产计时
};