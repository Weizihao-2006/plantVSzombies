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

void Bullet::updateLogic(float dt, float speed) 
{
    if (_isExploding) 
        return;
    this->setPositionX(this->getPositionX() + speed * dt);
}

void Bullet::onHit() 
{
    if (_isExploding) 
        return;
    _isExploding = true;

    this->setTexture(_props.hitTexturePath);

    // ÑÓÊ±ÏûÊ§ÐòÁÐ
    auto delay = DelayTime::create(0.1f);
    auto remove = RemoveSelf::create();
    this->runAction(Sequence::create(delay, remove, nullptr));
}