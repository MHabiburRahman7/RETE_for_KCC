#include <iostream>
#include <vector>
#include <string>
#include <list>

#include "ReteNet.h"

#include "RTree.h"

using namespace std;

//struct Rect
//{
//	Rect() {}
//
//	Rect(float a_minX, float a_minY, float a_maxX, float a_maxY)
//		//Rect(int a_minX, int a_minY, int a_minZ, int a_maxX, int a_maxY, int a_maxZ)
//	{
//		min[0] = a_minX;
//		min[1] = a_minY;
//
//		max[0] = a_maxX;
//		max[1] = a_maxY;
//	}
//
//
//	float min[2];
//	float max[2];
//};

queue<EventPtr> generateSamepleLatLong(int number) {

	queue<EventPtr> ev;
	//scenario --> 4 vessel - 2 ally, 2 enemy

	Event* e;
	int time_now = 0;
	for (int i = 0; i < number; i++) {
		e = new Event(Utilities::id++, time_now);
		if (i % 4 == 3)
			time_now++;

		if (i % 2 == 0) {
			e->addAttr("iff", "ally");
		}
		else {
			e->addAttr("iff", "enemy");
		}

		e->addAttr("type", "recon");
		e->addAttr("lon", (float)Utilities::randomFloat(127, 129));
		e->addAttr("lat", (float)Utilities::randomFloat(35, 37));
		float ele = (float)Utilities::randomFloat(0, 10);

		e->addAttr("objid", i % 4);

		ev.push(EventPtr(e));
	}

	return ev;
}

bool MySearchCallback(int id, void* arg)
{
	printf("Hit data rect %d\n", id);
	return true; // keep going
}

int main() {

	queue<EventPtr> input = generateSamepleLatLong(32);

	float range = 0.9;

	RTree<int, float, 2, float> tree;

	//vector<pair<int, pair<float, float>>> latestAnchorPos = {};
	//pair<float, float> latestAnchorPos[5] = {};

	unordered_map<int, pair<float, float>> hash_latestUpdate;
	unordered_map<int, bool> hash_needUpdate;

	for (int i = 0; i < 5; i++) {
		hash_latestUpdate[i].first = 0;
		hash_latestUpdate[i].second = 0;

		hash_needUpdate[i] = true;
	}

	//bool needUpdate[5] = { true, true, true, true };

	vector<int>anchor = { 0,1 }, obj{ 2,3 };
	while (input.size() > 0) {
		int currTime = input.front()->getInt("time");

		//dummy for tree
		queue<EventPtr> oneTimeEvent = {};
		while(input.front()->getInt("time") == currTime) {
			oneTimeEvent.push(input.front());
			input.pop();
		}

		//decide the anchor
		// 0 1 --> anchor
		// 2 3 --> object
		Rect * tempAnchor;
		int nhits = 0;
		while (oneTimeEvent.size() > 0) {
			if (oneTimeEvent.front()->getInt("objid") == 0 || oneTimeEvent.front()->getInt("objid") == 1) {
				
				int objId = oneTimeEvent.front()->getInt("objid");
				//check obj position
				//Compare with the latest
				if (hash_latestUpdate[objId].first != 0 && hash_latestUpdate[objId].second != 0) {
					//calculate with prev position
					float lat, lon;
					lat = hash_latestUpdate[objId].first - oneTimeEvent.front()->getFloat("lat");
					lon = hash_latestUpdate[objId].second - oneTimeEvent.front()->getFloat("lon");

					if (abs(lat) > 0.05 && abs(lon) > 0.5) { //tolerance --> more than tolerance need update
						hash_needUpdate[objId] = true;
						//needUpdate[objId] = true;
					}
					else {
						hash_needUpdate[objId] = false;
						//needUpdate[objId] = false;
					}
				}

				//do we need to update?
				if (hash_needUpdate[objId] == true) {

					//the not the beginning
					if (hash_latestUpdate[objId].first != 0 && hash_latestUpdate[objId].second != 0) {
						tempAnchor = new Rect(hash_latestUpdate[objId].first - (range / 2), hash_latestUpdate[objId].second - (range / 2)
							, hash_latestUpdate[objId].first + (range / 2), hash_latestUpdate[objId].second + (range / 2));

						//delete the previous
						tree.Remove(tempAnchor->min, tempAnchor->max, oneTimeEvent.front()->getInt("objid"));
					}
					else {
						hash_latestUpdate[objId].first = oneTimeEvent.front()->getFloat("lat");
						hash_latestUpdate[objId].second = oneTimeEvent.front()->getFloat("lon");

						hash_needUpdate[objId] = false;
					}

					tempAnchor = new Rect(oneTimeEvent.front()->getFloat("lat") - (range / 2), oneTimeEvent.front()->getFloat("lon") - (range / 2)
						, oneTimeEvent.front()->getFloat("lat") + (range / 2), oneTimeEvent.front()->getFloat("lon") + (range / 2));

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
			else {
				tempAnchor = new Rect(oneTimeEvent.front()->getFloat("lat") - (range / 2), oneTimeEvent.front()->getFloat("lon") - (range / 2)
					, oneTimeEvent.front()->getFloat("lat") + (range / 2), oneTimeEvent.front()->getFloat("lon") + (range / 2));

				int objId = oneTimeEvent.front()->getInt("objid");

				nhits = tree.Search(tempAnchor->min, tempAnchor->max, MySearchCallback, NULL);

				oneTimeEvent.pop();
				continue;
			}
		}
	}

	return 0;
}