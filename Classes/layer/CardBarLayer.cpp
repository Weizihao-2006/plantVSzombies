#include "CardBarLayer.h"
#include "layer/ControlLayer.h"
#include "manager/PlantMgr.h"
USING_NS_CC;

CardBarLayer* CardBarLayer::create()
{
    auto p = new (std::nothrow) CardBarLayer();
    if (p && p->init()) { p->autorelease(); return p; }
    CC_SAFE_DELETE(p); return nullptr;
}

bool CardBarLayer::init() {
    if (!Layer::init()) return false;

    // 把layer的锚点设置在可视区域左下角
    this->setPosition(Vec2::ZERO);

    // 创建卡牌栏背景
    createCardSlotBg();

    _menu = Menu::create();
    _menu->setPosition(Vec2::ZERO);

    addChild(_menu);

    ///update 2025/12/19
    ///
    /// 
    /// 
    /// 
    _cardMgr = CardMgr::getInstance();
    _cardMgr->initLevelDeck(CardDeck);
    // 获取当前关卡的卡组序列（PlantType 向量）
    const auto& deck = _cardMgr->getLevelDeck();


    for (size_t i = 0; i < deck.size(); ++i) {
        PlantType type = deck[i];
        // 获取该植物的全套属性
        auto props = PlantData::getProps(type);

        // 1. 创建卡牌按钮 (使用 PlantData 中的图标路径)
        auto btn = MenuItemImage::create(props.cardIcon, props.cardIcon,
            [this, type](Ref*) {
                // 点击后直接通知管理器，由管理器分发给 ControlLayer
                CardMgr::getInstance()->onCardSelected(type);
            });

        btn->setAnchorPoint(Vec2::ZERO);
        btn->setScale(2.0f);
        btn->setPosition(Vec2(800 + i * 110, 1197));
        _menu->addChild(btn);

        // 2. 初始化进度条 (对应冷却视觉)
        auto cdSprite = Sprite::create(props.cardIcon_locked); // 使用锁定的灰色图标作为覆盖层
        auto progressTimer = ProgressTimer::create(cdSprite);
        progressTimer->setType(ProgressTimer::Type::BAR);
        progressTimer->setMidpoint(Vec2(0.5, 0)); // 从下往上遮盖
        progressTimer->setBarChangeRate(Vec2(0, 1));
        progressTimer->setPosition(btn->getPosition() + btn->getContentSize()); // 需根据缩放微调坐标
        this->addChild(progressTimer, 1);

        _cdBars.push_back(progressTimer);
    }

    // 启动定时器更新视觉效果
    this->scheduleUpdate();
    return true;
}

void CardBarLayer::createCardSlotBg() 
{
    // 创建卡牌栏背景容器
    _cardBarBg = Node::create();
    _cardBarBg->setPosition(Vec2::ZERO);
    addChild(_cardBarBg);

    // 添加整体背景
    auto bg = Sprite::create("Bank.png");  // 需要提供图片资源
    if (bg) {
        bg->setScaleX(2.5f);
        bg->setScaleY(2.0f);
        bg->setAnchorPoint(Vec2::ZERO); // 把锚点设置在左下角
        bg->setPosition(Vec2(600, 1180));
        _cardBarBg->addChild(bg, -1);  // 放在最后
    }
}

void CardBarLayer::update(float dt) 
{
    const auto& deck = _cardMgr->getLevelDeck();

    for (size_t i = 0; i < deck.size(); ++i) {
        PlantType type = deck[i];

        // 更新冷却条
        float percent = _cardMgr->getCoolPercent(type);
        _cdBars[i]->setPercentage(percent);

        // 更新阳光充足/不足的视觉状态
        bool canAfford = _cardMgr->canAfford(type);
        // 如果阳光不足，可以设置卡片变暗（透明度）
        _menu->getChildren().at(i)->setOpacity(canAfford ? 255 : 128);
    }
}
