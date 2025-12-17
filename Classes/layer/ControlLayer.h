#pragma once
#ifndef CONTROL_LAYER_H
#define CONTROL_LAYER_H



/*
完整流程梳理
1. 点击植物卡槽
事件触发：玩家点击植物卡槽中的某个植物图标。
处理逻辑：CardBarLayer 捕获点击事件，调用 CardMgr 检查阳光是否足够且该植物是否处于冷却状态。
结果：如果条件满足，CardMgr 通知 PlantInputLayer 玩家选择了某种植物，并进入种植预览模式。
2. 进入种植预览模式
事件触发：PlantInputLayer 收到 CardMgr 的通知。
处理逻辑：PlantInputLayer 显示一个半透明的植物预览，并跟随鼠标或触摸移动。
结果：玩家可以拖动预览植物到地图上的任意位置。
3. 点击地图格子
事件触发：玩家在地图上点击或触摸某个格子。
处理逻辑：
PlantInputLayer 捕获触摸事件，调用 ControlLayer 判断点击位置是否在地图内且是否可以种植植物。
如果可以种植，PlantInputLayer 调用 PlantMgr 的 plantAtPosition 方法，在指定位置种植植物。
如果该位置已有植物，调用 PlantMgr 的 removePlantAtPosition 方法移除植物。
结果：植物被种植到地图上，或者已有植物被移除。
4. 更新地图状态
事件触发：PlantMgr 完成种植或移除操作。
处理逻辑：
PlantMgr 更新 GameMapInformation 中的地图状态。
PlantMgr 创建或移除对应的植物节点。
结果：地图状态更新，植物节点出现在地图上或被移除。
5. 触发冷却和阳光扣除
事件触发：植物成功种植。
处理逻辑：
CardMgr 为该植物卡槽启动冷却计时。
CardMgr 扣除种植该植物所需的阳光。
结果：植物卡槽进入冷却状态，阳光数量减少。
*/

#include "cocos2d.h"
#include<vector>
#include<string>

struct GameMapInformation 
{
    GameMapInformation() : rowNumbers(5), columnNumbers(9) {
        plantsMap.resize(rowNumbers, std::vector<int>(columnNumbers, -1));
    }

    std::vector<std::vector<int>> plantsMap; // 植物种植地图，-1 表示空闲
    unsigned int rowNumbers;                // 行数
    unsigned int columnNumbers;             // 列数

    float mapLeft;   // 地图左边界
    float mapTop;    // 地图上边界
    float mapRight;  // 地图右边界
    float mapBottom; // 地图下边界
};

extern std::vector<std::string> _CardPreview;

class ControlLayer : public cocos2d::Layer {
public:
    CREATE_FUNC(ControlLayer);
    bool init() override;

    /*--- 外部调用 ---*/
    void setPlantMapCanPlant(const unsigned int colum, const unsigned int row);
    // 设置选中的植物ID（替代PlantInputLayer的选择逻辑）
    void setSelectedPlantId(int plantId);

private:
    void createSchedule(); // 创建定时器
    void controlCardEnabled(); // 控制卡牌是否可用
    void calculatePlantPosition(); // 计算植物种植位置
    void createTouchListener(); // 创建触摸监听
    bool touchBegan(cocos2d::Touch* touch, cocos2d::Event* event); // 触摸开始
    void touchMoved(cocos2d::Touch* touch, cocos2d::Event* event); // 触摸移动
    void touchEnded(cocos2d::Touch* touch, cocos2d::Event* event); // 触摸结束
    bool judgeTouchPositionIsInMap(); // 判断触摸位置是否在地图内
    bool judgeTouchPositionIsCanPlant(); // 判断触摸位置是否可以种植植物
    bool judgeTouchPositionHavePlant(); // 判断触摸位置是否有植物


    // 植物预览相关方法
    void showPlantPreview();    // 显示植物预览
    void hidePlantPreview();    // 隐藏植物预览
    void updatePreviewPosition(); // 更新预览位置

    // 植物预览相关变量
    cocos2d::Sprite* _plantPreview = nullptr; // 预览精灵
    bool _isPreviewShowing = false;           // 预览是否显示
    bool _isPreviewSpriteCreated = false;     // 检查精灵是否已创建

    // 高亮条相关处理
    cocos2d::DrawNode* _highlightRow = nullptr; // 横向高亮条
    cocos2d::DrawNode* _highlightCol = nullptr; // 纵向高亮条
    void updateHighlightBars(int row, int col); // 更新高亮条位置的方法
    void clearHighlightBars();                  // 隐藏/清除高亮条

    /*--- 数据成员 ---*/
    GameMapInformation _gameMapInformation; // 游戏地图信息
    cocos2d::Vec2 _cur; // 触摸坐标
    cocos2d::Vec2 _plantsPosition; // 植物种植坐标
    int _selectedPlantId = -1; // 所选植物 ID
};

#endif // CONTROL_LAYER_H