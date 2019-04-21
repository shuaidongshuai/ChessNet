#ifndef __PLATE_H__
#define __PLATE_H__
#include "Common.h"

class Plate:public CCSprite
{
public:
	CREATE_FUNC(Plate);
	bool init();
};

#endif