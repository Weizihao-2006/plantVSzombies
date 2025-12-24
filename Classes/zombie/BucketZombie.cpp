#include "zombie/BucketZombie.h"
#include "manager/MapMgr.h"
#include "layer/PlantLayer.h"
#include "AudioEngine.h"

using namespace cocos2d;

bool BucketheadZombie::init() {
    // 1. 初始化铁桶僵尸属性（ZombieData里配置的HP应该是1370左右）
    if (!Zombie::initWithZombieType(ZombieType::Buckethead)) {
        return false;
    }

    // 2. 动态获取掉落阈值（不硬编码）
    // _armorBrokenThreshold = ZombieData::getProps(ZombieType::Normal).health;
    _isArmorBroken = false;
    _state = ZombieState::WALK;

    this->scheduleUpdate();
    return true;
}

void BucketheadZombie::update(float dt) {
    if (_state == ZombieState::DYING || _state == ZombieState::DEAD || _state == ZombieState::BOOMDIE)
        return;

    auto normalProps = ZombieData::getProps(ZombieType::Normal);
    int normalHealth = normalProps.health;

    // --- 核心逻辑：检测铁桶是否该掉了 ---
    if (!_isArmorBroken && _currentHealth <= normalHealth) {
        _isArmorBroken = true;
        this->convertToNormal();
    }

    // --- 以下逻辑与普通僵尸一致：检测植物、切换攻击/行走 ---
    auto scene = Director::getInstance()->getRunningScene();
    auto plantLayer = dynamic_cast<PlantLayer*>(scene->getChildByName("PlantLayer"));

    bool findPlant = false;
    if (plantLayer) {
        auto& allPlants = plantLayer->getAllPlants();
        for (auto plant : allPlants) {
            if (plant->getState() == PlantState::DYING) continue;

            float dist = this->getPositionX() - plant->getPositionX();
            auto mapPos = MapManager::getInstance()->getPositionInMap(this->getRow(), 0);

            if (abs(mapPos.y - plant->getPositionY()) < 50.0f) {
                if (dist > 0 && dist < 60.0f) {
                    findPlant = true;
                    if (_state != ZombieState::ATTACK) {
                        _state = ZombieState::ATTACK;
                        // 切换到铁桶僵尸的啃咬动画（带铁桶音效感的那种）
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

    if (!findPlant && _state == ZombieState::ATTACK) {
        _state = ZombieState::WALK;
        this->changeAnimation(_props.animationName);
    }

    if (_state == ZombieState::WALK) {
        Zombie::update(dt);
    }
}

void BucketheadZombie::convertToNormal() {
    // 1. 播放铁桶掉落碎片特效
    auto specialAnims = ZombieData::getSpecialAnimMap(ZombieType::Buckethead);
    if (specialAnims.count(ZombieState::ARMOR_LOSS)) {
        auto armorData = specialAnims.at(ZombieState::ARMOR_LOSS);
        auto armor = Sprite::create();
        armor->setPosition(this->getPosition() + Vec2(0, 80));
        this->getParent()->addChild(armor, this->getLocalZOrder() + 1);

        auto anim = AnimationCache::getInstance()->getAnimation(armorData.animationName);
        if (anim) {
            armor->runAction(Sequence::create(Animate::create(anim), RemoveSelf::create(), nullptr));
        }
    }

    // 播放金属掉落音效
    AudioEngine::play2d("Music/bucket_fall.mp3");

    // 2. 转换配置为普通僵尸
    auto normalProps = ZombieData::getProps(ZombieType::Normal);
    _props.animationName = normalProps.animationName;
    _props.animPrefix = normalProps.animPrefix;

    // 3. 刷新当前动画
    if (_state == ZombieState::ATTACK) {
        auto anims = ZombieData::getSpecialAnimMap(ZombieType::Normal);
        this->changeAnimation(anims.at(ZombieState::ATTACK).animationName);
    }
    else {
        this->changeAnimation(_props.animationName);
    }
}

void BucketheadZombie::eatPlant(float dt, Plants* plant) {
    _eatTimer += dt;
    if (_eatTimer >= _props.attackInterval) {
        _eatTimer = 0.0f;
        AudioEngine::play2d("Music/zombie_eb_1.mp3");
        if (plant) plant->takeDamage(_props.attackPower);
    }
}