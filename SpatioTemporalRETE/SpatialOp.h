#pragma once
#include <queue>
#include <math.h>
#include "Event.h"
#include "SlidingWindow.h"

#include "RTree.h"

struct Rect
{
	Rect() {}

	Rect(float a_minX, float a_minY, float a_maxX, float a_maxY)
		//Rect(int a_minX, int a_minY, int a_minZ, int a_maxX, int a_maxY, int a_maxZ)
	{
		min[0] = a_minX;
		min[1] = a_minY;

		max[0] = a_maxX;
		max[1] = a_maxY;
	}


	float min[2];
	float max[2];
};

class SpatialOp
{
public:
	SpatialOp(string name);
	queue<EventPtr> process(SlidingWindow* win, vector<int> anchorObj);
	EventPtr calculate(EventPtr a, EventPtr b);
	void setVarLimit(string limit);
	void setVarCondition(string cond);
	
	//miscs

	//anchor object id thing
	void anchorPush(vector<int> vec);
	int isAnchor(int objId);
	float getLimitFloat();

	//Ok, this is new
	bool intersectLineSegment(EventPtr StartA, EventPtr EndA, EventPtr StartB, EventPtr EndB);

private:
	string queryName;
	string varLimit;
	string varCondition;

	string triggerType;
	float triggTime;
	//Event tempEvent;

	vector <int> anchorObjList;
};

