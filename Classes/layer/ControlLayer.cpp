#include "ControlLayer.h"
#include"scene/GameScene.h"
#include "manager/PlantMgr.h"
#include"manager/CardMgr.h"
#include <string>

using namespace cocos2d;
USING_NS_CC;

const PlantType ControlLayer::SelectNoPlant = PlantType::Error;



bool ControlLayer::init()
{
    if (!Layer::init()) 
        return false;


    this->setPosition(Vec2::ZERO);


    // 订阅 CardMgr 的选择事件,就是说一旦卡槽按钮被点击,会自动调用
    CardMgr::getInstance()->onCardSelected = [this](PlantType type) {
        // 这里不需要检查阳光和冷却，因为 CardMgr 已经检查过了
        this->setSelectedPlantId(type);
        CCLOG("ControlLayer: Received PlantType %d from CardMgr", static_cast<int>(type));
        };

    _mapManager = MapManager::getInstance();
    createTouchListener(); // 创建触摸和鼠标监听
    return true;
}


void ControlLayer::createTouchListener() {
    // 1. 触摸监听器：处理触摸屏输入或鼠标拖动（按下时）
    auto touchListener = EventListenerTouchOneByOne::create();

    // 触摸开始 (按下)
    touchListener->onTouchBegan = [this](Touch* touch, Event* event) {
        //世界坐标
        _cur = touch->getLocation();
        // 判定条件：如果没有选中植物，或者点在了地图外，或者点在了不可种植的地方
        bool isPlantingMode = (_selectedPlantType != SelectNoPlant);
        bool isInMap = _mapManager->judgeScreenPositionIsInMap(_cur);//这个判断不会修改_cur!!!

        // 只有当“不在种植模式”或者“在地图外”时，优先判定阳光
        // 这样可以保证你手里抓着植物时，点击地图依然是种植
        if (!isPlantingMode || !isInMap) {
            auto gameScene = dynamic_cast<GameScene*>(this->getScene());
            if (gameScene && gameScene->getSunLayer()) {
                if (gameScene->getSunLayer()->containsAndCollectSun(_cur)) {
                    return true; // 捡起阳光成功，中断事件，不执行后续种植判断
                }
            }
        }
        return false;
        };

    //经过测试,下面这一块代码去掉不影响!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if 0
    // 触摸移动 (拖动)
    touchListener->onTouchMoved = [this](Touch* touch, Event* event) {
        _cur = touch->getLocation(); // 更新当前触摸位置
        if (_selectedPlantType != SelectNoPlant) {
            if (UpdateTouchPositionIsInMap()) {
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

        if (UpdateTouchPositionIsInMap()) {//注重数据成员的修改
            if (_selectedPlantType != SelectNoPlant && judgeTouchPositionIsCanPlant()) {
                // 种植植物逻辑

               // _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = _selectedPlantType;
                //PlantMgr::getInstance()->createPlantAt(_plantsPosition, _selectedPlantType); // 通知PlantMgr实际创建植物

                _selectedPlantType = SelectNoPlant; // 重置选中状态
            }
            else if (judgeTouchPositionHavePlant()) {
                // 移除植物逻辑
                _mapManager->setMapCellStatus(_plantsPosition.y, _plantsPosition.x, SelectNoPlant);
                //_gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = -1;
                // TODO: 通知PlantMgr移除植物
                _selectedPlantType = SelectNoPlant; // 重置选中状态，防止误操作
            }
            else {
                _selectedPlantType = SelectNoPlant; // 如果只是拖动，结束时重置选中状态
            }
        }
        else {
            _selectedPlantType = SelectNoPlant; // 如果在地图外松开，重置选中状态
        }
        };

#endif
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


    // 2. 鼠标监听器：处理桌面端鼠标的非按下移动 (幽灵跟随) 和点击 (种植)
    auto mouseListener = EventListenerMouse::create();

    // 鼠标移动 (未按下时)
    mouseListener->onMouseMove = [this](EventMouse* event) {
        _cur = event->getLocation(); // 获取鼠标当前位置

        // 仅在选中植物且鼠标未按下时，更新预览位置（实现松开状态下的幽灵跟随）
        //update: 2025/12/19,加入CardMgr::getInstance()->canPlant(_selectedPlantType)判断冷却时间和阳光
        if (_selectedPlantType != SelectNoPlant && event->getMouseButton() == EventMouse::MouseButton::BUTTON_UNSET
            &&CardMgr::getInstance()->canPlant(_selectedPlantType)) {

            // 关键新增：如果幽灵精灵还未创建，现在创建它！
            if (!_isPreviewSpriteCreated) {

                //现在改成从PlantData中获取信息
                auto props = PlantData::getProps(_selectedPlantType);
                std::string filename = props.previewFrame;


                _plantPreview = Sprite::create(filename);
                if (_plantPreview) {
                    _plantPreview->setOpacity(128); // 半透明效果
                    _plantPreview->setScale(1.5f); // 放大1.5倍
                    this->addChild(_plantPreview, 10);
                    _isPreviewSpriteCreated = true;
                    _isPreviewShowing = true;
                }
            }

            if (UpdateTouchPositionIsInMap()) {
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
            //update: 2025/12/19,加入CardMgr::getInstance()->canPlant(_selectedPlantType)判断冷却时间和阳光
            if (_selectedPlantType != SelectNoPlant && UpdateTouchPositionIsInMap() 
                && CardMgr::getInstance()->canPlant(_selectedPlantType)) {
                // calculatePlantPosition 在 schedule 中持续运行，_plantsPosition 应该是最新的

                if (judgeTouchPositionIsCanPlant()) {
                    // 种植植物逻辑
                    _mapManager->setMapCellStatus(_plantsPosition.y, _plantsPosition.x, _selectedPlantType);
                    //_gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = _selectedPlantType;
                    PlantMgr::getInstance()->createPlantAt(_plantsPosition, _selectedPlantType); // 通知PlantMgr实际创建植物
                    //确认种植后调用
                    CardMgr::getInstance()->onPlantConfirmed(_selectedPlantType);
                }
                else if (judgeTouchPositionHavePlant()) {//这个是什么意思?
                    // 移除植物逻辑
                    _mapManager->setMapCellStatus(_plantsPosition.y, _plantsPosition.x, SelectNoPlant);
                    //_gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = -1;
                    // TODO: 通知PlantMgr移除植物
                }

                hidePlantPreview();
                clearHighlightBars();

                _selectedPlantType = SelectNoPlant; // 重置选中状态，退出幽灵模式
            }
            _isPreviewSpriteCreated = false;
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}


// 设置选中的植物ID（从卡片点击事件调用）
void ControlLayer::setSelectedPlantId(PlantType plantId) {
    _selectedPlantType = plantId;

    if (plantId != SelectNoPlant) {
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

bool ControlLayer::UpdateTouchPositionIsInMap() {

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

