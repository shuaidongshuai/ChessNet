#ifndef __backMusic_H__
#define __backMusic_H__

#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

class backMusic :public CCLayer
{
	CCMenuItem* itemOn;
	CCMenuItem* itemOff;
public:
	CREATE_FUNC(backMusic);
	bool init();
	void musicOff(CCObject*);
	void musicOn(CCObject*);
};

#endif

