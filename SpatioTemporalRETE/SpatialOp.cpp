#include "SpatialOp.h"

SpatialOp::SpatialOp(string name)
{
	queryName = name;
}

bool ReturnVal(int id, void* arg)
{
	//dynamic_cast<Event*>(arg)->addAttr("leftobj", id);
	//printf("Hit data rect %d\n", id);
	return true; // keep going
}

queue<EventPtr> SpatialOp::process(SlidingWindow* win, vector<int> anchorObj)
{
	//add anchor obj id
	anchorPush(anchorObj);
	
	queue<EventPtr> final_res;
	
	//if (queryName == "crossing") {
	//	queue<pair<EventPtr, EventPtr>> local_event = win->getDoubleRes();
	//
	//	//objectId distinction --> left is the anchor --> HAVE TO WORK ON IT
	//	//bypass this/ No we can't --> aaaah screaming in confusion ._.
	//
	//	//lets work on vectors
	//	vector<pair<EventPtr, EventPtr>> v_local_event;
	//	while (local_event.size() > 0) {
	//		v_local_event.push_back(local_event.front());
	//		local_event.pop();
	//	}
	//
	//	for (int i = 0; i < v_local_event.size()-1; i++) {
	//		for (int j = 0; j < v_local_event.size()-1; j++) {
	//			bool ans = intersectLineSegment(v_local_event[i].first, v_local_event[i + 1].first, v_local_event[j].second, v_local_event[j + 1].second);
	//
	//			if (ans) {
	//				//time is based on the latest event happend
	//				Event* e = new Event(Utilities::id++, v_local_event[i].second->getInt("time"));
	//				e->addAttr("cross", "true");
	//				e->addAttr("objId_left", v_local_event[i].first->getInt("objId"));
	//				e->addAttr("objId_right", v_local_event[i].second->getInt("objId"));
	//
	//				final_res.push(EventPtr(e));
	//			}
	//		}
	//	}
	//	return final_res;
	//}
	if (queryName == "crossing") { // still need to be fixed
		queue<EventPtr> res = win->getOriginalRes();

		//separate each event
		map<int, vector<EventPtr>> separatedEvents;
		for (; res.size() > 0; res.pop()) {
			separatedEvents[res.front()->getInt("time")].push_back(res.front());
		}

		//evaluate each time ticks
		for (int i = 1; i < separatedEvents.size(); i++) {

		}

		//for(int i=0; i<)
	}
	else if (queryName == "distance") {
		// so the window is full, what do we do?
		//pull the window first
		queue<EventPtr> res = win->getOriginalRes();

		////distinct the window based on objId
		//vector<pair<int, queue<EventPtr>>> distinctObj;
		//while(res.size() > 0){
		//	for (int i = 0; i < distinctObj.size(); i++) {
		//		if (distinctObj[i].first == res.front()->getInt("objid")) {
		//			distinctObj[i].second.push(res.front());
		//			res.pop();
		//			continue;
		//		}
		//	}
		//	queue<EventPtr> newObj;
		//	newObj.push(res.front());
		//	distinctObj.push_back({ res.front()->getInt("objid"), newObj });
		//	res.pop();
		//}

		/*
#pragma region Indexed_mode
		//we don't need to re-calculate based on r-tree again, because it is already calculated by node indexing
		//we just pass all of the input buffer to result buffer
		//dont forget to pass the anchors
		string tempAnchors;
		char buff[10];
		for (auto a : anchorObjList) {
			tempAnchors += _itoa(a, buff, 10);
			tempAnchors += ",";
		}
		for (; res.size() > 0; res.pop()) {
			res.front()->addAttr("anchors", tempAnchors);

			win->addResultEvent(res.front());
		}
#pragma endregion
		*/
/*
#pragma region RTreeRegion
		RTree<int, float, 2, float> tree;

		unordered_map<int, pair<float, float>> hash_latestUpdate;
		unordered_map<int, bool> hash_needUpdate;

		//new data, format the tree
		tree.RemoveAll();

		for (int i = 0; i < anchorObjList.size(); i++) {
			//hash_latestUpdate[i].first = 0;
			//hash_latestUpdate[i].second = 0;
			hash_latestUpdate[anchorObjList[i]].first = 0;
			hash_latestUpdate[anchorObjList[i]].second = 0;

			hash_needUpdate[i] = true;
		}

		while (res.size() > 0) {
			int currTime = res.front()->getInt("time");

			//dummy for tree
			queue<EventPtr> oneTimeEvent = {};
			while (res.front()->getInt("time") == currTime) {
				oneTimeEvent.push(res.front());
				res.pop();

				if (res.size() <= 0)
					break;
			}

			Rect* tempAnchor;
			vector<int> nhits_vec;

			while (oneTimeEvent.size() > 0) {
				// if this is one of anchor
				int validAnchor = isAnchor(oneTimeEvent.front()->getInt("objid"));
				if (validAnchor > -1) {
					//check obj position
					//Compare with the latest
					if (hash_latestUpdate[validAnchor].first != 0 && hash_latestUpdate[validAnchor].second != 0) {
						//calculate with prev position
						float lat, lon;
						lat = hash_latestUpdate[validAnchor].first - oneTimeEvent.front()->getFloat("lat");
						lon = hash_latestUpdate[validAnchor].second - oneTimeEvent.front()->getFloat("lon");

						if (abs(lat) > 0.05 && abs(lon) > 0.5) { //tolerance --> more than tolerance need update
							hash_needUpdate[validAnchor] = true;
						}
						else {
							hash_needUpdate[validAnchor] = false;
						}
					}

					//do we need to update?
					if (hash_needUpdate[validAnchor] == true) {

						//the not the beginning
						if (hash_latestUpdate[validAnchor].first != 0 && hash_latestUpdate[validAnchor].second != 0) {
							tempAnchor = new Rect(hash_latestUpdate[validAnchor].first - (atof(varLimit.c_str()) / 2), hash_latestUpdate[validAnchor].second - (atof(varLimit.c_str()) / 2)
								, hash_latestUpdate[validAnchor].first + (atof(varLimit.c_str()) / 2), hash_latestUpdate[validAnchor].second + (atof(varLimit.c_str()) / 2));

							//delete the previous
							tree.Remove(tempAnchor->min, tempAnchor->max, oneTimeEvent.front()->getInt("objid"));
						}
						else {
							hash_latestUpdate[validAnchor].first = oneTimeEvent.front()->getFloat("lat");
							hash_latestUpdate[validAnchor].second = oneTimeEvent.front()->getFloat("lon");

							hash_needUpdate[validAnchor] = false;
						}

						tempAnchor = new Rect(oneTimeEvent.front()->getFloat("lat") - (atof(varLimit.c_str()) / 2), oneTimeEvent.front()->getFloat("lon") - (atof(varLimit.c_str()) / 2)
							, oneTimeEvent.front()->getFloat("lat") + (atof(varLimit.c_str()) / 2), oneTimeEvent.front()->getFloat("lon") + (atof(varLimit.c_str()) / 2));

						//insert the new one
						tree.Insert(tempAnchor->min, tempAnchor->max, oneTimeEvent.front()->getInt("objid"));

						oneTimeEvent.pop();
						continue;
					}
					else {
						oneTimeEvent.pop();
						continue;
					}
				}
				//if it is not the anchor object
				else {
					tempAnchor = new Rect(oneTimeEvent.front()->getFloat("lat") - (atoi(varLimit.c_str()) / 2), oneTimeEvent.front()->getFloat("lon") - (atoi(varLimit.c_str()) / 2)
						, oneTimeEvent.front()->getFloat("lat") + (atoi(varLimit.c_str()) / 2), oneTimeEvent.front()->getFloat("lon") + (atoi(varLimit.c_str()) / 2));

					int objId = oneTimeEvent.front()->getInt("objid");

					nhits_vec = tree.Search_vec(tempAnchor->min, tempAnchor->max, ReturnVal, NULL);
					//nhits = tree.Search(tempAnchor->min, tempAnchor->max, ReturnVal, NULL);

					//--------------------------------------------------------------------------------
					Event* e;
					for (int i = 0; i < nhits_vec.size(); i++) {
						e = new Event(Utilities::id++, oneTimeEvent.front()->getInt("time"));

						char buff[4];
						e->addAttr(queryName, varCondition + " " + _itoa(getLimitFloat(), buff, 10));

						//e->addAttr("anchorid", oneTimeEvent.front()->getInt("objid"));
						//e->addAttr("objid", nhits_vec[i]);

						final_res.push(EventPtr(e));
					}
					//--------------------------------------------------------------------------------
					//there will be (i am working on it) 2 buffer right now
					//1. about
					//

					oneTimeEvent.pop();
					continue;
				}
			}
		}
		//------------------------------------------------------------------------------
		//querry time -------
		if (final_res.size() > 0)
			win->addResultEvent(final_res.back());

		//------------------------------------------------------------------------------

#pragma endregion
		*/


#pragma region ManualDistCalculation
	float res_calc;

	while (res.size() > 0) {
		int time_now = res.front()->getTime();

		//distinguish the anchor
		vector<EventPtr> one_time_event_a, one_time_event_t;
		while ( res.front()->getTime() == time_now) {
			//one_time_event.push_back(res.front());
			
			bool isAnchor = false;
			for (int i = 0; i < anchorObjList.size(); i++) {
				if (anchorObjList[i] == res.front()->getInt("objid")) {
					one_time_event_a.push_back(res.front());
					isAnchor = true;
				}
			}
			if (isAnchor) {
				res.pop();
				continue;
			}

			one_time_event_t.push_back(res.front());
			res.pop();

			if (res.size() <= 0)
				break;
		}

		//get the anchor
		for (int i = 0; i < one_time_event_a.size(); i++) {
			for (int j = 0; j < one_time_event_t.size(); j++) {
				res_calc = sqrt(pow((one_time_event_a[i]->getFloat("lat") - (one_time_event_t[j]->getFloat("lat"))), 2) 
					+ pow((one_time_event_t[j]->getFloat("lon") - (one_time_event_a[i]->getFloat("lon"))), 2));

				if (varCondition == "<" && res_calc < atof(varLimit.c_str())) {
					

					string tempAnchors;
					char buff[10];
					for (auto a : anchorObjList) {
						tempAnchors += _itoa(a, buff, 10);
						tempAnchors += ",";
					}
					one_time_event_a[i]->addAttr("anchors", tempAnchors);
					one_time_event_t[j]->addAttr("anchors", tempAnchors);

					win->addResultEvent(one_time_event_a[i]);
					win->addResultEvent(one_time_event_t[j]);
				}
			}
		}
	}

	//res_calc = sqrt(pow((b->getFloat("lat") - (a->getFloat("lat"))), 2) + pow((b->getFloat("lon") - (a->getFloat("lon"))), 2));

	//if (varCondition == "<" && res < atof(varLimit.c_str())) {
	//	Event* e = new Event(Utilities::id++, a->getInt("time"));
	//	e->addAttr("dist", res);
	//	e->addAttr("objLeft", a->getInt("objid"));
	//	e->addAttr("objright", b->getInt("objid"));

	//	return (EventPtr(e));
	//}
#pragma endregion



		return win->getFinalRes();
	}
	else if (queryName == "exist") {
	//at least there is one of it, so it is considered as true
		queue<EventPtr> res = win->getOriginalRes();

		if (res.size() > 0) {
			//Event* e = new Event(Utilities::id++, res.back()->getInt("time"));
			Event* e = new Event(Utilities::id++, win->getInitTime());
			win->addResultEvent(EventPtr(e));
		}
		
		return win->getFinalRes();
	}
	else if (queryName == "stay") {
		queue<EventPtr> res = win->getOriginalRes();

		//separate each event
		map<int, vector<int>> separatedEvents;
		for (; res.size() > 0; res.pop()) {
			//separatedEvents[res.front()->getInt("objid")].push(res.front());
			separatedEvents[res.front()->getInt("time")].push_back(res.front()->getInt("objid"));
		}

		int old_diff = separatedEvents.begin()->second.size();
		int new_diff = separatedEvents.begin()->second.size();
		int lastTime = 0;
		bool ischanged = false;

		for (auto& it : separatedEvents) {
			new_diff = it.second.size();
			if (old_diff != new_diff) { //this is the difference
				ischanged = true;
			}
			old_diff = new_diff;
			lastTime = it.first;
		}

		if (!ischanged) {
			//Event* e = new Event(Utilities::id++, lastTime);
			Event* e = new Event(Utilities::id++, win->getInitTime());
			win->addResultEvent(EventPtr(e));
		}

		return win->getFinalRes();
	}
	else if (queryName == "exit") {// exit, enter, and stay is pretty simillar
		queue<EventPtr> res = win->getOriginalRes();

		//separate each event
		map<int, vector<int>> separatedEvents;
		for (; res.size() > 0; res.pop()) {
			//separatedEvents[res.front()->getInt("objid")].push(res.front());
			separatedEvents[res.front()->getInt("time")].push_back(res.front()->getInt("objid"));
		}

		int old_diff = separatedEvents.begin()->second.size();
		int new_diff = separatedEvents.begin()->second.size();

		for (auto& it : separatedEvents) {
			new_diff = it.second.size();
			if (old_diff > new_diff) { // this is the difference
				//Event* e = new Event(Utilities::id++, it.first);
				Event* e = new Event(Utilities::id++, win->getInitTime());
				win->addResultEvent(EventPtr(e));
			}
			old_diff = new_diff;
		}

		return win->getFinalRes();
	}
	else if (queryName == "enter") { // exit, enter, and stay is pretty simillar
		queue<EventPtr> res = win->getOriginalRes();

		//separate each event
		map<int, vector<int>> separatedEvents;
		for (; res.size() > 0 ;res.pop()) {
			//separatedEvents[res.front()->getInt("objid")].push(res.front());
			separatedEvents[res.front()->getInt("time")].push_back(res.front()->getInt("objid"));
		}

		int old_diff = separatedEvents.begin()->second.size();
		int new_diff = separatedEvents.begin()->second.size();

		for (auto &it : separatedEvents) {
			new_diff = it.second.size();
			if (old_diff < new_diff) { // this is the difference
				//Event* e = new Event(Utilities::id++, it.first);
				Event* e = new Event(Utilities::id++, win->getInitTime());
				win->addResultEvent(EventPtr(e));
			}
			old_diff = new_diff;
		}

		return win->getFinalRes();
	}
	else if (queryName.find("count") > 0) {

	}

	return final_res;
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

			return (EventPtr(e));
		}
		else if (varCondition == "<=" && res <= atof(varLimit.c_str())) {
			Event* e = new Event(Utilities::id++, a->getInt("time"));
			e->addAttr("dist", res);

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
	//because it is latitude and longitude, so 0.1 lat or long equal with 10 km
	//float temp;
	//temp = atof(limit.c_str());
	//temp /= 100;
	//varLimit = to_string(temp);
	
	varLimit = limit;
}

void SpatialOp::setVarCondition(string cond)
{
	varCondition = cond;
}

void SpatialOp::anchorPush(vector<int> vec)
{
	//sort and distinct
	//sort(vec.begin(), vec.end());
	//vec.erase(unique(vec.begin(), vec.end()), vec.end());

	//duplicate check
	for (int j = 0; j < vec.size(); j++) {
		
		anchorObjList.push_back(vec[j]);
		//sort and distinct
		sort(anchorObjList.begin(), anchorObjList.end());
		anchorObjList.erase(unique(anchorObjList.begin(), anchorObjList.end()), anchorObjList.end());


		/*if (anchorObjList.size() == 0) {
			anchorObjList.push_back(vec[j]);
		}
		else {
			bool dup = false;
			for (int i = 0; i < anchorObjList.size(); i++) {
				if (anchorObjList[i] == vec[j]) {
					dup = true;
					break;
				}
			}
			if (!dup) {
				anchorObjList.push_back(vec[j]);
			}
		}*/
	}
}

int SpatialOp::isAnchor(int objId)
{
	for (int i = 0; i < anchorObjList.size(); i++) {
		if (anchorObjList[i] == objId)
			return i;
	}
	return -1;
}

float SpatialOp::getLimitFloat()
{
	return atof(varLimit.c_str());
}

bool SpatialOp::intersectLineSegment(EventPtr StartA, EventPtr EndA, EventPtr StartB, EventPtr EndB)
{
	pair<float, float> A({ StartA->getFloat("lat"), StartA->getFloat("lon") });
	pair<float, float> B({ EndA->getFloat("lat"), EndA->getFloat("lon") });
	pair<float, float> C({ StartB->getFloat("lat"), StartB->getFloat("lon") });
	pair<float, float> D({ EndB->getFloat("lat"), EndB->getFloat("lon") });

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
