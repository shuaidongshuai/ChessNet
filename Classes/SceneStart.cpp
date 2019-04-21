#include "SceneStart.h"
#include "SceneGame.h"

bool SceneStart::init()
{
	CCLayer::init();
	//背景图片
	CCSprite* s = CCSprite::create("desk.png");
	addChild(s);
	s->setPosition(ccp(winSize.width / 2, winSize.height / 2));

	//创建开始按钮
	CCSprite* sprite1 = CCSprite::create("bkg1.png");
	CCSprite* sprite2 = CCSprite::create("bkg2.png");
	addChild(sprite1);
	addChild(sprite2);
	sprite2->setPosition(ccp(winSize.width/4, winSize.height / 2));
	sprite1->setPosition(ccp(winSize.width - winSize.width/4, winSize.height / 2));
	
	_black = sprite1;
	_red = sprite2;

	//使能触摸，使用单点
	setTouchEnabled(true);
	setTouchMode(kCCTouchesOneByOne);

	//启动帧循环定时器(不要放在 ccTouchBegan 里面，不然有一个Bug就是：点击两次，不仅会加速，而且会启动两次定时器)
	scheduleUpdate();

	return true;
}

bool SceneStart::ccTouchBegan(CCTouch* touch, CCEvent*)
{
	//获取触摸位置
	CCPoint pt = touch->getLocation();
	//判断是否触摸到  黑白按钮
	if (_red->boundingBox().containsPoint(pt))
	{
		_bred = true;
	}
	else if (_black->boundingBox().containsPoint(pt))
	{
		_bred = false;
	}
	else return 0;

	//点中棋子后的效果
	CCMoveTo* move1 = CCMoveTo::create(0.5, ccp(winSize.width / 2 + 10, winSize.height / 2));
	CCMoveTo* move2 = CCMoveTo::create(0.5, ccp(winSize.width / 2, winSize.height / 2));
	CCRotateBy* rotate1 = CCRotateBy::create(2, 360);
	CCRotateBy* rotate2 = CCRotateBy::create(2, -360);
	_red->runAction(CCSpawn::createWithTwoActions(move1,rotate1));
	_black->runAction(CCSpawn::createWithTwoActions(move2, rotate2));

	return true;
}

void SceneStart::update(float dt)
{
	//要对两个棋子进行碰撞检测  _red->getContentSize().width  是图片的宽度
	//if (_red->boundingBox().intersectsRect(_black->boundingBox()))//判断矩形是否接触  intersectsRect
	if (_red->getPositionX() - _black->getPositionX() > 0 )
	{
		Game::_redStart = _bred;
		CCDirector::sharedDirector()->replaceScene(CreateScene(SceneGame::create()));
	}

}
