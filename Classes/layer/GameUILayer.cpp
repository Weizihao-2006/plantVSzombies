#include"GameUILayer.h"
#include"UI/UIScale9Sprite.h"
USING_NS_CC;
using namespace cocos2d::ui;
bool GameUILayer::init()
{
    if (!Layer::init()) 
        return false;

    createSunDisplay();  // 1. 阳光
    createCardBar();     // 2. 卡槽
    createShovel();      // 3. 铲子
    createProgressBar(); // 4. 进度条
    return true;
}

void GameUILayer::createSunDisplay()
{
	auto visible_size = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
}

void GameUILayer::createCardBar()
{
    auto bar = Scale9Sprite::create(""); 
    bar->setAnchorPoint(Vec2(0, 0));
    bar->setPosition(Vec2(230, 15)); // 紧挨阳光右侧
    addChild(bar);
    // 以后真正的植物卡片由 CardLayer 动态挂到 bar 上
}

void GameUILayer::createProgressBar()
{
    auto vis = Director::getInstance()->getVisibleSize();

    /* 进度条底图 */
    auto barBg = Sprite::create("progress_bar_bg.png");
    barBg->setAnchorPoint(Vec2(1, 0));
    barBg->setPosition(Vec2(vis.width - 20, 20));
    addChild(barBg);

    /* 进度条本身 */
    auto barFill = Sprite::create("progress_bar_fill.png");
    _progress = ProgressTimer::create(barFill);
    _progress->setType(ProgressTimer::Type::BAR);
    _progress->setMidpoint(Vec2(1, 0));
    _progress->setBarChangeRate(Vec2(1, 0));
    _progress->setPercentage(0);   // 初始 0%
    _progress->setPosition(barBg->getContentSize() / 2);
    barBg->addChild(_progress);

    /* 僵尸头（随进度移动） */
    auto head = Sprite::create("zombie_head.png");
    head->setName("head");
    head->setPosition(Vec2(barBg->getContentSize().width, barBg->getContentSize().height / 2));
    barBg->addChild(head);
}

//控制僵尸头从右往左移动,根据百分比
void GameUILayer::setProgress(float pct)
{
    _progress->setPercentage(pct);
    if (auto head = dynamic_cast<Sprite*>(_progress->getParent()->getChildByName("head")))
    {
        float maxX = _progress->getParent()->getContentSize().width;
        float nowX = maxX - maxX * pct * 0.01f;
        head->setPositionX(nowX);
    }
}