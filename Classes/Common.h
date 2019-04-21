#ifndef __COMMON_H__
#define __COMMON_H__
#include "cocos2d.h"
USING_NS_CC;

#define winSize CCDirector::sharedDirector()->getWinSize()

static inline CCScene* CreateScene(CCLayer* layer)
{
	CCScene* s = CCScene::create();
	s->addChild(layer);
	return s;
}

#endif