#include <iostream>
#include <vector>
#include <string>
#include <list>

#include "ReteNet.h"

using namespace std;

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
		e->addAttr("lon", (float)Utilities::randomFloat(127, 129));
		e->addAttr("lat", (float)Utilities::randomFloat(35, 37));
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
			string temp, finalTerm;

			cout << "Static Pre Defined Rule" << endl;
			
			//getline(cin, temp);

			vector<string> made;
			//made.push_back("IF DISTANCE(A,B) < 100 & a.speed > 50 & b.type=recon & elevation > 100 & lala");
			//made.push_back("IF a.speed > 50");
			//made.push_back("IF a.speed > 50 & elevation > 100");
			made.push_back("IF a.speed > 50 & elevation > 100 & iff = ally");
			made.push_back("WINDOW range = 10");
			made.push_back("THEN result");

			vector<vector<pair<string, string>>> colMade;
			colMade = ReteNet::parseConditionOriginal(made);
			
			ReteNet::growTheNodes(colMade);

			cout << made[0] << endl;
			//cin.ignore();

			break;
		}case 2:{
			cout << "Rules for checking distance" << endl;

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

			//SPATIAL IF ELSE
			made = {};
			made.push_back("IF distance(allyvessel,enemyvessel)<10 & allyvessel.type=recon");
			made.push_back("WINDOW range=5");
			made.push_back("THEN navalthreat");

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			//Now Exist
			made = {};
			made.push_back("IF exist(navalthreat)");
			made.push_back("WINDOW range=10");
			made.push_back("THEN navalresponse");

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			ReteNet::buildNetNode();

			break;
		}case 3: {
			cout << "Building Node" << endl;
			ReteNet::buildNetNode();
			break;
		}case 4: {
			cout << "Generate WM EVENTS" << endl;
			tempWM = generateSample(1000);
			break;
		}case 5: {
			cout << "Execute RETE" << endl;

			while (tempWM.size() > 0) {
				EventPtr e = tempWM.front();
				queue<EventPtr> singlePoint = {};
				singlePoint.push(e);

				ReteNet::cloneToWm(singlePoint);

				ReteNet::ExecuteRete(100);

				tempWM.pop();
			}
			break;
		}
		}

		cout << "select what you want " << endl;
		cin >> select;
	}

	return 0;
}