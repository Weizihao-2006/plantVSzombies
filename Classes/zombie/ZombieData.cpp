#include "ZombieData.h"

std::map<ZombieType, ZombieProperties> ZombieData::_dataConfig;

// 修复错误 1：默认构造函数
ZombieProperties::ZombieProperties()
    : type(ZombieType::Error), name(""), health(0), speed(0),
    attackPower(0), attackInterval(0), animFrameCount(0), animDelay(0)
{
}

// 修复错误 4：全参数构造函数
ZombieProperties::ZombieProperties(ZombieType t, std::string n, int hp, float spd, int atk, float interval,
    std::string plist, std::string prefix, int count, float delay, std::string animN)
    : type(t), name(n), health(hp), speed(spd), attackPower(atk), attackInterval(interval),
    plistPath(plist), animPrefix(prefix), animFrameCount(count), animDelay(delay), animationName(animN)
{
    if (!animPrefix.empty()) {
        this->spriteFrameName = animPrefix + "1.png";
    }
}

const std::map<ZombieType, ZombieProperties>& ZombieData::getAllConfigs() {
    if (_dataConfig.empty()) init();
    return _dataConfig;
}

const ZombieProperties& ZombieData::getProps(ZombieType type) {
    if (_dataConfig.empty()) init();
    return _dataConfig.count(type) ? _dataConfig[type] : _dataConfig[ZombieType::Error];
}

void ZombieData::init() {
    // 普通僵尸
    _dataConfig[ZombieType::Normal] = ZombieProperties(
        ZombieType::Normal, "Zombie", 200, 30.0f, 20, 1.0f,
        "image/Zombie.plist", "Zombie", 22, 0.12f, "Zombie_Walk"
    );

    // 错误占位
    _dataConfig[ZombieType::Error] = ZombieProperties(ZombieType::Error, "Error", 0, 0, 0, 0, "", "", 0, 0, "");
}