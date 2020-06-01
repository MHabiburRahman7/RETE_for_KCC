#include "Node.h"

Node::Node(int id, string condition)
//Node::Node()
{
	SuperNodeID = id;
	justCondition = condition;
}

void Node::setNewMasterID(int newId)
{
	SuperNodeID = newId;
}

