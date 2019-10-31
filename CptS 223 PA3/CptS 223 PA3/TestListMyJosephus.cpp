#include "TestListMyJosephus.h"

void listTest(int N, int M)
{
	ListMyJosephus circle(N, M);
	high_resolution_clock::time_point time;
	duration<double> tempTime;
	double totalTime = 0;
	Person winner;

	while (circle.currentSize() > 1)
	{
		time = high_resolution_clock::now();
		winner = circle.eliminateNext();
		tempTime = (high_resolution_clock::now() - time);
		totalTime += tempTime.count();
		cout << "Eliminated position: ";
		winner.print();
		cout << endl;
		circle.printAll();
	}

	winner = circle.eliminateNext(); // get last person in list
	cout << "\nList Winner: ";
	winner.print();
	cout << endl << "Total Time Taken: " << totalTime << " seconds" <<endl;
	if (N > 1) // prevents incorrect time values fror n = 0 and n = 1
	{
		cout << "Average Time per deletion: " << totalTime / (N - 1) << " seconds\n\n";
	}
	else
	{
		cout << "Average Time per deletion: " << totalTime << " seconds\n\n";
	}
}