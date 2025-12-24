#include "zombie/ConeZombie.h"
#include "manager/MapMgr.h"
#include "layer/PlantLayer.h"
#include "AudioEngine.h"

using namespace cocos2d;

bool ConeheadZombie::init() {
    // 1. 调用基类的初始化，传入 Conehead 类型       
    // 基类会自动拉取 ZombieData 里 hp 为 640 的配置并播放路障行走动画
    if (!Zombie::initWithZombieType(ZombieType::Conehead)) {
        return false;
    }

    // 2. 初始状态为行走
    _state = ZombieState::WALK;
    _isConeBroken = false;

    this->scheduleUpdate();
    return true;
}

void ConeheadZombie::update(float dt) {
    // 如果正在死亡或已死亡，不执行逻辑
    if (_state == ZombieState::DYING || _state == ZombieState::DEAD || _state == ZombieState::BOOMDIE)
        return;

    auto scene = Director::getInstance()->getRunningScene();
    auto plantLayer = dynamic_cast<PlantLayer*>(scene->getChildByName("PlantLayer"));

    // 1. 动态获取普通僵尸的配置
    auto normalProps = ZombieData::getProps(ZombieType::Normal);
    int normalHealth = normalProps.health;
    // 降级逻辑
    if (!_isConeBroken && _currentHealth <= normalHealth) {
        _isConeBroken = true;
        this->convertToNormal(); // 执行转化逻辑
    }

    bool findPlant = false;
    if (plantLayer) {
        auto& allPlants = plantLayer->getAllPlants();
        for (auto plant : allPlants) {
            // 只有存活的植物才会被咬
            if (plant->getState() == PlantState::DYING) continue;

            float dist = this->getPositionX() - plant->getPositionX();

            // 这里的 Y 轴判定逻辑复用你的写法
            auto mapPos = MapManager::getInstance()->getPositionInMap(this->getRow(), 0);
            if (abs(mapPos.y - plant->getPositionY()) < 50.0f) {
                // 距离判定：在僵尸前方 0~60 像素内
                if (dist > 0 && dist < 60.0f) {
                    findPlant = true;
                    if (_state != ZombieState::ATTACK) {
                        _state = ZombieState::ATTACK;
                        // 切换到路障僵尸的啃咬动画
                        auto anims = ZombieData::getSpecialAnimMap(_props.type);
                        if (anims.count(ZombieState::ATTACK)) {
                            this->changeAnimation(anims.at(ZombieState::ATTACK).animationName);
                        }
                    }
                    this->eatPlant(dt, plant);
                    break;
                }
            }
        }
    }

    // 如果没找到植物且处于攻击态，恢复行走
    if (!findPlant && _state == ZombieState::ATTACK) {
        _state = ZombieState::WALK;
        this->changeAnimation(_props.animationName);
    }

    // 如果是行走状态，调用基类的位移逻辑
    if (_state == ZombieState::WALK) {
        Zombie::update(dt);
    }
}

void ConeheadZombie::eatPlant(float dt, Plants* plant) {
    _eatTimer += dt;
    if (_eatTimer >= _props.attackInterval) {
        _eatTimer = 0.0f;

        // 播放啃咬音效
        // AudioEngine::play2d("Music/zombie_eb_1.mp3"); // 建议检查路径

        // 对植物造成伤害
        if (plant) {
            plant->takeDamage(_props.attackPower);
        }
    }
}

void ConeheadZombie::convertToNormal() {
    // 1. 播放路障掉落的碎片动画（如果有资源）
    

    // 2. 关键：修改当前僵尸的配置信息，让它“变成”普通僵尸
    // 获取普通僵尸的配置
    auto normalProps = ZombieData::getProps(ZombieType::Normal);

    // 更新当前僵尸的动画名称和前缀
    _props.animationName = normalProps.animationName;
    _props.animPrefix = normalProps.animPrefix;

    // 3. 立即刷新动画
    // 如果正在啃咬，切到普通啃咬；如果正在行走，切到普通行走
    if (_state == ZombieState::ATTACK) {
        auto anims = ZombieData::getSpecialAnimMap(ZombieType::Normal); // 注意这里用 Normal 的啃咬
        this->changeAnimation(anims.at(ZombieState::ATTACK).animationName);
    }
    else {
        this->changeAnimation(_props.animationName);
    }

    CCLOG("Conehead Zombie armor broken! Converted to Normal.");
}