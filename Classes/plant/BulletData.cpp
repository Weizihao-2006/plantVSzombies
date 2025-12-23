#include "plant\BulletData.h"

std::map<PlantType, BulletProperties> BulletData::_bulletConfigs;

void BulletData::initConfigs() {
    if (!_bulletConfigs.empty()) return;

    // 豌豆射手 & 双发射手
    _bulletConfigs[PlantType::PeaShooter] = BulletProperties("image/Pea.png", "image/PeaDisappear.png", 450.0f);

    // 寒冰射手
    _bulletConfigs[PlantType::SnowPea] = BulletProperties("image/SnowPeaBullet.png", "image/SnowPeaDisappear.png", 450.0f);

    // 以后可以扩展更多...
}

BulletProperties BulletData::getBulletProps(PlantType type) {
    initConfigs();
    if (_bulletConfigs.count(type)) 
        return _bulletConfigs[type];

    return BulletProperties("image/Pea.png", "image/PeaDisappear.png", 450.0f); // 默认保底
}