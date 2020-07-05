#include <iostream>
#include <vector>

#include "AlphaNode.h"
#include "BetaNode.h"

#include "Node.h"

#include "RTree.h"

using namespace std;

vector<int>alphaListIDDictionary;
vector<int>betaListIDDictionary;
vector<Node*> NodeList;

using namespace std;

struct RectSpatial
{
	RectSpatial() {}

	RectSpatial(float a_minX, float a_minY, float a_maxX, float a_maxY, Node* corr_node)
		//Rect(int a_minX, int a_minY, int a_minZ, int a_maxX, int a_maxY, int a_maxZ)
	{
		min[0] = a_minX;
		min[1] = a_minY;

		max[0] = a_maxX;
		max[1] = a_maxY;

		node_handler = corr_node;
	}

	Node* node_handler;
	float min[2];
	float max[2];
};

bool ReturnValHere(int id, void* arg)
{
	//dynamic_cast<Event*>(arg)->addAttr("leftobj", id);
	//printf("Hit data rect %d\n", id);
	return true; // keep going
}

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
			e->addAttr("iff", "ally"); // scalar
		}
		else {
			e->addAttr("iff", "enemy");
		}

		e->addAttr("type", "recon"); //sclar
		e->addAttr("speed", (float)Utilities::randomFloat(3, 10)); //scalar
		e->addAttr("lon", (float)Utilities::randomFloat(120, 133)); //spatial
		e->addAttr("lat", (float)Utilities::randomFloat(30, 42)); //spatial
		float ele = (float)Utilities::randomFloat(0, 10); 
		e->addAttr("elevation", ele); //scalar

		e->addAttr("objid", i % 4);

		ev.push(EventPtr(e));
	}

	return ev;
}

void connectNodes(Node& n1, Node& n2, Node& n3)
{
	int danger = 0;
	bool left = true, right = true;
	if (static_cast<BetaNode*>(&n3)->getLeftConnNode() != NULL && &n1 != NULL) {
		danger++;
		cout << "ALERT, LEFT OF :" << static_cast<BetaNode*>(&n3)->getProduct() << " IS REPLACED" << endl;
	}
	if (static_cast<BetaNode*>(&n3)->getRightConnNode() != NULL && &n2 != NULL) {
		danger++;
		cout << "ALERT, RIGHT OF :" << static_cast<BetaNode*>(&n3)->getProduct() << " IS REPLACED" << endl;
	}

	if (danger >= 2) {
		cout << "ALERT, BOTH OF :" << static_cast<BetaNode*>(&n3)->getProduct() << " IS REPLACED" << endl;
		return;
	}

	//this should be exception, but i will think about this later
	/*if (&n1 != NULL) {
		n1.addBetaPair(static_cast<BetaNode*>(&n3));
	}
	if (&n2 != NULL) {
		n2.addBetaPair(static_cast<BetaNode*>(&n3));
	}*/

	n1.addBetaPair(static_cast<BetaNode*>(&n3));
	n2.addBetaPair(static_cast<BetaNode*>(&n3));

	static_cast<BetaNode*>(&n3)->setLeftConnection(&n1);
	static_cast<BetaNode*>(&n3)->setRightConnection(&n2);
}

void connectNodes(Node& n1, Node& n2)
{
	if (static_cast<AlphaNode*>(&n2)->getPrevPair() != NULL) {
		cout << "ALERT :" << static_cast<AlphaNode*>(&n2)->getCondition() << " IS ALREADY OCCUPIED SOURCE" << endl;
		return;
	}

	n1.addBetaPair(&n2);

	static_cast<AlphaNode*>(&n2)->setPrevConnection(&n1);
}

Node* findNode(string expression, int nodeType)
{
	// 0 --> alpha
	// 1 --> beta

	//if (expression.size() > 0 && expression[0] == ' ') { // if the beginning contain space
	//	expression = expression.erase(0,1); //only erase the str 0 until 1
	//}
	//
	//if (expression.size() > 0 && expression[expression.size()-1] == ' ') { // if the end contain space
	//	expression = expression.erase(expression.size() - 1);
	//}

	if (nodeType == 0) {
		for (int i = 0; i < alphaListIDDictionary.size(); i++) {
			if (NodeList[alphaListIDDictionary[i]]->justCondition == expression) {
				return NodeList[alphaListIDDictionary[i]];
			}
		}
	}
	else if (nodeType == 1) {
		for (int i = 0; i < betaListIDDictionary.size(); i++) {
			if (static_cast<BetaNode*>(NodeList[betaListIDDictionary[i]])->getProduct() == expression) {
				return NodeList[betaListIDDictionary[i]];
			}
		}
	}
	return nullptr;
}

void buildNetNode()
{
	//check available Nodes
	for (int i = 0; i < NodeList.size(); i++) {
		if (NodeList[i]->prevNodeAlreadyConnected())
			continue;

		if (dynamic_cast<AlphaNode*>(NodeList[i])) {
			if (static_cast<AlphaNode*>(NodeList[i])->getPrevPairStr() == "")
				continue;
			else {
				Node* prev;
				//check alpha
				prev = findNode(static_cast<AlphaNode*>(NodeList[i])->getPrevPairStr(), 0);
				if (prev) {
					connectNodes(*NodeList[i], *prev);
					continue;
				}

				//check beta
				prev = findNode(static_cast<AlphaNode*>(NodeList[i])->getPrevPairStr(), 1);
				if (prev) {
					connectNodes(*prev, *NodeList[i]);
					continue;
				}

				//it there are no option, then make a new one
				//prev = addAlphaReturnNode(static_cast<AlphaNode*>(NodeList[i])->getPrevPairStr());
				//connectNodes()
			}
		}
		else if (dynamic_cast<BetaNode*>(NodeList[i])) {
			Node* prevL, * prevR;

			//alpha - alpha
			prevL = findNode(static_cast<BetaNode*>(NodeList[i])->getLeftConnName(), 0);
			prevR = findNode(static_cast<BetaNode*>(NodeList[i])->getRightConnName(), 0);
			if (prevL && prevR) {
				connectNodes(*prevL, *prevR, *NodeList[i]);
			}

			//alpha - beta
			prevL = findNode(static_cast<BetaNode*>(NodeList[i])->getLeftConnName(), 0);
			prevR = findNode(static_cast<BetaNode*>(NodeList[i])->getRightConnName(), 1);
			if (prevL && prevR) {
				connectNodes(*prevL, *prevR, *NodeList[i]);
			}

			//beta - alpha
			prevL = findNode(static_cast<BetaNode*>(NodeList[i])->getLeftConnName(), 1);
			prevR = findNode(static_cast<BetaNode*>(NodeList[i])->getRightConnName(), 0);
			if (prevL && prevR) {
				connectNodes(*prevL, *prevR, *NodeList[i]);
			}

			//beta - beta
			prevL = findNode(static_cast<BetaNode*>(NodeList[i])->getLeftConnName(), 1);
			prevR = findNode(static_cast<BetaNode*>(NodeList[i])->getRightConnName(), 1);
			if (prevL && prevR) {
				connectNodes(*prevL, *prevR, *NodeList[i]);
			}

			continue;
		}
		else
			continue;
	}
	int a = 12;
}

void processRete(int timeSlice, queue<EventPtr>* ev)
{
	/*if (m_WMSet.getWMInputQueue().size() < 1)
		return;*/

	vector<pair<Node*, int>> visitedMark;
	vector<Node*> pushedBeta;

	bool isDone = false;

	//activate all alpha
	for (int j = 0; j < alphaListIDDictionary.size(); j++) {
		queue<EventPtr> local_event = *ev;

		//test on alpha
		if (static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->ItIsDirect());
		static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->testAlphaAndPush(&local_event, timeSlice);

		//activate the beta
		//searching all pair and push it
		for (int k = 0; k < static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->getAllPairs().size(); k++) {

			//check for duplicate
			bool isADuplicate = false;
			for (int l = 0; l < pushedBeta.size(); l++) {
				if (pushedBeta[l] == static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->getSinglePair(k)) {
					isADuplicate = true;
					break;
				}
			}
			if (!isADuplicate) {
				pushedBeta.push_back(static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->getSinglePair(k));
				visitedMark.push_back({ static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->getSinglePair(k), 0 });
			}
		}

		//break;
	}

	//BFS based on Queue
	while (!pushedBeta.empty()) {
		
		//This is the borderline ----------------------------------------------------------------------------
		//let the spatial rule processed outside
		if (dynamic_cast<BetaNode*>(pushedBeta[0])) {
			if (dynamic_cast<BetaNode*>(pushedBeta[0])->getSpecialOpName() != "") {
				pushedBeta.erase(pushedBeta.begin());
				continue;
			}
		}

		int res = pushedBeta[0]->testNode(timeSlice);

		if (res == 0) {
			pushedBeta.erase(pushedBeta.begin());
			continue;
		}

		//Activate successor node
		for (int i = 0; i < pushedBeta[0]->getAllPairs().size(); i++) {

			//Check to avoid duplication
			bool isDuplicate = false;
			for (int j = 0; j < pushedBeta.size(); j++) {
				if (pushedBeta[0]->getSinglePair(i) == pushedBeta[j]) {
					isDuplicate = true;
					break;
				}
			}
			if (!isDuplicate)
				pushedBeta.push_back(pushedBeta[0]->getSinglePair(i));
		}

		//Mark the current node in visited mark
		for (int i = 0; i < visitedMark.size(); i++) {
			if (visitedMark[i].first == pushedBeta[0]) {
				visitedMark[i].second++;

				//Evaluate visited mark
				if (visitedMark[i].second >= 3)
					isDone = true;
				break;
			}
		}
		//}

		pushedBeta.erase(pushedBeta.begin());

		if (isDone)
			break;
	}
	int a = 0;
}

int main() {
	
	Node* tempNode;
	tempNode = new AlphaNode(0, "speed>3");
	NodeList.push_back(tempNode);
	alphaListIDDictionary.push_back(0);
	tempNode = new AlphaNode(1, "elevation<10");
	NodeList.push_back(tempNode);
	alphaListIDDictionary.push_back(1);
	tempNode = new AlphaNode(2, "iff=ally");
	NodeList.push_back(tempNode);
	alphaListIDDictionary.push_back(2);
	tempNode = new AlphaNode(3, "iff=enemy");
	alphaListIDDictionary.push_back(3);
	NodeList.push_back(tempNode);

	tempNode = new BetaNode(4, "speed>3 and elevation<10 then sp3el10");
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(4);
	tempNode = new BetaNode(5, "sp3el10 and iff=ally then allyvessel");
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(5);

	tempNode = new BetaNode(6, "sp3el10 and iff=enemy then enemyvessel");
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(6);

	connectNodes(*NodeList[0], *NodeList[1], *NodeList[4]);
	connectNodes(*NodeList[2], *NodeList[4], *NodeList[5]); //allyvessel

	connectNodes(*NodeList[3], *NodeList[4], *NodeList[6]); //enemyvessel

	tempNode = new AlphaNode(7, "allyvessel.type=recon");
	NodeList.push_back(tempNode);
	alphaListIDDictionary.push_back(7);
	tempNode = new BetaNode(8, "distance(allyvessel,enemyvessel)<10");
	static_cast<BetaNode*>(tempNode)->setMatchingKey("time");
	static_cast<BetaNode*>(tempNode)->setWindow(10, 0);
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(8);
	tempNode = new BetaNode(9, "distance(allyvessel,enemyvessel)<10 and allyvessel.type=recon then navalthreat_far"); //---------->
	static_cast<BetaNode*>(tempNode)->setMatchingKey("time");
	static_cast<BetaNode*>(tempNode)->setWindow(10,0);
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(9);

	tempNode = new BetaNode(10, "distance(allyvessel,enemyvessel)<8 and allyvessel.type=recon then navalthreat_medium"); //------------>
	static_cast<BetaNode*>(tempNode)->setMatchingKey("time");
	static_cast<BetaNode*>(tempNode)->setWindow(8, 0);
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(10);

	tempNode = new BetaNode(11, "distance(allyvessel,enemyvessel)<5 and allyvessel.type=recon then navalthreat_close"); //---------->
	static_cast<BetaNode*>(tempNode)->setMatchingKey("time");
	static_cast<BetaNode*>(tempNode)->setWindow(5, 0);
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(11);

	tempNode = new BetaNode(12, "distance(allyvessel,enemyvessel)<8");
	static_cast<BetaNode*>(tempNode)->setMatchingKey("time");
	static_cast<BetaNode*>(tempNode)->setWindow(8, 0);
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(12);

	tempNode = new BetaNode(13, "distance(allyvessel,enemyvessel)<5");
	static_cast<BetaNode*>(tempNode)->setMatchingKey("time");
	static_cast<BetaNode*>(tempNode)->setWindow(5, 0);
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(13);

	connectNodes(*NodeList[5], *NodeList[7]); //recon
	connectNodes(*NodeList[5], *NodeList[6], *NodeList[8]); //distance
	connectNodes(*NodeList[5], *NodeList[6], *NodeList[12]); //distance
	connectNodes(*NodeList[5], *NodeList[6], *NodeList[13]); //distance

	connectNodes(*NodeList[8], *NodeList[7], *NodeList[9]); //alert
	connectNodes(*NodeList[12], *NodeList[7], *NodeList[10]); //alert
	connectNodes(*NodeList[13], *NodeList[7], *NodeList[11]); //alert

	buildNetNode();

	//tempNode = new AlphaNode(14, "exist(navalthreat)");
	//NodeList.push_back(tempNode);
	//alphaListIDDictionary.push_back(14);

	//filter the spatial node that we can do indexing
	vector<Node*> nodewithspatialIndexing;
	//list the anchor class
	//vector format --> left input, right input, node id
	vector<pair<pair<string, string>, int>> vec_anchor_id;
	for (int i = 0; i < betaListIDDictionary.size(); i++) {
		if (static_cast<BetaNode*>(NodeList[betaListIDDictionary[i]])->getSpecialOpName() != "") {
			nodewithspatialIndexing.push_back(NodeList[betaListIDDictionary[i]]);
			vec_anchor_id.push_back({ {static_cast<BetaNode*>(NodeList[betaListIDDictionary[i]])->getLeftConnName(), static_cast<BetaNode*>(NodeList[betaListIDDictionary[i]])->getRightConnName()}, NodeList[betaListIDDictionary[i]]->getID() });
		}
	}
	////Let's see, what node that considered as anchors?
	//for (int i = 0; i < nodewithspatialIndexing.size(); i++) {
	//	anchor_node.push_back(static_cast<BetaNode*>(nodewithspatialIndexing[i])->getLeftConnNode());

	//	//distinct enemy event
	//	sort(anchor_node.begin(), anchor_node.end());
	//	anchor_node.erase(unique(anchor_node.begin(), anchor_node.end()), anchor_node.end());
	//}

	//this is the event --> gajadi deh

	//hash the object node ids
	//unordered_map<int, vector<pair<float, float>>> hash_latestUpdate;
	//unordered_map<int, bool> hash_needUpdate;

	//generate events
	queue<EventPtr> wm = generateSamepleLatLong(1000);

	int event_count = 0;
	while (wm.size() > 900) {
		
		int currTime = wm.front()->getInt("time");

		//dummy for tree
		queue<EventPtr> oneTimeEvent = {};
		while (wm.front()->getInt("time") == currTime) {
			oneTimeEvent.push(wm.front());
			wm.pop();

			if (wm.size() <= 0)
				break;
		}

		processRete(100, &oneTimeEvent);
		//now corresponding node (allyvessel & enemy vessel has value in it)

		//RTree<int, float, 4, float> tree_scalar; // this one responsible for scalar node indexing --> later
		RTree<int, float, 2, float> tree; // this one responsible for spatial node indexing

		//ok, now lets see, does the events are same?
		//ok, ok, just consider all of them are allyvessel ._. --> i think we should extract the events first ._.
		for (int j = 0; j < nodewithspatialIndexing.size(); j++) {
			//argh cannot think ._.

		}

		//lets try to process the ally first --> 2 ally 2 enemy --> these event happened at time t
		//onetime event contain 2 ally & 2 enemy respectively
		for (int j = 0; j < vec_anchor_id.size(); j++) {
			if (vec_anchor_id[j].first.first == "allyvessel") // ok ok , it is separated perfectly --> actually this make the output double ._.
			{
				//get current event's position
				queue<EventPtr> anchorEventQueue = static_cast<BetaNode*>(nodewithspatialIndexing[j])->getLeftInput();

				//first time, everything is new
				//hash_needUpdate[nodewithspatialIndexing[j]->getID()] = true;
				for (; anchorEventQueue.size()>0;) {

					float xpos[2], ypos[2];

					//just initiated
					//if (hash_needUpdate[j] == true && hash_latestUpdate[j].first == 0) {
						xpos[0] = anchorEventQueue.front()->getFloat("lat") - (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);
						xpos[1] = anchorEventQueue.front()->getFloat("lat") + (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);

						ypos[0] = anchorEventQueue.front()->getFloat("lon") - (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);
						ypos[1] = anchorEventQueue.front()->getFloat("lon") + (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);

						tree.Insert(xpos, ypos, nodewithspatialIndexing[j]->getID());
					//}
					//really need update
					//else if (hash_needUpdate[j] == true) {

					//}
					//else {

					//}

					

					//anchor always on the left --> it is already inside the node
					/*if (dynamic_cast<BetaNode*>(nodewithspatialIndexing[j])) {
						dynamic_cast<BetaNode*>(nodewithspatialIndexing[j])->forcePushInQueue(&anchorEventQueue.front(), true);
					}*/
					anchorEventQueue.pop();
				}
			}
		}
		//the tree is set now
		
		//now test with the enemy
		vector<Node*> pushedNode = {};
		vector<EventPtr> distinctEnemyVector = {};
		vector<int> nhits_vec;
		for (int j = 0; j < vec_anchor_id.size(); j++) {
			if (vec_anchor_id[j].first.second == "enemyvessel") // ok ok , it is separated perfectly
			{
				queue<EventPtr> testEventQueue = static_cast<BetaNode*>(nodewithspatialIndexing[j])->getRightInput();

				for (; testEventQueue.size() > 0; testEventQueue.pop()) {
					distinctEnemyVector.push_back(testEventQueue.front());

					//distinct enemy event
					sort(distinctEnemyVector.begin(), distinctEnemyVector.end());
					distinctEnemyVector.erase(unique(distinctEnemyVector.begin(), distinctEnemyVector.end()), distinctEnemyVector.end());
				}
			}
		}
		
		//------------------------------------------------------------------------------------------------
		//after it is dinctinct, so we can process the stabbing process
		for (int i = 0; i < distinctEnemyVector.size(); i++) {
			float xpos[2], ypos[2];
			xpos[0] = xpos[1] = distinctEnemyVector[i]->getFloat("lat");
			ypos[0] = ypos[1] = distinctEnemyVector[i]->getFloat("lon");

			nhits_vec = tree.Search_vec(xpos, ypos, ReturnValHere, NULL);

			//distinct the correlated beta nodes that hit
			sort(nhits_vec.begin(), nhits_vec.end());
			nhits_vec.erase(unique(nhits_vec.begin(), nhits_vec.end()), nhits_vec.end());

			//push this into rete net
			//this must be on the right
			for (int j = 0; j < nhits_vec.size(); j++) {
				Node* tempNode = NodeList[nhits_vec[j]];
				if (dynamic_cast<BetaNode*>(tempNode)) {
					dynamic_cast<BetaNode*>(tempNode)->forcePushInQueue(&distinctEnemyVector[i], false);

					pushedNode.push_back(tempNode);

					//sort and distinct
					sort(pushedNode.begin(), pushedNode.end());
					pushedNode.erase(unique(pushedNode.begin(), pushedNode.end()), pushedNode.end());
				}
			}
		}

		while (!pushedNode.empty()) {
			//Maybe this is the indexing?
			int testStatus = pushedNode[0]->testNode(10);

			if (testStatus == 0) {
				pushedNode.erase(pushedNode.begin());
				continue;
			}

			//Activate successor node	
			for (int i = 0; i < pushedNode[0]->getAllPairs().size(); i++) {

				//Check to avoid duplication
				bool isDuplicate = false;
				for (int j = 0; j < pushedNode.size(); j++) {
					if (pushedNode[0]->getSinglePair(i) == pushedNode[j]) {
						isDuplicate = true;
						break;
					}
				}
				if (!isDuplicate)
					pushedNode.push_back(pushedNode[0]->getSinglePair(i));
			}

			pushedNode.erase(pushedNode.begin());
		}

		//due to constant movement, so just format the tree

		tree.RemoveAll();
		int a = 0;

	}

	//Lets talk about spatial node indexing 
	//format (lat, long, anchorObjName, testObjName, corrNodes[]);

	//non spatial node indexing will be 4 dimension
	//format (speed, elevation, iff, type)

	RTree<int, float, 4, float> tree_scalar; // this one responsible for scalar node indexing

	RTree<int, float, 2, float> tree; // this one responsible for spatial node indexing

	
#pragma region RegionForceInput
	//consider the separation is done before
	vector<EventPtr> enemyVess, allyVess;
	for (; wm.size() > 0; wm.pop()) {
		EventPtr originalFrontEvent = wm.front();
		if (originalFrontEvent->getString("iff") == "ally") {
			allyVess.push_back(originalFrontEvent);
		}
		else {
			enemyVess.push_back(originalFrontEvent);
		}
	}

	//err, again force push the join node
	for (int i = 0; i < 2; i++) {
		for (int j = 9; j <= 11; j++) { //-------> related node id
			dynamic_cast<BetaNode*>(NodeList[j])->forcePushInQueue(&allyVess[i], false);
		}
	}
#pragma endregion


#pragma region SpatioTemporalIndexing
	//dinstinct the event processing range --> only process event that happened at time x
	
	//lets try to process the ally first --> 2 ally 2 enemy --> these event happened at time 0
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < vec_anchor_id.size(); j++) {
			//if(vec_anchor_id[i].first == allyVess) // ok ok , it is separated perfectly

			float xpos[2], ypos[2];
			xpos[0] = allyVess[i]->getFloat("lat") - (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);
			xpos[1] = allyVess[i]->getFloat("lat") + (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);

			ypos[0] = allyVess[i]->getFloat("lon") - (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);
			ypos[1] = allyVess[i]->getFloat("lon") + (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);

			tree.Insert(xpos, ypos, nodewithspatialIndexing[j]->getID());

			//anchor always on the left
			if (dynamic_cast<BetaNode*>(nodewithspatialIndexing[j])) {
				dynamic_cast<BetaNode*>(nodewithspatialIndexing[j])->forcePushInQueue(&allyVess[i], true);
			}
		}
	}

	//now test with the enemy
	vector<Node*> pushedNode = {};
	vector<int> nhits_vec;
	//-------------------------------------------------------------
	for (int i = 0; i < 2; i++) {

		float xpos[2], ypos[2];
		xpos[0] = xpos[1] = enemyVess[i]->getFloat("lat");
		ypos[0] = ypos[1] = enemyVess[i]->getFloat("lon");

		cout << *enemyVess[i] << endl;
		nhits_vec = tree.Search_vec(xpos, ypos, ReturnValHere, NULL);

		//distinct the correlated beta nodes that hit
		sort(nhits_vec.begin(), nhits_vec.end());
		nhits_vec.erase(unique(nhits_vec.begin(), nhits_vec.end()), nhits_vec.end());

		//push this into rete net
		//this must be on the right
		for (int j = 0; j < nhits_vec.size(); j++) {
			Node* tempNode = NodeList[nhits_vec[j]];
			if (dynamic_cast<BetaNode*>(tempNode)) {
				dynamic_cast<BetaNode*>(tempNode)->forcePushInQueue(&enemyVess[i], false);
				
				pushedNode.push_back(tempNode);

				//sort and distinct
				sort(pushedNode.begin(), pushedNode.end());
				pushedNode.erase(unique(pushedNode.begin(), pushedNode.end()), pushedNode.end());
			}
		}
	}

	while (!pushedNode.empty()) {
		//Maybe this is the indexing?
		int testStatus = pushedNode[0]->testNode(10);

		if (testStatus == 0) {
			pushedNode.erase(pushedNode.begin());
			continue;
		}

		//Activate successor node	
		for (int i = 0; i < pushedNode[0]->getAllPairs().size(); i++) {

			//Check to avoid duplication
			bool isDuplicate = false;
			for (int j = 0; j < pushedNode.size(); j++) {
				if (pushedNode[0]->getSinglePair(i) == pushedNode[j]) {
					isDuplicate = true;
					break;
				}
			}
			if (!isDuplicate)
				pushedNode.push_back(pushedNode[0]->getSinglePair(i));
		}

		pushedNode.erase(pushedNode.begin());
	}
	int a = 0;

	//run the rete;


	int b = 10;
#pragma endregion

	

	
	
	//-----------------------------------------------------------------



	//-----------------------------------------------------------------

	return 0;
}