#include "SpatialOp.h"

SpatialOp::SpatialOp(string name)
{
	queryName = name;
}

queue<EventPtr> SpatialOp::process(SlidingWindow* win)
{
	if (queryName == "crossing") {
		queue<EventPtr> res;
		queue<pair<EventPtr, EventPtr>> local_event = win->getDoubleRes();

		//objectId distinction --> left is the anchor --> HAVE TO WORK ON IT
		//bypass this/ No we can't --> aaaah screaming in confusion ._.

		//lets work on vectors
		vector<pair<EventPtr, EventPtr>> v_local_event;
		while (local_event.size() > 0) {
			v_local_event.push_back(local_event.front());
			local_event.pop();
		}

		for (int i = 0; i < v_local_event.size()-1; i++) {
			for (int j = 0; j < v_local_event.size()-1; j++) {
				bool ans = intersectLineSegment(v_local_event[i].first, v_local_event[i + 1].first, v_local_event[j].second, v_local_event[j + 1].second);

				if (ans) {
					//time is based on the latest event happend
					Event* e = new Event(Utilities::id++, v_local_event[i].second->getInt("time"));
					e->addAttr("cross", "true");
					e->addAttr("objId_left", v_local_event[i].first->getInt("objId"));
					e->addAttr("objId_right", v_local_event[i].second->getInt("objId"));

					res.push(EventPtr(e));
				}
			}
		}
		return res;
	}
	else if (queryName == "distance") {
		//queue<EventPtr> local_event = win->getFinalRes();
		//while (local_event.size() > 0) {
		//	//distance calculation

		//	//return result

		//}
	}
	else if (queryName == "exist") {
		return *win->getFinalRes();
	}
}

EventPtr SpatialOp::calculate(EventPtr a, EventPtr b)
{
	//do calculation
	float res;
	if (queryName == "distance") {
		res = sqrt(pow((b->getFloat("lat") - (a->getFloat("lat"))), 2) + pow((b->getFloat("lon") - (a->getFloat("lon"))), 2));

		if (varCondition == "<" && res < atof(varLimit.c_str())) {
			Event* e = new Event(Utilities::id++, a->getInt("time"));
			e->addAttr("dist", res);
			e->addAttr("objLeft", a->getInt("objid"));
			e->addAttr("objright", b->getInt("objid"));
			//e->addAttr("leftId", a->getInt("objId"));
			//e->addAttr("rightId", b->getInt("objId"));

			return (EventPtr(e));
		}
		else if (varCondition == "<=" && res <= atof(varLimit.c_str())) {
			Event* e = new Event(Utilities::id++, a->getInt("time"));
			e->addAttr("dist", res);
			//e->addAttr("leftId", a->getInt("objId"));
			//e->addAttr("rightId", b->getInt("objId"));

			return (EventPtr(e));
		}
		else if (varCondition == "=") {

		}
	}
	else if (queryName == "hovering") {

	}

	return NULL;
}

void SpatialOp::setVarLimit(string limit)
{
	varLimit = limit;
}

void SpatialOp::setVarCondition(string cond)
{
	varCondition = cond;
}

bool SpatialOp::intersectLineSegment(EventPtr StartA, EventPtr EndB, EventPtr StartC, EventPtr EndD)
{
	pair<float, float> A({ StartA->getFloat("lat"), StartA->getFloat("lon") });
	pair<float, float> B({ EndB->getFloat("lat"), EndB->getFloat("lon") });
	pair<float, float> C({ StartC->getFloat("lat"), StartC->getFloat("lon") });
	pair<float, float> D({ EndD->getFloat("lat"), EndD->getFloat("lon") });

	float CmPx = C.first - A.first;
	float CmPy = C.second - A.second;
	float rx = B.first - A.first;
	float ry = B.second - A.second;
	float sx = D.first - C.first;
	float sy = D.second - C.second;

	float CmPxr = CmPx * ry - CmPy * rx;
	float CmPxs = CmPx * sy - CmPy * sx;
	float rxs = rx * sy - ry * sx;

	if (CmPxr == 0) {
		// Lines are collinear, and so intersect if they have any overlap
		return ((C.first - A.first < 0) != (C.first - B.first < 0))
			|| ((C.second - A.second < 0) != (C.second - B.second < 0));
	}

	if (rxs == 0)
		return false; // Lines are parallel.

	float rxsr = 1 / rxs;
	float t = CmPxs * rxsr;
	float u = CmPxr * rxsr;

	return (t >= 0) && (t <= 1) && (u >= 0) && (u <= 1);
}
