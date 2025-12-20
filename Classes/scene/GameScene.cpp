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
#include "util/Global.h"
#include "cocos2d.h"
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
	// _zombieLayer = ZombieLayer::create();
	// _bulletLayer = BulletLayer::create();
	 _uiLayer = GameUILayer::create();

	// 按优先级添加
	this->addChild(_sunLayer, 10);
	_sunLayer->setName("SunLayer");
	this->addChild(_cardBarLayer, 20);
	_controlLayer->setName("ControlLayer");
	this->addChild(_controlLayer, 30);
	this->addChild(_plantLayer, 30);
	_plantLayer->setName("PlantLayer");
	// this->addChild(_zombieLayer, 40);
	// this->addChild(_bulletLayer, 50);
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
		Director::getInstance()->pause();
	}
	else {
		CCLOG("Game resumed");
		Director::getInstance()->resume();
	}
}

void GameScene::onSpeedChanged(float s)
{
	CCLOG("GameScene:  speed changed to %. 1fx", s);
	Director::getInstance()->getScheduler()->setTimeScale(s);
}