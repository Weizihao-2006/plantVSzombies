#include "ZombieData.h"

std::map<ZombieType, ZombieProperties> ZombieData::_dataConfig;
std::map<ZombieType, std::map<ZombieState, ZombieSpecialAnimData>> ZombieData::_specialAnimConfig;

// 修复错误 1：默认构造函数
ZombieProperties::ZombieProperties()
    : type(ZombieType::Error), name(""), health(0), speed(0),
    attackPower(0), attackInterval(0), animFrameCount(0), animDelay(0),filename("")
{
}

// 修复错误 4：全参数构造函数
ZombieProperties::ZombieProperties(ZombieType t, std::string n, int hp, float spd, int atk, float interval,
    std::string plist, std::string prefix, int count, float delay, std::string animN, std::string fName)
    : type(t), name(n), health(hp), speed(spd), attackPower(atk), attackInterval(interval),
    plistPath(plist), animPrefix(prefix), animFrameCount(count), animDelay(delay), animationName(animN),filename(fName)
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

        // --- 巨人僵尸特殊动画 ---
        auto& giant = _specialAnimConfig[ZombieType::Giant];
        // 正常攻击
        giant[ZombieState::ATTACK] = { "GiantAttack_Normal", "giant_attack_normal_", 33, 0.12f, "", "GiantAttack", "image/giant_attack_normal/giant_attack_normal_%d.png" };
        // 抛出小鬼 (用自定义状态或借用已有的)
        giant[ZombieState::ARMOR_LOSS] = { "GiantThrow", "giant_throw_", 34, 0.12f, "", "GiantThrow", "image/giant_throw/giant_throw_%d.png" };
        // 丢出小鬼后的攻击 (受伤态攻击)
        giant[ZombieState::LOST_ARMOR] = { "GiantAttack_Damaged", "giant_attack_damaged_", 33, 0.12f, "", "GiantAttack_Damaged", "image/giant_attack_damaged/giant_attack_damaged_%d.png" };
        // 受伤行走
        giant[ZombieState::HEADLESS_WALK] = { "GiantWalk_Damaged", "giant_walk_damaged_", 49, 0.15f, "", "GiantWalk_Damaged", "image/giant_walk_damaged/giant_walk_damaged_%d.png" };
        // 死亡
        giant[ZombieState::DYING] = { "GiantDie", "giant_die_", 59, 0.15f, "", "GiantDie", "image/giant_die/giant_die_%d.png" };

        // --- 小鬼僵尸 (Imp) 特殊动画 ---
        auto& imp = _specialAnimConfig[ZombieType::Imp];

        // 1. 行走 (其实在基础属性已配，这里配特殊状态供切换)
        imp[ZombieState::WALK] = { "ImpWalk", "imp_walk_", 10, 0.1f, "", "ImpWalk", "image/imp_walk/imp_walk_%d.png" };

        // 2. 攻击
        imp[ZombieState::ATTACK] = { "ImpAttack", "imp_attack_", 10, 0.1f, "", "ImpAttack", "image/imp_attack/imp_attack_%d.png" };

        // 3. 飞行状态 (被巨人抛出时使用)
        // 这里我们映射到LOST_ARMOR
        imp[ZombieState::LOST_ARMOR] = { "ImpThrow", "imp_throw_", 10, 0.1f, "", "ImpThrow", "image/imp_throw/imp_throw_%d.png" };

        // 4. 普通死亡 (倒地)
        imp[ZombieState::DYING] = { "ImpDie", "imp_die_", 10, 0.1f, "", "ImpDie", "image/imp_die/imp_die_%d.png" };

        // 5. 爆炸死亡 (被樱桃炸弹炸碎)
        imp[ZombieState::BOOMDIE] = { "ImpBoomDie", "imp_boomdie_", 10, 0.1f, "", "ImpBoomDie", "image/imp_boomdie/imp_boomdie_%d.png" };
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
        ZombieType::Normal, "Zombie", 100, 30.0f, 20, 1.0f,
        "image/Zombie_default.plist", "Zombie", 22, 0.12f, "ZombieWalk_Default"
    );

    // 2. 路障僵尸 (Conehead)
    // 资源名: ConeheadZombie_default.plist
    _dataConfig[ZombieType::Conehead] = ZombieProperties(
        ZombieType::Conehead, "ConeheadZombie", 200, 30.0f, 20, 1.0f,
        "image/ConeheadZombie_default.plist", "ConeheadZombie", 21, 0.15f, "ConeheadZombieWalk_Default"
    );

    // 3. 铁桶僵尸 (Buckethead)
    // 资源名: BucketheadZombie_default.plist
    _dataConfig[ZombieType::Buckethead] = ZombieProperties(
        ZombieType::Buckethead, "BucketheadZombie", 350, 30.0f, 20, 1.0f,
        "image/BucketheadZombie_default.plist", "BucketheadZombie", 15, 0.15f, "BucketheadZombieWalk_Default"
    );

    _dataConfig[ZombieType::Giant] = ZombieProperties(
        ZombieType::Giant,"GiantWalk_Normal",1500,15.0f,10000,1.0f,"","giant_walk_normal_",49,0.15f,"GiantWalk_Normal","image/giant_walk_normal/giant_walk_normal_%d.png"
    );

    // 5. 小鬼僵尸 (Imp)
    // 参数含义：类型, 名字, 血量(40), 速度(45), 攻击力(30), 攻击间隔(0.5s), 
    // Plist路径(空), 前缀, 帧数, 帧间隔, 默认动画名, 文件夹路径
    _dataConfig[ZombieType::Imp] = ZombieProperties(
        ZombieType::Imp, "Imp", 100, 45.0f, 30, 0.8f,
        "", "imp_walk_", 10, 0.1f, "ImpWalk", "image/imp_walk/imp_walk_%d.png"
    );
    // 错误占位
    _dataConfig[ZombieType::Error] = ZombieProperties(ZombieType::Error, "Error", 0, 0, 0, 0, "", "", 0, 0, "");
}