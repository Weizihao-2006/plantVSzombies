#include "AnimationHelper.h"
#include "plant/PlantData.h"
#include "zombie/ZombieData.h"


void AnimationHelper::initResources() 
{
    auto frameCache = SpriteFrameCache::getInstance();

    // B. 加载植物图集 (可以从 PlantData 获取当前关卡所有植物的资源路径)
    const auto& configs = PlantData::getAllConfigs();
    for (auto const& [type, props] : configs) {
        if (!props.plistPath.empty()) { // 建议在 PlantProperties 增加这个字段
            frameCache->addSpriteFramesWithFile(props.plistPath);
        }
    }

    // 2. 加载僵尸图集 (修复报错的关键)
    const auto& zombieConfigs = ZombieData::getAllConfigs();
    for (auto const& [type, props] : zombieConfigs) {
        if (!props.plistPath.empty()) {
            // 确保 ZombieData 里的 plistPath 路径是对的，比如 "image/Zombie.plist"
            frameCache->addSpriteFramesWithFile(props.plistPath);
        }
    }

    // C. 资源进内存后，统一生成并缓存动画
    loadAllAnimations();
}

void AnimationHelper::loadAllAnimations() 
{
    const auto& configs = PlantData::getAllConfigs();

    // 遍历所有植物配置，自动生成动画
    for (auto const& [type, props] : configs) {
        // 只有配置了动画前缀且帧数大于 0 的才处理
        if (!props.animPrefix.empty() && props.animFrameCount > 0) {
            createAndCache(props.animPrefix, props.animFrameCount, props.animDelay, props.animationName);
        }
    }

    // 新增对僵尸动画的加载
    for (auto const& [type, props] : ZombieData::getAllConfigs()) {
        if (!props.animPrefix.empty()) {
            createAndCache(props.animPrefix, props.animFrameCount, props.animDelay, props.animationName);
        }
    }
    // 处理非植物类的杂项动画（如阳光旋转、僵尸行走、爆炸效果）
    // 这些可以直接硬编码在此，因为它们不涉及卡槽和种植逻辑
 
    //.......
}

void AnimationHelper::createAndCache(const std::string& prefix, int frameCount, float delay, const std::string& animName) 
{
    auto cache = cocos2d::AnimationCache::getInstance();
    if (cache->getAnimation(animName)) 
        return;

    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    auto frameCache = cocos2d::SpriteFrameCache::getInstance();
    for (int i = 0; i < frameCount; i++) {
        // 格式化帧名：例如 "SunFlower_0.png"
        std::string name = cocos2d::StringUtils::format("%s%d.png", prefix.c_str(), i);
        auto frame = frameCache->getSpriteFrameByName(name);
        if (frame) {
            frames.pushBack(frame);
        }
    }

    if (!frames.empty()) {
        auto anim = cocos2d::Animation::createWithSpriteFrames(frames, delay);
        cache->addAnimation(anim, animName);
    }
}