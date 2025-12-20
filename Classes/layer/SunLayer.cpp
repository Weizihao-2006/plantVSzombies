#include "layer/SunLayer.h"
#include "util/Global.h"
#include"manager/MapMgr.h"
// --- SunSprite ---

const int SunSprite::exitSeqTag = 100;
const float SunSprite::SUN_SCALE = 1.5f;


SunSprite* SunSprite::createSun(int amount)
{
    auto sun = new (std::nothrow) SunSprite();

    if (sun && sun->initWithSpriteFrameName("Sun_0.png")) {
        sun->_amount = amount;
        sun->setScale(SunSprite::SUN_SCALE);
        sun->autorelease();

        // --- 1. 序列帧旋转动画 (保持不变) ---
        Vector<SpriteFrame*> frames;
        for (int i = 0; i <= 21; i++) {
            std::string frameName = StringUtils::format("Sun_%d.png", i);
            auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
            if (frame) frames.pushBack(frame);
        }
        if (!frames.empty()) {
            auto animation = Animation::createWithSpriteFrames(frames, 0.05f);
            sun->runAction(RepeatForever::create(Animate::create(animation)));
        }

        // --- 2. 自动消失逻辑 ---
        // 停留8秒 -> 闪烁 1.5秒 (警告) -> 消失
        auto wait = DelayTime::create(8.0f);
        auto blink = Blink::create(1.5f, 5); // 2秒闪烁10次，提醒玩家快消失了
        auto fadeOut = FadeOut::create(0.5f);
        auto removeSelf = CallFunc::create([sun]() {
            // 从父节点移除（这会触发清理逻辑）
            sun->removeFromParent();
            });

        auto autoExitSeq = Sequence::create(wait, blink, fadeOut, removeSelf, nullptr);
        autoExitSeq->setTag(SunSprite::exitSeqTag); // 设置一个特定的Tag
        sun->runAction(autoExitSeq);
        return sun;
    }
    CC_SAFE_DELETE(sun);
    return nullptr;
}

//这个函数的调用时机是点击了阳光
void SunSprite::collect(Vec2 destPos)
{
    // 停止所有动作 (包括旋转动画和自动消失倒计时)
    this->stopAllActions();

    auto move = MoveTo::create(0.6f, destPos);
    auto scale = ScaleTo::create(0.6f, 0.8f);
    auto callback = CallFunc::create([this]() {
        Global::getInstance()->addSun(_amount); // 增加阳光
        this->removeFromParent(); // 飞到终点后移除
        });

    this->runAction(Sequence::create(Spawn::create(move, scale, nullptr), callback, nullptr));
}

// --- SunLayer ---
bool SunLayer::init() 
{
    if (!Layer::init()) 
        return false;

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("images/Sun.plist");
    this->scheduleUpdate();
    return true;
}

//自动从天空掉落
void SunLayer::update(float dt) 
{
    _clearTimer += dt;
    _dropTimer += dt;

    if (_dropTimer >= _dropInterval) {
        spawnSkySun();
        _dropTimer = 0;
    }
    //遍历Vector，如果发现某个阳光已经没有父节点了，说明它已经执行了removeFromParent
    //用不着每帧都操作,每隔一秒操作一次即可
    if (_clearTimer >= 1.0f) {
        _clearTimer = 0.0f;
        for (int i = _suns.size() - 1; i >= 0; --i) {
            auto sun = _suns.at(i);
            if (sun->getParent() == nullptr) {
                _suns.erase(i); //Vector会自动调用 release()，真正释放内存
            }
        }
    }
    
}

void SunLayer::spawnSkySun() 
{

    auto mapTop = MapManager::getInstance()->getMapTop();
    auto mapLeft = MapManager::getInstance()->getMapLeft();
    auto mapRight= MapManager::getInstance()->getMapRight();
    auto mapBottom= MapManager::getInstance()->getMapBottom();

    float startX = random(mapLeft + 100.0f, mapRight - 100.0f);
    float endY = random(mapBottom + 50.0f, mapTop - 400.0f);

    auto sun = SunSprite::createSun();
    sun->setPosition(startX, mapTop);
    this->addChild(sun);

    _suns.pushBack(sun); //加入管理

    sun->runAction(MoveTo::create(_dropTimeCost, Vec2(startX, endY)));
}

void SunLayer::spawnSunFromPlant(Vec2 pos) {

    auto sun = SunSprite::createSun();
    sun->setPosition(pos+Vec2(30.0f,30.0f));
    this->addChild(sun);

    _suns.pushBack(sun);

    // 经典的向日葵弹出动画：向上划出并落地
    auto jump = JumpBy::create(0.6f, Vec2(random(-100, 100), -50), 100, 1);
    sun->runAction(jump);
}

bool SunLayer::containsAndCollectSun(const Vec2& worldPos) {
    //资源栏位置(飞向的目标)
    Vec2 collectorPos = Vec2(730.0f, 1150.0f);

    //将点击的世界坐标转换到SunLayer坐标系中
    Vec2 localPos = this->convertToNodeSpace(worldPos);
    for (int i = _suns.size() - 1; i >= 0; --i) {
        auto sun = _suns.at(i);
        //sun->getBoundingBox() 返回的是它在父节点坐标系下的范围。
        //_cur = touch->getLocation(); 获取的是屏幕坐标（World Space）
        if (sun->getBoundingBox().containsPoint(localPos)) {
            //调用collect函数执行Global中阳光的更新以及动画
            sun->collect(collectorPos);
            _suns.erase(i); // 从 Vector移除会自动release
            return true;
        }
    }
    return false;
}