#include "zombie/GiantZombie.h"
#include "manager/MapMgr.h"
#include "layer/PlantLayer.h"
#include "manager/MapMgr.h"
#include"zombie/ImpZombie.h"
#include"manager/ZombieMgr.h"
#include "AudioEngine.h"

Rect GiantZombie::getHitbox() const {
    if (!_mainSprite) return Rect::ZERO;

    // 获取巨人的世界位置
    Vec2 worldPos = this->convertToWorldSpace(_mainSprite->getPosition());

    // 手动定义巨人的“肉身”宽度和高度
    // 假设一格的高度是 140 像素，两格高就是 280
    float customWidth = 100.0f * this->getScale();
    float customHeight = 200.0f * this->getScale();

    // 我们可以通过偏移量，让受击框更贴合身体中心
    // 比如：x 向左偏移一半宽度，y 从脚底（origin）向上延伸
    return Rect(
        worldPos.x - customWidth / 2,
        worldPos.y,
        customWidth,
        customHeight
    );
}

bool GiantZombie::init() {
    if (!Zombie::initWithZombieType(ZombieType::Giant)) return false;
    _state = ZombieState::WALK;
    return true;
}

void GiantZombie::update(float dt) {
    if (_state == ZombieState::DYING || _state == ZombieState::DEAD||
        _state==ZombieState::ATTACK||_state==ZombieState::HEADLESS_ATTACK||
        _state == ZombieState::ARMOR_LOSS)//这些状态都不更新
        return;

    handleStateSwitch();

    // 1. 获取植物层
    auto scene = Director::getInstance()->getRunningScene();
    auto plantLayer = dynamic_cast<PlantLayer*>(scene->getChildByName("PlantLayer"));
    if (!plantLayer)
        return;

    auto& allPlants = plantLayer->getAllPlants();

    for (auto plant : allPlants) {

        if (plant->getRow() != this->getRow())
            continue;
        
        // 计算水平距离（巨人坐标 - 植物坐标）
        float dist = this->getPositionX() - plant->getPositionX();
        if (dist < 0 || dist > 110.0f)
            continue;

        // 攻击冷却递减
        if (_attackCooldown > 0) {
            _attackCooldown -= dt;
            _state = ZombieState::STAY;
            break;
        }
        //可以攻击
        _state = _hasThrownImp ? ZombieState::HEADLESS_ATTACK : ZombieState::ATTACK;
        _mainSprite->stopAllActions();

        auto animName = _hasThrownImp ? "GiantAttack_Damaged" : "GiantAttack_Normal";
        auto anim = AnimationCache::getInstance()->getAnimation(animName);
        if (!anim)
            return;

        float totalDuration = anim->getDuration(); // 获取动画总时长
        float hitDelay = totalDuration;    // 假设第 70% 帧是砸下的瞬间

        // 1. 伤害逻辑：延迟执行
        auto delayToHit = DelayTime::create(hitDelay);
        auto doDamage = CallFunc::create([plant, this]() {
            // 二次确认：植物还活着且巨人在攻击态（没被控死或炸死）
            if (plant && !plant->isDead() && (_state == ZombieState::ATTACK|| _state==ZombieState::HEADLESS_ATTACK)) {
                plant->takeDamage(ZombieData::getProps(ZombieType::Giant).attackPower);
               
                CCLOG("Giant hit the plant at mid-animation!");
            }
            });

        // 2. 完成逻辑：动画播完后恢复行走
        auto delayToFinish = DelayTime::create(totalDuration - hitDelay);
        auto finish = CallFunc::create([plant, this]() {
            _state = _hasThrownImp ? ZombieState::HEADLESS_WALK : ZombieState::WALK;
            _attackCooldown = _props.attackInterval;
            this->changeAnimation(_hasThrownImp ? "GiantWalk_Damaged" : "GiantWalk_Normal");
            });

        // 让主精灵运行动画
        _mainSprite->runAction(Animate::create(anim));

        // 让自身（Node）运行逻辑序列，确保即使 _mainSprite 切换了动画，逻辑依然按时执行
        this->runAction(Sequence::create(delayToHit, doDamage, delayToFinish, finish, nullptr));
    }

    // 2. 移动与冷却逻辑
    if (_state == ZombieState::WALK||_state==ZombieState::HEADLESS_WALK) {

        // 攻击冷却递减
        if (_attackCooldown > 0) {
            _attackCooldown -= dt;
        }
        Zombie::update(dt);
    }
   
}

void GiantZombie::handleStateSwitch() {
    // 半血判定
   
    if (!_hasThrownImp && _currentHealth < (_props.health / 2)&&_state!=ZombieState::ATTACK) {
        this->throwImp();
    }
}
void GiantZombie::throwImp() {
    _hasThrownImp = true;
    _state = ZombieState::ATTACK; // 借用攻击状态来暂停移动
    _mainSprite->stopAllActions();

    auto anim = AnimationCache::getInstance()->getAnimation("GiantThrow");
    if (!anim)
        return;
    // 2. 在动画播放到完（手挥出去）时生成小鬼
    float totalTime = anim->getDuration();


    auto spawnAction = CallFunc::create([this]() {
        // 创建小鬼
        auto imp = ImpZombie::create();
        imp->setScale(1.6f);
        imp->setRow(this->getRow());

        // 初始位置
        float targetX = this->getPositionX() - 350.0f;
        float MaxLeft = MapManager::getInstance()->getPositionInMap(0, 0).x + 130.0f;

        targetX=(targetX < MaxLeft ? MaxLeft : targetX);


        imp->setPositionY(MapManager::getInstance()->getPositionInMap(imp->getRow(), 0).y);
        imp->setPositionX(targetX);
        ZombieMgr::getInstance()->addZombie(imp);

        // 计算落点：通常投掷到左侧 3-4 格位置（约 300 像素）
        
        // 调用小鬼自己的飞行方法
        imp->flyTo(imp->getPositionY());
        });

    // 3. 投掷动作结束后的回调
    auto finish = CallFunc::create([this]() {
        _state = ZombieState::HEADLESS_WALK;
        // 切换到“受伤版”的行走动画
        this->changeAnimation("GiantWalk_Damaged");
        });

    if (anim) {//播放动画
        _mainSprite->runAction(Animate::create(anim));
    }
    this->runAction(Sequence::create(DelayTime::create(totalTime*0.6f),spawnAction, finish, nullptr));
}
void GiantZombie::onDie(ZombieState dieType) 
{
    // 强制将死亡类型转为普通倒地，因为巨人没有 BOOMDIE 素材
    // 这样即便被樱桃炸弹炸死，巨人也只是播放正常的死亡倒地动画
    ZombieState forcedDieType = ZombieState::DYING;
    this->stopAllActions();
    // 调用基类 Zombie::onDie，传入强制修正后的类型
    Zombie::onDie(forcedDieType);
}