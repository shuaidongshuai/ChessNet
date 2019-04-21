#include "Stone.h"
#include "Game.h"
CCPoint Stone::_ptOff = ccp(45, 25);//偏移值初始化
float Stone::_myWidth;// = winSize.height / 10;//棋子宽度初始化(千万注意。这里调用winSize会出问题。调用不了winSize，只能在函数里面调用了)

Stone* Stone::create(int id)
{
	Stone* stone = new Stone;
	stone->init(id);
	stone->autorelease();
	return stone;
}
bool Stone::init(int id)
{
	if (Game::_redStart)
	{
		_row = getRowByID(id);
		_col = getColByID(id);
	}
	else
	{
		_row = getRowByID(id);
		_col = getColByID(id);
		_row = 9 - _row;
		_col = 8 - _col;
	}
	_id = id;
	_type = getTypeByID(id);
	_red = id < 16;
	_dead = false;

	const char* bfile[] = { "bche.png", "bma.png", "bxiang.png", "bshi.png", "bjiang.png", "bpao.png", "bzu.png" };
	const char* rfile[] = { "rche.png", "rma.png", "rxiang.png", "rshi.png", "rshuai.png", "rpao.png", "rbing.png" };

	if (_red)
		CCSprite::initWithFile(rfile[_type]);
	else
		CCSprite::initWithFile(bfile[_type]);
	//棋子设置自己的位置
	setPosition(getPos());
	//发现棋子有点大了，改小一点
	setScale(.9f);

	return true;
}
//获取类型。类型只有6种{ CHE, MA, XIANG, SHI, SHUAI, PAO, BING };也就是说数字只有 0 -- 5
STONE_TYPE Stone::getTypeByID(int id)
{
	//每一个ID 对应一个位置，对应一个type
	STONE_TYPE type[32] = { CHE, MA, XIANG, SHI, SHUAI, SHI, XIANG, MA, CHE,
		PAO, PAO,
		BING, BING, BING, BING, BING,
		BING, BING, BING, BING, BING,
		PAO, PAO,
		CHE, MA, XIANG, SHI, SHUAI, SHI, XIANG, MA, CHE };
	return type[id];
}
//判断每个棋子处于哪一行
int Stone::getRowByID(int id)
{
	int row[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0,
		2, 2,
		3, 3, 3, 3, 3,
		6, 6, 6, 6, 6,
		7, 7,
		9, 9, 9, 9, 9, 9, 9, 9, 9 };
	return row[id];
}
//判断每个棋子处于哪一列
int Stone::getColByID(int id)
{
	int col[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 8,
		1, 7,
		0, 2, 4, 6, 8,
		0, 2, 4, 6, 8,
		1, 7,
		0, 1, 2, 3, 4, 5, 6, 7, 8 };
	return col[id];
}
//获取现在棋子的位置（世界坐标）
CCPoint Stone::getPos()
{
	float x = _col*_myWidth;
	float y = _row*_myWidth;
	return ccp(x, y) + _ptOff;//暂时不知道为什么不直接给一个特定的值，非得定义静态成员函数，然后让Game来给值？？？要是后面没用到就删除静态
}
void Stone::move(int row, int col)
{
	_row = row;
	_col = col;
	//this->setPosition(getPos());
	CCPoint pt = getPos();
	this->setPosition(pt);
}