#include "Step.h"

Step* Step::create(int moveid, int rowFrom, int colFrom, int rowTo, int colTo, int killid)
{
	Step* step = new Step;
	step->_moveid = moveid;
	step->_rowFrom = rowFrom;
	step->_colFrom = colFrom;
	step->_rowTo = rowTo;
	step->_colTo = colTo;
	step->_killid = killid;

	step->autorelease();
	return step;
}
