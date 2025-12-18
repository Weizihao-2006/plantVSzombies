#include "ControlLayer.h"
#include "manager/PlantMgr.h"
#include <string>
#include "MapCoordinate.h"

using namespace cocos2d;

std::vector<std::string> _CardPreview = {
    "cardPreview/SunFlower_0.png",
    "cardPreview/CherryBomb_0.png",
    "cardPreview/Peashooter_0.png",
    "cardPreview/Repeater_0.png",
    "cardPreview/SnowPea_0.png",
    "cardPreview/WallNut_0.png",
    "cardPreview/WallNut_0.png",
    "cardPreview/WallNut_0.png"
};

bool ControlLayer::init()
{
    if (!Layer::init()) return false;

    this->setPosition(Vec2::ZERO);
    // 保存地图边界值
    _gameMapInformation.mapLeft = 490.0f;
    _gameMapInformation.mapRight = 1900.0f;
    _gameMapInformation.mapTop = 1150.0f;
    _gameMapInformation.mapBottom = 287.0f;

    // 设置地图的行数和列数
    _gameMapInformation.columnNumbers = 9;
    _gameMapInformation.rowNumbers = 5;

    createSchedule();      // 创建定时器 (用于实时计算网格位置)
    createTouchListener(); // 创建触摸和鼠标监听
    return true;
}

void ControlLayer::createSchedule() {
    // 保留定时器用于持续计算当前的网格坐标 (_plantsPosition)，供所有输入事件使用
    schedule([this](float dt) {
        calculatePlantPosition();
        // 移除原有的更新预览位置逻辑，改由 EventListenerMouse::onMouseMove 处理
        }, 0.1f, "calculatePosUpdate");
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
            if (judgeTouchPositionIsInMap()) {
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

        if (judgeTouchPositionIsInMap()) {
            if (_selectedPlantId != -1 && judgeTouchPositionIsCanPlant()) {
                // 种植植物逻辑
                _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = _selectedPlantId;
                PlantMgr::getInstance()->createPlantAt(_plantsPosition, _selectedPlantId); // 通知PlantMgr实际创建植物
                _selectedPlantId = -1; // 重置选中状态
            }
            else if (judgeTouchPositionHavePlant()) {
                // 移除植物逻辑
                _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = -1;
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
                std::string filename = _CardPreview[_selectedPlantId];
                _plantPreview = Sprite::create(filename);
                if (_plantPreview) {
                    _plantPreview->setOpacity(128); // 半透明效果
                    _plantPreview->setScale(1.5f); // 放大1.5倍
                    this->addChild(_plantPreview, 10);
                    _isPreviewSpriteCreated = true;
                    _isPreviewShowing = true;
                }
            }

            if (judgeTouchPositionIsInMap() && judgeTouchPositionIsCanPlant()) {
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
                    _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = _selectedPlantId;
                    PlantMgr::getInstance()->createPlantAt(_plantsPosition, _selectedPlantId); // 通知PlantMgr实际创建植物
                }
                else if (judgeTouchPositionHavePlant()) {
                    // 移除植物逻辑
                    
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

// 更新高亮条
void ControlLayer::updateHighlightBars(int row, int col) {
    // 如果还没创建，先创建
    if (!_highlightRow) {
        _highlightRow = DrawNode::create();
        this->addChild(_highlightRow, 5); // 层级在地图之上，植物预览之下
    }
    if (!_highlightCol) {
        _highlightCol = DrawNode::create();
        this->addChild(_highlightCol, 5);
    }

    _highlightRow->clear();
    _highlightCol->clear();

    // 设置高亮颜色：半透明白色
    Color4F highlightColor(1.0f, 1.0f, 1.0f, 0.2f);

    // 获取当前格子的中心点
    Vec2 center = MapCoordinate[row][col];

    // --- 绘制横条 (Row) ---
    // 假设地图横向范围是从 mapLeft 到 mapRight
    float rowLeft = _gameMapInformation.mapLeft;
    float rowRight = _gameMapInformation.mapRight;
    float rowHeight = 140.0f; // 高亮条的高度，可以根据感观调整
    _highlightRow->drawSolidRect(Vec2(rowLeft, center.y - rowHeight / 2),
        Vec2(rowRight, center.y + rowHeight / 2),
        highlightColor);

    // --- 绘制竖条 (Col) ---
    // 假设地图纵向范围是从 mapBottom 到 mapTop
    float colBottom = _gameMapInformation.mapBottom;
    float colTop = _gameMapInformation.mapTop;
    float colWidth = 140.0f; // 高亮条的宽度
    _highlightCol->drawSolidRect(Vec2(center.x - colWidth / 2, colBottom),
        Vec2(center.x + colWidth / 2, colTop),
        highlightColor);
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
    _plantPreview->setPosition(MapCoordinate[row][col]);
}

// 兼有更新网络坐标索引的功能
bool ControlLayer::judgeTouchPositionIsInMap() {
    float screenHeight = Director::getInstance()->getVisibleSize().height;
    // 统一转换 Y 坐标，确保和 MapCoordinate 坐标系一致
    float targetY = screenHeight - _cur.y;
    Vec2 currentPos(_cur.x, targetY);

    // 定义邻域阈值：比如 80 像素（你可以根据格子大小调整这个值）
    const float threshold = 75.0f;

    for (int row = 0; row < MapRow; ++row) {
        for (int col = 0; col < MapCol; ++col) {
            // 计算当前点与中心点的距离
            float distance = currentPos.distance(MapCoordinate[row][col]);

            if (distance < threshold) {
                // 如果在邻域内，顺便更新网格坐标索引
                _plantsPosition.x = col;
                _plantsPosition.y = row;
                return true;
            }
        }
    }
    return false; // 不在任何格子的邻域内
}

bool ControlLayer::judgeTouchPositionIsCanPlant() {
    // 判断是否可以种植植物 (格子为空)
    if (!judgeTouchPositionIsInMap()) return false;
    return _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] == -1;
}

bool ControlLayer::judgeTouchPositionHavePlant() {
    // 判断是否有植物
    if (!judgeTouchPositionIsInMap()) return false;
    return _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] != -1;
}

void ControlLayer::calculatePlantPosition() {
    // 将触摸坐标转换为格子坐标

    // 检查 _cur 是否已被初始化（避免在没有触摸/鼠标事件前运行）
    if (_cur.x == 0 && _cur.y == 0) return;

    // 计算每个格子的宽和高
    float cellWidth = (_gameMapInformation.mapRight - _gameMapInformation.mapLeft) / _gameMapInformation.columnNumbers;
    float cellHeight = (_gameMapInformation.mapTop - _gameMapInformation.mapBottom) / _gameMapInformation.rowNumbers;

    // 计算格子坐标，左下角为(0,0)
    float rawX = (_cur.x - _gameMapInformation.mapLeft) / cellWidth;
    float rawY = (_cur.y - _gameMapInformation.mapBottom) / cellHeight;

    _plantsPosition.x = std::floor(rawX);
    _plantsPosition.y = std::floor(rawY);

    // 确保坐标在地图范围内
    _plantsPosition.x = std::max(0.0f, std::min(_plantsPosition.x, static_cast<float>(_gameMapInformation.columnNumbers - 1)));
    _plantsPosition.y = std::max(0.0f, std::min(_plantsPosition.y, static_cast<float>(_gameMapInformation.rowNumbers - 1)));
}