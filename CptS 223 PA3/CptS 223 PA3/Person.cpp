#include "Person.h"

//	default constructor
Person::Person()
{
	this->position = 0;
}
//	Constructor accepting input position
Person::Person(int pos)
{
	this->position = pos;
}

// Destructor
// ...
Person::~Person() {}

// print the position
void Person::print()
{
	cout << this->position;
}