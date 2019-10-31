/*
Jack Wharton and Hunter Ferrell
CptS 223 PA5
10/25/18
*/

#include "Scheduler.h"

int main(void)
{
	ifstream infile;
	infile.open("test.txt");
	Scheduler s1(12);
	while (!infile.eof() || !s1.isEmpty())
	{
		s1.testTick(infile);
	}
	return 0;
}