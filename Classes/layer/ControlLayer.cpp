#include "ControlLayer.h"

using namespace cocos2d;

bool ControlLayer::init() 
{
    if (!Layer::init()) return false;

    // 动态计算地图边界
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    float mapLeft = origin.x + 200; // 地图左边界
    float mapTop = origin.y + visibleSize.height - 100; // 地图上边界
    float mapRight = origin.x + visibleSize.width - 200; // 地图右边界
    float mapBottom = origin.y + 110; // 地图下边界

    // 保存边界值
    _gameMapInformation.mapLeft = mapLeft;
    _gameMapInformation.mapTop = mapTop;
    _gameMapInformation.mapRight = mapRight;
    _gameMapInformation.mapBottom = mapBottom;

    createSchedule(); // 创建定时器
    createTouchListener(); // 创建触摸监听
    return true;
}

void ControlLayer::createSchedule() {
    schedule([this](float dt) {
        controlCardEnabled(); // 控制卡牌是否可用
        calculatePlantPosition(); // 计算植物种植位置
       //createZombies(); // 创建僵尸
        }, 0.1f, "mainUpdate");
}

void ControlLayer::createTouchListener() {
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        return touchBegan(touch, event);
        };
    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        touchMoved(touch, event);
        };
    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        touchEnded(touch, event);
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool ControlLayer::touchBegan(Touch* touch, Event* event) {
    _cur = touch->getLocation();
    if (judgeTouchPositionIsInMap()) {
        // 可以在这里处理触摸开始的逻辑
    }
    return true; // 返回 true 表示事件被处理
}

void ControlLayer::touchMoved(Touch* touch, Event* event) {
    _cur = touch->getLocation();
    if (judgeTouchPositionIsInMap()) {
        // 可以在这里处理触摸移动的逻辑
    }
}

void ControlLayer::touchEnded(Touch* touch, Event* event) {
    _cur = touch->getLocation();
    if (judgeTouchPositionIsInMap()) {
        if (judgeTouchPositionIsCanPlant()) {
            // 种植植物逻辑
            _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = _selectedPlantId;
        }
        else if (judgeTouchPositionHavePlant()) {
            // 移除植物逻辑
            _gameMapInformation.plantsMap[_plantsPosition.y][_plantsPosition.x] = -1;
        }
    }
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
    float cellWidth = (_gameMapInformation.mapRight - _gameMapInformation.mapLeft) / _gameMapInformation.columnNumbers;
    float cellHeight = (_gameMapInformation.mapTop - _gameMapInformation.mapBottom) / _gameMapInformation.rowNumbers;

    _plantsPosition.x = std::floor((_cur.x - _gameMapInformation.mapLeft) / cellWidth);
    _plantsPosition.y = std::floor((_cur.y - _gameMapInformation.mapBottom) / cellHeight);

    // 确保坐标在地图范围内
    _plantsPosition.x = std::max(0.0f, std::min(_plantsPosition.x, static_cast<float>(_gameMapInformation.columnNumbers - 1)));
    _plantsPosition.y = std::max(0.0f, std::min(_plantsPosition.y, static_cast<float>(_gameMapInformation.rowNumbers - 1)));
}