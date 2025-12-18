#include "ControlLayer.h"
#include "manager/PlantMgr.h"
#include"manager/CardMgr.h"
#include <string>

using namespace cocos2d;


bool ControlLayer::init()
{
    if (!Layer::init()) return false;

    this->setPosition(Vec2::ZERO);
    
    _mapManager = MapManager::getInstance();
    createTouchListener(); // 创建触摸和鼠标监听
    return true;
}


void ControlLayer::createTouchListener() {
    // 1. 触摸监听器：处理触摸屏输入或鼠标拖动（按下时）
    auto touchListener = EventListenerTouchOneByOne::create();

    // 触摸开始 (按下)
    touchListener->onTouchBegan = [this](Touch* touch, Event* event) {
        _cur = touch->getLocation();
        // 按下时，不立即显示预览，让 onMouseMove/onTouchMoved 控制
        return true;
        };

    // 触摸移动 (拖动)
    touchListener->onTouchMoved = [this](Touch* touch, Event* event) {
        _cur = touch->getLocation(); // 更新当前触摸位置
        if (_selectedPlantId != -1) {
            if (judgeTouchPositionIsInMap()) {//原来是judgeScreenPositionIsInMap,顺便修改了plantPosition,先不改
                showPlantPreview();      // 地图内显示预览
                updatePreviewPosition(); // 实时更新位置（跟随鼠标/手指）
            }
            else {
                hidePlantPreview(); // 地图外隐藏
            }
        }
        };

    // 触摸结束 (松开)
    touchListener->onTouchEnded = [this](Touch* touch, Event* event) {
        _cur = touch->getLocation();
        hidePlantPreview(); // 结束触摸时隐藏预览

        if (judgeTouchPositionIsInMap()) {//注重数据成员的修改
            if (_selectedPlantId != -1 && judgeTouchPositionIsCanPlant()) {
                // 种植植物逻辑
                _mapManager->setMapCellStatus(_plantsPosition.y, _plantsPosition.x, _selectedPlantId);
               // _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = _selectedPlantId;
                PlantMgr::getInstance()->createPlantAt(_plantsPosition, _selectedPlantId); // 通知PlantMgr实际创建植物
                _selectedPlantId = -1; // 重置选中状态
            }
            else if (judgeTouchPositionHavePlant()) {
                // 移除植物逻辑
                _mapManager->setMapCellStatus(_plantsPosition.y, _plantsPosition.x, -1);
                //_gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = -1;
                // TODO: 通知PlantMgr移除植物
                _selectedPlantId = -1; // 重置选中状态，防止误操作
            }
            else {
                _selectedPlantId = -1; // 如果只是拖动，结束时重置选中状态
            }
        }
        else {
            _selectedPlantId = -1; // 如果在地图外松开，重置选中状态
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


    // 2. 鼠标监听器：处理桌面端鼠标的非按下移动 (幽灵跟随) 和点击 (种植)
    auto mouseListener = EventListenerMouse::create();

    // 鼠标移动 (未按下时)
    mouseListener->onMouseMove = [this](EventMouse* event) {
        _cur = event->getLocation(); // 获取鼠标当前位置

        // 仅在选中植物且鼠标未按下时，更新预览位置（实现松开状态下的幽灵跟随）
        if (_selectedPlantId != -1 && event->getMouseButton() == EventMouse::MouseButton::BUTTON_UNSET) {

            // 关键新增：如果幽灵精灵还未创建，现在创建它！
            if (!_isPreviewSpriteCreated) {

                //现在改成从哈希表中读取预览图,这样随便是什么顺序都可以快速访问到对应的植物类型
                std::string filename = _CardPreview.at(CardMgr::getInstance()->getPlantType(_selectedPlantId));
                _plantPreview = Sprite::create(filename);
                if (_plantPreview) {
                    _plantPreview->setOpacity(128); // 半透明效果
                    _plantPreview->setScale(1.5f); // 放大1.5倍
                    this->addChild(_plantPreview, 10);
                    _isPreviewSpriteCreated = true;
                    _isPreviewShowing = true;
                }
            }

            if (judgeTouchPositionIsInMap()) {
                updatePreviewPosition(); // 更新位置 (此时位置就是准确的鼠标位置)
                showPlantPreview();      // 显示预览（如果之前隐藏了）
                updateHighlightBars(_plantsPosition.y, _plantsPosition.x);
            }
            else {
                hidePlantPreview();      // 地图外隐藏
                clearHighlightBars();    // 地图外清除高亮
            }
        }
        };

    // 鼠标按下 (用于在幽灵模式下点击种植)
    mouseListener->onMouseDown = [this](EventMouse* event) {
        if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
            _cur = event->getLocation();

            // 只有在幽灵模式下点击地图，才执行种植/移除逻辑并退出幽灵模式
            if (_selectedPlantId != -1 && judgeTouchPositionIsInMap()) {
                // calculatePlantPosition 在 schedule 中持续运行，_plantsPosition 应该是最新的

                if (judgeTouchPositionIsCanPlant()) {
                    // 种植植物逻辑
                    _mapManager->setMapCellStatus(_plantsPosition.y, _plantsPosition.x, _selectedPlantId);
                    //_gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = _selectedPlantId;
                    PlantMgr::getInstance()->createPlantAt(_plantsPosition, _selectedPlantId); // 通知PlantMgr实际创建植物
                }
                else if (judgeTouchPositionHavePlant()) {
                    // 移除植物逻辑
                    _mapManager->setMapCellStatus(_plantsPosition.y, _plantsPosition.x, -1);
                    //_gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = -1;
                    // TODO: 通知PlantMgr移除植物
                }

                hidePlantPreview();
                clearHighlightBars();

                _isPreviewSpriteCreated = false;
                _selectedPlantId = -1; // 重置选中状态，退出幽灵模式
            }
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}


// 设置选中的植物ID（从卡片点击事件调用）
void ControlLayer::setSelectedPlantId(int plantId) {
    _selectedPlantId = plantId;

    if (plantId != -1) {
        //// 如果精灵已经存在，显示它（但初始位置可能不正确，会在 onMouseMove 中立即修正）
        //if (_plantPreview) {
        //    _plantPreview->setVisible(true);
        //    _isPreviewShowing = true;
        //}
        //// 如果精灵不存在，什么都不做，等待 onMouseMove 创建它。
    }
    else {
        hidePlantPreview();
    }
}


void ControlLayer::updateHighlightBars(int row, int col) {
    // 1. 延迟初始化 DrawNode
    if (!_highlightRow) {
        _highlightRow = DrawNode::create();
        this->addChild(_highlightRow, 5);
    }
    if (!_highlightCol) {
        _highlightCol = DrawNode::create();
        this->addChild(_highlightCol, 5);
    }

    _highlightRow->clear();
    _highlightCol->clear();

    // 2. 设置高亮颜色：半透明白色
    Color4F highlightColor(1.0f, 1.0f, 1.0f, 0.2f);

    // 3. 通过 Manager 获取格子中心点，不再直接访问全局数组
    Vec2 center = _mapManager->getPositionInMap(row, col);

    // 4. 从 Manager 获取地图边界参数
    float rowLeft = _mapManager->getMapLeft();
    float rowRight = _mapManager->getMapRight();
    float colBottom = _mapManager->getMapBottom();
    float colTop = _mapManager->getMapTop();

    float barSize = 140.0f; // 高亮条的厚度

    // --- 绘制横条 (Row) ---
    _highlightRow->drawSolidRect(
        Vec2(rowLeft, center.y - barSize / 2),
        Vec2(rowRight, center.y + barSize / 2),
        highlightColor
    );

    // --- 绘制竖条 (Col) ---
    _highlightCol->drawSolidRect(
        Vec2(center.x - barSize / 2, colBottom),
        Vec2(center.x + barSize / 2, colTop),
        highlightColor
    );
}

// 隐藏高亮条
void ControlLayer::clearHighlightBars() {
    if (_highlightRow) _highlightRow->clear();
    if (_highlightCol) _highlightCol->clear();
}

// 显示植物预览
void ControlLayer::showPlantPreview() {
    if (_plantPreview && !_isPreviewShowing) {
        _plantPreview->setVisible(true);
        _isPreviewShowing = true;
    }
}

// 隐藏植物预览
void ControlLayer::hidePlantPreview() {
    // 仅隐藏，不移除精灵，以便下次移动时可以立即显示
    if (_plantPreview && _isPreviewShowing) {
        _plantPreview->setVisible(false);
        _isPreviewShowing = false;
    }
}

// 更新预览位置到当前触摸/鼠标位置
void ControlLayer::updatePreviewPosition() {
    if (!_plantPreview) return;

    // CCLOG("%f %f", _cur.x, targetY);

    // 可选：限制预览图只能在地图范围内显示
    // float x = std::max(_gameMapInformation.mapLeft, std::min(_cur.x, _gameMapInformation.mapRight));
    // float y = std::max(_gameMapInformation.mapBottom, std::min(_cur.y, _gameMapInformation.mapTop));
    int row = _plantsPosition.y;
    int col = _plantsPosition.x;
    _plantPreview->setPosition(_mapManager->getPositionInMap(row,col));
}

// 兼有更新网络坐标索引的功能

bool ControlLayer::judgeTouchPositionIsInMap() {

    auto pos = _mapManager->convertScreenPosToMapPos(_cur);
    if (!pos.equals(MapManager::FalsePosition)) {//位置合法
        _plantsPosition = pos;
        return true;
    }
    return false;
}

bool ControlLayer::judgeTouchPositionIsCanPlant() {
    return _mapManager->judgeScreenPositionIsCanPlant(_cur);
}

bool ControlLayer::judgeTouchPositionHavePlant() {
    return _mapManager->judgeScreenPositionHavePlant(_cur);
}

