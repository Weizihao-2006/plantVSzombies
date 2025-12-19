#pragma once
#include "cocos2d.h"
#include"plant/PlantData.h"
#include <string>

USING_NS_CC;
class AnimationHelper {
public:
	/**
  * @brief 一键解析 PlantData 配置表并批量生成/缓存所有植物动画
  * * 该函数会遍历整个配置表，通过 animPrefix 等参数自动调用 createAndCache。
  * 调用前提：必须先将对应的 .plist 文件载入 SpriteFrameCache。
  */
	static void loadAllAnimations();

	/**
	 * @brief 通用的动画创建并缓存工具
	 * * @param prefix    帧名字的前缀 (例如 "SunFlower_")，函数会自动匹配 "前缀0.png" 到 "前缀N.png"
	 * @param frameCount 动画包含的总帧数
	 * @param delay     每帧之间的切换间隔时间 (秒)
	 * @param animName  存入 AnimationCache 的唯一标识名，后续通过此名字获取动画
	 */
	static void createAndCache(const std::string& prefix, int frameCount, float delay, const std::string& animName);
};