#include "CardMgr.h"
#include "util/Global.h"
#include "cocos2d.h"

USING_NS_CC;

CardMgr* CardMgr::getInstance() {
    static CardMgr mgr;
    return &mgr;
}

void CardMgr::initLevelDeck(const std::vector<PlantType>& deck) {
    _levelDeck = deck;
    _rt.clear();
    for (auto type : _levelDeck) {
        _rt[type] = Runtime{ false, 0.0f };
    }
}

void CardMgr::updateCoolTime(float dt) {
    for (auto& [type, state] : _rt) {
        if (state.inCD) {
            state.cdLeft -= dt;
            if (state.cdLeft <= 0) {
                state.cdLeft = 0.0f;
                state.inCD = false;
                CCLOG("CardMgr: %s cooldown finished", PlantData::getProps(type).name.c_str());
            }
        }
    }
}

bool CardMgr::canPlant(PlantType type) const {
    return !isInCoolDown(type) && canAfford(type);
}

bool CardMgr::canAfford(PlantType type) const {
    auto props = PlantData::getProps(type);
    if (props.type == PlantType::Error) return false;
    return Global::getInstance()->getSun() >= props.sunCost;
}

void CardMgr::startCool(PlantType type) 
{
    auto props = PlantData::getProps(type);
    if (props.type == PlantType::Error) return;

    _rt[type].inCD = true;
    _rt[type].cdLeft = props.coolDownTime;
}

void CardMgr::onPlantConfirmed(PlantType type) {
    auto props = PlantData::getProps(type);
    if (props.type == PlantType::Error) return;

    if (Global::getInstance()->consumeSun(props.sunCost)) {
        startCool(type);
        CCLOG("CardMgr: Planted %s, cost %d", props.name.c_str(), props.sunCost);
    }
}

float CardMgr::getCoolPercent(PlantType type) const {
    if (!isInCoolDown(type)) return 0.0f;

    float total = PlantData::getProps(type).coolDownTime;
    if (total <= 0) return 0.0f;

    return (_rt.at(type).cdLeft / total) * 100.0f;
}

float CardMgr::getCoolTimeLeft(PlantType type) const {
    if (isInCoolDown(type)) return _rt.at(type).cdLeft;
    return 0.0f;
}