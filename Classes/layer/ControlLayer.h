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

    void showPreview(cocos2d::Sprite* preview, bool& isShowing);
    void hidePreview(cocos2d::Sprite* preview, bool& isShowing);
    void updatePreviewPosition(cocos2d::Sprite* preview);//更新预览位置
    // 植物预览相关方法
    
    // 植物预览相关变量
    cocos2d::Sprite* _plantPreview = nullptr; // 预览精灵
    bool _isPlantPreviewShowing = false;           // 预览是否显示
    Sprite* createPreview(const std::string& filename, float scale = 1.5f, uint8_t opacity = 128);
    void cancelCurrentAction();
    void handlePreviewLogic(Sprite*& targetSprite, bool& isShowingFlag, bool canShowCondition);

    // 高亮条相关处理
    cocos2d::DrawNode* _highlightRow = nullptr; // 横向高亮条
    cocos2d::DrawNode* _highlightCol = nullptr; // 纵向高亮条
    void updateHighlightBars(int row, int col); // 更新高亮条位置的方法
    void clearHighlightBars();                  // 隐藏/清除高亮条

    static const PlantType SelectNoPlant;

    /*--- 数据成员 ---*/
    cocos2d::Vec2 _cur; // 触摸坐标
    cocos2d::Vec2 _MapPosition; //地图坐标
    PlantType _selectedPlantType = PlantType::Error;
    MapManager* _mapManager=nullptr;

private:
    bool _isShovelSelected = false;  // 是否选中了铲子
    bool _isShovelPreviewShowing = false;           // 预览是否显示
    cocos2d::Sprite* _shovelPreview = nullptr; // 铲子的预览精灵

public:
    // 供 UI 层调用的接口
    void setShovelActive(bool active);
};

#endif // CONTROL_LAYER_H