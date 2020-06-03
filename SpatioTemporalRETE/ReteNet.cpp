#include "ReteNet.h"

WorkingMemory ReteNet::m_WMSet;

vector<int>ReteNet::alphaListIDDictionary;
vector<int>ReteNet::betaListIDDictionary;
vector<int>ReteNet::termListIDDictionary;
vector<Node*> ReteNet::NodeList;

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
			SingleExpressionMade.push_back({"window", value});
		}
		else if ("then" == clause) {//Window length=, sliding=
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

	////alpha - alpha - beta
	//if (dynamic_cast<AlphaNode*>(&n1) && dynamic_cast<AlphaNode*>(&n2)
	//	&& dynamic_cast<BetaNode*>(&n3)) {

	//	static_cast<AlphaNode*>(&n1)->addBetaPair(static_cast<BetaNode*>(&n3));
	//	static_cast<AlphaNode*>(&n2)->addBetaPair(static_cast<BetaNode*>(&n3));

	//	static_cast<BetaNode*>(&n3)->setLeftConnection(&n1);
	//	static_cast<BetaNode*>(&n3)->setRightConnection(&n2);
	//}
	////beta - alpha - beta
	//else if (dynamic_cast<BetaNode*>(&n1) && dynamic_cast<AlphaNode*>(&n2)
	//	&& dynamic_cast<BetaNode*>(&n3)) {

	//	static_cast<BetaNode*>(&n1)->addBetaPair(static_cast<BetaNode*>(&n3));
	//	static_cast<AlphaNode*>(&n2)->addBetaPair(static_cast<BetaNode*>(&n3));

	//	static_cast<BetaNode*>(&n3)->setLeftConnection(&n1);
	//	static_cast<BetaNode*>(&n3)->setRightConnection(&n2);
	//}
	////alpha - beta - beta
	//else if (dynamic_cast<AlphaNode*>(&n1) && dynamic_cast<BetaNode*>(&n2)
	//	&& dynamic_cast<BetaNode*>(&n3)) {

	//	static_cast<AlphaNode*>(&n1)->addBetaPair(static_cast<BetaNode*>(&n3));
	//	static_cast<BetaNode*>(&n2)->addBetaPair(static_cast<BetaNode*>(&n3));

	//	static_cast<BetaNode*>(&n3)->setLeftConnection(&n1);
	//	static_cast<BetaNode*>(&n3)->setRightConnection(&n2);
	//}
	////beta - beta - beta
	//else if (dynamic_cast<BetaNode*>(&n1) && dynamic_cast<BetaNode*>(&n2)
	//	&& dynamic_cast<BetaNode*>(&n3)) {

	//	static_cast<BetaNode*>(&n1)->addBetaPair(static_cast<BetaNode*>(&n3));
	//	static_cast<BetaNode*>(&n2)->addBetaPair(static_cast<BetaNode*>(&n3));

	//	static_cast<BetaNode*>(&n3)->setLeftConnection(&n1);
	//	static_cast<BetaNode*>(&n3)->setRightConnection(&n2);
	//}
	//terminal - beta - beta --> coming soon ~!!
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
			if (exp_vec[i][0].first == "window") {
				int index, index1;
				index = exp_vec[i][0].second.find("range");
				index1 = exp_vec[i][0].second.find("step");

				range = atoi(exp_vec[i][0].second.substr(index + 6, exp_vec[i][0].second.size() - index -6).c_str());

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
		//if (!static_cast<BetaNode*>(static_cast<BetaNode*>(pushedBeta[0])->getLeftConnNode())->isEmptyResult()
		//	&& !static_cast<BetaNode*>(static_cast<BetaNode*>(pushedBeta[0])->getRightConnNode())->isEmptyResult()) {

		//Maybe this is the indexing?
		int testStatus = pushedNode[0]->testNode(TimeSlice);

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
	int a = 12;
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
