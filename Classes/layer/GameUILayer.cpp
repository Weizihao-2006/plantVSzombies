#include"GameUILayer.h"
#include "MainMenu.h"
#include"scene/GameScene.h"
#include"util/Global.h"
#include"UI/UIScale9Sprite.h"
USING_NS_CC;
using namespace cocos2d::ui;

void GameUILayer::update(float dt)
{
    //目前先只更新阳光
    if (_sunLabel) {

        _sunLabel->setString(std::to_string(Global::getInstance()->getSun()));

    }
}



bool GameUILayer::init() {
    if (!Layer::init()) return false;
    createSunDisplay(); // 创建阳光数量显示

    createPauseButton(); // 创建暂停按钮
    // createShovelButton(); // 创建铲子按钮
    // createProgressBar(); // 创建进度条
    
    this->scheduleUpdate();
    return true;
}


void GameUILayer::createSunDisplay() 
{
    _sunLabel = Label::createWithTTF("0", "fonts/arial.ttf", 36); // 创建阳光数量显示
    _sunLabel->setPosition(690.0f, 1210.0f); // 设置位置
    _sunLabel->setTextColor(Color4B::BLACK);
    addChild(_sunLabel); // 添加到当前层
}



void GameUILayer::createPauseButton() {
    // 创建"菜单"键精灵实现暂停
    _pauseButton = MenuItemImage::create("Button/btn_Menu.png", "Button/btn_Menu.png",
        [this](Ref* sender) {
            showPauseMenu();
        });

    _pauseButton->setAnchorPoint(Vec2(0, 1));
    _pauseButton->setScale(1.3f);
    auto menu = Menu::create(_pauseButton, nullptr);
    menu->setPosition(Vec2(1830, 1360)); // 放置在右上角
    this->addChild(menu);
}

void GameUILayer::createShovelButton() {
    _shovelButton = MenuItemImage::create("shovel_normal.png", "shovel_pressed.png", // 创建铲子按钮
        [this](Ref*) {
            // 铲子按钮的逻辑，例如移除植物
        });
    _shovelButton->setPosition(Director::getInstance()->getVisibleSize().width - 120, // 设置位置
        Director::getInstance()->getVisibleSize().height - 60);
    auto menu = Menu::create(_shovelButton, nullptr); // 创建菜单
    menu->setPosition(Vec2::ZERO); // 菜单位置归零
    addChild(menu); // 添加到当前层
}

void GameUILayer::createProgressBar() {
    auto progressBarBg = Sprite::create("progress_bar_bg.png"); // 创建进度条背景
    progressBarBg->setPosition(Director::getInstance()->getVisibleSize().width - 100, 50); // 设置位置
    addChild(progressBarBg); // 添加到当前层

    _progress = ProgressTimer::create(Sprite::create("progress_bar.png")); // 创建进度条
    _progress->setType(ProgressTimer::Type::BAR); // 设置进度条类型为条形
    _progress->setMidpoint(Vec2(0, 0)); // 设置进度条起点
    _progress->setBarChangeRate(Vec2(1, 0)); // 设置进度条变化方向
    _progress->setPercentage(0); // 设置初始进度为 0
    _progress->setPosition(progressBarBg->getContentSize() / 2); // 设置进度条位置
    progressBarBg->addChild(_progress); // 将进度条添加到背景上
}

void GameUILayer::setProgress(float pct) {
    _progress->setPercentage(pct * 100); // 更新进度条百分比
}

void GameUILayer::showPauseMenu() {
    // 1. 通知场景暂停游戏逻辑
    auto scene = dynamic_cast<GameScene*>(this->getParent());
    if (scene) scene->onPause(true);

    // 2. 创建一个半透明黑色遮罩背景，拦截所有点击
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 50));
    this->addChild(menuLayer, 100); // 确保在最顶层
    // 放一张背景图
    auto menubackground = Sprite::create("Button/Menu.png");
    menubackground->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    menuLayer->addChild(menubackground);

    // 3. 创建返回游戏按钮
    auto resumeBtn = ui::Button::create("Button/btn_Back.png");
    resumeBtn->setScale(1.5f);
    resumeBtn->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 60));
    resumeBtn->addClickEventListener([this, menuLayer, scene](Ref*) {
        if (scene) scene->onPause(false); // 恢复游戏
        menuLayer->removeFromParent();    // 移除菜单
        });
    menuLayer->addChild(resumeBtn);

    // 4. 创建重新开始按钮
    auto restartBtn = ui::Button::create("Button/btn_Restart.png");
    restartBtn->setScale(1.5f);
    restartBtn->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 40));
    restartBtn->addClickEventListener([this, scene](Ref*) {
        int levelId = scene->getLevelID();
        // 替换为新的当前关卡场景
        Director::getInstance()->replaceScene(GameScene::createWithLevel(levelId));
        });
    menuLayer->addChild(restartBtn);

    // 5. 创建退出按钮
    auto exitBtn = ui::Button::create("Button/out1.png");
    exitBtn->setScale(1.2f);
    exitBtn->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 184));
    exitBtn->addClickEventListener([](Ref*) {
        // Director::getInstance()->resume();
        // 主菜单场景是 MainMenu
        Director::getInstance()->replaceScene(MainMenu::create());
        // Director::getInstance()->end(); // 或者直接关闭（调试用）
        });
    menuLayer->addChild(exitBtn);

    // 6. 吞噬触摸：防止点击菜单按钮时穿透到下面的游戏层
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, menuLayer);
}