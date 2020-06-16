#pragma once

#include "Node.h"
//#include "BetaNode.h"
#include <string>
#include <cstdlib>
#include "SlidingWindow.h"
#include "SpatialOp.h"

#include "Utilities.h"

//#include "../execution/EventProcess.h"

//enum comparisonCondition { // Do i need this?
//	LessThanEqual = 0,
//	moreThanEqual = 1,
//	lessThan = 2,
//	moreThan = 3,
//	equalWith = 4
//} m_comparison;

class AlphaNode : public Node
{
public:

	//Derived function from Node class
	int testNode(int TimeSlice);
	//int getID();
	string getType();
	bool isEmptyResult();
	void pushResult(queue<EventPtr>* result, Node* inputNode);
	Node* getSinglePair(int i);
	vector<Node*> getAllPairs();
	bool prevNodeAlreadyConnected();

	//Starter kit
	AlphaNode(int id, string condition);

	//Lets connect it
	int checkExistPair(Node* pairs);
	int addBetaPair(Node* pair);
	int setPrevConnection(Node* node);

	//Initialization
	int ResetNode();
	int ClearResults();

	void testAlphaAndPush(queue<EventPtr>* input, int timeSlice);
	int justTest(int TimeSlice);

	//get/set function
	string getAlphaDataType();
	string getCondition();
	string getVarLimit();
	Node* getPrevPair();
	string getPrevPairStr();

	bool ItIsDirect();

	//Will be depreciated soon
	queue<EventPtr>* getEvRes();

	//Sliding Window
	void setWindow(int len = -1, int step = -1);

private:
	//int id;
	queue<EventPtr>* EventResult = new queue<EventPtr>();

	//Condition -- Still confused
	string thisDataType;
	string thisCondition;
	string thisVarLimit;

	//special operations
	string specialOp;
	SpatialOp* m_spatialOp;

	bool directToWM = true;

	SlidingWindow* win;

	//Connected Node (I am not sure with this)
	vector<Node*> listOfNextPair;

	//If this is middle Alpha Node
	pair<string, Node*> prevNode;
	pair<queue<EventPtr>, Node*> inputQueue;
};

