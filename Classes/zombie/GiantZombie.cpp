#include "zombie/GiantZombie.h"
#include "manager/MapMgr.h"
#include "layer/PlantLayer.h"
#include "manager/MapMgr.h"
#include "AudioEngine.h"

bool GiantZombie::init() {
    if (!Zombie::initWithZombieType(ZombieType::Giant)) return false;
    _state = ZombieState::WALK;
    return true;
}

void GiantZombie::update(float dt) {
    if (_state == ZombieState::DYING || _state == ZombieState::DEAD) return;

    handleStateSwitch();

    // 1. 寻找攻击目标
    auto scene = Director::getInstance()->getRunningScene();
    auto plantLayer = dynamic_cast<PlantLayer*>(scene->getChildByName("PlantLayer"));

    bool targetInFront = false;
    if (plantLayer) {
        auto& allPlants = plantLayer->getAllPlants();
        for (auto plant : allPlants) {
            // 巨人攻击距离稍远 (约80像素)
            float dist = this->getPositionX() - plant->getPositionX();
            if (abs(this->getPositionY() - plant->getPositionY()) < 50.0f) {
                if (dist > 0 && dist < 100.0f) {
                    targetInFront = true;
                    if (_state != ZombieState::ATTACK && _attackCooldown <= 0) {
                        _state = ZombieState::ATTACK;
                        _mainSprite->stopAllActions();

                        // 根据是否丢过小鬼决定用哪套攻击动画
                        auto animName = _hasThrownImp ? "GiantAttack_Damaged" : "GiantAttack_Normal";
                        auto anim = AnimationCache::getInstance()->getAnimation(animName);

                        // 砸击动画逻辑：动画到一半时产生伤害
                        auto hitEffect = CallFunc::create([plant]() {
                            plant->takeDamage(9999); // 秒杀植物
                            AudioEngine::play2d("Music/giant_thump.ogg");
                            });

                        auto finish = CallFunc::create([this]() {
                            _state = ZombieState::WALK;
                            _attackCooldown = _props.attackInterval;
                            // 恢复行走动画
                            this->changeAnimation(_hasThrownImp ? "GiantWalk_Damaged" : "GiantWalk_Normal");
                            });

                        _mainSprite->runAction(Sequence::create(Animate::create(anim), hitEffect, finish, nullptr));
                    }
                    break;
                }
            }
        }
    }

    // 2. 移动逻辑
    if (_state == ZombieState::WALK) {
        this->setPositionX(this->getPositionX() - _props.speed * dt);
        if (_attackCooldown > 0) _attackCooldown -= dt;
    }
}

void GiantZombie::handleStateSwitch() {
    // 半血判定
    if (!_hasThrownImp && _currentHealth < (_props.health / 2)) {
        _hasThrownImp = true;
        _state = ZombieState::ARMOR_LOSS; // 借用状态触发抛出小鬼动画
        _mainSprite->stopAllActions();

        const auto special_map = ZombieData::getSpecialAnimMap(ZombieType::Giant);
        const auto& armor_loss_data = special_map.at(_state);
        const auto& headless_walk_data = special_map.at(ZombieState::HEADLESS_WALK);
        auto throwAnim = AnimationCache::getInstance()->getAnimation(armor_loss_data.animationName);
        auto spawnImp = CallFunc::create([this]() {
            // 这里可以添加生成小鬼僵尸的代码并给它一个抛物线动作
            CCLOG("Gargantuar: Imp Thrown!");
            });

        auto backToWalk = CallFunc::create([this, headless_walk_data]() {
            _state = ZombieState::HEADLESS_WALK;
            this->changeAnimation(headless_walk_data.animationName);
            });

        _mainSprite->runAction(Sequence::create(Animate::create(throwAnim), spawnImp, backToWalk, nullptr));
    }
}

void GiantZombie::onDie(ZombieState dieType) 
{
    // 强制将死亡类型转为普通倒地，因为巨人没有 BOOMDIE 素材
    // 这样即便被樱桃炸弹炸死，巨人也只是播放正常的死亡倒地动画
    ZombieState forcedDieType = ZombieState::DYING;

    // 调用基类 Zombie::onDie，传入强制修正后的类型
    Zombie::onDie(forcedDieType);
}