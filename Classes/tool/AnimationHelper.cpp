#include "AnimationHelper.h"
#include "plant/PlantData.h"
#include "zombie/ZombieData.h"


void AnimationHelper::initResources() 
{
    auto frameCache = SpriteFrameCache::getInstance();

    // B. 加载植物图集 (可以从 PlantData 获取当前关卡所有植物的资源路径)
    const auto& configs = PlantData::getAllConfigs();
    for (auto const& [type, props] : configs) {
        if (!props.plistPath.empty()) {
            frameCache->addSpriteFramesWithFile(props.plistPath);
        }
        auto specialAnims = PlantData::getSpecialAnimMap(type);//获取对应植物类型的特殊动画
        for (auto const& [stateName, anim] : specialAnims) {
            // 如果有独立的 plist，先载入
            if (!anim.plistPath.empty()) {
                SpriteFrameCache::getInstance()->addSpriteFramesWithFile(anim.plistPath);
            }
        }
    }

    

    // 2. 加载僵尸图集
    const auto& zombieConfigs = ZombieData::getAllConfigs();
    for (auto const& [type, props] : zombieConfigs) {
        if (!props.plistPath.empty()) {
            frameCache->addSpriteFramesWithFile(props.plistPath);
        }
        auto specialAnims = ZombieData::getSpecialAnimMap(type);//获取对应植物类型的特殊动画
        for (auto const& [stateName, anim] : specialAnims) {
            // 如果有独立的 plist，先载入
            if (!anim.plistPath.empty()) {
                SpriteFrameCache::getInstance()->addSpriteFramesWithFile(anim.plistPath);
            }
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
        // 只有配置了.plist才处理
        if (!props.plistPath.empty()) {
            createAndCache(props.animPrefix, props.animFrameCount, props.animDelay, props.animationName);
        }
        else {
            // 如果没有 plist，说明是张单图，构造路径格式：image/Repeater/Repeater_%d.png
            std::string pathFormat = "image/" + props.name + "/" + props.animPrefix+"%d.png";
            createAndCacheFromFiles(pathFormat, props.animFrameCount, props.animDelay, props.animationName);
        }


        auto specialAnims = PlantData::getSpecialAnimMap(type);//获取对应植物类型的特殊动画
        for (auto const& [stateName, anim] : specialAnims) {//遍历可能的特殊动画

            if (!anim.prefix.empty() && anim.frameCount > 0) {
                createAndCache(anim.prefix, anim.frameCount, anim.delay, anim.animationName);
            }


        }
    }

    // 新增对僵尸动画的加载
    for (auto const& [type, props] : ZombieData::getAllConfigs()) {
        if (!props.animPrefix.empty()) {
            createAndCache(props.animPrefix, props.animFrameCount, props.animDelay, props.animationName);
        }
        auto specialAnims = ZombieData::getSpecialAnimMap(type);//获取对应僵尸类型的特殊动画
        for (auto const& [stateName, anim] : specialAnims) {//遍历可能的僵尸特殊动画

            if (!anim.prefix.empty() && anim.frameCount > 0) {
                createAndCache(anim.prefix, anim.frameCount, anim.delay, anim.animationName);
            }
        }

    }

}


void AnimationHelper::createAndCacheFromFiles(const std::string& pathFormat, int frameCount, float delay, const std::string& animName)
{
    auto cache = AnimationCache::getInstance();
    auto sfc = SpriteFrameCache::getInstance(); // 获取帧缓存引用

    if (cache->getAnimation(animName)) return;

    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= frameCount; i++) {
        // 1. 生成完整的文件路径，例如 "image/Repeater/Repeater_1.png"
        std::string fullPath = StringUtils::format(pathFormat.c_str(), i);

        // 2. 检查缓存中是否已经存在这个帧（避免重复加载）
        // 我们可以根据 pathFormat 提取出帧名，例如 "Repeater_1.png"
        // 简单处理：直接用 fullPath 里的文件名作为帧名
        std::string frameName = fullPath.substr(fullPath.find_last_of("/") + 1);

        SpriteFrame* frame = sfc->getSpriteFrameByName(frameName);

        if (!frame) {
            // 如果缓存里没有，先通过文件路径创建一个 Sprite
            auto sprite = Sprite::create(fullPath);
            if (sprite) {
                frame = sprite->getSpriteFrame();
                // 【核心修改】：将这个帧手动存入 SpriteFrameCache，并绑定名字
                sfc->addSpriteFrame(frame, frameName);
            }
        }

        if (frame) {
            frames.pushBack(frame);
        }
    }

    if (!frames.empty()) {
        auto animation = Animation::createWithSpriteFrames(frames, delay);
        cache->addAnimation(animation, animName);
        CCLOG("Animation cached from single files, FrameName example: %s", animName.c_str());
    }
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


