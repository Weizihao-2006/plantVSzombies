#include "CardBarLayer.h"
#include "manager/CardMgr.h"
USING_NS_CC;

CardBarLayer* CardBarLayer::createWithFixedDeck(const std::vector<int>& deck)
{
    auto p = new (std::nothrow) CardBarLayer();
    if (p && p->initWithFixedDeck(deck)) { p->autorelease(); return p; }
    CC_SAFE_DELETE(p); return nullptr;
}
bool CardBarLayer::initWithFixedDeck(const std::vector<int>& deck) {
    if (!Layer::init()) return false;

    _menu = Menu::create();
    _menu->setPosition(Vec2::ZERO);
    addChild(_menu);

    auto* mgr = CardMgr::getInstance();
    for (size_t i = 0; i < deck.size(); ++i) {
        const auto& def = g_cardAtlas[deck[i]];
        auto btn = MenuItemImage::create(def.icon, def.icon,
            [this, i](Ref*) { onCardClicked(i); });
        btn->setPosition(Vec2(80 + i * 110, 60));
        _menu->addChild(btn);

        // 冷却遮罩（初始隐藏）
        auto bar = ProgressTimer::create(Sprite::create("cd_mask.png"));
        bar->setType(ProgressTimer::Type::BAR);
        bar->setMidpoint(Vec2(0, 1));
        bar->setBarChangeRate(Vec2(0, 1));
        bar->setPercentage(0);
        btn->addChild(bar);
        _cdBars.push_back(bar);
    }
    return true;
}
void CardBarLayer::onCardClicked(int idx) 
{
    auto mgr = CardMgr::getInstance();
    if (!mgr->canPlant(idx)) 
        return;                 // 阳光或冷却不足
    mgr->onCardSelected(idx);  //通知外部（PlantInputLayer）
}