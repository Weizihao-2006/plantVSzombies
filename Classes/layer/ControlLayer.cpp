#include "ControlLayer.h"
#include"scene/GameScene.h"
#include "manager/PlantMgr.h"
#include"manager/CardMgr.h"
#include"GameUILayer.h"
#include <string>
#include "AudioEngine.h"

using namespace cocos2d;
USING_NS_CC;

const PlantType ControlLayer::SelectNoPlant = PlantType::Error;



bool ControlLayer::init()
{
    if (!Layer::init())
        return false;


    this->setPosition(Vec2::ZERO);


    // 订阅 CardMgr 的选择事件,就是说一旦卡槽按钮被点击,会自动调用
    CardMgr::getInstance()->onCardSelected = [this](PlantType type) {
        // 这里不需要检查阳光和冷却，因为 CardMgr 已经检查过了
        this->setSelectedPlantId(type);
        CCLOG("ControlLayer: Received PlantType %d from CardMgr", static_cast<int>(type));
        };

    _mapManager = MapManager::getInstance();
    createTouchListener(); // 创建触摸和鼠标监听
    return true;
}


void ControlLayer::createTouchListener() {
    // 1. 触摸监听器：处理触摸屏输入或鼠标拖动（按下时）
    auto touchListener = EventListenerTouchOneByOne::create();

    // 触摸开始 (按下)
    touchListener->onTouchBegan = [this](Touch* touch, Event* event) {
        //世界坐标
        _cur = touch->getLocation();
        // 判定条件：如果没有选中植物，或者点在了地图外，或者点在了不可种植的地方
        bool isPlantingMode = (_selectedPlantType != SelectNoPlant);
        bool isInMap = _mapManager->judgeScreenPositionIsInMap(_cur);//这个判断不会修改_cur!!!

        // 只有当“不在种植模式”或者“在地图外”时，优先判定阳光
        // 这样可以保证你手里抓着植物时，点击地图依然是种植
        if (!isPlantingMode || !isInMap) {
            auto gameScene = dynamic_cast<GameScene*>(this->getScene());
            if (gameScene && gameScene->getSunLayer()) {
                if (gameScene->getSunLayer()->containsAndCollectSun(_cur)) {
                    return true; // 捡起阳光成功，中断事件，不执行后续种植判断
                }
            }
        }
        return false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


    // 2. 鼠标监听器：处理桌面端鼠标的非按下移动 (幽灵跟随) 和点击 (种植)
    auto mouseListener = EventListenerMouse::create();

    // 鼠标移动 (未按下时)
    mouseListener->onMouseMove = [this](EventMouse* event) {
        _cur = event->getLocation(); // 获取鼠标当前位置

        if (event->getMouseButton() != EventMouse::MouseButton::BUTTON_UNSET) 
            return;

        if (_selectedPlantType != SelectNoPlant&& CardMgr::getInstance()->IsNotInCoolAndCanAfford(_selectedPlantType)) {

            //如果幽灵精灵还未创建，现在创建它！
            if (!_plantPreview) {
                auto props = PlantData::getProps(_selectedPlantType);
                _plantPreview = createPreview(props.previewFrame, 1.5f);  
            }
            handlePreviewLogic(_plantPreview, _isPlantPreviewShowing, _mapManager->judgeScreenPositionIsCanPlant(_cur));
        }
        else if ( _isShovelSelected) {

            //创建或显示铲子预览
            if (!_shovelPreview) {
                _shovelPreview = createPreview("Shovel.png", 1.0f, 255);
                _shovelPreview->setRotation(-45.0f);
            }
            auto gameUi = dynamic_cast<GameUILayer*>(Director::getInstance()->getRunningScene()->getChildByName("GameUILayer"));
            if (gameUi) {
                gameUi->setShoveButtonVisible(false);
            }
            handlePreviewLogic(_shovelPreview, _isShovelPreviewShowing,_mapManager->judgeScreenPositionIsInMap(_cur));
        }
        };


    // 鼠标按下 (用于在幽灵模式下点击种植)
    mouseListener->onMouseDown = [this](EventMouse* event) 
    {
            //右键取消
            if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT) {
                auto gameUi = dynamic_cast<GameUILayer*>(Director::getInstance()->getRunningScene()->getChildByName("GameUILayer"));
                if (gameUi) {
                    gameUi->setShoveButtonVisible(true);
                }
                cancelCurrentAction();
                return;
            }

            if (event->getMouseButton() != EventMouse::MouseButton::BUTTON_LEFT)
                return;

            _cur = event->getLocation();

            if (_selectedPlantType != SelectNoPlant) {
                if (_mapManager->judgeScreenPositionIsCanPlant(_cur) &&
                    CardMgr::getInstance()->IsNotInCoolAndCanAfford(_selectedPlantType)) {
                    //先更新坐标
                    _MapPosition = _mapManager->convertScreenPosToMapPos(_cur);

                    // 通知PlantMgr实际创建植物,同时会更新地图系统的植物以及CardMgr的冷却
                    PlantMgr::getInstance()->createPlantAt(_MapPosition, _selectedPlantType);

                    //播放种植音乐
                    AudioEngine::play2d("Music/plant.ogg", false, 1.0f);
                    cancelCurrentAction();
                }
            }
            else if (_isShovelSelected) {

                if (_mapManager->judgeScreenPositionIsInMap(_cur)) {
                    _MapPosition = _mapManager->convertScreenPosToMapPos(_cur);

                    //移除植物
                    bool success = PlantMgr::getInstance()->removePlantAt(_MapPosition);

                    if (success) {
                        AudioEngine::play2d("Music/Shovel.ogg", false, 1.0f);
                        
                    }
                    auto gameUi = dynamic_cast<GameUILayer*>(Director::getInstance()->getRunningScene()->getChildByName("GameUILayer"));
                    if (gameUi) {
                        gameUi->setShoveButtonVisible(true);
                    }
                    cancelCurrentAction(); 
                }
            }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}


Sprite* ControlLayer::createPreview(const std::string& filename,float scale, uint8_t opacity)
{
    auto preview = Sprite::create(filename);
    if (preview) {
        preview->setOpacity(opacity); // 统一半透明
        preview->setScale(scale); // 统一缩放（如 1.5f）
        this->addChild(preview, 15); // 确保在顶层
        preview->setVisible(false);
    }
    return preview;
}
void ControlLayer::handlePreviewLogic(Sprite*& targetSprite, bool& isShowingFlag, bool canShowCondition) {
    // 1. 如果不满足基本条件（比如冷却中或鼠标超出），直接隐藏并清理
    if (!canShowCondition) {
        if (targetSprite) {
            hidePreview(targetSprite, isShowingFlag);
        }
        clearHighlightBars();
        return;
    }

    // 2. 满足条件，开始更新
    _MapPosition = _mapManager->convertScreenPosToMapPos(_cur);

    updatePreviewPosition(targetSprite); // 复用你之前的函数
    showPreview(targetSprite, isShowingFlag);
    updateHighlightBars(_MapPosition.y, _MapPosition.x);
}

void ControlLayer::cancelCurrentAction() 
{
    // 1. 隐藏并清理植物预览
    if (_plantPreview) {
        hidePreview(_plantPreview, _isPlantPreviewShowing);
        _plantPreview->removeFromParent(); // 彻底移除，配合你逻辑中的 nullptr 设置
        _plantPreview = nullptr;
    }
    // 2. 隐藏并清理铲子预览
    if (_shovelPreview) {
        hidePreview(_shovelPreview, _isShovelPreviewShowing);
        _shovelPreview->removeFromParent();
        _shovelPreview = nullptr;
    }

    // 3. 状态重置
    _selectedPlantType = SelectNoPlant;
    _isShovelSelected = false;

    // 4. 视觉清理
    clearHighlightBars();
}

void ControlLayer::setShovelActive(bool active) 
{
    _isShovelSelected = active;

    if (_isShovelSelected) {
        // 1. 取消植物选择
        this->setSelectedPlantId(SelectNoPlant);
       
    }
    else {
        if (_shovelPreview) 
            _shovelPreview->setVisible(false);
    }
}

// 设置选中的植物ID（从卡片点击事件调用）
void ControlLayer::setSelectedPlantId(PlantType plantId) {
    _selectedPlantType = plantId;

    if (plantId == SelectNoPlant) {
        hidePreview(_plantPreview,_isPlantPreviewShowing);
    }

   
}


void ControlLayer::updateHighlightBars(int row, int col) {
    // 1. 延迟初始化 DrawNode
    if (!_highlightRow) {
        _highlightRow = DrawNode::create();
        this->addChild(_highlightRow, 5);
    }
    if (!_highlightCol) {
        _highlightCol = DrawNode::create();
        this->addChild(_highlightCol, 5);
    }

    _highlightRow->clear();
    _highlightCol->clear();

    // 2. 设置高亮颜色：半透明白色
    Color4F highlightColor(1.0f, 1.0f, 1.0f, 0.2f);

    // 3. 通过 Manager 获取格子中心点，不再直接访问全局数组
    Vec2 center = _mapManager->getPositionInMap(row, col);

    // 4. 从 Manager 获取地图边界参数
    float rowLeft = _mapManager->getMapLeft();
    float rowRight = _mapManager->getMapRight();
    float colBottom = _mapManager->getMapBottom();
    float colTop = _mapManager->getMapTop();

    float barSize = 140.0f; // 高亮条的厚度

    // --- 绘制横条 (Row) ---
    _highlightRow->drawSolidRect(
        Vec2(rowLeft, center.y - barSize / 2),
        Vec2(rowRight, center.y + barSize / 2),
        highlightColor
    );

    // --- 绘制竖条 (Col) ---
    _highlightCol->drawSolidRect(
        Vec2(center.x - barSize / 2, colBottom),
        Vec2(center.x + barSize / 2, colTop),
        highlightColor
    );
}

// 隐藏高亮条
void ControlLayer::clearHighlightBars() {
    if (_highlightRow) _highlightRow->clear();
    if (_highlightCol) _highlightCol->clear();
}



void ControlLayer::showPreview(Sprite* preview,bool& isShowing) {
    if (preview && !isShowing) {
        preview->setVisible(true);
        isShowing = true;
    }
}

void ControlLayer::hidePreview(Sprite* preview, bool& isShowing) {
    // 仅隐藏，不移除精灵，以便下次移动时可以立即显示
    if (preview && isShowing) {
        preview->setVisible(false);
        isShowing = false;
    }
}



// 更新预览位置到当前触摸/鼠标位置
void ControlLayer::updatePreviewPosition(cocos2d::Sprite* targetSprite) {
    if (!targetSprite) 
        return;
    int row = _MapPosition.y;
    int col = _MapPosition.x;
    targetSprite->setPosition(_mapManager->getPositionInMap(row,col));

}



