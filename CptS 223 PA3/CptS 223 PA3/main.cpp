#include "TestListMyJosephus.h"
#include "TestVectorMyJosephus.h"

int main(void)
{
	int n = 0, m = 0;
	cout << "Enter N: ";
	cin >> n;
	cout << "Enter M: ";
	cin >> m;

	listTest(n, m);
	vectorTest(n, m);

	return 0;
}