#pragma once
#ifndef CONTROL_LAYER_H
#define CONTROL_LAYER_H


#include "cocos2d.h"
#include<vector>
#include<string>
#include"manager/MapMgr.h"
#include"plant/PlantData.h"
/// <summary>
/// update:2025/12/19
/// 
/// 引入了PlantData
/// 从此使用PlantType _selectedPlantType = PlantType::Error;代替原有的数据int
/// 
/// 
/// </summary>

class ControlLayer : public cocos2d::Layer {
public:
    CREATE_FUNC(ControlLayer);
    bool init() override;

    // 设置选中的植物ID（替代PlantInputLayer的选择逻辑）
    void setSelectedPlantId(PlantType plantId);

private:
    void createTouchListener(); // 创建触摸监听


    bool UpdateTouchPositionIsInMap(); // 判断触摸位置是否在地图内
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

    static const PlantType SelectNoPlant;

    /*--- 数据成员 ---*/
    //GameMapInformation _gameMapInformation; // 游戏地图信息
    cocos2d::Vec2 _cur; // 触摸坐标
    cocos2d::Vec2 _plantsPosition; // 植物种植坐标
    PlantType _selectedPlantType = PlantType::Error;
    MapManager* _mapManager=nullptr;

};

#endif // CONTROL_LAYER_H