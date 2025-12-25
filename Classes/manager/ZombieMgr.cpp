#include "manager/ZombieMgr.h"
#include "layer/ZombieLayer.h"
#include "manager/MapMgr.h"
#include "AudioEngine.h"

USING_NS_CC;

ZombieMgr* ZombieMgr::_instance = nullptr;

ZombieMgr* ZombieMgr::getInstance() {
    if (!_instance) {
        _instance = new ZombieMgr();
    }
    return _instance;
}

ZombieMgr::ZombieMgr() : _isLevelStarted(false), _currentWave(0), _waveTimer(0.0f) {}

void ZombieMgr::startLevel() {
    _isSpawningWave = false; // 初始化为没有在产生僵尸
    _currentWave = 0;
    _waveTimer = 0.0f;
    _nextWaveInterval = 10.0f; // 10秒后开始第一波
    _isLevelStarted = true;
    CCLOG("ZombieMgr: Level Started!");
}

void ZombieMgr::update(float dt) {
    if (!_isLevelStarted) return;

    _waveTimer += dt;

    int aliveCount = this->getAliveZombieCount();

    // CCLOG("aliveCount is %d", aliveCount);

    // --- 核心逻辑：衔接判定 ---
    // 只有在已经开始刷怪且场上没怪时，才强制结束当前等待并进入下一波
    if (!_isSpawningWave && aliveCount == 0 && _currentWave > 0) {
        // 时间大于渲染时间才算提前解干净
        if (_waveTimer > _waveDelay + 0.5f) {
            CCLOG("Wave %d solved in advance", _currentWave);
            if (_waveTimer < _nextWaveInterval) {
                _waveTimer = _nextWaveInterval; // 直接跳到时间点，让下面的逻辑统一处理
            }
        }
    }

    // 时间到了，进入下一波
    if (_waveTimer >= _nextWaveInterval) {
        if (!_isSpawningWave) {
            _waveTimer = 0.0f;
            _currentWave++;
            this->generateNextWave(_currentWave);
        }
    }
}

void ZombieMgr::generateNextWave(int waveIndex) {
    CCLOG("--- Wave %d Started ---", waveIndex);
    // 正在生成僵尸
    _isSpawningWave = true;

    // Zombies Are Coming音乐，在第一波播放
    if (waveIndex == 1)AudioEngine::play2d("Music/StartBGM.MP3", false, 1.0f);

    // 每 10 波触发一次大波 (Huge Wave)
    if (waveIndex % 10 == 0) {
        this->spawnHugeWave(waveIndex);
        _nextWaveInterval = 35.0f; // 大波后间隔长一些
        return;
    }

    // 普通波次：僵尸数量随波次缓慢增长
    int spawnCount = 1 + (waveIndex / 2);
    for (int i = 0; i < spawnCount; ++i) {
        _waveDelay = i * 2.0f; // 僵尸错开出现

        Director::getInstance()->getRunningScene()->scheduleOnce([this, waveIndex](float dt) {
            int row = rand() % 5;
            ZombieType type = this->getRandomZombieTypeByWave(waveIndex);
            this->spawnZombie(type, row);
            }, _waveDelay, "spawn_zombie_" + std::to_string(waveIndex) + "_" + std::to_string(i));
    }

    // 僵尸生成结束
    _isSpawningWave = false;
    
    CCLOG("Wave %d complete generating", _currentWave);
    // 动态调整下一波间隔（越往后节奏越快）
    // _nextWaveInterval = std::max(6.0f, 20.0f - (waveIndex * 0.4f));
}

void ZombieMgr::spawnHugeWave(int waveIndex) {
    CCLOG("A HUGE WAVE OF ZOMBIES IS COMING!");

    // 这里可以添加全局红色文字提示逻辑

    for (int row = 0; row < 5; ++row) {
        int numInRow = 2 + (rand() % 2); // 每行 2-3 只
        for (int i = 0; i < numInRow; ++i) {
            float delay = i * 1.0f + (rand() % 10) * 0.2f;
            Director::getInstance()->getRunningScene()->scheduleOnce([this, row, waveIndex](float dt) {
                ZombieType type = this->getRandomZombieTypeByWave(waveIndex + 5); // 大波怪更强
                this->spawnZombie(type, row);
                }, delay, "huge_wave_" + std::to_string(row) + "_" + std::to_string(i));
        }
    }
}
//这里稍微测试一下!!!!!!!!!!!!!!!!!!!!!!!!!!


ZombieType ZombieMgr::getRandomZombieTypeByWave(int waveIndex) {

#if 0
    int r = rand() % 100;
    if (waveIndex < 3) return ZombieType::Normal;
    if (waveIndex < 8) {
        return (r < 35) ? ZombieType::Conehead : ZombieType::Normal;
    }
    // 8波以后加入铁桶
    if (r < 20) return ZombieType::Buckethead;
    if (r < 50) return ZombieType::Conehead;
    return ZombieType::Normal;
#endif
    int r = rand() % 100;
    if (waveIndex < 3) return ZombieType::Giant;
    if (waveIndex < 8) {
        return (r < 35) ? ZombieType::Conehead : ZombieType::Normal;
    }
    // 8波以后加入铁桶
    if (r < 20) return ZombieType::Buckethead;
    if (r < 50) return ZombieType::Conehead;
    return ZombieType::Normal;
}

void ZombieMgr::spawnZombie(ZombieType type, int row) {
    auto scene = Director::getInstance()->getRunningScene();
    auto zLayer = dynamic_cast<ZombieLayer*>(scene->getChildByName("ZombieLayer"));
    if (!zLayer) return;

    Zombie* zombie = nullptr;
    if (type == ZombieType::Normal) zombie = CommonZombie::create();
    else if (type == ZombieType::Conehead) zombie = ConeheadZombie::create();
    else if (type == ZombieType::Buckethead) zombie = BucketheadZombie::create();
    else if (type == ZombieType::Giant) zombie = GiantZombie::create();
    if (zombie) {
        zombie->setRow(row);
        float y = MapManager::getInstance()->getPositionInMap(row, 0).y - 70;
        float x = Director::getInstance()->getWinSize().width + (rand() % 50); // 屏幕外随机偏移
        zombie->setPosition(Vec2(x, y));
        zombie->setScale(1.5f);

        zLayer->addChild(zombie);
        zLayer->getAllZombies().pushBack(zombie);
    }
}

int ZombieMgr::getAliveZombieCount() {
    auto scene = Director::getInstance()->getRunningScene();
    auto layer = dynamic_cast<ZombieLayer*>(scene->getChildByName("ZombieLayer"));
    if (layer) {
        return (int)layer->getAllZombies().size();
    }
    return 0;
}

void ZombieMgr::reset() {
    if (_instance) {
        delete _instance;
        _instance = nullptr;
    }
    CCLOG("ZombieMgr Reset Complete.");
}