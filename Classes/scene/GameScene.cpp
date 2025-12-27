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
#include "util/LevelConfig.h"
#include "MainMenu.h"
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

	// 初始化按钮文字
	Director::getInstance()->getScheduler()->setTimeScale(1.0f);

	// 先重置所有manager
	Global::getInstance()->reset();
	CardMgr::getInstance()->reset();
	MapManager::getInstance()->reset();
	ZombieMgr::getInstance()->reset();
	PlantMgr::getInstance()->reset();

	// 背景音乐
	bgmID = AudioEngine::play2d("Music/GameSceneBGM.MP3", true, 1.0f);

	if (!Scene::init())
		return false;

	_levelID = level_id;

	AnimationHelper::initResources();//加载动画资源
	createLayers();

	
	// 启动僵尸管理器
	auto Mode = Global::getInstance()->getGameMode();
	ZombieMgr::getInstance()->startLevel(Mode, level_id);

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
	// 关键绑定：当僵尸进屋时，调用 GameScene 的处理函数
	_zombieLayer->onZombieReachHouse = CC_CALLBACK_0(GameScene::onZombieEnterHouse, this);

	this->addChild(_bulletLayer, 50);
	_bulletLayer->setName("BulletLayer");

	this->addChild(_uiLayer, 100);
	_uiLayer->setName("GameUILayer");

}



void GameScene::onZombieEnterHouse()
{
	CCLOG("GameScene: zombie entered house -> fail");
	// 1. 停止所有更新
	this->onPause(true);

	ZombieMgr::getInstance()->setIsLevelStarted(false); // 停止出怪逻辑

	// 2. 停止背景音乐，播放失败音效
	AudioEngine::stop(bgmID); 
	AudioEngine::play2d("Music/loosemusic.ogg", false, 1.0f);

	// 3. 创建半透明遮罩背景
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto mask = LayerColor::create(Color4B(0, 0, 0, 180));
	this->addChild(mask, 200);

	// 4. 显示“僵尸吃掉了你的脑子”图片
	auto gameOverSprite = Sprite::create("gameOver.png");
	gameOverSprite->setScale(1.5f);
	gameOverSprite->setPosition(visibleSize.width / 2, visibleSize.height / 2 + 50);
	mask->addChild(gameOverSprite);

	// 5. 创建“重新开始”按钮
	auto restartItem = MenuItemImage::create("Button/btn_Restart.png", "Button/btn_Restart.png",
		[this](Ref* sender) {
			// 重新加载当前关卡
			auto newScene = GameScene::createWithLevel(this->_levelID);
			Director::getInstance()->replaceScene(newScene);
		});

	// 6. 创建“返回菜单”按钮
	auto menuItem = MenuItemImage::create("Button/btn_Back.png", "Button/btn_Back.png",
		[](Ref* sender) {
			// 假设你的菜单场景叫 MenuScene
			Director::getInstance()->replaceScene(MainMenu::create());
			CCLOG("Back to Menu");
		});

	// 7. 组合成菜单并添加到遮罩层
	auto menu = Menu::create(restartItem, menuItem, nullptr);
	restartItem->setScale(2.0f);
	menuItem->setScale(2.0f);
	menu->alignItemsHorizontallyWithPadding(50); // 横向排列，间隔 50
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 450);
	mask->addChild(menu);
}

void GameScene::onAllZombieClear()
{
	CCLOG("GameScene: all zombies cleared -> win");

	// 1. 停止游戏逻辑与计时器
	this->onPause(true);
	AudioEngine::play2d("Music/winmusic.ogg"); // 播放胜利音效

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto center = Vec2(visibleSize.width / 2, visibleSize.height / 2);

	// 2. 创建黑色半透明遮罩层
	auto mask = LayerColor::create(Color4B(0, 0, 0, 180));
	this->addChild(mask, 1000);

	// 3. 判定是否通关
	bool isGameFinished = (_levelID >= Global::getInstance()->getMaxLevel());

	// --- 处理通关奖励效果 ---
	if (isGameFinished) {
		// 恭喜通关标签
		auto winLabel = Label::createWithTTF("CONGRATULATIONS! ALL LEVELS CLEAR", "fonts/Marker Felt.ttf", 45);
		winLabel->setColor(Color3B::YELLOW);
		winLabel->setPosition(center.x, center.y + 150);
		mask->addChild(winLabel);

		auto trophy = Sprite::create("trophy.png");
		if (trophy) {
			trophy->setPosition(center.x, visibleSize.height + 100);
			mask->addChild(trophy);
			// 掉落并弹跳动画
			auto drop = EaseBounceOut::create(MoveTo::create(1.5f, Vec2(center.x, center.y + 50)));
			trophy->runAction(drop);
		}
	}
	else {
		// 还没通关，显示普通胜利提示
		auto levelClearLabel = Label::createWithTTF("LEVEL CLEAR!", "fonts/Marker Felt.ttf", 60);
		levelClearLabel->setPosition(center.x, center.y + 100);
		mask->addChild(levelClearLabel);
	}

	// 4. 创建文字按钮
	// 下一关 或 重新开始 按钮
	std::string primaryText = isGameFinished ? "RESTART FROM LEVEL 1" : "NEXT LEVEL";
	int NextLevel = _levelID + 1;
	auto primaryLabel = Label::createWithTTF(primaryText, "fonts/Marker Felt.ttf", 35);
	auto primaryItem = MenuItemLabel::create(primaryLabel, [this, isGameFinished, NextLevel](Ref* p) {
		// Director::getInstance()->getScheduler()->setTimeScale(1.0f); // 恢复时间流逝
		int targetLevel = isGameFinished ? 1 : NextLevel;
		Director::getInstance()->replaceScene(GameScene::createWithLevel(targetLevel));
		});

	// 返回主菜单按钮
	auto exitLabel = Label::createWithTTF("BACK TO MENU", "fonts/Marker Felt.ttf", 35);
	exitLabel->setColor(Color3B::GRAY);
	auto exitItem = MenuItemLabel::create(exitLabel, [](Ref* p) {
		// Director::getInstance()->getScheduler()->setTimeScale(1.0f);
		Director::getInstance()->replaceScene(MainMenu::createScene());
		});

	// 5. 组合菜单并添加
	auto menu = Menu::create(primaryItem, exitItem, nullptr);
	menu->alignItemsVerticallyWithPadding(40); // 纵向排列
	menu->setPosition(center.x, center.y - 80);
	mask->addChild(menu);
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
		// 停止出怪逻辑
		ZombieMgr::getInstance()->setIsLevelStarted(false);

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
		// 恢复出怪逻辑
		ZombieMgr::getInstance()->setIsLevelStarted(true);

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

	// 1. 创建遮罩
	auto mask = LayerColor::create(Color4B(0, 0, 0, 50));
	this->addChild(mask, 200);

	// --- 核心修复：添加触摸拦截逻辑 ---
	auto listener = EventListenerTouchOneByOne::create();
	// 设置为吞噬触摸，这样点击就不会传给底层的按钮
	listener->setSwallowTouches(true);

	listener->onTouchBegan = [](Touch* touch, Event* event) {
		// 返回 true 表示我们要消耗掉这个触摸
		return true;
		};

	// 将监听器绑定到 mask 上
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, mask);

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
	ZombieMgr::getInstance()->startLevel(Global::getInstance()->getGameMode(), _levelID);

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

void GameScene::showHugeWaveAlert()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto alert = Sprite::create("hugewave.png"); // 替换为你的大波提示图片名
	alert->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	alert->setScale(0.1f);
	alert->setOpacity(0);
	this->addChild(alert, 200);

	// 动作：渐显放大 -> 停留 -> 缩小消失
	auto action = Sequence::create(
		Spawn::create(FadeIn::create(0.3f), ScaleTo::create(0.3f, 1.0f), nullptr),
		DelayTime::create(2.0f),
		Spawn::create(FadeOut::create(0.3f), ScaleTo::create(0.3f, 1.5f), nullptr),
		RemoveSelf::create(),
		nullptr
	);
	alert->runAction(action);
}

void GameScene::showLastWaveAlert()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);

	// 创建两张图，初始隐藏
	auto img1 = Sprite::create("hugewave.png"); // 比如 "Final"
	auto img2 = Sprite::create("lastwave.png"); // 比如 "Wave"

	img1->setPosition(center); img1->setVisible(false);
	img2->setPosition(center); img2->setVisible(false);

	this->addChild(img1, 200);
	this->addChild(img2, 200);

	// 定义单张图展示动作
	auto showImg = [](Sprite* sp) {
		return Sequence::create(
			Show::create(),
			ScaleTo::create(0.2f, 1.2f),
			ScaleTo::create(0.1f, 1.0f),
			DelayTime::create(1.0f),
			Hide::create(),
			nullptr
		);
		};

	// 顺序执行：第一张播完，紧接着第二张
	img1->runAction(Sequence::create(
		showImg(img1),
		CallFunc::create([img2, showImg]() {
			img2->runAction(Sequence::create(showImg(img2), RemoveSelf::create(), nullptr));
			}),
		RemoveSelf::create(),
		nullptr
	));
}