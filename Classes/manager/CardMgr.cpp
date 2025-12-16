#include "CardMgr.h"
#include "util/Global.h"
#include "cocos2d.h"
USING_NS_CC;

CardMgr* CardMgr::getInstance()
{
    static CardMgr mgr;
    return &mgr;
}

void CardMgr::initWithDeck(const std::vector<int>& deck)
{
    _deck = deck;
    _rt.resize(deck.size());

    // 初始化运行时状态
    for (auto& rt : _rt) {
        rt.inCD = false;
        rt.cdLeft = 0.f;
    }
}

void CardMgr::update(float dt)
{
    if (_rt.empty()) return;

    for (size_t i = 0; i < _rt.size(); ++i) {
        if (_rt[i].inCD && _rt[i].cdLeft > 0) {
            _rt[i].cdLeft -= dt;

            // 冷却完成
            if (_rt[i].cdLeft <= 0) {
                _rt[i].cdLeft = 0.f;
                _rt[i].inCD = false;
                CCLOG("CardMgr:  Card [%d] cooldown finished", i);
            }
        }
    }
}

bool CardMgr::canPlant(int idx) const
{
    if (idx < 0 || idx >= _rt.size()) return false;
    return !_rt[idx].inCD && canAfford(idx);
}

void CardMgr::startCool(int idx)
{
    if (idx < 0 || idx >= _deck.size()) return;

    _rt[idx].inCD = true;
    _rt[idx].cdLeft = g_cardAtlas[_deck[idx]].coolTime;
    CCLOG("CardMgr: Card [%d] cooldown started, coolTime=%.2f", idx, _rt[idx].cdLeft);
}

float CardMgr::getCoolPercent(int idx) const
{
    if (idx < 0 || idx >= _rt.size() || !_rt[idx].inCD) {
        return 0.f;
    }

    const float total = g_cardAtlas[_deck[idx]].coolTime;
    if (total <= 0) return 0.f;

    return 100.f * (_rt[idx].cdLeft / total);
}

float CardMgr::getCoolTimeLeft(int idx) const
{
    if (idx < 0 || idx >= _rt.size()) {
        return 0.f;
    }
    return _rt[idx].inCD ? _rt[idx].cdLeft : 0.f;
}