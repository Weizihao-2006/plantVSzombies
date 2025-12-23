#include "Bullet.h"

USING_NS_CC;

Bullet* Bullet::createWithConfig(int row, float damage, PlantType type) 
{
   
    auto bullet = new (std::nothrow) Bullet();
    if (bullet) {
        bullet->_props = BulletData::getBulletProps(type);
    }
    
    if (bullet && bullet->initWithFile(bullet->_props.texturePath)) {
        bullet->_row = row;
        bullet->_damage = damage;
        bullet->_originType = type;
        bullet->setScale(1.5f);
        bullet->autorelease();
        return bullet;
    }
    CC_SAFE_DELETE(bullet);
    return nullptr;
}

bool Bullet::init() {
    return true;
}
