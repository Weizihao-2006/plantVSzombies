#include "ZombieData.h"

std::map<ZombieType, ZombieProperties> ZombieData::_dataConfig;
std::map<ZombieType, std::map<ZombieState, ZombieSpecialAnimData>> ZombieData::_specialAnimConfig;

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

const std::map<ZombieState, ZombieSpecialAnimData> ZombieData::getSpecialAnimMap(ZombieType type)
{
    if (_specialAnimConfig.empty()) {

        // --- 普通僵尸 (Normal) 特殊动画 ---
        auto& normal = _specialAnimConfig[ZombieType::Normal];

        // 正常啃咬
        normal[ZombieState::ATTACK] = { "ZombieAttack", "ZombieAttack", 22, 0.15f, "image/ZombieAttack_default.plist" };
        // 掉落的头（飞出的动画）
        normal[ZombieState::HEAD_LOSS] = { "ZombieHead", "ZombieHead", 13, 0.12f, "image/ZombieHead_default.plist" };
        // 无头行走
        normal[ZombieState::HEADLESS_WALK] = { "ZombieLostHead", "ZombieLostHead", 19, 0.15f, "image/ZombieLostHead_default.plist" };
        // 无头啃咬
        normal[ZombieState::HEADLESS_ATTACK] = { "ZombieLostHeadAttack", "ZombieLostHeadAttack", 12, 0.15f, "image/ZombieLostHeadAttack_default.plist" };
        // 普通倒地死亡
        normal[ZombieState::DYING] = { "ZombieDie", "ZombieDie", 11, 0.15f, "image/ZombieDie_default.plist" };
        // 爆裂死亡
        normal[ZombieState::BOOMDIE] = { "ZombieBoomDie","BoomDie",19,0.15f,"image/BoomDie_default.plist" };


        // --- 2. 路障僵尸 (Conehead) ---
        auto& cone = _specialAnimConfig[ZombieType::Conehead];
        cone[ZombieState::WALK] = { "ConeheadZombie", "ConeheadZombie", 22, 0.15f, "image/ConeheadZombie_default.plist" };
        cone[ZombieState::ATTACK] = { "ConeheadZombieAttack", "ConeheadZombieAttack", 12, 0.15f, "image/ConeheadZombieAttack_default.plist" };

        // 当路障被击破，会降级为 Normal，所以死亡相关直接复用普通僵尸数据
        cone[ZombieState::HEAD_LOSS] = normal[ZombieState::HEAD_LOSS];
        cone[ZombieState::HEADLESS_WALK] = normal[ZombieState::HEADLESS_WALK];
        cone[ZombieState::DYING] = normal[ZombieState::DYING];
        cone[ZombieState::BOOMDIE] = normal[ZombieState::BOOMDIE];

        // --- 3. 铁桶僵尸 (Buckethead) ---
        auto& bucket = _specialAnimConfig[ZombieType::Buckethead];
        bucket[ZombieState::WALK] = { "BucketheadZombie", "BucketheadZombie", 16, 0.15f, "image/BucketheadZombie_default.plist" };
        bucket[ZombieState::ATTACK] = { "BucketheadZombieAttack", "BucketheadZombieAttack", 12, 0.15f, "image/BucketheadZombieAttack_default.plist" };
        bucket[ZombieState::BOOMDIE] = normal[ZombieState::BOOMDIE];

        // 同样在失去铁桶后复用普通僵尸逻辑
        bucket[ZombieState::HEAD_LOSS] = normal[ZombieState::HEAD_LOSS];
        bucket[ZombieState::HEADLESS_WALK] = normal[ZombieState::HEADLESS_WALK];
        bucket[ZombieState::DYING] = normal[ZombieState::DYING];
    }

    auto it = _specialAnimConfig.find(type);
    return (it != _specialAnimConfig.end()) ? it->second : std::map<ZombieState, ZombieSpecialAnimData>();
}

const std::map<ZombieType, ZombieProperties>& ZombieData::getAllConfigs() {
    if (_dataConfig.empty()) 
        init();
    return _dataConfig;
}

const ZombieProperties& ZombieData::getProps(ZombieType type) {
    if (_dataConfig.empty()) init();
    return _dataConfig.count(type) ? _dataConfig.at(type) : _dataConfig.at(ZombieType::Error);
}

void ZombieData::init() {
    // 普通僵尸
    // 资源名: Zombie_default.plist
    _dataConfig[ZombieType::Normal] = ZombieProperties(
        ZombieType::Normal, "Zombie", 200, 30.0f, 20, 1.0f,
        "image/Zombie_default.plist", "Zombie", 22, 0.12f, "ZombieWalk_Default"
    );

    // 2. 路障僵尸 (Conehead)
    // 资源名: ConeheadZombie_default.plist
    _dataConfig[ZombieType::Conehead] = ZombieProperties(
        ZombieType::Conehead, "ConeheadZombie", 640, 30.0f, 20, 1.0f,
        "image/ConeheadZombie_default.plist", "ConeheadZombie", 21, 0.15f, "ConeheadZombieWalk_Default"
    );

    // 3. 铁桶僵尸 (Buckethead)
    // 资源名: BucketheadZombie_default.plist
    _dataConfig[ZombieType::Buckethead] = ZombieProperties(
        ZombieType::Buckethead, "BucketheadZombie", 1370, 30.0f, 20, 1.0f,
        "image/BucketheadZombie_default.plist", "BucketheadZombie", 15, 0.15f, "BucketheadZombieWalk_Default"
    );


    // 错误占位
    _dataConfig[ZombieType::Error] = ZombieProperties(ZombieType::Error, "Error", 0, 0, 0, 0, "", "", 0, 0, "");
}