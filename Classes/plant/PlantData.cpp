#include "PlantData.h"

// --- PlantProperties 实现 ---

PlantProperties::PlantProperties()
    : sunCost(0), coolDownTime(0.0f), health(0), attackPower(0)
    , attackInterval(1.5f), typeAsInt(0), name("Unknown")
    , spriteFrameName(""), animationName("")
    , animPrefix(""), animFrameCount(0), animDelay(0.0f) {
}

PlantProperties::PlantProperties(int cost, float cd, int hp, int atk, float interval,
    const std::string& prefix, int fCount, float delay, const std::string& anim,
    int type, const std::string& n)
    : sunCost(cost), coolDownTime(cd), health(hp), attackPower(atk)
    , attackInterval(interval), typeAsInt(type), name(n)
    , animPrefix(prefix), animFrameCount(fCount), animDelay(delay), animationName(anim) {

    // 自动生成初始帧名：前缀 + "0.png"
    this->spriteFrameName = prefix + "0.png";
}

PlantProperties::PlantProperties(const PlantProperties& other) {
    *this = other;
}

PlantProperties& PlantProperties::operator=(const PlantProperties& other) {
    if (this != &other) {
        this->sunCost = other.sunCost;
        this->coolDownTime = other.coolDownTime;
        this->health = other.health;
        this->attackPower = other.attackPower;
        this->attackInterval = other.attackInterval;
        this->typeAsInt = other.typeAsInt;
        this->name = other.name;
        this->spriteFrameName = other.spriteFrameName;
        this->animationName = other.animationName;
        this->animPrefix = other.animPrefix;
        this->animFrameCount = other.animFrameCount;
        this->animDelay = other.animDelay;
    }
    return *this;
}

// --- PlantData 实现 ---

PlantProperties PlantData::getProps(int type) 
{
    auto& config = getAllConfigs();
    if (config.find(type) != config.end()) {
        return config[type];
    }
    return PlantProperties();
}

std::map<int, PlantProperties>& PlantData::getAllConfigs() {
    static std::map<int, PlantProperties> _dataConfig;

    if (_dataConfig.empty()) {
        // 向日葵 (类型ID: 1)
        _dataConfig[1] = PlantProperties(
            50, 7.5f, 300, 0, 15.0f,
            "SunFlower_", 18, 0.15f, "SunFlower_Anim",
            1, "SunFlower"
        );

        // 豌豆射手 (类型ID: 2)
        _dataConfig[2] = PlantProperties(
            100, 7.5f, 300, 20, 1.5f,
            "Peashooter_", 13, 0.1f, "Peashooter_Anim",
            2, "PeaShooter"
        );

        // 坚果墙 (类型ID: 5)
        _dataConfig[5] = PlantProperties(
            50, 20.0f, 4000, 0, 0.0f,
            "WallNut_", 16, 0.15f, "WallNut_Anim",
            5, "WallNut"
        );
    }
    return _dataConfig;
}