#include "VectorMyJosephus.h"
#include "Person.h"

// Default constructor
VectorMyJosephus::VectorMyJosephus()
{
	this->M = 0;
	this->N = 0;
	this->size = 0;
	this->target = 0;
	this->circ.clear();
}

//	Constructor accepting inputs of N and M
VectorMyJosephus::VectorMyJosephus(int inputN, int inputM)
{
	this->M = inputM;
	this->N = inputN;
	this->size = inputN;
	this->target = 0;
	//	Create a vector of N people
	for (int i = 0; i < inputN; i++)
	{
		circ.push_back(Person(i));
	}
}

// Destructor: make sure you free everything you allocated.
VectorMyJosephus::~VectorMyJosephus()
{
	circ.clear();
}

//	This init function does the same thing as the argument constructor above.
void VectorMyJosephus::init(int inputN, int inputM)
{
	this->M = inputM;
	this->N = inputN;
	this->size = inputN;
	for (int i = 0; i < inputN; i++)
	{
		circ.push_back(Person(i));
	}
}

//	makes the circle empty 
void VectorMyJosephus::clear()
{
	circ.clear();
}

//	prints the number of people still playing the game (i.e., not yet eliminated) 
int VectorMyJosephus::currentSize()
{
	return this->size;
}

//	returns true if circle is empty
bool VectorMyJosephus::isEmpty()
{
	return this->circ.empty();
}

// eliminates the next person as per the game's rule
Person VectorMyJosephus::eliminateNext()
{
	Person tempPerson;
	if (!circ.empty())
	{
		if (this->target == size)
		{
			target = 0;
		}
		if (this->target + this->M > this->size - 1) // simply adding M will cause target to go outside vector
		{
			if (this->M <= this->size) // only one reset back to beginning needed
			{
				this->target = this->M - ((this->size - 1) - this->target) - 1;
			}
			else // will need to reset multiple times
			{
				int tempM = this->M;
				while (tempM > this->size) // reduce temp M by size until it won't go outside the vector
				{
					tempM -= this->size;
				}
				if (this->target + tempM > this->size - 1)
				{
					this->target = tempM - ((this->size - 1) - this->target) - 1;
				}
				else
				{
					this->target += tempM;
				}
			}
		}
		else // safe to just add M
		{
			this->target += this->M;
		}
		tempPerson = circ.at(this->target);
		circ.erase(circ.begin() + this->target);
		this->size--;
	}
	return tempPerson;
}

// prints the current content of circle in sequence starting from the person with the least position number
void VectorMyJosephus::printAll()
{
	for (int i = 0; i < this->size; i++)
	{
		circ.at(i).print();
		cout << " ";
	}
	cout << endl;
}