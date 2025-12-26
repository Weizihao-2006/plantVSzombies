#include "PlantData.h"


PlantProperties::PlantProperties()
    : cardIcon("")
    , cardIcon_locked("")
    , previewFrame("")
    , spriteFrameName("")
    , sunCost(0)
    , coolDownTime(0.0f)
    , health(0)
    , attackPower(0)
    , attackInterval(1.5f)
    , type(PlantType::Error)
    , name("Unknown")
    , animPrefix("")
    , animFrameCount(0)
    , animDelay(0.0f)
    , animationName("")
{
    
}

PlantProperties::PlantProperties(PlantType t, const std::string& n, int sun, float cd, int hp, int atk, float interval,
    const std::string& cIcon, const std::string& cIconLocked, const std::string& preview, const std::string& plist,
    const std::string& aPrefix, int aCount, float aDelay, const std::string& aName)
    : type(t), name(n), sunCost(sun), coolDownTime(cd), health(hp), attackPower(atk), attackInterval(interval)
    , cardIcon(cIcon), cardIcon_locked(cIconLocked), previewFrame(preview), plistPath(plist)
    , animPrefix(aPrefix), animFrameCount(aCount), animDelay(aDelay), animationName(aName)
{

    if (!animPrefix.empty()) {
        this->spriteFrameName = animPrefix + "1.png";
    }
}


// --- PlantData 实现 ---

std::map<PlantType, std::map<PlantState, SpecialAnimData>> PlantData::_specialAnimConfig;

PlantProperties PlantData::getProps(PlantType type) 
{
    const auto& config = getAllConfigs();

    auto it = config.find(type);
    if (it != config.end()) {
        return it->second;
    }
    CCLOG("Warning: PlantProperties for type %d not found!", static_cast<int>(type));
    return PlantProperties();
}


const std::map<PlantType, PlantProperties>& PlantData::getAllConfigs()
{
    static std::map<PlantType, PlantProperties> _dataConfig;
    if (_dataConfig.empty()) {
        // 1. 向日葵 (SunFlower)
        _dataConfig[PlantType::SunFlower] = PlantProperties(
            PlantType::SunFlower, "SunFlower", 50, 7.5f, 300, 0, 8.0f,
            "plantCard/SunFlower.png", "plantCard/SunFlower_lock.png",
            "cardPreview/SunFlower_0.png", "image/SunFlower.plist",
            "SunFlower_", 17, 0.15f, "SunFlower_Anim"
        );

        // 2. 樱桃炸弹 (CherryBomb) - 高伤害，长CD
        _dataConfig[PlantType::CherryBomb] = PlantProperties(
            PlantType::CherryBomb, "CherryBomb", 150, 1.0f, 1000, 4000, 0.0f,
            "plantCard/CherryBomb.png", "plantCard/CherryBomb_lock.png",
            "cardPreview/CherryBomb_0.png", "image/CherryBomb.plist",
            "CherryBomb/CherryBomb_", 8, 0.12f, "CherryBomb_Anim"
        );

        // 3. 豌豆射手 (PeaShooter) - 基础攻击
        _dataConfig[PlantType::PeaShooter] = PlantProperties(
            PlantType::PeaShooter, "PeaShooter", 100, 7.5f, 300, 20, 1.5f,
            "plantCard/PeaShooter.png", "plantCard/PeaShooter_lock.png",
            "cardPreview/Peashooter_0.png", "image/PeaShooter.plist",
            "Peashooter_", 13, 0.1f, "Peashooter_Anim"
        );

        // 4. 双发射手 (Repeater) - 攻击间隔减半
        _dataConfig[PlantType::ReaPeater] = PlantProperties(
            PlantType::ReaPeater, "Repeater", 200, 7.5f, 300, 20, 0.75f,
            "plantCard/Repeater.png", "plantCard/Repeater_lock.png",
            "cardPreview/Repeater_0.png", "",
            "Repeater_", 15, 0.08f, "Repeater_Anim"
        );

        // 5. 寒冰射手 (SnowPea) - 附带减速效果
        _dataConfig[PlantType::SnowPea] = PlantProperties(
            PlantType::SnowPea, "SnowPea", 175, 7.5f, 300, 20, 1.5f,
            "plantCard/SnowPea.png", "plantCard/SnowPea_lock.png",
            "cardPreview/SnowPea_0.png", "image/SnowPea.plist",
            "SnowPea_", 15, 0.1f, "SnowPea_Anim"
        );

        // 6. 坚果墙 (WallNut) - 高血量
        _dataConfig[PlantType::WallNut] = PlantProperties(
            PlantType::WallNut, "WallNut", 50, 30.0f, 4000, 0, 0.0f,
            "plantCard/Wallnut.png", "plantCard/Wallnut_lock.png",
            "cardPreview/WallNut_0.png", "image/WallNut.plist",
            "WallNut_", 16, 0.15f, "WallNut_Anim"
        );
        // ... 其他植物配置 ...
    }
    return _dataConfig;
}




void PlantData::initSpecialAnims() 
{
    // 坚果墙的特殊状态
    _specialAnimConfig[PlantType::WallNut][PlantState::DAMAGED] = { "Wallnut_cracked_1","Wallnut_cracked1_", 10, 0.15f, "image/WallNut_Cracked1.plist" };
    _specialAnimConfig[PlantType::WallNut][PlantState::CRITICAL] = { "Wallnut_cracked_2","Wallnut_cracked2_", 10, 0.15f, "image/WallNut_Cracked2.plist" };

    // 以后可以加其他的，比如大嘴花
    // _specialAnimConfig[PlantType::Chomper]["Digesting"] = { "Chomper_Digest_", 8, 0.2f, "" };
}

const std::map<PlantState, SpecialAnimData> PlantData::getSpecialAnimMap(PlantType type)
{
    if (_specialAnimConfig.empty()) 
        initSpecialAnims();
    if (_specialAnimConfig.find(type)!=_specialAnimConfig.end()) {
        return _specialAnimConfig.at(type);
    }
    return std::map<PlantState, SpecialAnimData>();
    
}