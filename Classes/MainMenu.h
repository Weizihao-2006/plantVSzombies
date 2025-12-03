#pragma once
#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include "cocos2d.h"

class MainMenu :public cocos2d::Scene
{
public:
    // 创建场景
    static cocos2d::Scene* createScene();

    // 初始化场景
    virtual bool init();

    // 检查是否退出主菜单
    void MainMenuCloseCallback(cocos2d::Ref* pSender);

    // 手动调用create()函数
    CREATE_FUNC(MainMenu);
};

#endif // __MAINMENU_H__