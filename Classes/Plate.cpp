#include "Plate.h"


bool Plate::init()
{
	CCSprite::initWithFile("background.png");

	setPosition(ccp(0, 0));
	//设置锚点为（0，0）
	setAnchorPoint(ccp(0, 0));
	//缩放
	setScale(winSize.height / getContentSize().height);

	return true;
}
