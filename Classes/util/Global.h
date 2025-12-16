#ifndef GLOBAL_H
#define GLOBAL_H

#include<vector>
#include"cocos2d.h"


struct SelectedPlantCard {
	cocos2d::Node* cardNode;
	int id;
};

class Global {

public:
	static Global* getInstance() {
		static Global g;
		return &g;
	}

	void setLevelID(int level_id) { _levelID = level_id; }
	int getLevelID() { return _levelID; }

	int& getSun() { return _sun; }

	//返回玩家在selectPlantScene选择的卡片
	//返回引用
	std::vector<SelectedPlantCard>& getSelectedCards() {return _selectedCards;}

private:
	Global() = default;
	int _levelID = 1;
	int _sun = 0;
	std::vector<SelectedPlantCard> _selectedCards;
};


#endif // !GLOBAL_H
