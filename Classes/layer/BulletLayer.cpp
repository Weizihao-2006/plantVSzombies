#include "layer/BulletLayer.h"
#include "layer/ZombieLayer.h"

USING_NS_CC;

// 内部结构体，用来存储子弹的额外属性
struct BulletInfo {
    int row;
    float damage;
};

bool BulletLayer::init() {
    if (!Layer::init()) return false;
    this->scheduleUpdate();
    return true;
}

void BulletLayer::spawnBullet(Vec2 pos, int row, float damage) {
    // 因为你没有 plist，直接通过图片路径创建
    // 确保你的 Resources 文件夹下有 "bullet.png" (或者你实际的文件名)
    auto bullet = Sprite::create("image/Pea.png");
    bullet->setScale(1.5f);

    if (bullet) {
        bullet->setPosition(pos);
        this->addChild(bullet);
        _bullets.pushBack(bullet);

        // 使用 setUserData 绑定行号和伤害，方便 update 里做碰撞检测
        auto info = new BulletInfo();
        info->row = row;
        info->damage = damage;
        bullet->setUserData(info);
    }
}

void BulletLayer::update(float dt) {
    auto scene = Director::getInstance()->getRunningScene();
    auto zombieLayer = dynamic_cast<ZombieLayer*>(scene->getChildByName("ZombieLayer"));
    if (!zombieLayer) return;

    auto winSize = Director::getInstance()->getWinSize();

    for (int i = _bullets.size() - 1; i >= 0; --i) {
        auto b = _bullets.at(i);
        auto info = static_cast<BulletInfo*>(b->getUserData());

        // 1. 移动子弹
        b->setPositionX(b->getPositionX() + _bulletSpeed * dt);

        // 2. 越界销毁
        if (b->getPositionX() > winSize.width + 50) {
            delete info; // 释放内存
            b->removeFromParent();
            _bullets.erase(i);
            continue;
        }

        // 3. 碰撞检测
        auto& allZombies = zombieLayer->getAllZombies();
        for (auto z : allZombies) {
            // 只有同一行才检测
            if (z->getRow() == info->row) {
                float dist = b->getPositionX() - z->getPositionX();
                // 根据僵尸图片的宽度调整碰撞判定范围 (例如 40 像素)
                if (dist > -20 && dist < 20) {
                    // 1. 停止子弹的所有位移逻辑
                    // 立即从逻辑列表删除，防止它再次触发碰撞或继续飞行
                    _bullets.erase(i);

                    // 2. 切换纹理（注意路径要对，确保 Resources/image 下有这张图）
                    b->setTexture("image/PeaDisappear.png");

                    // 3. 僵尸扣血逻辑
                    z->takeDamage(info->damage);

                    // 4. 执行延时消失序列
                    auto delay = DelayTime::create(0.1f); // 爆炸显示 0.1 秒
                    auto remove = CallFunc::create([b, info]() {
                        delete info;       // 释放自定义数据内存
                        b->removeFromParent(); // 正式从场景删除
                        });

                    b->runAction(Sequence::create(delay, remove, nullptr));

                    break;
                }
            }
        }
    }
}

void BulletLayer::pauseAllBullets() {
    this->pause();
    for (auto z : _bullets) {
        z->pause();
        for (auto c : z->getChildren()) c->pause(); // 递归暂停动画精灵
    }
}

void BulletLayer::resumeAllBullets() {
    this->resume();
    for (auto z : _bullets) {
        z->resume();
        for (auto c : z->getChildren()) c->resume(); // 递归暂停动画精灵
    }
}