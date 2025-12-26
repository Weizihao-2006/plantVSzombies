#include "manager/ZombieMgr.h"
#include "layer/ZombieLayer.h"
#include "manager/MapMgr.h"
#include "AudioEngine.h"
#include "util/Global.h"
#include "scene/GameScene.h"
#include "climits"

USING_NS_CC;

ZombieMgr* ZombieMgr::_instance = nullptr;

ZombieMgr* ZombieMgr::getInstance() {
    if (!_instance) {
        _instance = new ZombieMgr();
    }
    return _instance;
}

ZombieMgr::ZombieMgr() : _isLevelStarted(false), _currentWave(0), _waveTimer(0.0f) {}

void ZombieMgr::startLevel(GameMode mode, int levelId) {
    _currentMode = mode;
    _currentWave = 0;
    _waveTimer = 0.0f;
    _isLevelStarted = true;

    if (_currentMode == GameMode::ADVENTURE) {
        // 从配置表中直接获取数据
        auto& allLevels = LevelConfig::getLevels();
        if (allLevels.find(levelId) != allLevels.end()) {
            _currentLevelData = allLevels.at(levelId);
            CCLOG("Loaded Adventure Level: %d, Total Waves: %d", levelId, _currentLevelData.totalWaves);
        }
    }
}

void ZombieMgr::update(float dt) {
    if (!_isLevelStarted) return; // 暂停或结束时，dt 不会被计算到逻辑中

    _waveTimer += dt;

    // --- 逻辑 A: 处理待生成队列 ---
    auto it = _pendingZombies.begin();
    while (it != _pendingZombies.end()) {
        it->delay -= dt;
        if (it->delay <= 0) {
            this->spawnZombie(it->type, it->row);
            it = _pendingZombies.erase(it);
        }
        else {
            ++it;
        }
    }

    // --- 逻辑 B: 获取当前状态判定 ---
    int aliveCount = this->getAliveZombieCount();
    int totalWaves = (_currentMode == GameMode::ADVENTURE) ? _currentLevelData.totalWaves : INT_MAX;

    // --- 逻辑 C: 判定全关胜利 (仅限冒险模式) ---
    if (_currentMode == GameMode::ADVENTURE && _currentWave >= totalWaves) {
        // 只有队列空了（怪出完了）且 场上没怪了，才算赢
        if (_pendingZombies.empty() && aliveCount == 0 && _waveTimer > _waveDelay + 0.5f) {
            _isLevelStarted = false;
            auto scene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
            if (scene) scene->onAllZombieClear();
            return;
        }
    }

    // --- 逻辑 D: 自动进入下一波的衔接 ---
    // 条件：队列空了（当前波次出完了）且 场上没怪了（或者时间到了）
    if (_pendingZombies.empty() && aliveCount == 0 && _currentWave > 0 && _currentWave < totalWaves) {
        // 强制缩短等待时间，让下一波快点来
        if (_waveTimer < _nextWaveInterval) {
            _waveTimer = _nextWaveInterval;
        }
    }

    // --- 逻辑 E: 触发下一波生成 ---
    if (_currentWave < totalWaves && _waveTimer >= _nextWaveInterval) {
        _waveTimer = 0.0f;
        _currentWave++;

        if (_currentMode == GameMode::ADVENTURE) {
            this->generateNextWave(_currentWave);
        }
        else {
            this->EndlessNextWave(_currentWave);
        }
    }
}

// 冒险模式的下一波
void ZombieMgr::generateNextWave(int waveIndex) {
    // 判定是否超过总波次
    if (waveIndex > _currentLevelData.totalWaves) return;

    // Zombies Are Coming音乐，在第一波播放
    if (waveIndex == 1)AudioEngine::play2d("Music/StartBGM.MP3", false, 1.0f);

    // 获取当前波次的配置
    auto& waveConfig = _currentLevelData.waves[waveIndex];

    // --- 新增：根据波次类型播放提示 ---
    if (waveConfig.type == WaveType::HUGE_WAVE) {
        AudioEngine::play2d("Music/hugewave.ogg", false, 1.0f);
        // 这里可以调用 GameScene 的方法显示 "A Huge Wave of Zombies is Approaching!" 动画
        // 假设你在 GameScene 有一个 showHugeWaveAlert()
        auto scene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
        scene->showHugeWaveAlert();
    }
    else if (waveConfig.type == WaveType::FINAL_WAVE) {
        AudioEngine::play2d("Music/hugewave.ogg", false, 1.0f);
        auto scene = dynamic_cast<GameScene*>(Director::getInstance()->getRunningScene());
        scene->showLastWaveAlert();
    }

    int totalInWave = 0;
    for (auto& g : waveConfig.groups) totalInWave += g.count;

    float baseDelay = 0.0f;
    for (auto& group : waveConfig.groups) {
        for (int i = 0; i < group.count; ++i) {
            PendingZombie p;
            p.type = group.type;
            p.row = rand() % 5;
            p.delay = baseDelay; // 记录相对延迟
            _pendingZombies.push_back(p);

            baseDelay += 2.0f; // 僵尸之间的间隔
        }
    }
    _waveDelay = baseDelay;
}

void ZombieMgr::EndlessNextWave(int waveIndex)
{
    CCLOG("--- Wave %d Started ---", waveIndex);

    // Zombies Are Coming音乐，在第一波播放
    if (waveIndex == 1)AudioEngine::play2d("Music/StartBGM.MP3", false, 1.0f);

    int count = 1 + waveIndex / 2; // 无尽模式增长公式
    
    float baseDelay = 0.0f;
    for (int i = 0; i < count; ++i) {
        PendingZombie p;
        p.type = getRandomZombieTypeByWave(waveIndex);
        p.row = rand() % 5;
        p.delay = baseDelay;
        _pendingZombies.push_back(p);

        baseDelay += 2.0f;
    }
    _waveDelay = baseDelay;

    // 僵尸生成结束
    CCLOG("Wave %d complete generating", _currentWave);
}

void ZombieMgr::spawnHugeWave(int waveIndex) {
    CCLOG("A HUGE WAVE OF ZOMBIES IS COMING!");

    // 这里可以添加全局红色文字提示逻辑

    float delay;
    for (int row = 0; row < 5; ++row) {
        int numInRow = 2 + (rand() % 2); // 每行 2-3 只
        for (int i = 0; i < numInRow; ++i) {
            delay = i * 1.0f + (rand() % 10) * 0.2f;
            Director::getInstance()->getRunningScene()->scheduleOnce([this, row, waveIndex](float dt) {
                ZombieType type = this->getRandomZombieTypeByWave(waveIndex + 5); // 大波怪更强
                this->spawnZombie(type, row);
                }, delay, "huge_wave_" + std::to_string(row) + "_" + std::to_string(i));
        }
    }
    _waveDelay = delay;
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
    
    Zombie* zombie = nullptr;
    if (type == ZombieType::Normal) zombie = CommonZombie::create();
    else if (type == ZombieType::Conehead) zombie = ConeheadZombie::create();
    else if (type == ZombieType::Buckethead) zombie = BucketheadZombie::create();
    else if (type == ZombieType::Giant) zombie = GiantZombie::create();
    if (zombie) {
        zombie->setRow(row);
        float y;
        if (type == ZombieType::Giant) {
            y = MapManager::getInstance()->getPositionInMap(row, 0).y-200.f;
        }
        else {
            y = MapManager::getInstance()->getPositionInMap(row, 0).y - 70;
        }
       
        float x = Director::getInstance()->getWinSize().width + (rand() % 50); // 屏幕外随机偏移
        zombie->setPosition(Vec2(x, y));
        zombie->setScale(1.5f);

        addZombie(zombie);
#if 0
        zombie->setDebugRectVisible(true);
#endif
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
    _isLevelStarted = false;
    _currentWave = 0;
    _waveTimer = 0.0f;
    _pendingZombies.clear(); // 核心：清理队列
    CCLOG("ZombieMgr Reset Complete.");
}