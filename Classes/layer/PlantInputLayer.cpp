#include "PlantInputLayer.h"
#include "manager/PlantMgr.h"
#include "manager/CardMgr.h"
USING_NS_CC;

bool PlantInputLayer::init() {
    if (!Layer::init()) return false;

    _touchListener = EventListenerTouchOneByOne::create();

    _touchListener->onTouchBegan = [this](Touch* t, Event* e) {
        if (this->_selectedId < 0)
            return false;          // 没选卡，不处理
        this->_ghost->setPosition(t->getLocation());
        this->_ghost->setVisible(true);
        return true;
        };


    _touchListener->onTouchEnded = [this](Touch* t, Event* e) {
        //总之就是在这个地方判断是否符合格点要求
        Vec2 rowCol = getRowCol(t->getLocation());
        if (rowCol.x < 0) { 
            _ghost->setVisible(false); return; 
        } 

        //符合格点要求,种植植物并且更新阳光 卡牌冷却状态
        PlantMgr::getInstance()->createPlantAt(rowCol, _selectedId);
        CardMgr::getInstance()->onPlantConfirmed(_selectedId); //冷却

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
    // 5×9 格子，左上起点，格子 100×100
    int col = (worldPos.x - 200) / 100;
    int row = (worldPos.y - 100) / 100;
    //格子状态不对
    if (row < 0 || row > 4 || col < 0 || col > 8) 
        return Vec2(-1, -1);
    return Vec2(col, row);
}