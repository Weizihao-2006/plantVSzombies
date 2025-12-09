#include "scene/GameScene.h"
#include "layer/BackgroundLayer.h"
#include "layer/CardBarLayer.h"
#include "layer/SunLayer.h"
#include "layer/PlantLayer.h"
#include "layer/ZombieLayer.h"
#include "layer/BulletLayer.h"
#include "layer/GameUILayer.h"
#include "util/Global.h"

USING_NS_CC;

GameScene* GameScene::createWithLevel(int level_id)
{
	auto p = new(std::nothrow) GameScene;
	if (p && p->initWithLevel(level_id)) {
		p->autorelease();
		return p;
	}
	CC_SAFE_DELETE(p);
	return nullptr;
}

bool GameScene::initWithLevel(int level_id)
{
	if (!Scene::init())
		return nullptr;
	_levelID = level_id;

	//
	Global::getInstance()->setLevelID(level_id);


}

//按顺序创建各Layers
void GameScene::createLayers()
{
	_bgLayer = BgLayer::create();
	_sunLayer = SunLayer::create();
	_cardBarLayer = CardBarLayer::create();
	_plantLayer = PlantLayer::create();
	_zombieLayer = ZombieLayer::create();
	_bulletLayer = BulletLayer::create();
	_uiLayer = GameUILayer::create();

	// 全部挂到场景树上
	this->addChild(_bgLayer, 0);
	this->addChild(_sunLayer, 10);
	this->addChild(_cardBarLayer, 20);
	this->addChild(_plantLayer, 30);
	this->addChild(_zombieLayer, 40);
	this->addChild(_bulletLayer, 50);
	this->addChild(_uiLayer, 100);
}
void GameScene::_bindLayerSignals()
{
	// 1. UI 层 -> 本场景：暂停/加速
	_uiLayer->onPauseBtnClicked = [this](bool p) { onPause(p); };
	_uiLayer->onSpeedBtnClicked = [this](float s) { onSpeedChanged(s); };

	// 2. 僵尸层 -> 本场景：失败/胜利
	_zombieLayer->onZombieReachHouse = [this]() { onZombieEnterHouse(); };
	_zombieLayer->onAllZombieDead = [this]() { onAllZombieClear(); };

	// 3. 阳光层 -> 植物层：阳光变化
	_sunLayer->onSunChanged = [this](int val) {
		_cardBarLayer->refreshSun(val);   // 更新卡槽灰化
		_plantLayer->setSun(val);         // 种下时二次判定
		};
}

/
void GameScene::onZombieEnterHouse()
{
	log("GameScene: zombie entered house -> fail");
	Director::getInstance()->replaceScene(MenuScene::create()); // 先简单跳回菜单
}
void GameScene::onAllZombieClear()
{
	log("GameScene: all zombies cleared -> win");
	Director::getInstance()->replaceScene(MenuScene::create());
}
void GameScene::onPause(bool pause)
{
	if (pause) Director::getInstance()->pause();
	else       Director::getInstance()->resume();
}
void GameScene::onSpeedChanged(float s)
{
	Director::getInstance()->getScheduler()->setTimeScale(s);
}