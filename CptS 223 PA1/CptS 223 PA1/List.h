#include "Node.h"

class List
{
public:
	List();
	~List();

	Node *getmpHead();
	void setmpHead(Node *newPtr);

	bool insertInOrder(int newData);
	void deleteAtFront();

	void fillList(fstream &infile);
	void findMax();
	void findMin();
	void findMed();

	void printData(duration<double> timeInsert, duration<double> timeMax, duration<double> timeMin, duration<double> timeMed);
private:
	Node *mpHead;
	int min;
	int max;
	int med;
};