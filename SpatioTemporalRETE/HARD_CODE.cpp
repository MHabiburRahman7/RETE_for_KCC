#include <iostream>
#include <vector>
//#include "ReteNet.h"

#include "AlphaNode.h"
#include "BetaNode.h"
//#include "TerminalNode.h"
#include "Node.h"

using namespace std;

vector<int>alphaListIDDictionary;
vector<int>betaListIDDictionary;
vector<int>termListIDDictionary;
vector<Node*> NodeList;

queue<EventPtr> generateEvents() {
	queue<EventPtr> ev;

	Event* e;

	for (int i = 0; i < 100; i++) {
		e = new Event(Utilities::id++, i%50);

		if (i > 50) {
			e->addAttr("iff", "ally");
		}
		else {
			e->addAttr("iff", "enemy");
		}

		e->addAttr("type", "recon");
		e->addAttr("speed", (float)Utilities::randomFloat(-10, 10));
		e->addAttr("lon", (float)Utilities::randomFloat(127, 129));
		e->addAttr("lat", (float)Utilities::randomFloat(35, 37));
		e->addAttr("dir", (float)Utilities::randomFloat(0, 360));
		float ele = (float)Utilities::randomFloat(-300, 120);
		e->addAttr("elevation", ele);
		ev.push(EventPtr(e));
	}

	return ev;
}

//connect 3 nodes
void connectNodes(Node& n1, Node& n2, Node& n3) {
	if (static_cast<BetaNode*>(&n3)->getLeftConnNode() != NULL &&
		static_cast<BetaNode*>(&n3)->getRightConnNode() != NULL) {
		cout << "ALERT :" << static_cast<BetaNode*>(&n3)->getProduct() << " IS ALREADY OCCUPIED SOURCE" << endl;
		return;
	}

	//alpha - alpha - beta
	if (dynamic_cast<AlphaNode*>(&n1) && dynamic_cast<AlphaNode*>(&n2)
		&& dynamic_cast<BetaNode*>(&n3)) {

		static_cast<AlphaNode*>(&n1)->addBetaPair(static_cast<BetaNode*>(&n3));
		static_cast<AlphaNode*>(&n2)->addBetaPair(static_cast<BetaNode*>(&n3));

		static_cast<BetaNode*>(&n3)->setLeftConnection(&n1);
		static_cast<BetaNode*>(&n3)->setRightConnection(&n2);
	}
	//beta - alpha - beta
	else if (dynamic_cast<BetaNode*>(&n1) && dynamic_cast<AlphaNode*>(&n2)
		&& dynamic_cast<BetaNode*>(&n3)) {

		static_cast<BetaNode*>(&n1)->addBetaPair(static_cast<BetaNode*>(&n3));
		static_cast<AlphaNode*>(&n2)->addBetaPair(static_cast<BetaNode*>(&n3));

		static_cast<BetaNode*>(&n3)->setLeftConnection(&n1);
		static_cast<BetaNode*>(&n3)->setRightConnection(&n2);
	}
	//alpha - beta - beta
	else if (dynamic_cast<AlphaNode*>(&n1) && dynamic_cast<BetaNode*>(&n2)
		&& dynamic_cast<BetaNode*>(&n3)) {

		static_cast<AlphaNode*>(&n1)->addBetaPair(static_cast<BetaNode*>(&n3));
		static_cast<BetaNode*>(&n2)->addBetaPair(static_cast<BetaNode*>(&n3));

		static_cast<BetaNode*>(&n3)->setLeftConnection(&n1);
		static_cast<BetaNode*>(&n3)->setRightConnection(&n2);
	}
	//beta - beta - beta
	else if (dynamic_cast<BetaNode*>(&n1) && dynamic_cast<BetaNode*>(&n2)
		&& dynamic_cast<BetaNode*>(&n3)) {

		static_cast<BetaNode*>(&n1)->addBetaPair(static_cast<BetaNode*>(&n3));
		static_cast<BetaNode*>(&n2)->addBetaPair(static_cast<BetaNode*>(&n3));

		static_cast<BetaNode*>(&n3)->setLeftConnection(&n1);
		static_cast<BetaNode*>(&n3)->setRightConnection(&n2);
	}
	//terminal - beta - beta --> coming soon ~!!
};
//connect 2 nodes
void connectNodes(Node& n1, Node& n2) {
	if (static_cast<AlphaNode*>(&n2)->getPrevPair() != NULL) {
		cout << "ALERT :" << static_cast<AlphaNode*>(&n2)->getCondition() << " IS ALREADY OCCUPIED SOURCE" << endl;
		return;
	}

	if (dynamic_cast<AlphaNode*>(&n1) && dynamic_cast<AlphaNode*>(&n2)) {
		//static_cast<AlphaNode*>(&n1)->addBetaPair()
		static_cast<AlphaNode*>(&n1)->addBetaPair(&n2);

		static_cast<AlphaNode*>(&n2)->setPrevConnection(&n1);
	}
	else if (dynamic_cast<BetaNode*>(&n1) && dynamic_cast<AlphaNode*>(&n2)) {
		static_cast<BetaNode*>(&n1)->addBetaPair(&n2);

		static_cast<AlphaNode*>(&n2)->setPrevConnection(&n1);
	}
};

//class WorkingMemory
//{
//public:
//	WorkingMemory();
//	~WorkingMemory();
//
//private:
//
//};
//
//WorkingMemory::WorkingMemory()
//{
//}
//
//WorkingMemory::~WorkingMemory()
//{
//}

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
		//if (!static_cast<BetaNode*>(static_cast<BetaNode*>(pushedBeta[0])->getLeftConnNode())->isEmptyResult()
		//	&& !static_cast<BetaNode*>(static_cast<BetaNode*>(pushedBeta[0])->getRightConnNode())->isEmptyResult()) {

			pushedBeta[0]->testNode(timeSlice);

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
	tempNode = new BetaNode(5, "sp3el10 and iff=true then allyvessel");
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(5);

	tempNode = new BetaNode(6, "sp3el10 and iff=false then enemyvessel");
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(6);

	connectNodes(*NodeList[0], *NodeList[1], *NodeList[4]);
	connectNodes(*NodeList[2], *NodeList[4], *NodeList[5]); //allyvessel

	connectNodes(*NodeList[3], *NodeList[4], *NodeList[6]); //enemyvessel

	tempNode = new AlphaNode(7, "allyvessel.type=recon");
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(7);
	tempNode = new BetaNode(8, "distance(allyvessel,enemyvessel)<10");
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(8);
	tempNode = new BetaNode(9, "distance(allyvessel,enemyvessel)<10 and allyvessel.type=recon then navalthreat");
	static_cast<BetaNode*>(tempNode)->setMatchingKey("time");
	NodeList.push_back(tempNode);
	betaListIDDictionary.push_back(9);

	connectNodes(*NodeList[5], *NodeList[7]); //recon
	connectNodes(*NodeList[5], *NodeList[6], *NodeList[8]); //distance
	connectNodes(*NodeList[8], *NodeList[7], *NodeList[9]); //alert

	//tempNode = new AlphaNode(10, "exist(navalthreat)");
	//NodeList.push_back(tempNode);
	//alphaListIDDictionary.push_back(10);

	int a = 0;
	//generate events
	queue<EventPtr> wm = generateEvents();
	
	processRete(100, &wm);

	return 0;
}