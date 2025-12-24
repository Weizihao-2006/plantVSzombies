#pragma once
#include "cocos2d.h"
#include "plant/PlantData.h"
#include <map>
#include<string>




struct BulletProperties {
    std::string texturePath;    // 正常飞行的图片
    std::string hitTexturePath; // 击中消失的图片
    float speed;                // 子弹速度
    bool isExplosive;           // 是否是爆炸类（如樱桃炸弹的火球，虽樱桃炸弹逻辑不同，但可扩展）

    BulletProperties() : texturePath(""), hitTexturePath(""), speed(450.0f), isExplosive(false) {}
    BulletProperties(std::string tex, std::string hitTex, float s)
        : texturePath(tex), hitTexturePath(hitTex), speed(s), isExplosive(false) {
    }
};

class BulletData {
public:
    static BulletProperties getBulletProps(PlantType type);
private:
    static std::map<PlantType, BulletProperties> _bulletConfigs;
    static void initConfigs();
};