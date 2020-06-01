#include "newAlphaNode.h"

newAlphaNode::newAlphaNode(int id_given, string condition): Node(id_given, condition)
{
	id = id_given;
	inputQueue.first = new queue<EventPtr>();
	int itt = 0;

	Utilities::tokenizeSingleExp(condition, prevNode.first, thisDataType, compExpression, thisVarLimit);
}

void newAlphaNode::testNode(int TimeSlice)
{
	//justTest(TimeSlice);
}

int newAlphaNode::getID()
{
	return id;
}

string newAlphaNode::getType()
{
	return "Alpha";
}

bool newAlphaNode::isEmptyResult()
{
	//if (EventResult == NULL || EventResult->size() == 0)
	//	return true;
	//return false;
	return false;
}

void newAlphaNode::pushResult(queue<EventPtr>* result, Node* inputNode)
{
	if (inputQueue.second == inputNode)
		inputQueue.first = result;
}

int newAlphaNode::checkExistPair(Node* pairs)
{
	for (int i = 0; i < listOfNextPair.size(); i++) {
		if (listOfNextPair[i] == pairs)
			return 1;
	}

	return 0;
}

int newAlphaNode::addBetaPair(Node* pair)
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

int newAlphaNode::setPrevConnection(Node* node)
{
	inputQueue.second = node;
	directToWM = false;
	return 0;
}

void newAlphaNode::justTest(int TimeSlice)
{

}
