#pragma once
#include"cocos2d.h"
#include<string>
#include <map>
USING_NS_CC;

//
// 所有的植物数据从getAllConfigs()中获取,因此添加植物要修改getAllConfigs()
//

const float NormalScale = 1.5f;

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

/**
 * @brief 植物配置属性结构体
 * 包含了一个植物从 UI 显示、种植预览到战斗逻辑和动画生成的全部静态数据
 */
struct PlantProperties {
    // --- 1. UI 与 视觉 ---
    std::string plistPath;       // .plist 路径，如 "image/SunFlower.plist",植物种下去之后晃来晃去
    std::string cardIcon;        // 卡槽内显示的图片路径 (例如: "plantCard/SunFlower.png")
    std::string cardIcon_locked; // 卡槽点击后的冷却图片路径

    std::string previewFrame;    // 鼠标点击后随鼠标移动的预览帧 (例如: "cardPreview/SunFlower_0.png")
    std::string spriteFrameName; // 植物创建后的初始帧名,无需传入,根据animPrefix自动生成

    // --- 2. 战斗逻辑参数 ---
    int sunCost;                 // 购买所需的阳光值
    float coolDownTime;          // 选卡后的冷却时间 (秒)
    int health;                  // 最大生命值
    int attackPower;             // 攻击力 (若为资源类植物则通常为0)
    float attackInterval;        // 触发逻辑的时间间隔 (产阳光频率或射击频率)
    PlantType type;              // 植物枚举类型
    std::string name;            // 内部名称 (用于日志打印或调试)

    // --- 3. 动画生成配方 ---
    std::string animPrefix;      // 序列帧前缀 (例如: "SunFlower_")
    int animFrameCount;          // 序列帧总数
    float animDelay;             // 动画帧间隔 (秒，例如 0.15f)
    std::string animationName;   // 存入 AnimationCache 的唯一标识名

    // 默认构造
    PlantProperties();

    // 全参数构造
    PlantProperties(PlantType t, const std::string& n, int sun, float cd, int hp, int atk, float interval,
        const std::string& cIcon, const std::string& cIconLocked, const std::string& preview, const std::string& plist,
        const std::string& aPrefix, int aCount, float aDelay, const std::string& aName);

    // 拷贝与赋值重载
    PlantProperties(const PlantProperties& other);
    PlantProperties& operator=(const PlantProperties& other);
};

class PlantData {
public:
    /** * @brief 获取指定类型的植物属性
     * @param type 植物类型枚举
     * @return 返回属性拷贝，若类型不存在则返回 type 为 Error 的默认属性
     */
    static PlantProperties getProps(PlantType type);

    /** * @brief 获取所有配置的引用 (供初始化或辅助类遍历)
    *  返回map的const&,只能使用.find()和.at(),不允许[]访问
     */
    static const std::map<PlantType, PlantProperties>& getAllConfigs();
};