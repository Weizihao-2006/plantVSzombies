#pragma once
#include<string>
#include"cocos2d.h"
#include"zombie/Zombie.h"
USING_NS_CC;

enum class PlantType
{
    Error,
    SunFlower,  /* 向日葵 */
    PeaShooter, /* 豌豆射手 */
    ReaPeater,  //双射
    SnowPea,
    WallNut,    /* 坚果墙 */
    CherryBomb, /* 樱桃炸弹 */
};

// 新增：植物状态枚举
enum PlantState {
    IDLE,
    ATTACKING,
    EATEN,
    DEAD,
    COOLDOWN
};

//植物属性结构体
struct PlantProperties {
    int sunCost;
    int coolDownTime;
    int health;
    int attackPower;
    PlantType type;
    std::string name;
    // 添加构造函数
    PlantProperties(int cost, float cd, int hp, int atk, PlantType t, std::string n)
        : sunCost(cost), coolDownTime(cd), health(hp), attackPower(atk), type(t), name(n) {
    }

    // 默认构造函数（必须保留，否则某些容器无法使用）
    PlantProperties() : sunCost(0), coolDownTime(0), health(0), attackPower(0), type(PlantType::Error), name("") {}

    PlantProperties& operator=(const PlantProperties& other) {
        // 防止自赋值
        if (this == &other) {
            return *this;
        }

        this->sunCost = other.sunCost;
        this->coolDownTime = other.coolDownTime;
        this->health = other.health;
        this->attackPower = other.attackPower;
        this->type = other.type;
        this->name = other.name;

        return *this;
    }
};

class Plants : public Node {
public:
   
    void update(float dt) override; // 执行攻击/生产逻辑检测
    //攻击逻辑（攻击型植物实现）
    virtual void attack() {}

    //生产资源（资源型植物实现）
    virtual void produceResource() {}

    //碰撞检测
    virtual bool checkZombieCollision(Zombie* zombie);

    //植物受伤
    virtual void takeDamage(float damage) {
        _currentHealth -= damage;
        if (_currentHealth <= 0) {
            this->setState(PlantState::DEAD);
            this->removeFromParent(); // 从 PlantLayer 移除
        }
    }

    // 优化：植物状态管理
    void setState(PlantState state);
    PlantState getState() const;

protected:
    PlantProperties _properties;  // 统一管理属性
    PlantState _currentState;     // 当前状态
    float _currentHealth;         // 当前生命值
    float _attackInterval; // 攻击间隔
    float _timer;          // 攻击计时器
};