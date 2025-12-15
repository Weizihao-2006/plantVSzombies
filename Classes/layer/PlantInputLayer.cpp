#include "PlantInputLayer.h"
#include "manager/PlantMgr.h"
#include "manager/CardMgr.h"
USING_NS_CC;

bool PlantInputLayer::init() {
    if (!Layer::init()) return false;

    _touchListener = EventListenerTouchOneByOne::create();

    _touchListener->onTouchBegan = [this](Touch* t, Event* e) {
        if (this->_selectedId < 0) return false; // 没选植物
        this->_ghost->setPosition(t->getLocation());
        this->_ghost->setVisible(true);
        return true;
        };

    _touchListener->onTouchEnded = [this](Touch* t, Event* e) {
        Vec2 rowCol = getRowCol(t->getLocation());
        if (rowCol.x < 0) {
            _ghost->setVisible(false); return;
        }

        PlantMgr::getInstance()->createPlantAt(rowCol, _selectedId);
        CardMgr::getInstance()->onPlantConfirmed(_selectedId); // 启动冷却

        _ghost->setVisible(false);
        _selectedId = -1;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
    return true;
}

void PlantInputLayer::onCardSelected(int plantId) {
    _selectedId = plantId;
    if (!_ghost) {
        _ghost = Sprite::create(g_cardAtlas[plantId].icon);
        _ghost->setOpacity(150);
        addChild(_ghost, 10);
    }
    else {
        _ghost->setSpriteFrame(g_cardAtlas[plantId].icon);
    }
}

Vec2 PlantInputLayer::getRowCol(const Vec2& worldPos) {
    int col = (worldPos.x - 200) / 100;
    int row = (worldPos.y - 100) / 100;
    if (row < 0 || row > 4 || col < 0 || col > 8) 
        return Vec2(-1, -1);
    return Vec2(col, row);
}