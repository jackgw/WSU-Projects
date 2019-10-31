#include "Node.h"

Node::Node(int newData)
{
	this->mpNext = NULL;
	this->mData = newData;
}

Node::~Node() {}

Node *Node::getmpNext()
{
	return this->mpNext;
}

int Node::getmData()
{
	return this->mData;
}

void Node::setmpNext(Node *newPtr)
{
	this->mpNext = newPtr;
}

void Node::setmData(int newData)
{
	this->mData = newData;
}

