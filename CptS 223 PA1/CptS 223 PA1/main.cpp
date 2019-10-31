#include "List.h"

int main(void)
{
	string fileName = "\0";
	cout << "Enter the file name: ";
	cin >> fileName;
	fstream infile(fileName.c_str());
	List L1;

	duration<double> timeInsert, timeMax, timeMin, timeMed;
	high_resolution_clock::time_point time;

	time = high_resolution_clock::now();
	L1.fillList(infile);
	timeInsert = (high_resolution_clock::now() - time); // microseconds
	infile.close();

	time = high_resolution_clock::now();
	L1.findMax();
	timeMax = (high_resolution_clock::now() - time);

	time = high_resolution_clock::now();
	L1.findMin();
	timeMin = (high_resolution_clock::now() - time);

	time = high_resolution_clock::now();
	L1.findMed();
	timeMed = (high_resolution_clock::now() - time);

	L1.printData(timeInsert, timeMax, timeMin, timeMed);
	return 0;
}