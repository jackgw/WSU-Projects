#include "List.h"

List::List()
{
	this->mpHead = NULL;
	this->max = 0;
	this->min = 0;
	this->med = 0;
}

List::~List()
{
	while (this->mpHead != NULL)
	{
		deleteAtFront();
	}
}

Node *List::getmpHead()
{
	return this->mpHead;
}

void List::setmpHead(Node *newPtr)
{
	this->mpHead = newPtr;
}

bool List::insertInOrder(int newData)
{
	Node *pMem = NULL, *pCur = NULL, *pPrev = NULL;
	bool success = false;
	pMem = new Node(newData);
	if (pMem != NULL)
	{
		success = true;
		pCur = this->mpHead;
		while (pCur != NULL && newData > pCur->getmData())
		{
			pPrev = pCur;
			pCur = pCur->getmpNext();
		}
		if (pPrev != NULL)
		{
			pPrev->setmpNext(pMem);
			pMem->setmpNext(pCur);
		}
		else
		{
			this->mpHead = pMem;
			pMem->setmpNext(pCur);
		}
	}
	return success;
}

void List::deleteAtFront()
{
	Node *pTemp = NULL;
	pTemp = this->mpHead;
	this->mpHead = this->mpHead->getmpNext();
	delete pTemp;
}

void List::fillList(fstream &infile)
{
	int newData = 0;
	while (!infile.eof())
	{
		infile >> newData;
		insertInOrder(newData);
	}
}

void List::findMax()
{
	Node *pTemp = NULL;
	pTemp = this->mpHead;
	while (pTemp->getmpNext() != NULL) 
	{
		pTemp = pTemp->getmpNext();
	}
	this->max = pTemp->getmData();
}

void List::findMin()
{
	this->min = this->mpHead->getmData();
}

void List::findMed()
{
	Node *pTemp = NULL;
	int size = 0, index = 0;
	pTemp = this->mpHead;
	if (pTemp != NULL)
	{
		while (pTemp->getmpNext() != NULL)
		{
			pTemp = pTemp->getmpNext();
			size++;
		}
		pTemp = this->mpHead;
		if (size % 2 == 0) // even
		{
			for (index = 0; index < size / 2; index++)
			{
				pTemp = pTemp->getmpNext();
			}
			this->med = (pTemp->getmData() + pTemp->getmpNext()->getmData()) / 2;
		}
		else // odd
		{
			for (index = 0; index < size + 1 / 2; index++)
			{
				pTemp = pTemp->getmpNext();
			}
			this->med = pTemp->getmData();
		}
	}
}

void List::printData(duration<double> timeInsert, duration<double> timeMax, duration<double> timeMin, duration<double> timeMed)
{
	cout << "Data Loaded. Load time: " << timeInsert.count() << " seconds" << endl;
	cout << "Max value: " << this->max << ", Max time: " << timeMax.count() << " seconds" << endl;
	cout << "Min value: " << this->min << ", Min time: " << timeMin.count() << " seconds" << endl;
	cout << "Med value: " << this->med << ", Med time: " << timeMed.count() << " seconds" << endl;
}