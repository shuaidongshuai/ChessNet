#include "backMusic.h"
#include "SimpleAudioEngine.h"
bool backMusic::init()
{
	CCLayer::init();

	CCMenu* menu = CCMenu::create();
	addChild(menu);
	itemOn = CCMenuItemImage::create("OnMusic.png", "OffMusic.png");
	itemOff = CCMenuItemImage::create("OffMusic.png", "OnMusic.png");
	itemOn->setTarget(this, menu_selector(backMusic::musicOn));
	itemOff->setTarget(this, menu_selector(backMusic::musicOff));
	menu->addChild(itemOn);
	menu->addChild(itemOff);
	itemOn->setPosition(150, 180);
	itemOff->setPosition(150, 180);
	itemOff->setVisible(false);

	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("backMusic.mp3", true);
	CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(1.0);
	return true;
}
void backMusic::musicOn(CCObject*)
{
	//CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	itemOff->setVisible(true);
	itemOn->setVisible(false);
}
void backMusic::musicOff(CCObject*)
{
	//CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("backMusic.mp3", true);
	CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
	itemOn->setVisible(true);
	itemOff->setVisible(false);
}


