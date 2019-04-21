#ifndef __STONE_H__
#define __STONE_H__

#include "Common.h"
enum STONE_TYPE{ CHE, MA, XIANG, SHI, SHUAI, PAO, BING };

class Stone:public CCSprite
{
public:
	int _id;//每个棋子都有一个id
	bool _red;//红色还是黑色
	STONE_TYPE _type;//类型
	int _row;//在棋盘中的行
	int _col;//在棋盘中的列
	bool _dead;//棋子死没死
	static CCPoint _ptOff;
	static float _myWidth;//棋子的宽度
	//创建棋子，和初始化
	static Stone* create(int id);
	bool init(int id);

	void move(int row, int col);
	void kill();
	STONE_TYPE getTypeByID(int id);
	int getRowByID(int);
	int getColByID(int);
	CCPoint getPos();
};

#endif