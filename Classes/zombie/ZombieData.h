#pragma once
#include "cocos2d.h"
#include <map>

enum class ZombieType { Error, Normal, Conehead, Buckethead };

struct ZombieProperties {
    ZombieType type;
    std::string name;
    int health;
    float speed;
    int attackPower;
    float attackInterval;

    // ¶¯»­×ÊÔ´
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
private:
    static std::map<ZombieType, ZombieProperties> _dataConfig;
    static void init();
};