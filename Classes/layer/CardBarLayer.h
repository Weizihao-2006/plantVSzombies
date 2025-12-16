#pragma once
#include <vector>
#include"cocos2d.h"
class CardBarLayer : public cocos2d::Layer 

{
public:
    static CardBarLayer* createWithFixedDeck(const std::vector<int>& deck);
    bool initWithFixedDeck(const std::vector<int>& deck);
private:

    //µ÷ÓÃ CardMgrµÄonCardSelected(idx)
    void onCardClicked(int idx);
    cocos2d::Menu* _menu = nullptr;
    std::vector<cocos2d::ProgressTimer*> _cdBars;
};