#include "QuadraticProbingFullLength.h"
#include "QuadraticProbingPrefix.h"
#include "QuadraticProbingSimple.h"
#include "SeparateChaining.h"
#include "LinearProbing.h"
#include <iostream>
#include <fstream>
using namespace std;

int main(void)
{
	string TempString;
	vector<string> DataArray;
	vector<string> QueryArray;
	fstream DataFile("OHenry.txt");
	fstream QueryFile("Queries.txt");

	//Load Data from Data File
	while (DataFile >> TempString)
	{
		DataArray.push_back(TempString);
	}
	//Load Data from Query File
	while (QueryFile >> TempString)
	{
		QueryArray.push_back(TempString);
	}
	DataFile.close();
	QueryFile.close();

	ChainingHT<string> ChainHT;
	LinearProbingHT<string> LinearHT;
	QuadraticProbingHT<string> QuadraticHT;
	QuadraticProbingPrefixHT<string> QuadraticPrefixHT;
	QuadraticProbingSimpleHT<string> QuadraticSimpleHT;

	// Separate Chaining
	ChainHT.InsertIntoChainingHT(DataArray);
	ChainHT.SearchChainingHT(QueryArray);
	ChainHT.PrintData();
	ChainHT.makeEmpty();

	// Linear probing
	LinearHT.InsertIntoLinearProbingHT(DataArray);
	LinearHT.SearchLinearProbingHT(QueryArray);
	LinearHT.PrintData();
	LinearHT.makeEmpty();

	// Quadratic probing with default (full length) hash function
	QuadraticHT.InsertIntoQuadraticProbingHT(DataArray);
	QuadraticHT.SearchQuadraticProbingHT(QueryArray);
	QuadraticHT.PrintData();
	QuadraticHT.makeEmpty();

	// Quadratic probing with prefix hash function
	QuadraticPrefixHT.InsertIntoQuadraticProbingHT(DataArray);
	QuadraticPrefixHT.SearchQuadraticProbingHT(QueryArray);
	QuadraticPrefixHT.PrintData();
	QuadraticPrefixHT.makeEmpty();

	// Quadratic probing with simple hash function
	QuadraticSimpleHT.InsertIntoQuadraticProbingHT(DataArray);
	QuadraticSimpleHT.SearchQuadraticProbingHT(QueryArray);
	QuadraticSimpleHT.PrintData();
	QuadraticSimpleHT.makeEmpty();
}