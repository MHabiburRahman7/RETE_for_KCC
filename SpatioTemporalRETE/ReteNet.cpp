#include "ReteNet.h"

WorkingMemory ReteNet::m_WMSet;

vector<int>ReteNet::alphaListIDDictionary;
vector<int>ReteNet::betaListIDDictionary;
vector<int>ReteNet::termListIDDictionary;
vector<Node*> ReteNet::NodeList;

//this is spatio temporal
vector<Node*> ReteNet::nodewithspatialIndexing;
vector<pair<pair<string, string>, int>> ReteNet::vec_anchor_id;
unordered_map<string, vector<string>> ReteNet::anchor_stab_map;
vector<string> ReteNet::observed_obj_dict;

vector<vector<pair<string, string>>> ReteNet::parseConditionOriginal(vector<string> condList)
{
	//Parsing parsing~
	vector<vector<pair<string, string>>> collectedMade;
	vector<string> newInputForm;

	for (auto v : condList) {
		newInputForm.push_back(v);
	}

	for (string s : newInputForm) {
		vector<pair<string, string>> SingleExpressionMade;

		s = Utilities::ToLower(Utilities::trim(s));
		if (s.size() == 0 || s[0] == '#') continue;

		size_t index = s.find_first_of(" ", 0);
		if (index < 0) {
			std::cout << "no clause";
			throw runtime_error("no clause");
		}

		string clause = s.substr(0, index);
		string value = s.substr(index + 1, s.size() - index - 1);
		Utilities::deleteAllMark(value, " ");

		if ("if" == clause) {
			tokenizeMultiExpCEP(value, SingleExpressionMade);
		}
		else if ("window" == clause) {//Window length=, sliding=
			
			vector<string> temp_string = Utilities::splitDelim(value, ",");

			for (int i = 0; i < temp_string.size(); i++) {
				vector<string> another_string = Utilities::splitDelim(temp_string[i], "=");
				SingleExpressionMade.push_back({ another_string[0],another_string[1] });
			}
		}
		else if ("then" == clause) {//then = 
			SingleExpressionMade.push_back({ "term", value });
		}

		collectedMade.push_back(SingleExpressionMade);
	}
	int a;

	return collectedMade;
}

void ReteNet::tokenizeMultiExpCEP(string input, vector<pair<string, string>>& res)
{
	vector<string> expressionList = Utilities::splitDelim(input, "&");
	if (expressionList.size() == 1) {
		res.push_back({ "alpha", expressionList[0] });
	}
	else if (expressionList.size() > 0) {
		for (string expression : expressionList) {

			int coma = expression.find(","); //so far, this is the decision maker whether it is beta or alpha
			int dot = expression.find("."); //oh, if it is an alpha node, define where do they get it

			string comparisonExp =Utilities::getComparisonExp(expression);
			
			if (dot > -1) { //continous alpha --> ally.speed
				res.push_back({ "alpha", expression });
			}
			else if (coma > -1) {//consist more than 1 source --> distance(x,y)
				res.push_back({ "beta", expression });
			}
			else { //just from soooo, it is beta node
				if (comparisonExp!= "") { //-->speed < 100 
					res.push_back({ "alpha", expression });
				}else// just string --> allyObj
					res.push_back({ "beta", expression });
			}

			//string stream;
			//string fieldName;
			//string mid;
			//string right;
			//tokenizeSingleExp(expression, stream, fieldName, mid, right);

			//int index1 = fieldName.find("(");
			//int index2 = fieldName.find(")");
			//if ((index1 > -1 && index2 > -1) /*|| OperatorRegister::isOperator(fieldName)*/) {
			//	andPredicate->addChild(parseExpressionWithOperator(expression));
			//}
			//else {
			//	andPredicate->addChild(parseValueExpression(fieldName, mid, right));
			//}

			res.push_back({ "condition", "and" });
		}

		res.erase(res.end() -1);
	}
}

Node* ReteNet::addAlphaReturnNode(string condition)
{
	Node* tempNode;
	//string space = " ";
	if (condition.back() == ' ') {
		condition.pop_back();
	}

	//duplicate check
	for (int i = 0; i < alphaListIDDictionary.size(); i++) {
		if (NodeList[alphaListIDDictionary[i]]->justCondition == condition)
			return NodeList[alphaListIDDictionary[i]];
	}
	
	//Try to use OOP
	int masterNodeID = NodeList.size();
	tempNode = new AlphaNode(masterNodeID, condition);
	NodeList.push_back(tempNode);
	alphaListIDDictionary.push_back(masterNodeID);

	return tempNode;
}

Node* ReteNet::addBetaReturnNode(string condition)
{
	if (condition.back() == ' ') {
		condition.pop_back();
	}

	Node* tempNode;
	//duplicate check
	for (int i = 0; i < betaListIDDictionary.size(); i++) {
		if (static_cast<BetaNode*>(NodeList[betaListIDDictionary[i]])->getProduct() == condition)
			return NodeList[betaListIDDictionary[i]];
	}

	//Try to use OOP
	tempNode = new BetaNode(0, condition);

	int masterNodeID = NodeList.size();
	tempNode->setNewMasterID(masterNodeID);
	NodeList.push_back(tempNode);

	betaListIDDictionary.push_back(masterNodeID);

	return tempNode;
}

int ReteNet::getCurrNodeListLen()
{
	return NodeList.size();
}

void ReteNet::connectNodes(Node& n1, Node& n2, Node& n3)
{
	int danger = 0;
	bool left=true, right=true;
	if (static_cast<BetaNode*>(&n3)->getLeftConnNode() != NULL && &n1 != NULL) {
		danger++;
		cout << "ALERT, LEFT OF :" << static_cast<BetaNode*>(&n3)->getProduct() << " IS ALREADY OCCUPIED SOURCE" << endl;
	}
	if (static_cast<BetaNode*>(&n3)->getRightConnNode() != NULL && &n2 != NULL) {
		danger++;
		cout << "ALERT, RIGHT OF :" << static_cast<BetaNode*>(&n3)->getProduct() << " IS ALREADY OCCUPIED SOURCE" << endl;
	}

	if (danger >= 2) {
		cout << "ALERT, BOTH OF :" << static_cast<BetaNode*>(&n3)->getProduct() << " NO CONNECTION CREATED" << endl;
		return;
	}

	if (&n1 != NULL) {
		n1.addBetaPair(static_cast<BetaNode*>(&n3));
	}
	if (&n2 != NULL) {
		n2.addBetaPair(static_cast<BetaNode*>(&n3));
	}

	static_cast<BetaNode*>(&n3)->setLeftConnection(&n1);
	static_cast<BetaNode*>(&n3)->setRightConnection(&n2);
}

void ReteNet::connectNodes(Node& n1, Node& n2)
{
	if (static_cast<AlphaNode*>(&n2)->getPrevPair() != NULL) {
		cout << "ALERT :" << static_cast<AlphaNode*>(&n2)->getCondition() << " IS ALREADY OCCUPIED SOURCE" << endl;
		return;
	}

	n1.addBetaPair(&n2);

	static_cast<AlphaNode*>(&n2)->setPrevConnection(&n1);
}

void ReteNet::growTheNodes(vector<vector<pair<string, string>>> exp_vec)
{
	vector<Node*> created_node;
	vector<pair<Node*, string>> compiled_expression;
	vector<string> joinExpression, BetaExpression;

	Node* tempNode;
	int range = -1, step = -1;
	for (int i = 0; i < exp_vec.size(); i++) {
		
		//If clause
		if (i == 0) {

			//Step 1 --> list the created nodes
			for (int j = 0; j < exp_vec[i].size(); j++) {
				if (exp_vec[i][j].first == "alpha") {
					tempNode = addAlphaReturnNode(exp_vec[i][j].second);
					//created_node.push_back(tempNode);
					compiled_expression.push_back({ tempNode , tempNode->justCondition});
				}
				else if (exp_vec[i][j].first == "beta") {
					//BetaExpression.push_back(exp_vec[i][j].second);
					compiled_expression.push_back({ NULL , exp_vec[i][j].second });
				}
				else if (exp_vec[i][j].first == "condition") {
					joinExpression.push_back(exp_vec[i][j].second);
				}
			}			
		}
		else { // can be window or then clause
			
			//Keep the window val
			if (exp_vec[i][0].first == "range") {
				int index, index1;
				//index = exp_vec[i][0].second.find("range");
				//index1 = exp_vec[i][0].second.find("trigger");

				//range = atoi(exp_vec[i][0].second.substr(index + 6, exp_vec[i][0].second.size() - index -6).c_str());

				range = atoi(exp_vec[i][0].second.c_str());
				step = atoi(exp_vec[i][1].second.c_str());

			}
			else if (exp_vec[i][0].first == "term") {
				//Step2 --> join all of them until the last beta --> then clause
				
				//Process the original input
				//Due to vector limitation, so we do it backwards --> there are no pop_front, and i didn't want to create it
				string tempBetaProduct, tempBetaCondition;
				Node* n1, * n2, * n3;
				while (compiled_expression.size() > 2) {
					//so in temporary beta condition, it has to be flipped again
					tempBetaCondition = "", tempBetaProduct = "";
					
					char buff[10];

					if (created_node.size() > 0) { // if there are previous node created
						n2 = created_node[0];
						tempBetaCondition = dynamic_cast<BetaNode*>(created_node[0])->getProduct() + _itoa(created_node[0]->getID(), buff, 10);
						tempBetaProduct += dynamic_cast<BetaNode*>(created_node[0])->getProduct().substr(0, 2) + _itoa(created_node[0]->getID(), buff, 10);
						created_node.pop_back();

						tempBetaCondition = " " + joinExpression[joinExpression.size() - 1] + " " + tempBetaCondition;
						joinExpression.pop_back();

						n1 = compiled_expression[compiled_expression.size() - 1].first;
						tempBetaCondition = compiled_expression[compiled_expression.size() - 1].second + tempBetaCondition;
						tempBetaProduct += compiled_expression[compiled_expression.size() - 1].second.substr(0, 2);
						compiled_expression.pop_back();

						n3 = addBetaReturnNode(tempBetaCondition + " then " + tempBetaProduct);

						connectNodes(*n1, *n2, *n3);

						created_node.push_back(n3);
					}
					else { // if there are no node created before
						n2 = compiled_expression[compiled_expression.size() - 1].first;
						tempBetaCondition = compiled_expression[compiled_expression.size() - 1].second;
						tempBetaProduct += compiled_expression[compiled_expression.size() - 1].second.substr(0, 2);

						tempBetaCondition = " " + joinExpression[joinExpression.size() - 1] + " " + tempBetaCondition;

						compiled_expression.pop_back();
						joinExpression.pop_back();

						n1 = compiled_expression[compiled_expression.size() - 1].first;
						tempBetaCondition = compiled_expression[compiled_expression.size() - 1].second + tempBetaCondition;
						tempBetaProduct += compiled_expression[compiled_expression.size() - 1].second.substr(0, 2);
						compiled_expression.pop_back();

						n3 = addBetaReturnNode(tempBetaCondition + " then " + tempBetaProduct);

						connectNodes(*n1, *n2, *n3);

						created_node.push_back(n3);
					}
				}

				//FINALIZE THE LAST BETA
				tempBetaCondition = "", tempBetaProduct = "";
				if (created_node.size() > 0 && compiled_expression.size() > 0) { //there is previous beta created
					n2 = created_node[0];
					tempBetaCondition += dynamic_cast<BetaNode*>(created_node[0])->getProduct();
					created_node.pop_back();

					tempBetaCondition = " " + joinExpression[joinExpression.size() - 1] + " " + tempBetaCondition;
					joinExpression.pop_back();

					n1 = compiled_expression[compiled_expression.size() - 1].first;
					tempBetaCondition = compiled_expression[compiled_expression.size() - 1].second + tempBetaCondition;
					compiled_expression.pop_back();

					n3 = addBetaReturnNode(tempBetaCondition + " then " + exp_vec[i][0].second);
					
					if(range > 0 || step > 0)
						(n3)->setWindow(range, step);

					connectNodes(*n1, *n2, *n3);
				}
				else { // no beta node created or this expression is single expression
					/*if (compiled_expression.size() == 0 && created_node.size() == 1) {
						break;
					}*/

					if (compiled_expression.size() > 1) {
						n2 = compiled_expression[compiled_expression.size() - 1].first;
						tempBetaCondition = compiled_expression[compiled_expression.size() - 1].second;
						tempBetaProduct += compiled_expression[compiled_expression.size() - 1].second.substr(0, 2);
						compiled_expression.pop_back();

						tempBetaCondition = " " + joinExpression[joinExpression.size() - 1] + " " + tempBetaCondition;
						joinExpression.pop_back();
					}
					else {
						n2 = compiled_expression[0].first;
						tempBetaCondition = compiled_expression[0].second;

						tempBetaCondition = " and " + tempBetaCondition;
					}
										
					n1 = compiled_expression[0].first;
					tempBetaCondition = compiled_expression[0].second + tempBetaCondition;
					compiled_expression.pop_back();

					n3 = addBetaReturnNode(tempBetaCondition + " then " + exp_vec[i][0].second);
					if (range > 0 || step > 0)
						(n3)->setWindow(range, step);

					connectNodes(*n1, *n2, *n3);
				}
			}
		}
	}
	int a = 11;
	//End of function
}

void ReteNet::ExecuteRete(int TimeSlice)
{
	bool isDone = false;
	vector<pair<Node*, int>> visitedMark;
	vector<Node*> pushedNode;

	//Step 0, make sure that the event is exist
	if (m_WMSet.getWMInputQueue().size() < 1)
		return;

	//Step 1 Activate all direct alpha
	for (int j = 0; j < alphaListIDDictionary.size(); j++) {

		//test on alpha
		if (static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->ItIsDirect())
			static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->testAlphaAndPush(&m_WMSet.getWMInputQueue(), TimeSlice); // original
			//static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->testAlphaAndPush(&m_WMSet.getWMInputQueue().front(), TimeSlice); // testing purpose
		else
			continue;

		//activate the beta
		//searching all pair and push it
		for (int k = 0; k < static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->getAllPairs().size(); k++) {

			//check for duplicate
			bool isADuplicate = false;
			for (int l = 0; l < pushedNode.size(); l++) {
				if (pushedNode[l] == static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->getSinglePair(k)) {
					isADuplicate = true;
					break;
				}
			}
			if (!isADuplicate) {
				pushedNode.push_back(static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->getSinglePair(k));
				visitedMark.push_back({ static_cast<AlphaNode*>(NodeList[alphaListIDDictionary[j]])->getSinglePair(k), 0 });
			}
		}
	}
	//once it is cleared, dont forget to clear the WM
	

	//BFS based on Queue
	while (!pushedNode.empty()) {
		
		//This is the borderline ----------------------------------------------------------------------------
		//let the spatial rule processed outside
		if (dynamic_cast<BetaNode*>(pushedNode[0])) {
			if (dynamic_cast<BetaNode*>(pushedNode[0])->getSpecialOpName() != "") {
				pushedNode.erase(pushedNode.begin());
				continue;
			}
		}

		//Maybe this is the indexing?
		int testStatus = pushedNode[0]->testNode(TimeSlice);

		if (testStatus == 0) {
			pushedNode.erase(pushedNode.begin());
			//for (auto p : pushedNode) {

			//}
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

		//Mark the current node in visited mark
		for (int i = 0; i < visitedMark.size(); i++) {
			if (visitedMark[i].first == pushedNode[0]) {
				visitedMark[i].second++;

				//Evaluate visited mark
				if (visitedMark[i].second >= 3)
					isDone = true;
				break;
			}
		}
		//}

		pushedNode.erase(pushedNode.begin());

		if (isDone)
			break;
	}
	int a = 0;
}

vector<int> findCorrespondingAnchor(string objType, vector<string> dictionary) {
	vector<int> result = {};
	for (int i = 0; i < dictionary.size(); i++) {
		if (objType == dictionary[i])
			result.push_back(i);
	}
	return result;
}

void ReteNet::SpatioTemporalExecution(int TimeSlice)
{
	//RTree<int, float, 4, float> tree_scalar; // this one responsible for scalar node indexing --> later


//#pragma region Original2Dimension
//	RTree<int, float, 2, float> tree; // this one responsible for spatial node indexing
//	//lets try to process the ally first --> 2 ally 2 enemy --> these event happened at time t
//	//onetime event contain 2 ally & 2 enemy respectively
//	for (int j = 0; j < vec_anchor_id.size(); j++) {
//		if (vec_anchor_id[j].first.first == "allyvessel") // ok ok , it is separated perfectly --> actually this make the output double ._.
//		{
//			//get current event's position
//			queue<EventPtr> anchorEventQueue = static_cast<BetaNode*>(nodewithspatialIndexing[j])->getLeftInput();
//
//			for (; anchorEventQueue.size() > 0;) {
//
//				float xpos[2], ypos[2];
//
//				//just initiated
//				//if (hash_needUpdate[j] == true && hash_latestUpdate[j].first == 0) {
//				xpos[0] = anchorEventQueue.front()->getFloat("lat") - (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);
//				xpos[1] = anchorEventQueue.front()->getFloat("lat") + (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);
//
//				ypos[0] = anchorEventQueue.front()->getFloat("lon") - (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);
//				ypos[1] = anchorEventQueue.front()->getFloat("lon") + (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);
//
//				tree.Insert(xpos, ypos, nodewithspatialIndexing[j]->getID());
//
//				anchorEventQueue.pop();
//			}
//		}
//	}
//	//the tree is set now
//
//	//now test with the enemys
//	vector<Node*> pushedNode = {};
//	vector<EventPtr> distinctEnemyVector = {};
//	vector<int> nhits_vec;
//	for (int j = 0; j < vec_anchor_id.size(); j++) {
//		if (vec_anchor_id[j].first.second == "enemyvessel") // ok ok , it is separated perfectly
//		{
//			queue<EventPtr> testEventQueue = static_cast<BetaNode*>(nodewithspatialIndexing[j])->getRightInput();
//
//			for (; testEventQueue.size() > 0; testEventQueue.pop()) {
//				distinctEnemyVector.push_back(testEventQueue.front());
//
//				//distinct enemy event
//				sort(distinctEnemyVector.begin(), distinctEnemyVector.end());
//				distinctEnemyVector.erase(unique(distinctEnemyVector.begin(), distinctEnemyVector.end()), distinctEnemyVector.end());
//			}
//		}
//	}
//
//	//------------------------------------------------------------------------------------------------
//	//after it is dinctinct, so we can process the stabbing process
//	for (int i = 0; i < distinctEnemyVector.size(); i++) {
//		float xpos[2], ypos[2];
//		xpos[0] = xpos[1] = distinctEnemyVector[i]->getFloat("lat");
//		ypos[0] = ypos[1] = distinctEnemyVector[i]->getFloat("lon");
//
//		nhits_vec = tree.Search_vec(xpos, ypos, NULL, NULL);
//
//		//distinct the correlated beta nodes that hit
//		sort(nhits_vec.begin(), nhits_vec.end());
//		nhits_vec.erase(unique(nhits_vec.begin(), nhits_vec.end()), nhits_vec.end());
//
//		//push this into rete net
//		//this must be on the right
//		for (int j = 0; j < nhits_vec.size(); j++) {
//			Node* tempNode = NodeList[nhits_vec[j]];
//			if (dynamic_cast<BetaNode*>(tempNode)) {
//				dynamic_cast<BetaNode*>(tempNode)->forcePushInQueue(&distinctEnemyVector[i], false);
//
//				pushedNode.push_back(tempNode);
//
//				//sort and distinct
//				sort(pushedNode.begin(), pushedNode.end());
//				pushedNode.erase(unique(pushedNode.begin(), pushedNode.end()), pushedNode.end());
//			}
//		}
//	}
//
//#pragma endregion

#pragma region 3DimRegion
	RTree<int, float, 3, float> tree; // lat, long, anchor_enum
	//enum dictionaries --> well it is based on the hash map then
	//number of vec_anchor_id and nodewithspatialindex are same
	//the difference is just one only handle string, one handle the node also
	for (int j = 0; j < vec_anchor_id.size(); j++) {
		if (anchor_stab_map.find(vec_anchor_id[j].first.first) != anchor_stab_map.end()) // check based on the dictionaries
		{
			//get current event's position
			queue<EventPtr> anchorEventQueue = static_cast<BetaNode*>(nodewithspatialIndexing[j])->getLeftInput();

			for (; anchorEventQueue.size() > 0;) {

				float xpos[3], ypos[3];

				//just initiated
				//if (hash_needUpdate[j] == true && hash_latestUpdate[j].first == 0) {
				xpos[0] = anchorEventQueue.front()->getFloat("lat") - (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);
				xpos[1] = anchorEventQueue.front()->getFloat("lat") + (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);

				ypos[0] = anchorEventQueue.front()->getFloat("lon") - (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);
				ypos[1] = anchorEventQueue.front()->getFloat("lon") + (static_cast<BetaNode*>(nodewithspatialIndexing[j])->getSpatialLimFloat() / 2);

				//3rd dimension is the enum
				vector<int> corresponding_dimension = findCorrespondingAnchor(static_cast<BetaNode*>(nodewithspatialIndexing[j])->getRightConnName(), observed_obj_dict);
				for (int k = 0; k < corresponding_dimension.size(); k++) {
					xpos[2] = corresponding_dimension[k];
					ypos[2] = corresponding_dimension[k];

					tree.Insert(xpos, ypos, nodewithspatialIndexing[j]->getID());
				}

				anchorEventQueue.pop();
			}
		}
	}

	//the tree is set now
	
	//now test with the enemys
	vector<Node*> pushedNode = {};
	//vector<EventPtr> distinctTestedObj = {};
	vector<vector<EventPtr>> organizedTestEvents = {};
	vector<int> nhits_vec;
	
	//initiate the organizedTestEvents
	for (int j = 0; j < observed_obj_dict.size(); j++) {
		organizedTestEvents.push_back({});
	}

	
	for (int j = 0; j < vec_anchor_id.size(); j++) {

		//find is it belong to which index?
		std::vector<string>::iterator it;
		it = find(observed_obj_dict.begin(), observed_obj_dict.end(), vec_anchor_id[j].first.second);
		int dist = distance(observed_obj_dict.begin(), it);

		//int dist = 0;

		//assign to each observed index
		queue<EventPtr> testEventQueue = static_cast<BetaNode*>(nodewithspatialIndexing[j])->getRightInput();
		for (; testEventQueue.size() > 0; testEventQueue.pop()) {

			organizedTestEvents[dist].push_back(testEventQueue.front());
		}

		//distinct enemy event
		sort(organizedTestEvents[dist].begin(), organizedTestEvents[dist].end());
		organizedTestEvents[dist].erase(unique(organizedTestEvents[dist].begin(), organizedTestEvents[dist].end()), organizedTestEvents[dist].end());
	}
	
	//------------------------------------------------------------------------------------------------
	//after it is dinctinct, so we can process the stabbing process
	//organizedevents is organized as follow --> [dictionary id][vector of event pointer]
	for (int i = 0; i < organizedTestEvents.size(); i++) {
		for (int j = 0; j < organizedTestEvents[i].size(); j++) {

			float xpos[3], ypos[3];
			xpos[0] = xpos[1] = organizedTestEvents[i][j]->getFloat("lat");
			ypos[0] = ypos[1] = organizedTestEvents[i][j]->getFloat("lon");

			xpos[2] = ypos[2] = i; // i is dictionary id

			nhits_vec = tree.Search_vec(xpos, ypos, NULL, NULL);

			//distinct the correlated beta nodes that hit
			sort(nhits_vec.begin(), nhits_vec.end());
			nhits_vec.erase(unique(nhits_vec.begin(), nhits_vec.end()), nhits_vec.end());

			//push this into rete net
			//this must be on the right
			for (auto n : nhits_vec) {
				Node* tempNode = NodeList[n];
				if (dynamic_cast<BetaNode*>(tempNode)) {
					dynamic_cast<BetaNode*>(tempNode)->forcePushInQueue(&organizedTestEvents[i][j], false);

					pushedNode.push_back(tempNode);

					//sort and distinct
					sort(pushedNode.begin(), pushedNode.end());
					pushedNode.erase(unique(pushedNode.begin(), pushedNode.end()), pushedNode.end());
				}
			}
		}
	}

#pragma endregion	

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

	//due to constant movement, so just format the tree --> well, this is totally wrong, but still wondering how to fix it
	tree.RemoveAll();
	
}

void ReteNet::buildNetNode()
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

	//spatio temporal node addressing
	for (int i = 0; i < betaListIDDictionary.size(); i++) {
		if (static_cast<BetaNode*>(NodeList[betaListIDDictionary[i]])->getSpecialOpName() == "distance") {
			nodewithspatialIndexing.push_back(NodeList[betaListIDDictionary[i]]);
			vec_anchor_id.push_back({ {static_cast<BetaNode*>(NodeList[betaListIDDictionary[i]])->getLeftConnName(), static_cast<BetaNode*>(NodeList[betaListIDDictionary[i]])->getRightConnName()}, NodeList[betaListIDDictionary[i]]->getID() });
		}
	}
	//discritize vec_anchor_id & node w/ spatial indexing
	sort(vec_anchor_id.begin(), vec_anchor_id.end());
	vec_anchor_id.erase(unique(vec_anchor_id.begin(), vec_anchor_id.end()), vec_anchor_id.end());
	sort(nodewithspatialIndexing.begin(), nodewithspatialIndexing.end());
	nodewithspatialIndexing.erase(unique(nodewithspatialIndexing.begin(), nodewithspatialIndexing.end()), nodewithspatialIndexing.end());


	//address the anchor and stabber
	//format = anchor -- desired stabber
	for (int i = 0; i < nodewithspatialIndexing.size(); i++) {
		//not exist
		if (anchor_stab_map.find(static_cast<BetaNode*>(nodewithspatialIndexing[i])->getLeftConnName()) == anchor_stab_map.end()) {
			vector<string> temp;
			temp.push_back(static_cast<BetaNode*>(nodewithspatialIndexing[i])->getRightConnName());
			anchor_stab_map[static_cast<BetaNode*>(nodewithspatialIndexing[i])->getLeftConnName()] = temp;
		}
		else {
			//duplicate check
			bool duplicate = false;
			for (auto a : anchor_stab_map[static_cast<BetaNode*>(nodewithspatialIndexing[i])->getLeftConnName()]) {
				if (static_cast<BetaNode*>(nodewithspatialIndexing[i])->getRightConnName() == a) {
					duplicate = true;
				}
			}
			if (!duplicate) {
				anchor_stab_map[static_cast<BetaNode*>(nodewithspatialIndexing[i])->getLeftConnName()].push_back(static_cast<BetaNode*>(nodewithspatialIndexing[i])->getRightConnName());
			}
		}
	}
	
	//build enum dictionary
	for (auto au : anchor_stab_map) {
		for (auto sec : au.second) {
			observed_obj_dict.push_back(sec);
		}
	}
	//disctitize the observed_obj enumeration
	sort(observed_obj_dict.begin(), observed_obj_dict.end());
	observed_obj_dict.erase(unique(observed_obj_dict.begin(), observed_obj_dict.end()), observed_obj_dict.end());

	//int a = 10;
}

Node* ReteNet::findNode(string expression, int nodeType)
{
	// 0 --> alpha
	// 1 --> beta
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

void ReteNet::cloneToWm(queue<EventPtr> input)
{
	m_WMSet.cloneInputQueues(input);
}
