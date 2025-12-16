#include "CardMgr.h"
#include "util/Global.h"

CardMgr* CardMgr::getInstance() 
{
    static CardMgr mgr; return &mgr; 
}


void CardMgr::initWithDeck(const std::vector<int>& deck)
{
    _deck = deck;
    _rt.resize(deck.size());
}
void CardMgr::update(float dt) 
{
    for (size_t i = 0; i < _rt.size(); ++i) {
        if (_rt[i].inCD && _rt[i].cdLeft > 0) {
            _rt[i].cdLeft -= dt;
            if (_rt[i].cdLeft <= 0) {
                _rt[i].inCD = false;
            }
        }
    }
}
bool CardMgr::canPlant(int idx) const 
{
    return !_rt[idx].inCD && canAfford(idx);
}
void CardMgr::startCool(int idx) 
{
    _rt[idx].inCD = true;
    _rt[idx].cdLeft = g_cardAtlas[_deck[idx]].coolTime;
}
float CardMgr::getCoolPercent(int idx) const 
{
    if (!_rt[idx].inCD) 
        return 0;
    const float total = g_cardAtlas[_deck[idx]].coolTime;
    return 100.f * (_rt[idx].cdLeft / total);
}