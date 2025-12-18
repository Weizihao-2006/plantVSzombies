#pragma once
#include"cocos2d.h"
#include <map>
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

enum class PlantState {
    IDLE,       // 正常
    ATTACKING,  // 正在攻击
    DAMAGED,    // 轻微受损（如坚果破损1）
    CRITICAL,   // 严重受损（如坚果破损2）
    DYING,      // 正在播放死亡动画
    DEAD        // 逻辑彻底销毁
};

// 植物属性结构体
struct PlantProperties {
    // 1. 游戏逻辑属性
    int sunCost;                 // 阳光消耗
    float coolDownTime;          // 冷却时间
    int health;                  // 生命
    int attackPower;             // 攻击力
    float attackInterval;        // 逻辑触发间隔
    int typeAsInt;               // 对应的类型整数值
    std::string name;            // 内部识别名

    // 2. 视觉与动画配方
    std::string spriteFrameName; // 初始静态帧名
    std::string animationName;   // 缓存标识名
    std::string animPrefix;      // 动画帧前缀
    int animFrameCount;          // 动画总帧数
    float animDelay;             // 帧间隔

    // 构造函数声明
    PlantProperties();
    PlantProperties(int cost, float cd, int hp, int atk, float interval,
        const std::string& prefix, int fCount, float delay, const std::string& anim,
        int type, const std::string& n);

    // 拷贝与赋值
    PlantProperties(const PlantProperties& other);
    PlantProperties& operator=(const PlantProperties& other);
};

class PlantData {
public:
    /** 获取指定类型的植物配置属性 */
    static PlantProperties getProps(int type);

    /** 获取完整配置表（静态常驻内存） */
    static std::map<int, PlantProperties>& getAllConfigs();
};