#pragma once
#include "cocos2d.h"
#include <map>

enum class ZombieType { 
    Error, 
    Normal, 
    Conehead, 
    Buckethead 

};

// 确保枚举在基类可见
enum class ZombieState {
    WALK,           // 正常行走
    ATTACK,         // 啃咬

    //帽子/头动画状态
    HEAD_LOSS,      //头的掉落
    ARMOR_LOSS,      //护具的掉落

    //身体动画
    LOST_ARMOR,     // 失去防具（例如路障、铁桶刚掉的一瞬间）
    LOST_HEAD,
    HEADLESS_WALK,  // 失去脑袋但还在走（临死前的最后挣扎）
    HEADLESS_ATTACK,// 失去脑袋但还在啃

    DYING,          // 正在播放死亡动画（倒地）
    BOOMDIE,        //爆裂死亡


    DEAD            // 彻底销毁
};

struct ZombieSpecialAnimData {
    std::string animationName; // 缓存名
    std::string prefix;        // 序列帧前缀
    int frameCount;
    float delay;
    std::string plistPath;
};

struct ZombieProperties {
    ZombieType type;
    std::string name;
    int health;
    float speed;
    int attackPower;
    float attackInterval;

    // 动画资源
    std::string plistPath;
    std::string animPrefix;
    int animFrameCount;
    float animDelay;
    std::string animationName;
    std::string spriteFrameName;

    ZombieProperties();
    ZombieProperties(ZombieType t, std::string n, int hp, float spd, int atk, float interval,
        std::string plist, std::string prefix, int count, float delay, std::string animN);
};

class ZombieData {
public:
    static const ZombieProperties& getProps(ZombieType type);
    static const std::map<ZombieType, ZombieProperties>& getAllConfigs();
    static const std::map<ZombieState, ZombieSpecialAnimData> getSpecialAnimMap(ZombieType type);

private:
    static std::map<ZombieType, ZombieProperties> _dataConfig;
    static std::map<ZombieType, std::map<ZombieState, ZombieSpecialAnimData>> _specialAnimConfig;

    static void init();
};