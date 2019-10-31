#include "ListMyJosephus.h"

//	Default Constructor
ListMyJosephus::ListMyJosephus()
{
	this->M = 0;
	this->N = 0;
	this->size = 0;
	this->circ.clear();
	this->target = circ.begin();
}

//	Constructor accepting N and as input
ListMyJosephus::ListMyJosephus(int inputN, int inputM)
{
	this->M = inputM;
	this->N = inputN;
	this->size = inputN;
	//	Create a vector of N people
	for (int i = 0; i < inputN; i++)
	{
		circ.push_back(Person(i));
	}
	this->target = circ.begin();
}

// Destructor: make sure you free everything you allocated.
ListMyJosephus::~ListMyJosephus()
{
	circ.clear();
}

//		This init function does the same thing as the argument constructor above.
void ListMyJosephus::init(int inputN, int inputM)
{
	this->M = inputM;
	this->N = inputN;
	this->size = inputN;
	//	Create a vector of N people
	for (int i = 0; i < inputN; i++)
	{
		circ.push_back(Person(i));
	}
	this->target = circ.begin();
}

//	makes the circle empty 
void ListMyJosephus::clear()
{
	circ.clear();
}

//	prints the number of people still playing the game (i.e., not yet eliminated) 
int ListMyJosephus::currentSize()
{
	return this->size;
}

//	returns true if circle is empty
bool ListMyJosephus::isEmpty()
{
	return this->circ.empty();
}

// eliminates the next person as per the game's rule
Person ListMyJosephus::eliminateNext()
{
	Person tempPerson;
	if (!circ.empty())
	{
		if (this->target == circ.end()) // case where target starts out of list
		{
			this->target = circ.begin();
		}
		for (int counter = 0; counter < this->M; counter++) // iterate m times
		{
			this->target++;
			if (this->target == circ.end()) // if iterator goes outside list, reset
			{
				this->target = circ.begin();
			}
		}
		tempPerson = *(this->target);
		this->target = circ.erase(this->target);
		size--;
	}
	return tempPerson;
}

// prints the current content of circle in sequence starting from the person with the least position number
void ListMyJosephus::printAll()
{
	for (list<Person>::iterator i = circ.begin(); i != circ.end(); i++)
	{
		(*i).print();
		cout << " ";
	}
	cout << endl;
}