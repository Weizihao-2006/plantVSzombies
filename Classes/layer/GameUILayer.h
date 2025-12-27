#ifndef GAME_UI_LAYER_H
#define GAME_UI_LAYER_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"


//更新 _sunLabel 的文本。 
/*


*/


class GameUILayer : public cocos2d::Layer {
public:
    CREATE_FUNC(GameUILayer);
    bool init() override;
    virtual void update(float dt) override;
    /*--- 外部调用 ---*/
    void setProgress(float pct); // 更新进度条百分比

    //让铲子显示/消失
    void setShoveButtonVisible(bool visible){
        if (_shovelButton)
            _shovelButton->setVisible(visible);
    }
    
        
    void toggleGameSpeed();
private:
    /*--- 节点 ---*/
    cocos2d::Label* _sunLabel = nullptr; // 阳光数量显示
    cocos2d::Label* _speedLabel = nullptr; //倍速按钮
    cocos2d::ProgressTimer* _progress = nullptr; // 进度条
    cocos2d::MenuItemImage* _pauseButton = nullptr; // 暂停按钮
    cocos2d::MenuItemImage* _shovelButton = nullptr; // 铲子按钮

    void createSpeedButton();
    void createSunDisplay(); // 创建阳光数量显示
    void createPauseButton(); // 创建暂停按钮
    void createShovelButton(); // 创建铲子按钮
    void createProgressBar(); // 创建进度条
    void showPauseMenu(); // 显示暂停菜单
};

#endif // !GAME_UI_LAYER_H