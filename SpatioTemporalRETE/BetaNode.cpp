#include "BetaNode.h"

//BetaNode::BetaNode()
//	: Node(BaseNodeID, tempComingCondition)
//{
//}

BetaNode::BetaNode(int id_given, string condition)
	: Node(id_given, condition)
{
	int itt = 0;

	//id = id_given;

	string limit;
	
	Utilities::tokenizeBetaExp(condition, leftSourcePair.first, rightSourcePair.first, specialOperation, limit, thisCondition, thisProduct);

	if (specialOperation == "") {
		int leftBr = -1, rightBr = -1, coma = -1;
		//check left and right
		leftBr = leftSourcePair.first.find("(");
		rightBr = leftSourcePair.first.find(")");
		coma = leftSourcePair.first.find(",");

		if (coma > -1) { //if spatial function
			Node* temp = ReteNet::addBetaReturnNode(leftSourcePair.first);
			//*id_given = *(temp->getID() + 1);
			//int masterNodeID = ReteNet::NodeList.size();
			//connect both
			static_cast<BetaNode*>(temp)->addBetaPair(this);
			leftSourcePair.second = temp;
			leftInputQueue.second = temp;

			key = "time";
		}
		if (leftBr > -1 && rightBr > -1 && coma == -1) { //only aggregate function
			specialOperationLeft = leftSourcePair.first.substr(0, leftBr);

			string leftCompExp = Utilities::getComparisonExp(leftSourcePair.first);
			if (leftCompExp != "") {
				string leftLimit = leftSourcePair.first.substr(rightBr, leftSourcePair.first.size() - rightBr);
			}
		}

		leftBr = rightSourcePair.first.find("(");
		rightBr = rightSourcePair.first.find(")");
		coma = rightSourcePair.first.find(",");

		if (coma > -1) { //if spatial function
			Node* temp = ReteNet::addBetaReturnNode(rightSourcePair.first);

			//connect both
			static_cast<BetaNode*>(temp)->addBetaPair(this);
			rightSourcePair.second = temp;
			rightInputQueue.second = temp;

			key = "time";
		}
		if (leftBr > -1 && rightBr > -1 && coma == -1) { //only aggregate function
			specialOperationLeft = rightSourcePair.first.substr(0, leftBr);

			string leftCompExp = Utilities::getComparisonExp(rightSourcePair.first);
			if (leftCompExp != "")
				string leftLimit = rightSourcePair.first.substr(rightBr, rightSourcePair.first.size() - rightBr);

		}
	}
	else {
		key = "time";
		thisSpatialOp = new SpatialOp(specialOperation);
		if (limit != "")
			thisSpatialOp->setVarLimit(limit);
		if (thisCondition != "")
			thisSpatialOp->setVarCondition(thisCondition);
	}
}

string BetaNode::getType()
{
	return "Beta";
}

Node* BetaNode::getLeftConnNode()
{
	return leftSourcePair.second;
}

Node* BetaNode::getRightConnNode()
{
	return rightSourcePair.second;
}

string BetaNode::getLeftConnName()
{
	return leftSourcePair.first;
}

string BetaNode::getRightConnName()
{
	return rightSourcePair.first;
}

void BetaNode::setWindow(int len, int step)
{
	win = new SlidingWindow(len, step);
}

void BetaNode::setMatchingKey(string newKey)
{
	key = newKey;
}



Node* BetaNode::getSinglePair(int i)
{
	return listOfNextPair[i];
}

vector<Node*> BetaNode::getAllPairs()
{
	return listOfNextPair;
}

bool BetaNode::prevNodeAlreadyConnected()
{
	if(leftSourcePair.second != NULL && rightSourcePair.second != NULL)
		return true;
	return false;
}

int BetaNode::setLeftConnection(Node* node)
{
	if (leftSourcePair.second == NULL) {
		leftSourcePair.second = node;

		leftInputQueue.first = {};
		leftInputQueue.second = node;
		return 1;
	}
	
	if (leftInputQueue.second == NULL) {
		leftInputQueue.first = {};
		leftInputQueue.second = node;
		return 1;
	}
	
	return 0;
}

int BetaNode::setRightConnection(Node* node)
{
	if (rightSourcePair.second == NULL) {
		rightSourcePair.second = node;

		rightInputQueue.first = {};
		rightInputQueue.second = node;
		return 1;
	}
	
	if (rightInputQueue.second == NULL) {
		rightInputQueue.first = {};
		rightInputQueue.second = node;
		return 1;
	}
	
	return 0;
	
}

bool BetaNode::isEmptyResult()
{
	if (EventResult.size() == 0)
		return true;
	return false;
}

int BetaNode::justTest()
{
	//in case both of the inputs are same
	//idk why but only left that is working
	if (leftSourcePair.second == rightSourcePair.second) {

		if (leftInputQueue.first.empty())
			return 0;

		while (1) {
			if (leftInputQueue.first.size() == 0)
				break;

			//push to window
			if (win) {
				win->addEvent(leftInputQueue.first.front());
			}

			EventResult.push(leftInputQueue.first.front()); //this add 1

			leftInputQueue.first.pop(); // this also add 2 ???  ._.
		}
	}
	//Ordered join method
	else if (Utilities::ToUpper(thisCondition) == "AND" || specialOperation != "") {// if it is and or some specialOperation thing

		if (leftInputQueue.first.size() == 0 || rightInputQueue.first.size() == 0)
			return 0;

		//left is less than right - HAVE TO
		queue<EventPtr> *left, *right;
		if (leftInputQueue.first.size() <= rightInputQueue.first.size()) {
			left = &leftInputQueue.first;
			right = &rightInputQueue.first;
		}
		else
		{
			left = &rightInputQueue.first;
			right = &leftInputQueue.first;
		}

		while (1) {
			if (left->size() == 0 || right->size() == 0)
				break;

			if (left->size() == 4294967294 || right->size() == 4294967294)
				break;

			EventPtr frontLeftEvent = left->front();
			EventPtr frontRightEvent = right->front();

			if (frontLeftEvent->getInt(key) == frontRightEvent->getInt(key)) {
				EventPtr res;

				if (specialOperation == "distance") { //in case of distance --> need to calculate each time ticks
					res = thisSpatialOp->calculate(frontLeftEvent, frontRightEvent);
				}

				//push to window
				if (win) {
					if (specialOperation == "distance") {
						win->addEvent(res);
					}
					else if (specialOperation != "") {
						win->addEvent(frontLeftEvent, frontRightEvent);
					}
					else
						win->addEvent(frontLeftEvent);
				}

				//push to result
				if (specialOperation != "") {
					EventResult.push(res);
				}
				else
					EventResult.push(frontLeftEvent);
			}
			if (frontLeftEvent->getInt(key) >= frontRightEvent->getInt(key)) {
				right->pop();
			}
			else if (frontLeftEvent->getInt(key) < frontRightEvent->getInt(key)) {
				left->pop();
			}
		}
	}else if (Utilities::ToUpper(thisCondition) == "OR") {
		//OR Statement
	}
	
	if(win != NULL && specialOperation != ""){
		//Spatial Op
		EventResult = thisSpatialOp->process(win);
	}

	if (win != NULL && EventResult.size() > 0) {
		queue<EventPtr>* local_win = win->getFinalRes();
		EventResult = *local_win;
	}

	//at the end, re evaluate the spatial operation again -- final re evaluation

	//PUSH TO PROCEEDING NODE .-.
	if (listOfNextPair.size() > 0) {
		for (Node* n : listOfNextPair) {
			n->pushResult(&EventResult, this);
		}
	}
	//If it doesn't have, maybe this is the end?
	else {
		cout << "Result from " << thisProduct << " as follows: " << endl;
		queue<EventPtr> local_ptr = EventResult;
		while (local_ptr.size() > 0) {
			cout << *local_ptr.front() << endl;
			local_ptr.pop();
		}
	}

	if (EventResult.size() > 0) {
		ClearResult();
		return 1;
	}
	else {
		return 0;
	}
}

queue<EventPtr>* BetaNode::getEvRes()
{
	return &EventResult;
}

void BetaNode::refreshEvent(queue<EventPtr>& inputEvent) {
	long long curr = Utilities::getTime();
	/*while (!inputEvent.empty() && inputEvent.front()->getTime() + winLen < curr) {
		inputEvent.pop();
	}*/
}

void BetaNode::pushResult(queue<EventPtr>* result, Node* source)
{
	queue<EventPtr> dummy_res = *result;
	if (source == leftInputQueue.second) {
		while (dummy_res.size() > 0) {
			leftInputQueue.first.push(dummy_res.front());
			dummy_res.pop();
		}
	}
	else if (source == rightInputQueue.second) {
		while (dummy_res.size() > 0) {
			rightInputQueue.first.push(dummy_res.front());
			dummy_res.pop();
		}
	}
	/*if (source == leftInputQueue.second)
		leftInputQueue.first = *result;
	else if (source == rightInputQueue.second)
		rightInputQueue.first = *result;*/
}

string BetaNode::getProduct()
{
	return thisProduct;
}

int BetaNode::testNode(int TimeSlice)
{
	if (leftSourcePair.second == rightSourcePair.second) {
		return justTest();
	}

	if(leftInputQueue.first.size() > 0 && rightInputQueue.first.size() > 0){
		return justTest();
	}

	return 0;
	//if (win) {
	//	for(int )
	//	win->addEvent
	//}
}

int BetaNode::getID()
{
	return id;
}


int BetaNode::addBetaPair(Node* pair)
{
	int isExist = checkBeta(pair);
	if (isExist == 1)
		return 1;
	else {
		listOfNextPair.push_back(pair);
		return 1;
	}

	return -1;
}

int BetaNode::checkBeta(Node* pair)
{
	for (int i = 0; i < listOfNextPair.size(); i++) {
		if (listOfNextPair[i] == pair)
			return 1;
	}
	return 0;
}

int BetaNode::ClearConnection()
{
	//betaPair = NULL;
	//leftSourceBool = false;
	//rightSourceBool = false;
	termPair.second = NULL;
	termPair.first = "";
	return 1;
}

int BetaNode::ResetNode()
{
	ClearConnection();
	//testRes = {};
	//EventResult;
	listOfNextPair = {};

	ClearResult();
	return 0;
}

int BetaNode::ClearResult()
{
	while (!EventResult.empty()) {
		EventResult.pop();
	}
	return 1;
}