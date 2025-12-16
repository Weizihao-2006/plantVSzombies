#include "CardBarLayer.h"
#include "manager/CardMgr.h"
#include "layer/ControlLayer.h"
#include "manager/PlantMgr.h"
USING_NS_CC;

CardBarLayer* CardBarLayer::createWithFixedDeck(const std::vector<int>& deck)
{
    auto p = new (std::nothrow) CardBarLayer();
    if (p && p->initWithFixedDeck(deck)) { p->autorelease(); return p; }
    CC_SAFE_DELETE(p); return nullptr;
}

bool CardBarLayer::initWithFixedDeck(const std::vector<int>& deck) {
    if (!Layer::init()) return false;

    // 把layer的锚点设置在可视区域左下角
    this->setPosition(Vec2::ZERO);

    // 创建卡牌栏背景
    createCardSlotBg();

    _menu = Menu::create();
    _menu->setPosition(Vec2::ZERO);

    addChild(_menu);
    auto* mgr = CardMgr::getInstance();
    for (size_t i = 0; i < deck.size(); ++i) {
        const auto& def = g_cardAtlas[deck[i]];

        // 创建卡牌按钮
        auto btn = MenuItemImage::create(def.icon, def.icon,
            [this, i](Ref*) { onCardClicked(i); });
        btn->setAnchorPoint(Vec2::ZERO); // 把锚点设置在左下角，方便找位置
        btn->setScale(2.0f); // 把植物卡扩大
        btn->setPosition(Vec2(800 + i * 110, 1197));
        _menu->addChild(btn);

        // 创建冷却条，显示在卡牌上方
        

        // 添加冷却文字标签
        
    }
    // 用来找坐标点
    /*auto TestSpr = Sprite::create("plantCard/Repeater.png");
    TestSpr->setAnchorPoint(Vec2::ZERO);
    TestSpr->setPosition(1900, 1150);
    addChild(TestSpr);*/
    return true;
}

void CardBarLayer::createCardSlotBg() {
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

void CardBarLayer::updateCoolDown(int idx, float percent) {
    if (idx < 0 || idx >= _cdBars.size()) return;

    // 更新进度条
    _cdBars[idx]->setPercentage(percent);

    // 更新冷却文字
    auto label = _cardBarBg->getChildByTag(100 + idx);
    if (label) {
        auto textLabel = dynamic_cast<Label*>(label);
        if (textLabel && percent > 0) {
            float cdTime = percent > 0 ? (percent / 100.f) * g_cardAtlas[idx].coolTime : 0;
            textLabel->setString(StringUtils::format("%. 1f", cdTime));
            textLabel->setVisible(true);
        }
        else {
            textLabel->setVisible(false);
        }
    }
}

void CardBarLayer::updateCardState(int idx, bool canUse) {
    if (idx < 0 || idx >= _cardSlots.size()) return;

    auto slot = _cardSlots[idx];
    if (canUse) {
        slot->setOpacity(255);  // 完全不透明
    }
    else {
        slot->setOpacity(128);  // 半透明（禁用状态）
    }
}

void CardBarLayer::onCardClicked(int idx)
{
    // 暂不考虑冷却，直接选中植物
    auto controlLayer = this->getParent()->getChildByName("ControlLayer"); // 获取控制层
    if (controlLayer) {
        // 通过deck映射真实植物ID（LEVEL1_DECK中存储的是植物ID）
        int plantId = idx;
        static_cast<ControlLayer*>(controlLayer)->setSelectedPlantId(plantId);
    }

    //if (!mgr->canPlant(idx))
    //    return;                 // 不能种植（冷却中或阳光不足）
    //mgr->onCardSelected(idx);   // 通知外部（PlantInputLayer等）
}