#include "MainMenu.h"
#include"scene/GameScene.h"
#include "AudioEngine.h"
USING_NS_CC;

Scene* MainMenu::createScene()
{
    return MainMenu::create();
}

// 当文件不存在时，打印有用的错误信息而非直接段错误崩溃。
static void problemLoading(const char* filename)
{
    printf("加载时出错：%s\n", filename);
}

bool MainMenu::init()
{
    // 优先调用基类的初始化函数
    if (!Scene::init())
    {
        return false;
    }

    // 获得分辨率尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();
    // 获取显示偏移(windows默认为0)
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 播放背景音乐
    int MainMenuBgmID = AudioEngine::play2d("Music/MainMenuBGM.MP3", true, 1.0f);

    // 创建主菜单背景
    auto spriteMainMenu = Sprite::create("main_background.png");
    if (spriteMainMenu == nullptr)
    {
        problemLoading("'main_background.png'");
    }
    else
    {
        // 把图像放在屏幕中央
        spriteMainMenu->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

        // 在MainMenu层加入背景结点
        this->addChild(spriteMainMenu, 0);
    }

    // 设置退出的标签
    auto ExitText = Label::createWithSystemFont("退出游戏", "Arial", 50);
    // 设置成军绿色
    ExitText->setColor(Color3B(85, 107, 47));
    ExitText->enableShadow();

    auto ExitItem = MenuItemLabel::create(ExitText, [this](Ref* p) {this->MainMenuCloseCallback(p);});
    ExitItem->setName("退出");

    auto StartGameItem = MenuItemImage::create("select10.png", "select11.png", [MainMenuBgmID](Ref* pSender) {
        // 关闭音乐
        AudioEngine::stop(MainMenuBgmID);

        Director::getInstance()->replaceScene(TransitionCrossFade::create(0.5f, GameScene::createWithLevel(1)));
        });

    StartGameItem->setScale(4.6f);

    // 创建 Menu 容器，将菜单项放入其中
    auto menu = Menu::create(ExitItem, StartGameItem, nullptr); // 多个菜单项用逗号分隔，末尾加 nullptr
    menu->setPosition(Vec2::ZERO); // Menu 容器位置设为原点(后续调整菜单项位置)
    ExitItem->setPosition(1898, 275);
    StartGameItem->setPosition(1440, 1100);



    // 将 menu 加入图层
    this->addChild(menu, 1);

   
    return true;
}

void MainMenu::MainMenuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end(); 
}