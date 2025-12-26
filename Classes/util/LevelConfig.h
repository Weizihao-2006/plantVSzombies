#ifndef LEVEL_CONFIG_H
#define LEVEL_CONFIG_H

#include <vector>
#include <map>
#include "zombie/ZombieData.h" // 确保能用到 ZombieType

enum class WaveType {
    NORMAL,      // 普通波
    HUGE_WAVE,   // 一大波僵尸即将来袭
    FINAL_WAVE   // 最后一波
};

enum class GameMode {
    ADVENTURE, // 冒险模式（有终点）
    ENDLESS    // 无尽模式（原有的增长逻辑）
};

// 每一组僵尸的定义
struct ZombieGroup {
    ZombieType type;
    int count;
};

// 每一波的定义
struct WaveConfig {
    WaveType type; // 新增：标记这一波的类型
    std::vector<ZombieGroup> groups;
};

// 整个关卡的定义
struct LevelData {
    int totalWaves;
    std::map<int, WaveConfig> waves; // key 是波次序号 (1, 2, 3...)
};

// 更新静态配置表
class LevelConfig {
public:
    static const std::map<int, LevelData>& getLevels() {
        static const std::map<int, LevelData> adventureLevels = {
            { 1, { 3, {
                // 第一关共 3 波示例
                { 1, {WaveType::NORMAL, {{{ZombieType::Normal, 1}}}}},
                { 2, {WaveType::NORMAL, {{{ZombieType::Normal, 2}}}}},
                { 3, {WaveType::FINAL_WAVE, {{{ZombieType::Conehead, 1}, {ZombieType::Normal, 2}}}}}
            }}},
            { 2, { 5, {
                // 第二关共 5 波示例
                { 1, {WaveType::NORMAL, {{{ZombieType::Normal, 1}}}}},
                { 2, {WaveType::NORMAL, {{{ZombieType::Normal, 1}, {ZombieType::Conehead, 1}}}}},
                { 3, {WaveType::HUGE_WAVE, {{{ZombieType::Normal, 4},{ZombieType::Conehead, 2}}}}},
                { 4, {WaveType::NORMAL, {{{ZombieType::Normal, 2}, {ZombieType::Buckethead, 1}}}}},
                { 5, {WaveType::FINAL_WAVE, {{{ZombieType::Conehead, 2}, {ZombieType::Normal, 2}, {ZombieType::Buckethead, 3}}}}}
            }}},
            { 3, { 10, {
                // 第三关共 10 波示例
                { 1, {WaveType::NORMAL, {{{ZombieType::Normal, 1}}}}},
                { 2, {WaveType::NORMAL, {{{ZombieType::Normal, 2}}}}},
                { 3, {WaveType::NORMAL, {{{ZombieType::Normal, 2}, {ZombieType::Conehead, 1}}}}},
                { 4, {WaveType::NORMAL, {{{ZombieType::Conehead, 1}, {ZombieType::Buckethead, 1}}}}},
                { 5, {WaveType::HUGE_WAVE, {{{ZombieType::Normal, 3},{ZombieType::Buckethead, 1}, {ZombieType::Conehead, 1}, {ZombieType::Giant, 1}}}}},
                { 6, {WaveType::NORMAL, {{{ZombieType::Normal, 2}, {ZombieType::Conehead, 2}}}}},
                { 7, {WaveType::NORMAL, {{{ZombieType::Normal, 1}, {ZombieType::Conehead, 2}, {ZombieType::Buckethead, 1}}}}},
                { 8, {WaveType::NORMAL, {{{ZombieType::Conehead, 2},{ZombieType::Buckethead, 2}}}}},
                { 9, {WaveType::NORMAL, {{{ZombieType::Normal, 2}, {ZombieType::Buckethead, 1}}}}},
                { 10, {WaveType::FINAL_WAVE, {{{ZombieType::Conehead, 2}, {ZombieType::Normal, 2}, {ZombieType::Buckethead, 3}, {ZombieType::Giant, 2}}}}}
            }}}
        };
        return adventureLevels;
    }
};

#endif