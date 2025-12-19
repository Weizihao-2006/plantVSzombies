#include "manager/MapMgr.h"

USING_NS_CC;

const Vec2 MapManager::FalsePosition = { -1,-1 };

MapManager* MapManager::getInstance() 
{
    static MapManager instance;
    return &instance;
}

// 1. 基础坐标转换：通过行列获取该格子的中心点坐标
Vec2 MapManager::getPositionInMap(int row, int col) {
    if (row >= 0 && row < static_cast<int>(_gameMapInformation.rowNumbers) &&
        col >= 0 && col < static_cast<int>(_gameMapInformation.columnNumbers)) {
        return MapCoordinate[row][col];
    }
    return Vec2::ZERO;
}

// 2. 核心转换逻辑：将屏幕像素坐标转换为地图行列索引 (x=col, y=row)
// 使用距离检测算法，返回 (-1,-1) 表示非法
Vec2 MapManager::convertScreenPosToMapPos(const cocos2d::Vec2& screenPos) {
    float screenHeight = Director::getInstance()->getVisibleSize().height;

    // 坐标系转换：处理 Cocos2d 鼠标事件坐标与渲染坐标的差异
    float targetY = screenHeight - screenPos.y;
    Vec2 currentPos(screenPos.x, targetY);

    const float threshold = 75.0f; // 邻域判定阈值

    for (int row = 0; row < static_cast<int>(_gameMapInformation.rowNumbers); ++row) {
        for (int col = 0; col < static_cast<int>(_gameMapInformation.columnNumbers); ++col) {
            float distance = currentPos.distance(MapCoordinate[row][col]);
            if (distance < threshold) {
                return Vec2(col, row); // x 对应列，y 对应行
            }
        }
    }
    return FalsePosition; // 即 (-1, -1)
}

// 3. 更新格子状态
void MapManager::setMapCellStatus(int row, int col, PlantType plantId) {
    if (row >= 0 && row < static_cast<int>(_gameMapInformation.rowNumbers) &&
        col >= 0 && col < static_cast<int>(_gameMapInformation.columnNumbers)) {
        _gameMapInformation.plantsMap[row][col] = plantId;
    }
}

// 4. 判断位置是否在地图内
bool MapManager::judgeScreenPositionIsInMap(const Vec2& screenPos) {
    Vec2 res = convertScreenPosToMapPos(screenPos);
    return res != FalsePosition;
}

// 5. 判断是否可以种植 (位置合法 且 没植物)
bool MapManager::judgeScreenPositionIsCanPlant(const Vec2& screenPos) {
    Vec2 res = convertScreenPosToMapPos(screenPos);
    if (res == FalsePosition) 
        return false;

    return canPlantHere(static_cast<int>(res.y), static_cast<int>(res.x));
}

// 6. 判断是否有植物 (位置合法 且 有植物)
bool MapManager::judgeScreenPositionHavePlant(const Vec2& screenPos) {
    Vec2 res = convertScreenPosToMapPos(screenPos);
    if (res == FalsePosition) 
        return false;

    auto plantId = _gameMapInformation.plantsMap[static_cast<int>(res.y)][static_cast<int>(res.x)];
    return plantId != PlantType::Error;
}

// 7. 内部判定逻辑
bool MapManager::canPlantHere(int row, int col) {
    // 边界检查已经在调用前通过 convertScreenPosToMapPos 完成，这里双重保险
    if (row < 0 || row >= static_cast<int>(_gameMapInformation.rowNumbers) ||
        col < 0 || col >= static_cast<int>(_gameMapInformation.columnNumbers)) {
        return false;
    }
    return _gameMapInformation.plantsMap[row][col] == PlantType::Error;
}