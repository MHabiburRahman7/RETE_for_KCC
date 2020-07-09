#pragma once

#include "AlphaNode.h"
#include "BetaNode.h"
//#include "TerminalNode.h"
#include "Node.h"

#include "WorkingMemory.h"
#include "RTree.h"

#include <list>
#include <algorithm>

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

	//Main execution of RETE
	static void ExecuteRete(int TimeSlice);

	static void buildNetNode();
	//Node type 0 --> alpha, 1-->beta
	static Node* findNode(string expression, int type = -1);

	//About Working Memory
	static void cloneToWm(queue<EventPtr> input);

	//About Spatiotemporal special handling
	static void SpatioTemporalExecution(int TimeSlice);
	//filter the spatial node that we can do indexing
	static vector<Node*> nodewithspatialIndexing;
	//list the anchor class
	//vector format --> left input, right input, node id
	static vector<pair<pair<string, string>, int>> vec_anchor_id;
	//map format --> anchor, vector of corresponding obj type
	static unordered_map<string, vector<string>> anchor_stab_map;
	static vector<string> observed_obj_dict;

	//priority queue for different temporal execution
	bool Compare_pq(Node* a, Node* b) {
		return a->getExecutionEstimated() < b->getExecutionEstimated();
	};
	static priority_queue < Node, vector<Node>, decltype(&Compare_pq)> p_queue;

private:
	static WorkingMemory m_WMSet;
	static vector<int>alphaListIDDictionary;
	static vector<int>betaListIDDictionary;
	static vector<int>termListIDDictionary;
	static vector<Node*> NodeList;
};

