#include "Game.h"
#include "SceneGame.h"
#include "Net.h"
#include "backMusic.h"
#include "SimpleAudioEngine.h"

bool Game::_redStart;//定义   .h文件中是申明
bool Game::_redTurn;
void Game::onEnter()
{
	CCLayer::onEnter();
	_steps = CCArray::create();//没有加入到任何子节点中，下一帧自动清除
	_steps->retain();//所以保存
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("backMusic.mp3", true);
}
void Game::onExit()
{
	CCLayer::onExit();
	_steps->release();//retain 之后一定要 release 
	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}

bool Game::init()
{
	CCLayer::init();

	//添加自己照片。。哈哈
	CCSprite* sprite = CCSprite::create("3.jpg");
	sprite->setPosition(ccp(630, 200));
	sprite->setScale(0.4);
	addChild(sprite);

	//添加音乐
	backMusic* music = backMusic::create();
	addChild(music);

	//把棋盘放到Game中
	_plate = Plate::create();
	addChild(_plate);
	//摆好棋子
	Stone::_myWidth = winSize.height / 10;
	for (size_t i = 0; i < 32; i++)
	{
		_stone[i] = Stone::create(i);
		addChild(_stone[i]);
	}

	//启动触摸
	setTouchEnabled(true);
	setTouchMode(kCCTouchesOneByOne);

	//初始化按钮
	initItem();

	//设置关闭按钮
	CCMenuItem* closeItem = CCMenuItemImage::create("CloseNormal.png", "CloseSelected.png", this, menu_selector(Game::closeGame));
	closeItem->setPosition(ccp(370, -200));
	menu->addChild(closeItem);

	//初始化选中时的精灵
	_selectFlag = CCSprite::create("selected.png");
	addChild(_selectFlag);
	_selectFlag->setVisible(false);

	//一开始没有任何棋子被选中
	_selectid = -1;

	//悔棋标志为false
	bool regretFlag = false;

	_redTurn = true;

	//启动定时器
	scheduleUpdate();

	return true;
}

bool Game::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void Game::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	int clickid = getClickStone(pTouch);//根据触摸点获取棋子，如果这个点没有棋子返回 -1
	if (_selectid == -1)//如果还没有棋子被选中
		selectStone(clickid);
	else//走棋
		moveStone(clickid,pTouch->getLocation());
}
//选择棋子
void Game::selectStone(int clickid)
{
	if (clickid != -1 && _redTurn == _redStart && _redTurn == _stone[clickid]->_red)//如果1.点击到棋子  2.轮到这我方走  3.是我方的棋子
	{
		doSelectStone(clickid);

		sendSelectStoneToNet();
	}
}
//移动棋子
void Game::moveStone(int clickid,CCPoint pos)
{
	if (clickid != -1)//首先判断点中的位置是不是棋子
	{
		if (_stone[clickid]->_red == _stone[_selectid]->_red)//如果是己方的棋子再次选中
		{
			doSelectStone(clickid);
			
			sendSelectStoneToNet();
			return;
		}
	}
	//棋子走动
	int row, col;
	if (!getRowColByPos(row, col,pos))//看能否获得行列
		return;
	if (!canMove(_selectid, row, col, clickid))//每个棋子有自己固定的走法，必须经过审核才能走
		return;

	//向对方发送信息
	sendMoveStoneToNet(row, col);

	//走棋								注意这里两个selectid  一个是发送过来的_selectid 一个是自己方点击的 _selectid
	doMoveStone(_selectid, row, col);//两个功能，1.对方发信息来走棋（需要ID和目标位置）2.自己方走棋（需要选中的棋和目标位置）
}
//发送选棋信息
void Game::sendSelectStoneToNet()
{
	char buf[2];
	buf[0] = 1;//命令字1  表示选择棋子
	buf[1] = _selectid;
	Net::getInstance()->Send(buf, 2);
}
//发送走棋信息
void Game::sendMoveStoneToNet(int row, int col)
{
	char buf[4];
	buf[0] = 2;
	buf[1] = _selectid;
	buf[2] = 9 - row;
	buf[3] = 8 - col;
	Net::getInstance()->Send(buf, 4);
}

//用于接收数据
void Game::update(float)
{
	if (_redStart == _redTurn && !regretFlag)//此时不接受消息
		return;

	char buf;
	char id;
	char row;
	char col;
	if (Net::getInstance()->Recv(&buf, 1) != 1)
		return;
	
	switch (buf)
	{
	case 1://接收对方选择棋子的命令
		Net::getInstance()->Recv(&id, 1);
		doSelectStone(id);//把对方选择棋子显示出来
		break;
	case 2:
		Net::getInstance()->Recv(&id, 1);
		Net::getInstance()->Recv(&row, 1);
		Net::getInstance()->Recv(&col, 1);
		doMoveStone(id, row, col);
		break;
	case 3:
		judgeRegret();
		break;
	case 'y': 
		waitRegretYes();
		break;
	case 'n':
		waitRegretNo();
		break;
	case 9:
		CCDirector::sharedDirector()->replaceScene(CreateScene(Game::create()));
	}
}
void Game::initItem()
{
	menu = CCMenu::create();
	addChild(menu);

	//创建悔棋按钮
	initRegretButton();

	//其他按钮
	regretTextItem = CCMenuItemImage::create("huiqikuang.png", "huiqikuang2.png");
	yesItem = CCMenuItemImage::create("tongyi.png", "tongyi2.png");
	noItem = CCMenuItemImage::create("butongyi.png", "butongyi2.png");
	giveMyItem = CCMenuItemImage::create("shenqing2.png", "huiqikuang2.png");
	giveMyItemYes = CCMenuItemImage::create("duifangtongyi2.png", "duifangtongyi.png");
	giveMyItemNo = CCMenuItemImage::create("duifangbutongyi2.png", "duifangbutongyi.png");
	menu->addChild(regretTextItem);
	menu->addChild(yesItem);
	menu->addChild(noItem);
	menu->addChild(giveMyItem);
	menu->addChild(giveMyItemYes);
	menu->addChild(giveMyItemNo);

	regretTextItem->setVisible(false);
	yesItem->setVisible(false);
	noItem->setVisible(false);
	giveMyItem->setVisible(false);
	giveMyItemYes->setVisible(false);
	giveMyItemNo->setVisible(false);

	moveBy(ccp(-180, 120), regretTextItem);//设置位置
	moveBy(ccp(-280, -100), yesItem);//设置位置
	moveBy(ccp(-80, -100), noItem);//设置位置
	moveBy(ccp(-180, 120), giveMyItem);//设置位置
	moveBy(ccp(-180, 120), giveMyItemYes);//设置位置
	moveBy(ccp(-180, 120), giveMyItemNo);//设置位置

	yesItem->setTarget(this, menu_selector(Game::yesregret));
	noItem->setTarget(this, menu_selector(Game::noregret));
	giveMyItemYes->setTarget(this, menu_selector(Game::waitYes));
	giveMyItemNo->setTarget(this, menu_selector(Game::waitNo));
}
void Game::judgeRegret()
{
	regretTextItem->setVisible(true);
	yesItem->setVisible(true);
	noItem->setVisible(true);
}

//显示出 选择的棋子
void Game::doSelectStone(int clickid)
{
	//如果有棋子被点中，，就让【】 图形在那个位置显示
	_selectFlag->setPosition(_stone[clickid]->getPosition());
	_selectFlag->setVisible(true);
	_selectid = clickid;
}
//用于把一个id从当前位置，移动到 指定位置
void Game::doMoveStone(int selectid, int row, int col)
{
	int killid = getStoneIdByRowCol(row, col);
	//记录走棋信息
	Step* step = Step::create(selectid, _stone[selectid]->_row, _stone[selectid]->_col, row, col, killid);
	_steps->addObject(step);//加入到数组中

	//走棋
	_stone[_selectid]->move(row, col);
	_selectFlag->setVisible(false);

	_selectid = -1;//走完后一定要设置，不然会一直选中

	if (killid != -1)
	{
		_stone[killid]->_dead = true;
		_stone[killid]->setVisible(false);

		//检查游戏是否结束
		checkGameOver(killid);
	}
	_redTurn = !_redTurn;
}
void Game::checkGameOver(int killid)
{
	Stone* s = _stone[killid];
	if (s->_type == SHUAI)//游戏结束 如果这次杀死的是帅或将，那么就打印
	{
		CCMenu* menu = CCMenu::create();
		addChild(menu);
		if (s->_red == Game::_redStart)//开始选的棋和杀死的棋一样，，那么是输了
		{
			CCMenuItem* item1 = CCMenuItemImage::create("shuijiemian.png", "shuijiemian.png");
			menu->addChild(item1);
			item1->setPosition(ccp(-150, 50));
		}
		else//赢了
		{
			CCMenuItem* item1 = CCMenuItemImage::create("yingjiemian.png", "yingjiemian.png");
			menu->addChild(item1);
			item1->setPosition(ccp(-150, 50));

			CCMenuItem* item = CCMenuItemImage::create("new.jpg", "new2.jpg");
			menu->addChild(item);
			item->setTarget(this, menu_selector(Game::gameOverCallback));
			item->setPosition(ccp(-150, -150));
		}
		regretItem->setEnabled(false);

		// 红旗黑棋位置对调
		Game::_redStart = !Game::_redStart;
	}
}
void  Game::gameOverCallback(CCObject*)
{
	char buf = 9;
	Net::getInstance()->Send(&buf, 1);
	CCDirector::sharedDirector()->replaceScene(CreateScene(Game::create()));
}
//获取 点中 的棋子的id
int Game::getClickStone(CCTouch* touch)
{
	int row, col;
	if (!getRowColByPos(row, col, touch->getLocation()))
		return -1;
	return getStoneIdByRowCol(row, col);
}
//通过坐标转化成行列
bool Game::getRowColByPos(int &row, int &col, CCPoint pt)
{
	for (row = 0; row < 10; row++)
		for (col = 0; col < 9; col++)
		{
			//获取棋盘上点的世界坐标
			CCPoint ptStone = getPosByRowCol(row, col);
			//判断世界坐标上的点。。是否和触摸点的距离小于  棋子宽度的一半
			if (ptStone.getDistance(pt) < Stone::_myWidth / 2)
				return true;
		}
	return false;
}
//通过行列。看是否能找到某个棋子
int Game::getStoneIdByRowCol(int row, int col)
{
	for (int i = 0; i < 32; i++)
	{
		if (_stone[i]->_row == row && _stone[i]->_col == col && !_stone[i]->_dead)
			return i;
	}
	return -1;
}

//行列 转换成 世界坐标
CCPoint Game::getPosByRowCol(int row, int col)
{
	float x = col*Stone::_myWidth;
	float y = row*Stone::_myWidth;
	return ccp(x, y) + Stone::_ptOff;
}

bool  Game::canMove(int moveid, int row, int col, int killid)
{
	switch (_stone[moveid]->_type)
	{
	case CHE:
		return canMoveChe(moveid, row, col, killid);
	case MA:
		return canMoveMa(moveid, row, col, killid);
	case XIANG:
		return canMoveXiang(moveid, row, col, killid);
	case SHI:
		return canMoveShi(moveid, row, col, killid);
	case SHUAI:
		return canMoveShuai(moveid, row, col, killid);
	case PAO:
		return canMovePao(moveid, row, col, killid);
	case BING:
		return canMoveBing(moveid, row, col, killid);
	default:
		break;
	}
	return false;
}
int Game::getStoneCountInLine(int row1, int col1, int row2, int col2)
{
	if (row1 != row2 &&col1 != col2)
		return -1;
	int t = 0;
	if (row1 == row2)
	{
		int min = col1 > col2 ? col2 : col1;
		int max = col1 + col2 - min;
		for (int col = min + 1; col < max; col++)
		{
			int id = getStoneIdByRowCol(row1, col);
			if (id != -1)
				t++;
		}
	}
	else
	{
		int min = row1 > row2 ? row2 : row1;
		int max = row1 + row2 - min;
		for (int row = min + 1; row < max; row++)
		{
			int id = getStoneIdByRowCol(row,col1);
			if (id != -1)//如果有棋子
				t++;
		}
	}
	return t;
}

bool Game::canMoveChe(int moveid, int row, int col, int killid)//moveid 是 上次选中的棋子 killid是这次点击的棋子
{
	if (getStoneCountInLine(row, col, _stone[moveid]->_row, _stone[moveid]->_col) == 0)
		return true;
	return false;
}
bool Game::canMoveMa(int moveid, int row, int col, int killid)
{
	int r = _stone[moveid]->_row;
	int c = _stone[moveid]->_col;
	int d = abs(r - row) * 10 + abs(c - col);
	if (d != 12 && d != 21)
		return false;
	//判断马腿
	if (d == 21)//列相差1，航相差2
	{
		int mr = (r + row) / 2;
		int mc = c;
		if (getStoneIdByRowCol(mr, mc)!=-1)//说明这个位置有棋子
			return false;
	}
	else
	{
		int mr = r;
		int mc = (c + col) / 2;
		if (getStoneIdByRowCol(mr, mc) != -1)//说明这个位置有棋子
			return false;
	}
	return true;
}
bool Game::canMoveXiang(int moveid, int row, int col, int killid)
{
	if (_stone[moveid]->_red == _redStart)//只要动的棋子和开局选的棋子一样，那么就可以在下方动
	{
		if (row > 4)
		return false;
	}
	else
	{
		if (row < 5)
			return false;
	}
	int r = _stone[moveid]->_row;
	int c = _stone[moveid]->_col;
	int d = abs(r - row) * 10 + abs(c - col);
	if (d != 22)
		return false;

	int mr = (row + r) / 2;
	int mc = (col + c) / 2;
	int id = getStoneIdByRowCol(mr, mc);
	if (id != -1)
		return false;
	return true;
}
bool Game::canMoveShi(int moveid, int row, int col, int killid)
{
	if (_stone[moveid]->_red == _redStart)//只要动的棋子和开局选的棋子一样，那么就可以在下方动
	{
		if (row > 2)
		return false;
	}
	else
	{
		if (row < 7)
			return false;
	}
	if (col < 3 || col>5)
		return false;

	int r = _stone[moveid]->_row;
	int c = _stone[moveid]->_col;
	int d = abs(r - row) * 10 + abs(c - col);
	if (d == 11)//行相等，那么列只能差值为1
		return true;
	return false;
}
bool Game::canMoveShuai(int moveid, int row, int col, int killid)
{
	//老蒋照面
	if (killid != -1 && _stone[killid]->_type == SHUAI)
		return canMoveChe(moveid, row, col, killid);

	//开局选的棋子一定在下方
	if (_stone[moveid]->_red == _redStart)//只要动的棋子和开局选的棋子一样，那么就可以在下方动
	{
		if (row > 2)
			return false;
	}
	else//如果动的棋子，和开局选的棋子不一样，那么就动上面的棋子
	{
		if (row < 7)
			return false;
	}
	if (col < 3 || col>5)
		return false;

	int r = _stone[moveid]->_row;
	int c = _stone[moveid]->_col;
	int d = abs(r - row) * 10 + abs(c - col);
	if (d == 1 || d == 10)//行相等，那么列只能差值为1
		return true;
	return false;
}
bool Game::canMovePao(int moveid, int row, int col, int killid)
{
	if (killid == -1)//说明没有点中棋子
		return canMoveChe(moveid, row, col, killid);//和车一样的使用
	//点中棋子后，中间必须隔一个才能走
	int count = getStoneCountInLine(row, col, _stone[moveid]->_row, _stone[moveid]->_col);
	return  count == 1;//跑中间只能隔一个
}
bool Game::canMoveBing(int moveid, int row, int col, int killid)
{
	int r = _stone[moveid]->_row;
	int c = _stone[moveid]->_col;
	int d = abs(r - row) * 10 + abs(c - col);
	if (d != 1 && d != 10)//行相等，那么列只能差值为1
		return false;
	
	if (_stone[moveid]->_red == _redStart)//只要动的棋子和开局选的棋子一样，那么就可以在下方动
	{
		//不能后退
		if (row < r)//将要走的行 大于 现在的行
			return false;
		//没过河不能平移
		if (r <= 4 && col != c)
			return false;
	}
	else
	{
		//不能后退
		if (row > r)//将要走的行 大于 现在的行
			return false;
		//没过河不能平移
		if (r >= 5  && col != c)
			return false;
	}
	return true;
}

void Game::yesregret(CCObject*)
{
	char buf = 'y';
	Net::getInstance()->Send(&buf, 1);
	regret();
	yesItem->setVisible(false);
	noItem->setVisible(false);
	regretTextItem->setVisible(false);
}
void Game::noregret(CCObject*)
{
	char buf = 'n';
	Net::getInstance()->Send(&buf, 1);
	yesItem->setVisible(false);
	noItem->setVisible(false);
	regretTextItem->setVisible(false);
}
void Game::waitRegretYes()
{
	giveMyItem->setVisible(false);

	giveMyItemYes->setVisible(true);
	regret();
}
void Game::waitRegretNo()
{
	giveMyItem->setVisible(false);

	giveMyItemNo->setVisible(true);
}
void Game::waitYes(CCObject*)
{
	giveMyItemYes->setVisible(false);
	//打开触摸
	setTouchEnabled(true);
	//悔棋标记消失
	regretFlag = false;
}
void Game::waitNo(CCObject*)
{
	giveMyItemNo->setVisible(false);
	//打开触摸
	setTouchEnabled(true);
	//悔棋标记消失
	regretFlag = false;
}
//在悔棋之前进行判断，对方走时，自己不能悔棋
void Game::preregret(CCObject*)
{
	if (Game::_redStart != Game::_redTurn)
		return;

	//发消息给对方
	char buf = 3;
	Net::getInstance()->Send(&buf, 1);
	giveMyItem->setVisible(true);

	//屏蔽自己的触摸
	setTouchEnabled(false);

	//可以接收消息
	regretFlag = true;
 }
void Game::regret()
{
	for (size_t i = 0; i < 2; i++)
	{
		if (_steps->count() == 0)
			return;
		//取出上一次的棋子 的位置
		CCObject* obj = _steps->lastObject();
		Step* step = (Step*)obj;

		//将这个棋子，恢复到原来位置
		Stone* sMove = _stone[step->_moveid];//获取上一个棋子的id
		sMove->move(step->_rowFrom, step->_colFrom);

		//如果有死掉的棋子，应该复活
		if (step->_killid != -1)
		{
			Stone* sKill = _stone[step->_killid];
			sKill->_dead = false;//不死
			sKill->setVisible(true);//再显现出来
		}

		//初始化一些和移动相关的变量
		_selectid = -1;
		_selectFlag->setVisible(false);
		_redTurn = !_redTurn;

		//把Array里面最后一个对象删除
		_steps->removeLastObject();
	}
}

//创建悔棋按钮
void Game::initRegretButton()
{
	regretItem = CCMenuItemImage::create("regret.png", "regret2.png");
	menu->addChild(regretItem);
	regretItem->setTarget(this, menu_selector(Game::preregret));//设置触发函数
	moveBy(ccp(120, 0), regretItem);//设置位置
}

void Game::moveBy(CCPoint pt, CCNode* node)
{
	node->setPosition(pt + node->getPosition());
}

void Game::closeGame(CCObject*)
{
	if (Net::getInstance()->isServer())
#ifdef WIN32
		closesocket(Net::getInstance()->getServerSocket());
#else
		close(Net::getInstance()->getServerSocket());
#endif
	else
#ifdef WIN32
		closesocket(Net::getInstance()->getClientSocket());
#else
		close(Net::getInstance()->getClientSocket());
#endif
	exit(0);
}