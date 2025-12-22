#include "manager/ZombieMgr.h"
#include "layer/ZombieLayer.h"
#include "manager/MapMgr.h"

USING_NS_CC;

ZombieMgr* ZombieMgr::s_instance = nullptr;

ZombieMgr* ZombieMgr::getInstance() {
    if (!s_instance) {
        s_instance = new ZombieMgr();
        s_instance->init();
    }
    return s_instance;
}

bool ZombieMgr::init() {
    return true;
}

void ZombieMgr::spawnZombie(ZombieType type, int row) {
    auto scene = Director::getInstance()->getRunningScene();
    auto zombieLayer = dynamic_cast<ZombieLayer*>(scene->getChildByName("ZombieLayer"));

    if (!zombieLayer) {
        CCLOG("Error: ZombieLayer not found!");
        return;
    }

    Zombie* zombie = nullptr;
    switch (type) {
        case ZombieType::Normal:
            zombie = CommonZombie::create();
            break;
            // 可以在此添加路障僵尸、铁桶僵尸等分支
        default:
            break;
    }

    if (zombie) {
        // 设置行号
        zombie->setRow(row);

        // 计算初始位置：屏幕右侧边缘外 (MapRight + 偏移)
        float startX = MapManager::getInstance()->getMapRight() + 100.0f;
        // 使用 MapManager 获取该行对应的 Y 坐标
        float startY = MapManager::getInstance()->getPositionInMap(row, 0).y - 70;

        zombie->setAnchorPoint(Vec2(0.5, 0.5));
        zombie->setScale(1.5f);
        zombie->setPosition(Vec2(startX, startY));

        // 设置层级关系（ZOrder），让下方的僵尸遮挡上方的
        zombie->setLocalZOrder((MapRow - row) * 10);

        zombieLayer->addZombie(zombie);
    }
}