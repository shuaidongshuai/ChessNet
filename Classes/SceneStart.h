#ifndef __SCENESTART_H__
#define __SCENESTART_H__

#include "Common.h"
 
class SceneStart :public CCLayer
{
private:
	CCSprite* _red;//红精灵
	CCSprite* _black;//黑精灵
	bool _bred;//哪一方走，红为true
public:
	CREATE_FUNC(SceneStart);
	bool init();
	bool ccTouchBegan(CCTouch*, CCEvent*);
	void update(float);//帧循环
};

#endif