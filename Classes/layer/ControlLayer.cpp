#include "ControlLayer.h"
#include "manager/PlantMgr.h"

using namespace cocos2d;

bool ControlLayer::init() 
{
    if (!Layer::init()) return false;

    // 保存地图边界值
    _gameMapInformation.mapLeft = 490.0f;
    _gameMapInformation.mapTop = 1150.0f;
    _gameMapInformation.mapRight = 1900.0f;
    _gameMapInformation.mapBottom = 255.0f;

    // 设置地图的行数和列数
    _gameMapInformation.columnNumbers = 9;
    _gameMapInformation.rowNumbers = 5;

    createSchedule(); // 创建定时器
    createTouchListener(); // 创建触摸监听
    return true;
}

void ControlLayer::createSchedule() {
    schedule([this](float dt) {
        calculatePlantPosition();
        if (_selectedPlantId != -1 && judgeTouchPositionIsInMap()) {
            updatePreviewPosition(); // 更新预览位置
        }
        else {
            hidePlantPreview();      // 不在地图内则隐藏预览
        }
        }, 0.1f, "mainUpdate");
}

void ControlLayer::createTouchListener() {
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        _cur = touch->getLocation();
        if (judgeTouchPositionIsInMap()) {
            if (_selectedPlantId != -1) {
                showPlantPreview(); // 选中植物且在地图内，显示预览
            }
        }
        return true;
        };

    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        _cur = touch->getLocation(); // 更新当前触摸位置
        if (_selectedPlantId != -1) {
            if (judgeTouchPositionIsInMap()) {
                showPlantPreview(); // 地图内显示预览
                updatePreviewPosition(); // 实时更新位置（跟随鼠标）
            }
            else {
                hidePlantPreview(); // 地图外隐藏
            }
        }
        };

    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        _cur = touch->getLocation();
        hidePlantPreview(); // 结束触摸时隐藏预览

        if (judgeTouchPositionIsInMap()) {
            if (_selectedPlantId != -1 && judgeTouchPositionIsCanPlant()) {
                // 种植植物逻辑
                _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = _selectedPlantId;
                // TODO: 通知PlantMgr实际创建植物
                _selectedPlantId = -1; // 重置选中状态
            }
            else if (judgeTouchPositionHavePlant()) {
                // 移除植物逻辑
                _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = -1;
                // TODO: 通知PlantMgr移除植物
            }
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// 设置选中的植物ID（从卡片点击事件调用）
void ControlLayer::setSelectedPlantId(int plantId) {
    _selectedPlantId = plantId;
    if (plantId != -1) {
        //// 根据植物ID加载对应图片（假设图片命名为 plant_{id}.png）
        //std::string imgPath = StringUtils::format("plant_%d.png", plantId);
        _plantPreview = Sprite::create("plant/Peashooter_0.png");
        if (_plantPreview) {
            _plantPreview->setOpacity(128); // 半透明效果
            this->addChild(_plantPreview, 10);
        }
    }
    else {
        hidePlantPreview();
    }
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
    if (_plantPreview && _isPreviewShowing) {
        _plantPreview->setVisible(false);
        _isPreviewShowing = false;
    }
}

// 更新预览位置到当前网格
void ControlLayer::updatePreviewPosition() {
    if (!_plantPreview) return;

    // 直接使用当前触摸位置作为预览图位置（实时跟随鼠标）
    _plantPreview->setPosition(_cur);

    // 可选：限制预览图只能在地图范围内显示
    float x = std::max(_gameMapInformation.mapLeft, std::min(_cur.x, _gameMapInformation.mapRight));
    float y = std::max(_gameMapInformation.mapBottom, std::min(_cur.y, _gameMapInformation.mapTop));
    _plantPreview->setPosition(Vec2(x, y));
}


bool ControlLayer::judgeTouchPositionIsInMap() {
    // 判断触摸位置是否在地图内
    return _cur.x >= _gameMapInformation.mapLeft && _cur.x <= _gameMapInformation.mapRight &&
        _cur.y >= _gameMapInformation.mapBottom && _cur.y <= _gameMapInformation.mapTop;
}

bool ControlLayer::judgeTouchPositionIsCanPlant() {
    // 判断是否可以种植植物
    return _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] == -1;
}

bool ControlLayer::judgeTouchPositionHavePlant() {
    // 判断是否有植物
    return _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] != -1;
}

void ControlLayer::calculatePlantPosition() {
    // 将触摸坐标转换为格子坐标
    
    // 计算每个格子的宽和高
    float cellWidth = (_gameMapInformation.mapRight - _gameMapInformation.mapLeft) / _gameMapInformation.columnNumbers;
    float cellHeight = (_gameMapInformation.mapTop - _gameMapInformation.mapBottom) / _gameMapInformation.rowNumbers;

    // 计算格子坐标，左下角为(0,0)
    _plantsPosition.x = std::floor((_cur.x - _gameMapInformation.mapLeft) / cellWidth);
    _plantsPosition.y = std::floor((_cur.y - _gameMapInformation.mapBottom) / cellHeight);

    // 确保坐标在地图范围内
    _plantsPosition.x = std::max(0.0f, std::min(_plantsPosition.x, static_cast<float>(_gameMapInformation.columnNumbers - 1)));
    _plantsPosition.y = std::max(0.0f, std::min(_plantsPosition.y, static_cast<float>(_gameMapInformation.rowNumbers - 1)));
}