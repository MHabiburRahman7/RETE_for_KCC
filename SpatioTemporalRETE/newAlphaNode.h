#pragma once

#include <string>
#include <cstdlib>
#include "SlidingWindow.h"

#include "Utilities.h"

class newAlphaNode : public Node
{
public:
	newAlphaNode(int id, string condition);
	
	//Derived function from Node class
	void testNode(int TimeSlice);
	int getID();
	string getType();
	bool isEmptyResult();
	void pushResult(queue<EventPtr>* result, Node* inputNode);
	
	// Lets connect it
	int checkExistPair(Node * pairs);
	int addBetaPair(Node* pair);
	int setPrevConnection(Node* node);

	//testing function
	void justTest(int TimeSlice);

private:
	string specialOp;
	string thisVarLimit;
	string thisDataType;
	string compExpression;

	int id;

	bool directToWM = true;

	SlidingWindow* win;

	//Connected Node (I am not sure with this)
	vector<Node*> listOfNextPair;

	//If this is middle Alpha Node
	pair<string, Node*> prevNode;
	pair<queue<EventPtr>*, Node*> inputQueue;
};

