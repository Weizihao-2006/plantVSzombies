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

	// Zombies Are Coming音乐
	AudioEngine::play2d("Music/StartBGM.MP3", false, 1.0f);
	// 背景音乐
	bgmID = AudioEngine::play2d("Music/GameSceneBGM.MP3", true, 1.0f);

	if (!Scene::init())
		return false;

	_levelID = level_id;
	Global::getInstance()->setLevelID(level_id);


	AnimationHelper::initResources();//加载动画资源
	createLayers();

	// 初始化 CardMgr,确实已经初始化了,写在构造函数了
	CardMgr::getInstance();

	// 启动僵尸管理器
	ZombieMgr::getInstance()->startLevel();

	// 播放开始动画
	this->playStartAnimation();

	// 添加每帧更新
	scheduleUpdate();

	//// 单僵尸测试
	//this->scheduleOnce([this](float dt) {
	//	CCLOG("Test: Spawning a zombie!");
	//	// 注意：如果你还没写 ZombieMgr，可以先手动测试
	//	// 确保你已经把 ZombieMgr.h 包含在 GameScene.cpp 头部
	//	ZombieMgr::getInstance()->spawnZombie(ZombieType::Buckethead, 0);
	//	}, 3.0f, "test_zombie_spawn");

	return true;
}



// 每帧更新
void GameScene::update(float dt) {
	// 调用僵尸管理器的 update，它内部会处理：
	// 1. 计时刷新波次
	// 2. 检查场上僵尸是否死光（没怪不傻等）
	// 3. 产生大波预警
	ZombieMgr::getInstance()->update(dt);
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
	
	
	this->addChild(_plantLayer, 30);
	_plantLayer->setName("PlantLayer");

	this->addChild(_controlLayer, 35);
	_controlLayer->setName("ControlLayer");

	this->addChild(_sunLayer, 40);
	_sunLayer->setName("SunLayer");
	
	this->addChild(_zombieLayer, 40);
	_zombieLayer->setName("ZombieLayer");

	this->addChild(_bulletLayer, 50);
	_bulletLayer->setName("BulletLayer");

	this->addChild(_uiLayer, 100);
	_uiLayer->setName("GameUILayer");

	
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

void GameScene::playStartAnimation() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);

	// --- 新增：半透明黑色遮罩层 ---
	auto mask = LayerColor::create(Color4B(0, 0, 0, 50)); // 150 为透明度
	// 放在所有界面之上
	this->addChild(mask, 110);

	// 1. 创建精灵
	auto ready = Sprite::create("ready.png");
	auto set = Sprite::create("set.png");
	auto start = Sprite::create("start.png");

	// ... 初始设置 (ready, set, start) ...
	// 初始都设为不可见且缩放为 0
	ready->setOpacity(0); ready->setScale(0); ready->setPosition(center);
	set->setOpacity(0);   set->setScale(0);   set->setPosition(center);
	start->setOpacity(0); start->setScale(0); start->setPosition(center);

	this->addChild(ready, 100);
	this->addChild(set, 100);
	this->addChild(start, 100);

	// 2. 定义动画
	auto createAnimate = [](Sprite* sprite, float delayTime) {
		return Sequence::create(
			DelayTime::create(delayTime),
			Spawn::create(FadeIn::create(0.2f), ScaleTo::create(0.2f, 1.2f), nullptr),
			ScaleTo::create(0.1f, 1.0f),
			DelayTime::create(0.8f),
			Spawn::create(FadeOut::create(0.2f), ScaleTo::create(0.2f, 1.5f), nullptr),
			RemoveSelf::create(),
			nullptr
		);
		};

	// 3. 运行图片动画
	ready->runAction(createAnimate(ready, 0.0f));
	set->runAction(createAnimate(set, 1.2f));

	// 最后一个动画加一个特殊处理：结束后移除遮罩并启动游戏
	start->runAction(Sequence::create(
		createAnimate(start, 2.4f),
		CallFunc::create([this, mask]() {
			mask->removeFromParent(); // 移除遮罩
			this->startGameLogic();   // 统一启动游戏逻辑
			}),
		nullptr
	));
}

// 新增一个统一启动函数
void GameScene::startGameLogic() {
	// 1. 启动僵尸生成
	ZombieMgr::getInstance()->startLevel();

	// 2. 启动阳光产生（假设 SunLayer 有这个方法）
	auto sunLayer = dynamic_cast<SunLayer*>(getChildByName("SunLayer"));
	if (sunLayer) {
		// 你需要在 SunLayer 里实现 startGenerateSun，而不是在它的 init 里直接跑
		sunLayer->startSunLogic();
	}

	// 3. 启动场景更新
	this->scheduleUpdate();

	CCLOG("Game Logic Started after Animation.");
}