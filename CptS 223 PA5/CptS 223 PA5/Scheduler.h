#include "Job.h"
#include <queue>
#include <fstream>

using namespace std;

class Scheduler
{
public:
	Scheduler();
	Scheduler(int procs);
	~Scheduler();

	void testTick(ifstream &infile);

	int Tick();
	bool InsertJob(int id, string description, int procs, int ticks);
	Job FindShortest();
	bool isEmpty();
	bool CheckAvailability(Job newJob);
	void DeleteShortest();
	void RunJob(Job newJob);
	void DecrementTimer();
	void ReleaseProcs();
private:
	priority_queue <Job> waitQueue;
	priority_queue <pair<int, Job>> runningQueue;
	int availableProcs;
	int currentID;
	int totalProcessors;
	int globalTicks;
};

bool operator>(const pair<int, Job> &lhs, const pair<int, Job> &rhs);
bool operator<(const pair<int, Job> &lhs, const pair<int, Job> &rhs);
bool operator==(const pair<int, Job> &lhs, const pair<int, Job> &rhs);