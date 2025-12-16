#ifndef PLANT_INPUT_LAYER_H
#define PLANT_INPUT_LAYER_H

#include "cocos2d.h"

class PlantInputLayer : public cocos2d::Layer {
public:
    CREATE_FUNC(PlantInputLayer);
    bool init() override;
    void onCardSelected(int plantId);

private:
    int _selectedId = -1; // 当前选中的植物 ID
    cocos2d::Sprite* _ghost = nullptr; // 植物预览
    cocos2d::EventListenerTouchOneByOne* _touchListener;

    cocos2d::Vec2 getRowCol(const cocos2d::Vec2& worldPos);
};

#endif // PLANT_INPUT_LAYER_H
