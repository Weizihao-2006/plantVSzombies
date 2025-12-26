#include "zombie/ImpZombie.h"
#include "AudioEngine.h"

USING_NS_CC;


// ImpZombie.cpp
bool ImpZombie::init() {

    if (!Zombie::initWithZombieType(ZombieType::Imp))
        return false;

    _state = ZombieState::WALK;
    this->scheduleUpdate();
    return true;
}


void ImpZombie::flyTo(float targetPosX) 
{
    _isFlying = true;
    _state = ZombieState::ARMOR_LOSS;

    // 禁用碰撞和更新，防止飞行过程中吃植物或被打
    this->pause();

    // 计算跳跃高度和持续时间
    float distance = abs(this->getPositionX() - targetPosX);
    float duration = 1.0f + (distance / 500.0f); // 根据距离动态计算时间

    // 创建抛物线动作：JumpTo(时长, 目标点, 跳跃高度, 跳跃次数)
    auto jump = JumpTo::create(duration, Vec2(targetPosX, this->getPositionY()), 150.0f, 1);

    // 落地后的回调

    auto& anim = ZombieData::getProps(ZombieType::Imp);


    auto landCallback = CallFunc::create([this,&anim]() {
        _isFlying = false;
        this->resume(); // 恢复 update
        _state = ZombieState::WALK;
        this->changeAnimation(anim.animationName); // 切换为正常行走
       
        //AudioEngine::play2d("Music/imp_land.ogg");
        });

    this->runAction(Sequence::create(jump, landCallback, nullptr));
}

void ImpZombie::update(float dt) 
{
   
    // 2. 如果没在飞行，执行父类 CommonZombie 的逻辑（检测植物、啃咬等）
    CommonZombie::update(dt);
}