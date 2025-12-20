#pragma once
#include "plant/PlantData.h"
#include <vector>
#include <functional>
#include<unordered_map>

//对于卡槽,其实PlantType才是唯一的
class CardMgr {
public:
    static CardMgr* getInstance();

    // 每帧更新所有植物的冷却状态
    void updateCoolTime(float dt);

    // 检查某种植物是否能种植（阳光充足 且 不在冷却中）
    bool canPlant(PlantType type) const;

    // 启动某种植物的冷却计时
    void startCool(PlantType type);

    // 获取冷却进度 0~100 (100表示刚开始冷却，0表示就绪)
    float getCoolPercent(PlantType type) const;

    // 获取剩余冷却秒数
    float getCoolTimeLeft(PlantType type) const;

    // 判断是否在冷却中
    bool isInCoolDown(PlantType type) const {
        if (_rt.count(type)) return _rt.at(type).inCD;
        return false;
    }

    // 检查阳光是否足够
    bool canAfford(PlantType type) const;

    // 确认种植后调用（扣除阳光 + 启动冷却）
    void onPlantConfirmed(PlantType type);

    // 获取卡组大小（当前关卡可选植物数量）
    size_t getDeckSize() const { return _levelDeck.size(); }

    // 获取当前关卡的卡组序列
    const std::vector<PlantType>& getLevelDeck() const { return _levelDeck; }

    // 初始化当前关卡的卡组
    void initLevelDeck(const std::vector<PlantType>& deck);

    // 当卡牌被点击时通知 ControlLayer，参数直接传递类型
    std::function<void(PlantType)> onCardSelected;

    // 更新卡牌状态
    void reset();

private:
    struct Runtime {
        bool inCD = false;      // 是否在冷却中
        float cdLeft = 0.f;     // 剩余冷却时间
    };

    // 使用 Map存储运行时卡槽状态,用PlantType查询是否在冷却以及剩余冷却时间
    std::unordered_map<PlantType, Runtime> _rt;

    // 存储当前关卡选中的植物及其展示顺序
    std::vector<PlantType> _levelDeck;

    CardMgr() = default;
};