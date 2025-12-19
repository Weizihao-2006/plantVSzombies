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
        auto cdSprite = Sprite::create(props.cardIcon_locked); // 灰色图标
        auto progressTimer = ProgressTimer::create(cdSprite);

        // --- 修正点 1：位置对齐 ---
        // 因为 btn 的锚点是 ZERO(左下角)，所以 progressTimer 的位置直接等于 btn 的位置
        // 不要加 getContentSize()，否则会发生偏移
        progressTimer->setAnchorPoint(Vec2::ZERO);
        progressTimer->setPosition(btn->getPosition());

        // --- 修正点 2：缩放对齐 ---
        // btn 被缩放了 2.0f，遮罩也必须手动同步这个缩放，大小才会一致
        progressTimer->setScale(btn->getScale());

        // --- 修正点 3：遮罩方向 ---
        progressTimer->setType(ProgressTimer::Type::BAR);
        // Midpoint(0, 1) 代表从上方开始变化
        progressTimer->setMidpoint(Vec2(0, 1));
        // BarChangeRate(0, 1) 代表只在纵向（Y轴）有进度变化，横向不动
        progressTimer->setBarChangeRate(Vec2(0, 1));

        // --- 修正点 4：层级 ---
        // 必须确保遮罩在按钮上方，this 是 CardBarLayer，10 是一个较高的 ZOrder
        progressTimer->setOpacity(200); // 设置透明度，能看到底图但有阴影感
        this->addChild(progressTimer, 10);

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
        bool inCD = _cardMgr->isInCoolDown(type);
        bool canAfford = _cardMgr->canAfford(type);

        // 1. 更新遮罩进度
        float percent = _cardMgr->getCoolPercent(type);
        // CardMgr 返回的应该是：刚种下时 100，冷却结束时 0
        _cdBars[i]->setPercentage(percent);

        // 2. 核心逻辑：背景亮度处理
        // 只有 (不在冷却) 且 (买得起) 时，底图才恢复 255 亮度
        if (inCD) {
            // 只要在冷却中，底图保持最暗
            _menu->getChildren().at(i)->setOpacity(100);
        }
        else {
            // 冷却结束了，根据钱够不够决定是 255 还是 128
            _menu->getChildren().at(i)->setOpacity(canAfford ? 255 : 128);
        }
    }
}