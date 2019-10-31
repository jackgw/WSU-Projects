#include "Scheduler.h"

Scheduler::Scheduler()
{
	this->totalProcessors = 0;
	this->availableProcs = 0;
	this->currentID = 1;
	this->globalTicks = 1;
}

Scheduler::Scheduler(int procs)
{
	this->totalProcessors = procs;
	this->availableProcs = procs;
	this->currentID = 1;
	this->globalTicks = 1;
}

Scheduler::~Scheduler()
{
	// empty both queues if not already empty
	while (!this->waitQueue.empty())
	{
		this->waitQueue.pop();
	}
	while (!this->runningQueue.empty())
	{
		this->runningQueue.pop();
	}
}

int Scheduler::Tick()
{
	// Prompt user for Insert
	string desc;
	int procs = 0;
	int ticks = 0;
	cout << "Enter a Job description followed by the number of procs required and the number of ticks required in the format:\n(J1   1   3).\nIf no job insert if desired, specify the job description as \"NULL\".\n";
	cin >> desc >> procs >> ticks;
	if (desc != "NULL")
	{
		// Insert Job into Wait Queue
		InsertJob(this->currentID, desc, procs, ticks);
	}
	if (desc == "exit")
	{
		return 1;
	}
	//decrement the timer for running jobs
	this->DecrementTimer();
	//release jobs that are finished from running Queue and return procs to available
	ReleaseProcs();
	if (!this->waitQueue.empty() && CheckAvailability(this->waitQueue.top()))
	{
		Job tempJob;
		tempJob = this->FindShortest(); // find shortest waiting job
		this->DeleteShortest(); // remove job from wait queue
		RunJob(tempJob);
	}
	return 0;
}

void Scheduler::testTick(ifstream &infile)
{
	// get insert from file
	if (!infile.eof())
	{
		string desc;
		int procs = 0;
		int ticks = 0;
		infile >> desc >> procs >> ticks;
		if (desc != "NULL")
		{
			// Insert Job into Wait Queue
			if (InsertJob(this->currentID, desc, procs, ticks))
			{
				cout << "Tick " << this->globalTicks << " Job inserted: " << endl << "ID: " << this->currentID 
					<< " Job Description: " << desc << " Procs: " << procs << " Ticks: " << ticks << endl << endl;
			}
		}
		this->currentID++;
	}
	//decrement the timer for running jobs
	this->DecrementTimer();
	//release jobs that are finished from running Queue and return procs to available
	ReleaseProcs();
	while (!this->waitQueue.empty() && CheckAvailability(this->waitQueue.top()))
	{
		Job tempJob;
		tempJob = this->FindShortest(); // find shortest waiting job
		cout << "Tick " << this->globalTicks << " Job allocated to running Queue: " << endl << "ID: " <<tempJob.getID() << " Job Description: " << tempJob.getDescription()
			<< " Procs: " << tempJob.getProcs() << " Ticks: " << tempJob.getTicks() << endl << endl;
		this->DeleteShortest(); // remove job from wait queue
		RunJob(tempJob);
	}
}

bool Scheduler::InsertJob(int id, string description, int procs, int ticks)
{
	//check to see if ticks and procs are valid
	if ((0 < procs && procs <= this->totalProcessors) && (ticks > 0))
	{
		Job newJob(id, description, procs, ticks);
		this->waitQueue.push(newJob);
		return true;
	}
	else
	{
		cout << "Error: invalid ticks or procs" << endl;
		return false;
	}
}

Job Scheduler::FindShortest()
{
	return this->waitQueue.top();
}

bool Scheduler::isEmpty() // function to check if both queues are empty, for use in main.
{
	if (this->waitQueue.empty() && this->runningQueue.empty())
	{
		return true;
	}
	return false;
}

bool Scheduler::CheckAvailability(Job newJob)
{
	// if there are enough available processors
	if ((this->availableProcs - newJob.getProcs()) >= 0)
	{
		return true;
	}
	return false;
}

void Scheduler::DeleteShortest()
{
	this->waitQueue.pop();
}

void Scheduler::RunJob(Job newJob)
{
	this->availableProcs -= newJob.getProcs();
	this->runningQueue.push(pair<int, Job>(newJob.getTicks() + this->globalTicks, newJob));
}

void Scheduler::DecrementTimer()
{
	this->globalTicks++;
}

void Scheduler::ReleaseProcs()
{
	int releasedProcs = 0;
	if (!this->runningQueue.empty())
	{
		while (!this->runningQueue.empty() && (this->runningQueue.top().first == this->globalTicks))
		{
			// print message saying job was completed
			cout << "Tick: " << this->globalTicks << " Job Completed: " << endl << "ID: " << this->runningQueue.top().second.getID()
				<< " Job Description: " << this->runningQueue.top().second.getDescription()
				<< " Procs: " << this->runningQueue.top().second.getProcs()
				<< " Ticks: " << this->runningQueue.top().second.getTicks() << endl << endl;
			// add procs back to pool and remove job from running queue
			releasedProcs += this->runningQueue.top().second.getProcs();
			this->runningQueue.pop();
		}
		this->availableProcs += releasedProcs;
	}
}

// Running Queue Overloaded Comparisons
bool operator>(const pair<int, Job> &lhs, const pair<int, Job> &rhs)
{
	if (lhs.first < rhs.first)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool operator<(const pair<int, Job> &lhs, const pair<int, Job> &rhs)
{
	if (lhs.first > rhs.first)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool operator==(const pair<int, Job> &lhs, const pair<int, Job> &rhs)
{
	if (lhs.first == rhs.first)
	{
		return true;
	}
	else
	{
		return false;
	}
}