#include"GameUILayer.h"
#include "MainMenu.h"
#include"scene/GameScene.h"
#include"util/Global.h"
#include"UI/UIScale9Sprite.h"
#include "AudioEngine.h"
USING_NS_CC;
using namespace cocos2d::ui;

void GameUILayer::update(float dt)
{
    //目前先只更新阳光
    if (_sunLabel) {
        _sunLabel->setString(std::to_string(Global::getInstance()->getSun()));

    }
}

void GameUILayer::toggleGameSpeed()
{
    auto scheduler = Director::getInstance()->getScheduler();
    float currentScale = scheduler->getTimeScale();
    float nextScale = 1.0f;

    // 简单的循环切换逻辑：1x -> 2x -> 3x -> 1x
    if (currentScale >= 1.0f && currentScale < 2.0f) {
        nextScale = 2.0f;
    }
    else if (currentScale >= 2.0f && currentScale < 3.0f) {
        nextScale = 3.0f;
    }
    else {
        nextScale = 1.0f;
    }

    // 设置全局时间缩放 [参考全局调度器机制]
    scheduler->setTimeScale(nextScale);

    // 更新文本内容
    auto label = dynamic_cast<Label*>(this->getChildByName("SpeedLabel"));
    if (_speedLabel) {
        _speedLabel->setString(StringUtils::format("%.1fx", nextScale));
    }
}
void GameUILayer::createSpeedButton() {
    // 1. 创建按钮背景
    auto speedBtn = MenuItemImage::create(
        "btn_common_normal.png", // 正常状态图片
        "btn_common_disable.png", // 点击状态图片
        [this](Ref* sender) {
            this->toggleGameSpeed();
        }
    );

    // 2. 创建文字标签显示当前倍速
    _speedLabel = Label::createWithTTF("1.0x", "fonts/arial.ttf", 24);
    _speedLabel->setPosition(speedBtn->getContentSize() / 2);
    _speedLabel->setName("SpeedLabel");
    speedBtn->setScale(0.8f);
    speedBtn->addChild(_speedLabel);

    // 3. 创建菜单并添加到 UI 层
    auto menu = Menu::create(speedBtn, nullptr);
    menu->setPosition(1767.0f, 1210.0f);
    this->addChild(menu, 100);
}

bool GameUILayer::init() {
    if (!Layer::init()) return false;

    Director::getInstance()->getScheduler()->setTimeScale(1.0f);

    createSunDisplay(); // 创建阳光数量显示

    createPauseButton(); // 创建暂停按钮
    createShovelButton(); // 创建铲子按钮
    createSpeedButton();
    
    this->scheduleUpdate();
    return true;
}


void GameUILayer::createSunDisplay() 
{
    int origin_sun = Global::getInstance()->getSun();
    _sunLabel = Label::createWithTTF(std::to_string(origin_sun), "fonts/arial.ttf", 36); // 创建阳光数量显示
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

    auto shover_background = Sprite::create("ShovelBack.png");
    shover_background->setPosition(Vec2(1750.0f, 1300.0f));
    shover_background->setScaleX(1.1f);
    shover_background->setScaleY(3.0f);
    this->addChild(shover_background);

    _shovelButton = MenuItemImage::create("Shovel.png", "Shovel.png", // 创建铲子按钮
        [this](Ref*) {
            auto scene=Director::getInstance()->getRunningScene();
            auto controlLayer = dynamic_cast<ControlLayer*>(scene->getChildByName("ControlLayer"));
            if (controlLayer) {
                controlLayer->setShovelActive(true);//启动铲子
            }
        });

    _shovelButton->setPosition(Vec2(1750.0f, 1300.0f));
    _shovelButton->setRotation(-45.0f);
    auto menu = Menu::create(_shovelButton, nullptr); // 创建菜单
    menu->setPosition(Vec2::ZERO); // 菜单位置归零
    this->addChild(menu); // 添加到当前层
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
    // 播放暂停键音乐
    AudioEngine::play2d("Music/pause.MP3", false, 1.0f);

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