#pragma once

#include "cocos2d.h"
USING_NS_CC;
// 文件用于记录每个需要硬编码的坐标位置

// 地图的边界
const float MapTop = 1150.0f;
const float MapBottom = 287.0f;
const float MapRight = 1900.0f;
const float MapLeft = 490.0f;

// 地图的行数和列数
const int MapRow = 5;
const int MapCol = 9;

// 记录每一格的中心坐标
const cocos2d::Vec2 MapCoordinate[MapRow][MapCol] = {
	Vec2(569,335),Vec2(730,335),Vec2(894,335),Vec2(1054,335),Vec2(1214,335),Vec2(1366,335),Vec2(1511,335),Vec2(1664,335),Vec2(1845,335),
	Vec2(569,525),Vec2(730,525),Vec2(894,525),Vec2(1054,525),Vec2(1214,525),Vec2(1366,525),Vec2(1511,525),Vec2(1664,525),Vec2(1845,525),
	Vec2(569,714),Vec2(730,714),Vec2(894,714),Vec2(1054,714),Vec2(1214,714),Vec2(1366,714),Vec2(1511,714),Vec2(1664,714),Vec2(1845,714),
	Vec2(569,914),Vec2(730,914),Vec2(894,914),Vec2(1054,914),Vec2(1214,914),Vec2(1366,914),Vec2(1511,914),Vec2(1664,914),Vec2(1845,914),
	Vec2(569,1101),Vec2(730,1101),Vec2(894,1101),Vec2(1054,1101),Vec2(1214,1101),Vec2(1366,1101),Vec2(1511,1101),Vec2(1664,1101),Vec2(1845,1101)
};