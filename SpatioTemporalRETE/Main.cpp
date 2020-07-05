#include <iostream>
#include <vector>
#include <string>
#include <list>

#include "ReteNet.h"

using namespace std;

queue<EventPtr> generateSamepleLatLong(int number) {
	
	//queue<EventPtr> ev;
	////scenario --> 4 vessel - 2 ally, 2 enemy

	//Event* e;
	//int time_now = 0;
	//for (int i = 0; i < number; i++) {
	//	e = new Event(Utilities::id++, time_now);
	//	if (i % 4 == 3)
	//		time_now++;

	//	if (i % 2 == 0) {
	//		e->addAttr("iff", "ally");
	//	}
	//	else {
	//		e->addAttr("iff", "enemy");
	//	}

	//	e->addAttr("type", "recon");
	//	e->addAttr("lon", (float)Utilities::randomFloat(127, 129));
	//	e->addAttr("lat", (float)Utilities::randomFloat(35, 37));
	//	float ele = (float)Utilities::randomFloat(0, 10);

	//	e->addAttr("objid", i % 4);

	//	ev.push(EventPtr(e));
	//}

	//return ev;

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

queue<EventPtr> generateSample(int number) {
	queue<EventPtr> ev;

	Event* e;
	int time_now = 0;
	for (int i = 0; i < number; i++) {
		e = new Event(Utilities::id++, time_now);
		if (i % 2 == 1)
			time_now++;

		if (i % 2  == 0) {
			e->addAttr("iff", "ally");
		}
		else {
			e->addAttr("iff", "enemy");
		}

		e->addAttr("type", "recon");
		e->addAttr("speed", (float)Utilities::randomFloat(3, 10));
		e->addAttr("lon", (float)Utilities::randomFloat(127, 135));
		e->addAttr("lat", (float)Utilities::randomFloat(35, 44));
		//e->addAttr("dir", (float)Utilities::randomFloat(0, 360));
		float ele = (float)Utilities::randomFloat(0, 10);
		e->addAttr("elevation", ele);

		e->addAttr("objid", i % 4);

		ev.push(EventPtr(e));
	}

	//ReteNet::cloneToWm(ev);
	return ev;
}

int main() {

	int select;
	queue<EventPtr> tempWM;

	cout << "select what you want " << endl;
	cin >> select;
	while (select <= 20) {
		switch (select) {
		case 1: {
			
			//Test R-Tree



			break;
		}case 2:{
			cout << "Rules for checking distance" << endl;

			//BASIC IF ELSE
			vector<string> made;
			made.push_back("IF speed>3 & elevation<10 & iff=ally");
			made.push_back("THEN allyvessel");
			vector<vector<pair<string, string>>> colMade;
			colMade = ReteNet::parseConditionOriginal(made);

			cout << endl << "IF speed>3 & elevation<10 & iff=ally" << endl;
			cout << "THEN allyvessel" << endl;

			ReteNet::growTheNodes(colMade);

			made = {};
			made.push_back("IF speed>3 & elevation<10 & iff=enemy");
			made.push_back("THEN enemyvessel");
			colMade = ReteNet::parseConditionOriginal(made);

			cout << endl << "IF speed>3 & elevation<10 & iff=enemy" << endl;
			cout <<"THEN enemyvessel" << endl;

			ReteNet::growTheNodes(colMade);

			//SPATIAL IF ELSE
			made = {};
			made.push_back("IF distance(allyvessel,enemyvessel)<5 & allyvessel.type=recon");
			made.push_back("WINDOW range=5");
			made.push_back("THEN navalthreat");

			cout << endl << "IF distance(allyvessel,enemyvessel)<5 & allyvessel.type=recon" << endl;
			cout << "WINDOW range=5" << endl;
			cout << "THEN enemyvessel" << endl;

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			//Now Exist
			made = {};
			made.push_back("IF exist(navalthreat)");
			made.push_back("WINDOW range=10");
			made.push_back("THEN navalresponse");

			cout << endl << "IF exist(navalthreat)" << endl;
			cout << "WINDOW range=10" << endl;
			cout << "THEN navalresponse" << endl;

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			ReteNet::buildNetNode();

			cout << endl;
			cout << "Generate WM EVENTS" << endl;
			//tempWM = generateSample(1000);
			tempWM = generateSamepleLatLong(1000);

			cout <<endl<< "Execute RETE" << endl;

			while (tempWM.size() > 0) {

				/*
				//this is real time mode --------------------------------------------------
				EventPtr e = tempWM.front();
				queue<EventPtr> singlePoint = {};
				singlePoint.push(e);

				ReteNet::cloneToWm(singlePoint);
				//-------------------------------------------------------------------------
				*/

				//this is same-time mode --------------------------------------------------
				int currTime = tempWM.front()->getInt("time");

				queue<EventPtr> oneTimeEvent = {};
				while (tempWM.front()->getInt("time") == currTime) {
					oneTimeEvent.push(tempWM.front());
					tempWM.pop();

					if (tempWM.size() <= 0)
						break;
				}

				ReteNet::cloneToWm(oneTimeEvent);
				//-------------------------------------------------------------------------

				//do pre-processing
				ReteNet::ExecuteRete(100);

				//now the spatio temporal thing
				ReteNet::SpatioTemporalExecution(100);


				tempWM.pop();
			}

			break;
		}case 3: {

			// hmmm, i doubt it :"

			cout << "Rules for new rete" << endl;

			//BASIC IF ELSE
			vector<string> made;
			made.push_back("IF speed>3 & elevation<10 & iff=ally");
			made.push_back("THEN allyvessel");
			vector<vector<pair<string, string>>> colMade;
			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			made = {};
			made.push_back("IF speed>3 & elevation<10 & iff=enemy");
			made.push_back("THEN enemyvessel");
			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);
			break;
		}case 4: {
			
			break;
		}case 5: {
			
			break;
		}
		}

		cout << "select what you want " << endl;
		cin >> select;
	}

	return 0;
}