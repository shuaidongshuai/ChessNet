#ifndef __GAME_H__
#define __GAME_H__

#include "Common.h"
#include "Stone.h"
#include "Plate.h"
#include "Step.h"

class Game:public CCLayer
{
	int _selectid;//记录选中棋子的id
	bool _isOver;//游戏是否结束
	bool regretFlag;//是否正确按下悔棋
	CCSprite* _selectFlag;//选中时的精灵图片
	CCMenu* menu;
	CCMenuItem* regretItem;
	CCMenuItem* regretTextItem;
	CCMenuItem* yesItem;
	CCMenuItem* noItem;
	CCMenuItem* giveMyItem;
	CCMenuItem* giveMyItemYes;
	CCMenuItem* giveMyItemNo;
public:
	static bool _redStart;//红棋开始
	static bool _redTurn;//该不该红走 （轮到谁走了）
	Plate* _plate;//棋盘
	Stone* _stone[32];//棋子
	CCArray* _steps;//记录走棋的信息
	

	CREATE_FUNC(Game);
	bool init();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	//如果点到空地上，返回-1
	int getClickStone(CCTouch*);
	CCPoint getPosByRowCol(int, int);
	bool getRowColByPos(int &row, int &col, CCPoint pt);
	int getStoneIdByRowCol(int row, int col);
	bool canMove(int moveid, int row, int col, int killed);
	bool canMoveChe(int moveid, int row, int col, int killed);
	bool canMoveMa(int moveid, int row, int col, int killed);
	bool canMoveXiang(int moveid, int row, int col, int killed);
	bool canMoveShi(int moveid, int row, int col, int killed);
	bool canMoveShuai(int moveid, int row, int col, int killed);
	bool canMovePao(int moveid, int row, int col, int killed);
	bool canMoveBing(int moveid, int row, int col, int killed);

	int getStoneCountInLine(int row1, int col1, int row2, int col2);
	void onEnter();
	void onExit();
	void regret();
	void checkGameOver(int killid);
	void gameOverCallback(CCObject*);
	void closeGame(CCObject*);
	void preregret(CCObject*);
	void yesregret(CCObject*);
	void noregret(CCObject*);
	void waitYes(CCObject*);
	void waitNo(CCObject*);
	void initItem();


	void update(float);
	void doSelectStone(int id);
	void doMoveStone(int id, int row, int col);
	void selectStone(int id);
	void moveStone(int id, CCPoint pos);
	void sendSelectStoneToNet();
	void sendMoveStoneToNet(int row,int col);
	void initRegretButton();
	void moveBy(CCPoint pt, CCNode* node);
	void judgeRegret();
	void waitRegretYes();
	void waitRegretNo();
};

#endif