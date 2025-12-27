#include"manager/ZombieMgr.h"
#include"tool/LawnMower.h"
USING_NS_CC;


LawnMower* LawnMower::create(int row) {
    LawnMower* mower = new (std::nothrow) LawnMower();
    if (mower && mower->init(row)) {
        mower->autorelease();
        return mower;
    }
    CC_SAFE_DELETE(mower);
    return nullptr;
}

bool LawnMower::init(int row) {
    _row = row;
    _sprite = Sprite::create("LawnMower.png"); // 替换为你的路径
    this->addChild(_sprite);

    this->scheduleUpdate();
    return true;
}

void LawnMower::update(float dt) {
    if (!_isActivated) {
        // 1. 待机状态：检测这一行的僵尸
        auto zombies = ZombieMgr::getInstance()->getZombiesInRow(_row);
        for (auto zombie : zombies) {
            // 如果僵尸越过了小推车（通常是 x 坐标接近）
            if (zombie->getPositionX() <=this->getPositionX() + 20.0f) {
                _isActivated = true;
                //AudioEngine::play2d("Music/mower.ogg"); // 启动音效
                break;
            }
        }
    }
    else {
        // 2. 激活状态：向右狂奔
        this->setPositionX(this->getPositionX() + _speed * dt);

        // 3. 碾压逻辑
        auto zombies = ZombieMgr::getInstance()->getZombiesInRow(_row);
        for (auto zombie : zombies) {
            // 简单的距离判定
            if (abs(zombie->getPositionX() - this->getPositionX()) < 50) {
                // 直接调用死亡逻辑，传入死亡类型（不留尸体）
                zombie->stopAllActions();
                zombie->onDie(ZombieState::DYING);
            }
        }

        // 4. 飞出屏幕后自毁
        float maxRight = 1500.0f;

        if (this->getPositionX() > maxRight) {
            CCLOG("LAWNMOVER DELETE");
            this->removeFromParent();
        }
    }
}