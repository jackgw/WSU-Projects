#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

int maxSubSum1(const vector<int> & a);
int maxSubSum2(const vector<int> & a);
int maxSubSum3(const vector<int> & a);
int maxSubSum4(const vector<int> & a);
int maxSumRec(const vector<int> &a, int left, int right);
int max3(int sum1, int sum2, int sum3);
void loadFromFile(vector<int> & a, fstream &infile);

int main(void)
{
	vector<int> a;
	int sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
	duration<double> timeSum1, timeSum2, timeSum3, timeSum4;
	double avgTime1 = 0, avgTime2 = 0, avgTime3 = 0, avgTime4 = 0;
	high_resolution_clock::time_point time;
	stringstream fileName; // stream for formatting 
	fstream infile;
	for (int n = 8; n <= 8192; n *= 2)
	{
		cout << endl << "N = " << n << endl;
		avgTime1 = avgTime2 = avgTime3 = avgTime4 = 0;
		for (int i = 0; i < 10; i++)
		{
			fileName.str(""); // clear the stringstream
			fileName << "input_" << n << "_" << i << ".txt" << "\0";
			infile.open(fileName.str());
			a.clear();
			loadFromFile(a, infile);

			cout << "File: " << fileName.str() << endl;

			// execute and time each algorithm
			time = high_resolution_clock::now();
			sum4 = maxSubSum4(a);
			timeSum4 = (high_resolution_clock::now() - time);
			cout << "	Algorithm 4 result: " << sum4 << ", time taken: " << timeSum4.count() << "seconds" << endl;
			avgTime4 += timeSum4.count(); // keep track of total

			time = high_resolution_clock::now();
			sum3 = maxSubSum3(a);
			timeSum3 = (high_resolution_clock::now() - time);
			cout << "	Algorithm 3 result: " << sum3 << ", time taken: " << timeSum3.count() << "seconds" << endl;
			avgTime3 += timeSum3.count();

			time = high_resolution_clock::now();
			sum2 = maxSubSum2(a);
			timeSum2 = (high_resolution_clock::now() - time);
			cout << "	Algorithm 2 result: " << sum2 << ", time taken: " << timeSum2.count() << "seconds" << endl;
			avgTime2 += timeSum2.count();

			time = high_resolution_clock::now();
			sum1 = maxSubSum1(a);
			timeSum1 = (high_resolution_clock::now() - time);
			cout << "	Algorithm 1 result: " << sum1 << ", time taken: " << timeSum1.count() << "seconds" << endl;
			avgTime1 += timeSum1.count();

			infile.close();
			infile.clear();
		}

		// calculate average time for each algorithms
		avgTime1 = avgTime1 / 10;
		avgTime2 = avgTime2 / 10;
		avgTime3 = avgTime3 / 10;
		avgTime4 = avgTime4 / 10;

		// print out average times for set of 10
		cout << "Algorithm 1 average time: " << avgTime1 << endl;
		cout << "Algorithm 2 average time: " << avgTime2 << endl;
		cout << "Algorithm 3 average time: " << avgTime3 << endl;
		cout << "Algorithm 4 average time: " << avgTime4 << endl;
	}
	return 0;
}

void loadFromFile(vector<int> & a, fstream &infile)
{
	int temp = 0;
	while (infile >> temp)
	{
		a.push_back(temp);
	}
}

// code taken from Weiss Textbook
int maxSubSum1(const vector<int> & a)
{
	int maxSum = 0;

	for (int i = 0; i < a.size(); ++i)
	{
		for (int j = i; j < a.size(); j++)
		{
			int thisSum = 0;
			for (int k = i; k <= j; k++)
			{
				thisSum += a[k];
			}
			if (thisSum > maxSum)
			{
				maxSum = thisSum;
			}
		}
	}
	return maxSum;
}

// code taken from Weiss textbook
int maxSubSum2(const vector<int> & a)
{
	int maxSum = 0;

	for (int i = 0; i < a.size(); ++i)
	{
		int thisSum = 0;
		for (int j = i; j < a.size(); ++j)
		{
			thisSum += a[j];
			if (thisSum > maxSum)
			{
				maxSum = thisSum;
			}
		}
	}
	return maxSum;
}

// Proceduure call for recursive function
int maxSubSum3(const vector<int> & a)
{
	return maxSumRec(a, 0, a.size() - 1);
}

// code taken from Weiss textbook
int maxSumRec(const vector<int> &a, int left, int right)
{
	if (left == right) // base case
		if (a[left] > 0)
			return a[left];
		else
			return 0;

	int center = (left + right) / 2;
	int maxLeftSum = maxSumRec(a, left, center);
	int maxRightSum = maxSumRec(a, center + 1, right);

	int maxLeftBorderSum = 0, leftBorderSum = 0;
	for (int i = center; i >= left; --i)
	{
		leftBorderSum += a[i];
		if (leftBorderSum > maxLeftBorderSum)
		{
			maxLeftBorderSum = leftBorderSum;
		}
	}

	int maxRightBorderSum = 0, rightBorderSum = 0;
	for (int j = center + 1; j <= right; ++j)
	{
		rightBorderSum += a[j];
		if (rightBorderSum > maxRightBorderSum)
		{
			maxRightBorderSum = rightBorderSum;
		}
	}
	return max3(maxLeftSum, maxRightSum, maxLeftBorderSum + maxRightBorderSum);
}

// find the max of 3 integers
int max3(int sum1, int sum2, int sum3)
{
	int maxSum = sum1;
	if (sum2 > maxSum)
	{
		maxSum = sum2;
	}
	if (sum3 > maxSum)
	{
		maxSum = sum3;
	}
	return maxSum;
}

// code taken from Weiss textbook
int maxSubSum4(const vector<int> & a)
{
	int maxSum = 0, thisSum = 0;
	for (int j = 0; j < a.size(); ++j)
	{
		thisSum += a[j];
		if (thisSum > maxSum)
		{
			maxSum = thisSum;
		}
		else if (thisSum < 0)
		{
			thisSum = 0;
		}
	}
	return maxSum;
}
