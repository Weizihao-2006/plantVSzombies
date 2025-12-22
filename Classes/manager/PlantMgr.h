#pragma once
#include"cocos2d.h"
#include"layer/PlantLayer.h"
#include"manager/CardMgr.h"
#include"plant/Plant.h"
#include"plant/PlantData.h"

#include"plant/SunFlower.h"
#include"plant/WallNut.h"

/*
PlantMgr
  逻辑管理
植物创建/删除
植物数据存储
游戏逻辑处理
植物间协调

PlantLayer

显示渲染
显示植物精灵
管理显示层级
 处理渲染顺序


完整流程：
1. 玩家点击卡牌  ControlLayer进入预览模式
2. 玩家点击地图  ControlLayer调用PlantMgr创建植物
3. PlantMgr创建具体植物对象 -> 添加到PlantLayer
4. 植物定期update -> 攻击型植物检测前方僵尸
5. 植物攻击 -> 创建子弹 -> 添加到BulletLayer
6. 子弹移动 -> BulletLayer检测与ZombieLayer的碰撞
7. 子弹命中 -> 僵尸扣血 -> 血量为0则移除
8. 僵尸定期前进 -> ZombieLayer检测前方植物
9. 僵尸遇到植物 -> 开始吃植物 -> 植物扣血
10. 植物血量为0 -> 从PlantLayer移除
11. 僵尸到达房子 -> 游戏失败
12. 所有僵尸死亡 -> 游戏胜利

*/



class PlantMgr : public cocos2d::Node {

protected:
    PlantLayer* _plantLayer = nullptr;
    static PlantMgr* s_sharedPlantMgr;
public:
    void setPlantLayer(const PlantLayer* layer) {
    }

    // 移除植物
    void removePlantAt(const cocos2d::Vec2& gridPos);

    // 获取植物
    Plants* getPlantAt(const cocos2d::Vec2& gridPos);

    // 更新所有植物
    void updateAllPlants(float deltaTime);

   
    static PlantMgr* getInstance();

    bool init() override;

    void createPlantAt(const cocos2d::Vec2& rowCol, PlantType type);

    void reset();
};

