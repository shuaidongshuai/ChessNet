
#ifndef __SceneGame_H__
#define __SceneGame_H__

#include "Game.h"
#include "cocos-ext.h"
USING_NS_CC_EXT;

USING_NS_CC;

class SceneGame :public CCLayer
{
public:
	CREATE_FUNC(SceneGame);
	bool init();

	Game* _game;
	void regret(CCObject*);
	void moveBy(CCPoint pt, CCNode* node);

	void initStartServerButton();
	void initConnectButton();
	void startServer(CCObject*);
	void ConnectServer(CCObject*);
	void update(float);
	void RecvRadom(float);
	void initLabel();
	void closeGame(CCObject*);
	void pingIP(float);
	void getHostIP();
	//virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	static int _randomSelf;
	static int _randomOther;

	CCMenu* _menu;
	CCMenuItem* serverItem;
	CCMenuItem* clientItem;
	CCLabelTTF* _label;
	CCTextFieldTTF* _text;
	CCEditBox* box;
};

#endif

