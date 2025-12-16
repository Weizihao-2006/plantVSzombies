#include"layer/BackGroundLayer.h"
USING_NS_CC;


bool BgLayer::init()
{
	if (!Layer::init())
		return false;

	auto background = Sprite::create("PVZBackground_4.jpg");
	// 获取背景图片的原始尺寸
	Size textureSize = background->getContentSize();


	Rect newRect;
	newRect.origin = Vec2::ZERO;
	// 裁剪掉图片的右25%
	newRect.size = Size(textureSize.width * 0.75f, textureSize.height);

	// 设置新的纹理显示区域
	background->setTextureRect(newRect);

	// 按宽度缩放适应屏幕,有黑边,可以修改
	auto size = Director::getInstance()->getVisibleSize();
	background->setScale(size.width/background->getContentSize().width);
	background->setPosition(Vec2::ZERO);

	this->addChild(background);

	return true;
}