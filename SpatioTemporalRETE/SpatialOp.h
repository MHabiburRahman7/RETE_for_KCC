#pragma once
#include <queue>
#include <math.h>
#include "Event.h"
#include "SlidingWindow.h"

class SpatialOp
{
public:
	SpatialOp(string name);
	queue<EventPtr> process(SlidingWindow* win);
	EventPtr calculate(EventPtr a, EventPtr b);
	void setVarLimit(string limit);
	void setVarCondition(string cond);
	
	//Ok, this is new
	bool intersectLineSegment(EventPtr A, EventPtr B, EventPtr C, EventPtr D);

private:
	string queryName;
	string varLimit;
	string varCondition;
};

