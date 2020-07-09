#include "AlphaNode.h"

int AlphaNode::testNode(int TimeSlice)
{
	return justTest(TimeSlice);
}

//int AlphaNode::getID()
//{
//	return id;
//}

string AlphaNode::getType()
{
	return "Alpha";
}


AlphaNode::AlphaNode(int id_given, string condition) : Node(id_given, condition)
{
	inputQueue.first = {};
	int itt = 0;

	Utilities::tokenizeSingleExp(condition, prevNode.first, thisDataType, thisCondition, thisVarLimit, specialOp);
	if (specialOp != "") {
		m_spatialOp = new SpatialOp(specialOp);
		/*if (specialOp != "exist") {
			m_spatialOp->setVarCondition(thisCondition);
			m_spatialOp->setVarLimit(thisVarLimit);
		}*/
	}

}

string AlphaNode::getAlphaDataType()
{
	return thisDataType;
}

string AlphaNode::getCondition()
{
	return thisCondition;
}

string AlphaNode::getVarLimit()
{
	return thisVarLimit;
}

Node* AlphaNode::getSinglePair(int i)
{
	return listOfNextPair[i];
}

vector<Node*> AlphaNode::getAllPairs()
{
	return listOfNextPair;
}

bool AlphaNode::prevNodeAlreadyConnected()
{
	if(prevNode.second != NULL)
		return true;
	return false;
}

Node* AlphaNode::getPrevPair()
{
	return prevNode.second;
}

string AlphaNode::getPrevPairStr()
{
	return prevNode.first;
}

bool AlphaNode::ItIsDirect()
{
	return directToWM;
}

queue<EventPtr> AlphaNode::getEvRes()
{
	return EventResult;
}

void AlphaNode::pushResult(queue<EventPtr> result, Node*inputNode)
{
	//queue<EventPtr> dummy_res = result;
	if (inputQueue.second == inputNode) {
		while (result.size() > 0) {
			inputQueue.first.push(result.front());
			result.pop();
		}
	}
		//inputQueue.first = *result;
		//inputQueue.first.push(result);
}

int AlphaNode::setPrevConnection(Node* node)
{
	inputQueue.second = node;
	prevNode.second = node;
	directToWM = false;
	return 0;
}

void AlphaNode::setWindow(int len, int step)
{
	win = new SlidingWindow(len, step);
}

int AlphaNode::getExecutionEstimated()
{
	if (win == NULL)
		return -1;
	else
		return win->getInitTime();
}

int AlphaNode::checkExistPair(Node* pairs)
{
	for (int i = 0; i < listOfNextPair.size(); i++) {
		if (listOfNextPair[i] == pairs)
			return 1;
	}

	return 0;
}

int AlphaNode::addBetaPair(Node* pair)
{
	int isExist = checkExistPair(pair);
	if (isExist)
		return 1;
	else {
		//listOfBetaPairs.push_back(pair);
		listOfNextPair.push_back(pair);
		return 1;
	}
	return -1;
}

int AlphaNode::ResetNode()
{
	//testRes = {};
	//listOfBetaPairs = {};
	listOfNextPair = {};
	//betaPair = NULL;
	return 1;
}

int AlphaNode::ClearResults()
{
	while (!EventResult.empty()) {
		EventResult.pop();
	}
	return 1;
}

void AlphaNode::testAlphaAndPush(queue<EventPtr>* DirectInputQueue, int TimeSlice)
{
	inputQueue.first = *DirectInputQueue;
	justTest(TimeSlice);
}

int AlphaNode::justTest(int TimeSlice)
{
	if (inputQueue.first.empty()) {
		return 0;
	}

	float limit = atof(thisVarLimit.c_str());

	if (limit == 0 && m_spatialOp == NULL ) {//its not a integer val
		int cases = -1;
		if (thisCondition == "=" || thisCondition == "==")
			cases = 0;
		else if (thisCondition == "!=")
			cases = 1;

		int timeSlice_i = TimeSlice;
		while (!inputQueue.first.empty() && timeSlice_i > 0) {//current input queue is not empty.
			EventPtr originalFrontEvent = inputQueue.first.front();

			if (thisVarLimit == "All") {
				EventResult.push(originalFrontEvent);
			}
			else if (cases == 0 && originalFrontEvent->getString(thisDataType) == thisVarLimit) {
				EventResult.push(originalFrontEvent);
			}
			else if (cases == 1 && originalFrontEvent->getString(thisDataType) != thisVarLimit) {
				EventResult.push(originalFrontEvent);
			}

			inputQueue.first.pop();
			timeSlice_i--;
		}
	}
	else if (limit == 0 && m_spatialOp != NULL) {
		
		//copy the event to window
		//also distinct the anchor --> anchors are all same
		string tempAnchor = inputQueue.first.front()->getString("anchors");
		while (!inputQueue.first.empty() && TimeSlice > 0) {
			EventPtr originalFrontEvent = inputQueue.first.front();

			win->addEvent(originalFrontEvent);

			inputQueue.first.pop();
		}

		////check time
		//if (win->getInitTime() + win->getTriggerTime() < win->getHigheststOriginalTime()) {
		//	win->setInitTime(win->getInitTime() + win->getTriggerTime()); // update the execution time
		//}
		//else {
		//	return 0;
		//}

		vector<string> objAnchor_s = Utilities::splitDelim(tempAnchor, ",");
		vector<int> objAnchor_i;
		for (auto i : objAnchor_s) {
			objAnchor_i.push_back(atoi(i.c_str()));
		}
		objAnchor_i.pop_back();
		
		EventResult = m_spatialOp->process(win, objAnchor_i);

	}
	else {
		int cases = -1;
		// 0 -> equal less
		// 1 -> equal more
		// 2 -> less
		// 3 -> more
		if (thisCondition == "<=")
			cases = 0;
		else if (thisCondition == ">=")
			cases = 1;
		else if (thisCondition == "<")
			cases = 2;
		else if (thisCondition == ">")
			cases = 3;
		else if (thisCondition == "==")
			cases = 4;

		int timeSlice_i = TimeSlice;
		while (!inputQueue.first.empty() && timeSlice_i > 0) {//current input queue is not empty.
			EventPtr originalFrontEvent = inputQueue.first.front();

			if (cases == 0 && originalFrontEvent->getFloat(Utilities::ToLower(thisDataType)) <= limit) {
				EventResult.push(originalFrontEvent);
			}
			else if (cases == 1 && originalFrontEvent->getFloat(Utilities::ToLower(thisDataType)) >= limit) {
				EventResult.push(originalFrontEvent);
			}
			else if (cases == 2 && originalFrontEvent->getFloat(Utilities::ToLower(thisDataType)) < limit) {
				EventResult.push(originalFrontEvent);
			}
			else if (cases == 3 && originalFrontEvent->getFloat(Utilities::ToLower(thisDataType)) > limit) {
				EventResult.push(originalFrontEvent);
			}
			else if (cases == 4 && originalFrontEvent->getFloat(Utilities::ToLower(thisDataType)) == limit) {
				EventResult.push(originalFrontEvent);
			}

			inputQueue.first.pop();
			timeSlice_i--;
		}
	}

	//PUSH TO PROCEEDING NODE .-.
	for (Node* n : listOfNextPair) {
		n->pushResult(EventResult, this);
	}
	
	if (EventResult.size() > 0) {
		ClearResults();
		return 1;
	}
	else {
		return 0;
	}
}

bool AlphaNode::isEmptyResult()
{
	//if (EventResult == NULL || EventResult.size() == 0)
	if (EventResult.size() == 0)
		return true;
	return false;
}
