#include "scene/GameScene.h"
#include "layer/BackgroundLayer.h"
#include "layer/CardBarLayer.h"
#include "layer/SunLayer.h"
#include"layer/PlantLayer.h"
#include "layer/ZombieLayer.h"
#include "layer/BulletLayer.h"
#include "layer/GameUILayer.h"
#include "layer/ControlLayer.h"
#include"tool/AnimationHelper.h"
#include "manager/CardMgr.h"
#include "manager/PlantMgr.h"
#include "util/Global.h"
#include "cocos2d.h"
#include "AudioEngine.h"
#include "manager/ZombieMgr.h"
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
	// 先重置所有manager
	Global::getInstance()->reset();
	CardMgr::getInstance()->reset();
	MapManager::getInstance()->reset();
	ZombieMgr::getInstance()->reset();
	PlantMgr::getInstance()->reset();

	// 播放游戏音乐
	AudioEngine::play2d("Music/StartBGM.MP3", false, 1.0f);
	bgmID = AudioEngine::play2d("Music/GameSceneBGM.MP3", true, 1.0f);

	if (!Scene::init())
		return false;

	_levelID = level_id;
	Global::getInstance()->setLevelID(level_id);


	AnimationHelper::initResources();//加载动画资源
	createLayers();
	bindLayerSignals();

	// 初始化 CardMgr,确实已经初始化了,写在构造函数了
	CardMgr::getInstance();

	// 添加每帧更新
	scheduleUpdate();

	// 单僵尸测试
	this->scheduleOnce([this](float dt) {
		CCLOG("Test: Spawning a zombie!");
		// 注意：如果你还没写 ZombieMgr，可以先手动测试
		// 确保你已经把 ZombieMgr.h 包含在 GameScene.cpp 头部
		ZombieMgr::getInstance()->spawnZombie(ZombieType::Normal, 0);
		}, 3.0f, "test_zombie_spawn");

	return true;
}



// 每帧更新
void GameScene::update(float dt) {

	//事实上,CardLayer有一个update()会调用CardMgr中的卡槽冷却信息更新冷却遮罩等
	//因此GameScene只用传递时间即可
	CardMgr::getInstance()->updateCoolTime(dt);


}

// 按顺序创建 Layers
void GameScene::createLayers()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	_bgLayer = BgLayer::create();
	_bgLayer->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(_bgLayer, 0);

	// 初始化各个 Layer
	_sunLayer = SunLayer::create();
	_cardBarLayer = CardBarLayer::create();
	_controlLayer = ControlLayer::create();
	_plantLayer = PlantLayer::create();
	_zombieLayer = ZombieLayer::create();
	_bulletLayer = BulletLayer::create();
	_uiLayer = GameUILayer::create();

	// 按优先级添加
	this->addChild(_cardBarLayer, 10);
	
	this->addChild(_controlLayer, 30);
	_controlLayer->setName("ControlLayer");

	this->addChild(_plantLayer, 30);
	_plantLayer->setName("PlantLayer");

	this->addChild(_sunLayer, 40);
	_sunLayer->setName("SunLayer");
	
	this->addChild(_zombieLayer, 40);
	_zombieLayer->setName("ZombieLayer");

	this->addChild(_bulletLayer, 50);
	_bulletLayer->setName("BulletLayer");

	this->addChild(_uiLayer, 100);
}

void GameScene::bindLayerSignals()
{
	auto cardMgr = CardMgr::getInstance();

#if 0
	// 1. UI 层 -> 游戏逻辑（暂停/加速）
	if (_uiLayer) {
		_uiLayer->onPauseBtnClicked = [this](bool p) { onPause(p); };
		_uiLayer->onSpeedBtnClicked = [this](float s) { onSpeedChanged(s); };
	}

	// 2. 僵尸层 -> 游戏结束（失败/胜利）
	if (_zombieLayer) {
		_zombieLayer->onZombieReachHouse = [this]() { onZombieEnterHouse(); };
		_zombieLayer->onAllZombieDead = [this]() { onAllZombieClear(); };
	}

	// 3. 阳光层 -> 卡牌栏（显示阳光变化）
	if (_sunLayer && _cardBarLayer) {
		_sunLayer->onSunChanged = [this](int val) {
			auto mgr = CardMgr::getInstance();
			// 更新所有卡牌的可用状态
			for (size_t i = 0; i < mgr->getDeckSize(); ++i) {
				_cardBarLayer->updateCardState(i, mgr->canPlant(i));
			}
			CCLOG("GameScene: Sun changed to %d", val);
			};
	}

	// 4. 卡牌栏 -> 种植处理
	// CardMgr:: onCardSelected 被 CardBarLayer::onCardClicked 调用
	cardMgr->onCardSelected = [this](int cardIdx) {
		CCLOG("GameScene: Card [%d] selected", cardIdx);

		// TODO: 通知 PlantInputLayer 开始种植预览
		// 如果有 PlantInputLayer，应该在这里调用
		// if (_plantInput) {
		//     _plantInput->onCardSelected(cardIdx);
		// }

		// 种植完成后（在 PlantInputLayer 或其他地方调用）
		// CardMgr::getInstance()->onPlantConfirmed(cardIdx);
		};
#endif
}

void GameScene::onZombieEnterHouse()
{
	CCLOG("GameScene: zombie entered house -> fail");
	// Director::getInstance()->replaceScene(MenuScene::create());
}

void GameScene::onAllZombieClear()
{
	CCLOG("GameScene: all zombies cleared -> win");
	// Director::getInstance()->replaceScene(MenuScene:: create());
}

void GameScene::onPause(bool pause)
{
	if (pause) {
		CCLOG("Game paused");
		// 1. 暂停所有游戏逻辑相关的层
		// pause() 会停止该节点及其子节点的所有 Schedule 和 Action
		if (_bgLayer) _bgLayer->pause();
		if (_cardBarLayer) _cardBarLayer->pause();
		if (_sunLayer) _sunLayer->pauseAllSuns();
		if (_zombieLayer) _zombieLayer->pauseAllZombies();
		if (_plantLayer) _plantLayer->pauseAllPlants();
		if (_bulletLayer) _bulletLayer->pauseAllBullets();

		// 2. 特别注意：ControlLayer 必须暂停，否则玩家在暂停时还能点击地图种植物
		if (_controlLayer) _controlLayer->pause();

		// 3. 停止场景本身的 update (如果有的话)
		this->unscheduleUpdate();

		// 4.暂停游戏音乐
		AudioEngine::pause(bgmID);
	}
	else {
		CCLOG("Game resumed");
		// 恢复逻辑
		if (_bgLayer) _bgLayer->resume();
		if (_cardBarLayer) _cardBarLayer->resume();
		if (_sunLayer) _sunLayer->resumeAllSuns();
		if (_zombieLayer) _zombieLayer->resumeAllZombies();
		if (_plantLayer) _plantLayer->resumeAllPlants();
		if (_bulletLayer) _bulletLayer->resumeAllBullets();
		if (_controlLayer) _controlLayer->resume();

		this->scheduleUpdate();

		// 恢复游戏音乐
		AudioEngine::resume(bgmID);
	}
}

void GameScene::onSpeedChanged(float s)
{
	CCLOG("GameScene:  speed changed to %. 1fx", s);
	Director::getInstance()->getScheduler()->setTimeScale(s);
}