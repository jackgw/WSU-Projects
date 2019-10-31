#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
using namespace std;
using namespace std::chrono;

class Node
{
public:
	Node(int newData);
	~Node();
	
	int getmData();
	Node *getmpNext();

	void setmData(int newData);
	void setmpNext(Node *newNextPtr);

private:
	int mData;
	Node *mpNext;
};