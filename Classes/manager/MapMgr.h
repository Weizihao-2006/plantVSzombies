#pragma once
#include"cocos2d.h"
USING_NS_CC;

// 地图的边界
const float MapTop = 1150.0f;
const float MapBottom = 287.0f;
const float MapRight = 1900.0f;
const float MapLeft = 490.0f;

// 地图的行数和列数
const int MapRow = 5;
const int MapCol = 9;

// 记录每一格的中心坐标
static const cocos2d::Vec2 MapCoordinate[MapRow][MapCol] = {
    Vec2(569,335),Vec2(730,335),Vec2(894,335),Vec2(1054,335),Vec2(1214,335),Vec2(1366,335),Vec2(1511,335),Vec2(1664,335),Vec2(1845,335),
    Vec2(569,525),Vec2(730,525),Vec2(894,525),Vec2(1054,525),Vec2(1214,525),Vec2(1366,525),Vec2(1511,525),Vec2(1664,525),Vec2(1845,525),
    Vec2(569,714),Vec2(730,714),Vec2(894,714),Vec2(1054,714),Vec2(1214,714),Vec2(1366,714),Vec2(1511,714),Vec2(1664,714),Vec2(1845,714),
    Vec2(569,914),Vec2(730,914),Vec2(894,914),Vec2(1054,914),Vec2(1214,914),Vec2(1366,914),Vec2(1511,914),Vec2(1664,914),Vec2(1845,914),
    Vec2(569,1101),Vec2(730,1101),Vec2(894,1101),Vec2(1054,1101),Vec2(1214,1101),Vec2(1366,1101),Vec2(1511,1101),Vec2(1664,1101),Vec2(1845,1101)
};


struct GameMapInformation
{
    GameMapInformation() : rowNumbers(MapRow), columnNumbers(MapCol),
        mapLeft(MapLeft), mapRight(MapRight), mapTop(MapTop), mapBottom(MapBottom)
    {
        plantsMap.resize(rowNumbers, std::vector<int>(columnNumbers, -1));
    }

    std::vector<std::vector<int>> plantsMap; // 植物种植地图,-1表示空闲,其它的其实用植物种类表示更好
    unsigned int rowNumbers;                // 行数
    unsigned int columnNumbers;             // 列数

    float mapLeft;   // 地图左边界
    float mapTop;    // 地图上边界
    float mapRight;  // 地图右边界
    float mapBottom; // 地图下边界
};



class MapManager {
public:
    static const Vec2 FalsePosition;

    static MapManager* getInstance();

    // 基础坐标转换
    Vec2 getPositionInMap(int row, int col); // 返回中心点坐标

    //返回(-1,-1)表示不在地图中,兼具判断合法的功能
    Vec2 convertScreenPosToMapPos(const cocos2d::Vec2& screenPos);


    //在ControlLayer中调用可以用来更新Map的植物状态
    void setMapCellStatus(int row, int col, int plantId);

    // 判断触摸位置是否在地图内
    bool judgeScreenPositionIsInMap(const Vec2& screenPos);

    //判断是否可以种植植物
    bool judgeScreenPositionIsCanPlant(const Vec2& screenPos); // 判断触摸位置是否可以种植植物
    //判断屏幕坐标是否有植物
    bool judgeScreenPositionHavePlant(const Vec2& screenPos); // 判断触摸位置是否有植物


    //外取参数
    unsigned int getRowNumbers() const { return _gameMapInformation.rowNumbers; }
    unsigned int getColumnNumbers() const { return _gameMapInformation.columnNumbers; }

    // 获取地图边界
    float getMapLeft() const { return _gameMapInformation.mapLeft; }
    float getMapRight() const { return _gameMapInformation.mapRight; }
    float getMapTop() const { return _gameMapInformation.mapTop; }
    float getMapBottom() const { return _gameMapInformation.mapBottom; }


protected:
    bool canPlantHere(int row, int col);

private:

    GameMapInformation _gameMapInformation;
};