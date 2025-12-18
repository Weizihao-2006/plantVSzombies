#pragma once
#include "cocos2d.h"
#include"plant/PlantData.h"
#include <string>

USING_NS_CC;

/**
    * 快速创建并缓存动画
    * @param prefix 帧名字的前缀，如 "SunFlower_"
    * @param frameCount 帧的总数
    * @param delay 帧间隔时间
    * @param animName 存入缓存的动画标识名，如 "SunFlower_Anim"
    */

class AnimationHelper {
public:
    // 根据配方创建动画并放入缓存
    static void createAndCache(const std::string& prefix, int frameCount, float delay, 
        const std::string& animName) {

        if (animName.empty() || AnimationCache::getInstance()->getAnimation(animName)) 
            return;

        Vector<SpriteFrame*> frames;
        for (int i = 0; i < frameCount; i++) {
            std::string name = StringUtils::format("%s%d.png", prefix.c_str(), i);
            auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
            if (frame) frames.pushBack(frame);
        }

        if (!frames.empty()) {
            auto anim = Animation::createWithSpriteFrames(frames, delay);
            AnimationCache::getInstance()->addAnimation(anim, animName);
        }
    }

    // 自动加载：一键同步 PlantData 里的所有动画
    static void loadAllAnimations() {
        // 1. 遍历植物配置表自动加载
        auto& configs = PlantData::getAllConfigs();
        for (auto const& [type, props] : configs) {
            if (!props.animPrefix.empty()) {
                createAndCache(props.animPrefix, props.animFrameCount, props.animDelay, props.animationName);
            }
        }

        // 2. 加载非植物类的特殊动画（如阳光、爆炸、僵尸等）
        createAndCache("Sun_", 22, 0.05f, "Sun_Rotate");
        createAndCache("Boom_", 8, 0.1f, "CherryBomb_Boom");
    }
};