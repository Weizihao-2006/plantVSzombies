#ifndef GAME_UI_LAYER_H
#define GAME_UI_LAYER_H

#include"cocos2d.h"
#include<string>

//负责更新阳光总数 展示铲子 更新进度条

class GameUILayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(GameUILayer);
    bool init() override;

    /*--- 外部调用 ---*/
    void refreshSun(int val) { _sunLabel->setString(std::to_string(val)); }
    void setProgress(float pct);    //右下角进度条

private:

    /*--- 节点 ---*/
    cocos2d::Label* _sunLabel = nullptr;
    cocos2d::ProgressTimer* _progress = nullptr;
    cocos2d::MenuItemImage* _shovelBtn = nullptr;

    void createSunDisplay(); //阳光
    void createCardBar();    //植物卡槽背景
    void createShovel();     //铲子
    void createProgressBar();//右下角进度条
};


#endif // !GAME_UI_LAYER_H

