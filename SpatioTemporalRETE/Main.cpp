#include <iostream>
#include <vector>
#include <string>
#include <list>

#define INDEXED_MODE
//#define NON_INDEXED_MODE

#include "ReteNet.h"

//using namespace std;

queue<EventPtr> generateSamepleLatLong(int number) {
	
	queue<EventPtr> ev;

	Event* e;
	//use system wise time
	int time_now = Utilities::time;

	for (int i = 0; i < number; i++) {
		e = new Event(Utilities::id++, time_now);
		if (i % 4 == 3) {
			Utilities::time++;
			time_now = Utilities::time;
		}

//#pragma region 2 ally 2 enemy
//		if (i % 2 == 0) {
//			e->addAttr("iff", "ally"); // scalar
//		}
//		else {
//			e->addAttr("iff", "enemy");
//		}
//#pragma endregion

#pragma region 1 ally 3 enemy
		if (i % 4 == 0) {
			e->addAttr("iff", "ally"); // scalar
		}
		else {
			e->addAttr("iff", "enemy");
		}
#pragma endregion

		e->addAttr("type", "recon"); //sclar
		e->addAttr("speed", (float)Utilities::randomFloat(3, 10)); //scalar
		//e->addAttr("lon", (float)Utilities::randomFloat(120, 133)); //spatial_original_range
		//e->addAttr("lat", (float)Utilities::randomFloat(30, 42)); //spatial_original_range
		e->addAttr("lon", (float)Utilities::randomFloat(120, 131)); //spatial
		e->addAttr("lat", (float)Utilities::randomFloat(30, 41)); //spatial
		float ele = (float)Utilities::randomFloat(0, 10);
		e->addAttr("elevation", ele); //scalar

		e->addAttr("objid", i % 4);

		ev.push(EventPtr(e));
	}

	int a;
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
		e->addAttr("speed", (float)Utilities::randomFloat(3, 20));
		e->addAttr("lon", (float)Utilities::randomFloat(127, 135));
		e->addAttr("lat", (float)Utilities::randomFloat(35, 44));
		//e->addAttr("dir", (float)Utilities::randomFloat(0, 360));
		float ele = (float)Utilities::randomFloat(0, 20);
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
	cout << "0. exit " << endl;
	cout << "1. manually input the rule " << endl;
	cout << "2. generate pre defined rule" << endl;
	cout << "3. generate random event" << endl;
	cout << "4. run rete" << endl;
	cin >> select;
	while (select <= 20) {
		switch (select) {
			cin.ignore();
		case 0: {
			return 0;
		}
		case 1: {
			system("cls");
			string temp, then_detect;
			vector<string> master_str;
			cout << "there are 9 attributes in each event, those are:" << endl;
			cout << "1. event id -> already fixed from system" << endl;
			cout << "2. time --> generated new time (seconds) each 4 events" << endl;
			cout << "3. iff --> contain ('ally', 'enemy')" << endl;
			cout << "4. type --> represent object type, only contain ('recon')" << endl;
			cout << "5. lon --> represent longitude or x axis, random range (127, 135)" << endl;
			cout << "6. lat --> represent latitude or y axis, random range (35, 44)" << endl;
			cout << "7. elevation --> repesent object elevation, random range (0,20)" << endl;
			cout << "8. objid --> represent object id, distributed evenly from 0 until 3" << endl<<endl;

			cout << "distance is calculated by: object_latitude - (latitude/2), object_latitude + (latitude/2), object_longitude - (longitude/2), object_longitude + (longitude/2) ---" << endl << endl;

			cout << "start with IF and finish with THEN" << endl << endl;
			cout << "example:" << endl;
			cout << "IF speed > 10 & elevation > 10 & iff=ally" << endl;
			cout << "THEN allyaircraft" << endl << endl << "or" << endl << endl;
			cout << "IF distance(allyaircraft, enemyvessel) < 10 & allyaircraft.type=recon" << endl;
			cout << "WINDOW range=5, trigger=5" << endl;
			cout << "THEN airthreatdetected" << endl << endl;
			cout << "your input:" << endl;

			cin.ignore();
			getline(cin, temp);
			if (temp.substr(0, 2) != "IF") {
				break;
			}
			else {
				master_str.push_back(temp);

				while (then_detect != "THEN") {
					//cin.ignore();
					getline(cin, temp);
					then_detect = temp.substr(0, 4);
					master_str.push_back(temp);
				}
			}

			vector<vector<pair<string, string>>> colMade;
			colMade = ReteNet::parseConditionOriginal(master_str);

			ReteNet::growTheNodes(colMade);

			system("pause");
			break;
		}
		case 2: {
			system("cls");
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
			cout << "THEN enemyvessel" << endl;

			ReteNet::growTheNodes(colMade);

			//SPATIAL IF ELSE
			made = {};
			made.push_back("IF distance(allyvessel,enemyvessel)<10 & allyvessel.type=recon");
			made.push_back("WINDOW range=5, trigger=5");
			made.push_back("THEN navalthreat, count.enemyvessel.objid");
			//made.push_back("THEN navalthreat");

			cout << endl << "IF distance(allyvessel,enemyvessel)<10 & allyvessel.type=recon" << endl;
			cout << "WINDOW range=5, trigger=5" << endl;
			cout << "THEN navalthreat, count.enemyvessel.objid" << endl;
			//cout << "THEN enemyvessel" << endl;

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			//Now Exist
			made = {};
			made.push_back("IF exist(navalthreat)");
			made.push_back("WINDOW range=10, trigger=10");
			made.push_back("THEN navalresponse");

			cout << endl << "IF exist(navalthreat)" << endl;
			cout << "WINDOW range=10, trigger=10" << endl;
			cout << "THEN navalresponse" << endl;

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			system("pause");
			break;
		}case 3: {
			system("cls");
			cout << endl;
			cout << "Generate WM EVENTS" << endl << endl;
			cout << "Enter the number of events : " << endl;
			int n;
			cin >> n;
			tempWM = generateSamepleLatLong(n);

			break;
		}case 4: {
			system("cls");
			ReteNet::buildNetNode();

			cout << endl << "Execute RETE" << endl << endl;

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
				ReteNet::SpatioTemporalExecution(100, oneTimeEvent.front()->getInt("time"));

				//tempWM.pop();

			}

			system("pause");
			break;
		}case 5: {
			system("cls");
			ReteNet::buildNetNode();

			cout << endl << "calculate time spent" << endl << endl;
			cout << "Number of nodes: " << ReteNet::GetNumberOfNodes() << endl;

			long long startTime = Utilities::getTime();

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
				ReteNet::SpatioTemporalExecution(100, oneTimeEvent.front()->getInt("time"));
				
			}
			long long timeSpent = startTime - Utilities::getTime();
			cout << "time spent: " << timeSpent << endl <<endl;
			system("pause");
		}case 6: {
			system("cls");
			cout << "Rules for checking multiple vessel distances" << endl;

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
			cout << "THEN enemyvessel" << endl;

			ReteNet::growTheNodes(colMade);

			//SPATIAL IF ELSE
			made = {};
			made.push_back("IF distance(allyvessel,enemyvessel)<12 & allyvessel.type=recon");
			made.push_back("WINDOW range=5, trigger=5");
			made.push_back("THEN navalthreat_far, count.enemyvessel.objid");

			cout << endl << "IF distance(allyvessel,enemyvessel)<12 & allyvessel.type=recon" << endl;
			cout << "WINDOW range=5, trigger=5" << endl;
			cout << "THEN navalthreat_far, count.enemyvessel.objid" << endl;

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			made = {};
			made.push_back("IF distance(allyvessel,enemyvessel)<10 & allyvessel.type=recon");
			made.push_back("WINDOW range=5, trigger=5");
			made.push_back("THEN navalthreat_med, count.enemyvessel.objid");

			cout << endl << "IF distance(allyvessel,enemyvessel)<10 & allyvessel.type=recon" << endl;
			cout << "WINDOW range=5, trigger=5" << endl;
			cout << "THEN navalthreat_med, count.enemyvessel.objid" << endl;

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			made = {};
			made.push_back("IF distance(allyvessel,enemyvessel)<7 & allyvessel.type=recon");
			made.push_back("WINDOW range=5, trigger=5");
			made.push_back("THEN navalthreat_close, count.enemyvessel.objid");

			cout << endl << "IF distance(allyvessel,enemyvessel)<8 & allyvessel.type=recon" << endl;
			cout << "WINDOW range=5, trigger=5" << endl;
			cout << "THEN navalthreat_close, count.enemyvessel.objid" << endl;

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			//Now Exist
			made = {};
			made.push_back("IF exit(navalthreat_far)");
			made.push_back("WINDOW range=12, trigger=12");
			made.push_back("THEN navalresponse_far");

			cout << endl << "IF exist(navalthreat_far)" << endl;
			cout << "WINDOW range=12, trigger=12" << endl;
			cout << "THEN navalresponse_far" << endl;

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			made = {};
			made.push_back("IF enter(navalthreat_med)");
			made.push_back("WINDOW range=10, trigger=10");
			made.push_back("THEN navalresponse_med");

			cout << endl << "IF exist(navalthreat_med)" << endl;
			cout << "WINDOW range=10, trigger=10" << endl;
			cout << "THEN navalresponse" << endl;

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			made = {};
			made.push_back("IF stay(navalthreat_close)");
			made.push_back("WINDOW range=8, trigger=8");
			made.push_back("THEN navalresponse_close");

			cout << endl << "IF exist(navalthreat_close)" << endl;
			cout << "WINDOW range=8, trigger=18" << endl;
			cout << "THEN navalresponse" << endl;

			colMade = ReteNet::parseConditionOriginal(made);

			ReteNet::growTheNodes(colMade);

			system("pause");
			break;
		}
		}

		system("cls");
		cout << "select what you want " << endl;
		cout << "0. exit " << endl;
		cout << "1. manually input the rule " << endl;
		cout << "2. generate pre defined rule" << endl;
		cout << "3. generate random event" << endl;
		cout << "4. run rete" << endl;
		cin >> select;

	}

	return 0;
}