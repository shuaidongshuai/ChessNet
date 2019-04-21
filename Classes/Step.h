#ifndef __STEP_H__
#define __STEP_H__

#include "Common.h"

class Step :public CCObject
{
public:
	int _moveid;
	int _rowFrom;
	int _colFrom;
	int _rowTo;
	int _colTo;
	int _killid;

	static Step* create(int moveid, int rowFrom, int colFrom, int rowTo, int colTo, int killid);
};
#endif