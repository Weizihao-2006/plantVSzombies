#include"GameUILayer.h"
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
#if 0
    createPauseButton(); // 创建暂停按钮
    createShovelButton(); // 创建铲子按钮
    createProgressBar(); // 创建进度条

#endif
    
    this->scheduleUpdate();
    return true;
}


void GameUILayer::createSunDisplay() 
{
    _sunLabel = Label::createWithTTF("50", "fonts/arial.ttf", 36); // 创建阳光数量显示
    _sunLabel->setPosition(690.0f, 1210.0f); // 设置位置
    _sunLabel->setTextColor(Color4B::BLACK);
    addChild(_sunLabel); // 添加到当前层
}



void GameUILayer::createPauseButton() {
    _pauseButton = MenuItemImage::create("pause_normal.png", "pause_pressed.png", // 创建暂停按钮
        [this](Ref*) {
            auto* scene = dynamic_cast<GameScene*>(this->getParent()); // 获取父场景
            if (scene) {
                scene->onPause(true); // 调用父场景的暂停函数
                showPauseMenu(); // 显示暂停菜单
            }
        });
    _pauseButton->setPosition(Director::getInstance()->getVisibleSize().width - 60, // 设置位置
        Director::getInstance()->getVisibleSize().height - 60);
    auto menu = Menu::create(_pauseButton, nullptr); // 创建菜单
    menu->setPosition(Vec2::ZERO); // 菜单位置归零
    addChild(menu); // 添加到当前层
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
    auto menuLayer = Layer::create(); // 创建一个新层用于显示暂停菜单
    addChild(menuLayer, 100); // 添加到当前层，层级较高

    // 创建继续按钮
    auto continueButton = Button::create("continue_normal.png", "continue_pressed.png");


    continueButton->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2,
        Director::getInstance()->getVisibleSize().height / 2 + 100));

    continueButton->addClickEventListener([this,menuLayer](Ref*) {
        auto* scene = dynamic_cast<GameScene*>(this->getParent()); // 获取父场景
        if (scene) {
            scene->onPause(false); // 调用父场景的继续函数
        }
        menuLayer->removeFromParent(); // 移除暂停菜单
        });
    menuLayer->addChild(continueButton); // 添加到暂停菜单层

    // 创建重新开始按钮
    auto restartButton = Button::create("restart_normal.png", "restart_pressed.png");
    restartButton->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2, // 设置位置
        Director::getInstance()->getVisibleSize().height / 2));


    restartButton->addClickEventListener([this,menuLayer](Ref*) {
        auto* scene = dynamic_cast<GameScene*>(this->getParent()); // 获取父场景
        if (scene) {
            scene->onPause(false); // 调用父场景的继续函数
            Director::getInstance()->replaceScene(GameScene::createWithLevel(scene->getLevelID())); // 重新加载当前关卡
        }
        menuLayer->removeFromParent(); // 移除暂停菜单
        });
    menuLayer->addChild(restartButton); // 添加到暂停菜单层

    // 创建退出按钮
    auto exitButton = Button::create("exit_normal.png", "exit_pressed.png");

    exitButton->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2, // 设置位置
        Director::getInstance()->getVisibleSize().height / 2 - 100));


    exitButton->addClickEventListener([this](Ref*) {
        Director::getInstance()->popScene(); // 返回上一个场景
        });
    menuLayer->addChild(exitButton); // 添加到暂停菜单层


    
}