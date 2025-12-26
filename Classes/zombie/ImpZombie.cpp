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


void ImpZombie::flyTo(float startY) 
{
    _isFlying = true;
    _state = ZombieState::ARMOR_LOSS;

    // 禁用碰撞和更新，防止飞行过程中吃植物或被打
    this->pause();

    this->changeAnimation("ImpThrow");

    // 获取动画时长
    auto animation = AnimationCache::getInstance()->getAnimation("ImpThrow");
    float duration = animation->getDuration();

    auto& anim = ZombieData::getProps(ZombieType::Imp);

    // 落地后的动作序列
    auto landEffect = CallFunc::create([this]() {
        _isFlying = false;
        _state = ZombieState::WALK;
        this->resume(); // 真正开始工作（CommonZombie::update 会接管）
        this->changeAnimation("ImpWalk"); // 落地后切回行走
       
        // 可以在这里加一个震屏或尘土效果
        AudioEngine::play2d("Music/imp_land.ogg");
        });

    //修正y坐标
    auto correctY = CallFunc::create([this]() {
        this->setPositionY(this->getPositionY() - 70.0f);

        });
    // 运行动作：只等待动画播完，不移动位置（因为动画自带位移）
    this->runAction(Sequence::create(DelayTime::create(duration), landEffect, correctY, nullptr));
}

void ImpZombie::update(float dt) 
{
   
    // 2. 如果没在飞行，执行父类 CommonZombie 的逻辑（检测植物、啃咬等）
    CommonZombie::update(dt);
}