#pragma once

#include "AlphaNode.h"
#include "BetaNode.h"
//#include "TerminalNode.h"
#include "Node.h"

#include "WorkingMemory.h"

#include <list>



class ReteNet
{
public:
	static vector<vector<pair<string, string>>> parseConditionOriginal(vector<string> condList);
	static void tokenizeMultiExpCEP(string input, vector<pair<string, string>>& res);

	static Node* addAlphaReturnNode(string condition);
	static Node* addBetaReturnNode(string condition);
	static int getCurrNodeListLen();
	//connect 3 nodes
	static void connectNodes(Node& n1, Node& n2, Node& n3);
	//connect 2 nodes
	static void connectNodes(Node& n1, Node& n2);
	static void growTheNodes(vector<vector<pair<string, string>>> exp_vec);

	static void ExecuteRete(int TimeSlice);

	static void buildNetNode();
	//Node type 0 --> alpha, 1-->beta
	static Node* findNode(string expression, int type = -1);

	//About Working Memory
	static void cloneToWm(queue<EventPtr> input);

private:
	static WorkingMemory m_WMSet;
	static vector<int>alphaListIDDictionary;
	static vector<int>betaListIDDictionary;
	static vector<int>termListIDDictionary;
	static vector<Node*> NodeList;
};

