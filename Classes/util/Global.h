#ifndef GLOBAL_H
#define GLOBAL_H

#include"cocos2d.h"
#include"util/Util.h"
USING_NS_CC;


//数据的保存: 阳光的数据

const int ORIGIN_SUN = 5000;

class Global {

public:
	static Global* getInstance() {
		static Global g;
		return &g;
	}

	void setLevelID(int level_id) { _levelID = level_id; }
	void setGameMode(GameMode mode) { _myGameMode = mode; }

	int getLevelID() const { return _levelID; }

	GameMode getGameMode() const{ return _myGameMode; }

	// 获取阳光值（引用，允许直接修改）
	int& getSun() { return _sun; }

	// 设置阳光值
	void setSun(int sun) {
		_sun = (sun < 0) ? 0 : sun;  // 阳光不能为负
		CCLOG("Global: Sun changed to %d", _sun);
	}

	// 增加阳光（用于收集阳光）
	void addSun(int amount) {
		if (amount < 0) return;
		_sun += amount;
		CCLOG("Global: Sun added %d, total=%d", amount, _sun);
	}

	// 消耗阳光（用于种植）
	bool consumeSun(int amount) {
		if (_sun >= amount) {
			_sun -= amount;
			CCLOG("Global: Sun consumed %d, left=%d", amount, _sun);
			return true;
		}
		CCLOG("Global: Not enough sun!  Need %d, have %d", amount, _sun);
		return false;
	}

	// 重置游戏状态
	void reset() {
		_sun = ORIGIN_SUN;
		CCLOG("Global: Game state reset");
	}

	// 返回最多关卡数
	int getMaxLevel() {
		return MaxLevel;
	}
private:
	Global() = default;
	int _levelID = 1;
	int _sun = ORIGIN_SUN;
	GameMode _myGameMode = GameMode::ADVENTURE;
	int _currentLevel;
	int MaxLevel = 2; // 最大关卡数量
};


#endif // !GLOBAL_H