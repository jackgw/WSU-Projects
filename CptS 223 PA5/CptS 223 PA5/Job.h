#include <iostream>
#include <string>

using namespace std;

class Job
{
public:
	Job();
	Job(int id, string description, int procs, int ticks);
	~Job();

	int getTicks() const;
	int getProcs() const;
	int getID() const;
	string getDescription() const;

	//overloaded operators
	friend bool operator>(const Job &lhs, const Job &rhs);
	friend bool operator<(const Job &lhs, const Job &rhs);
	friend bool operator==(const Job &lhs, const Job &rhs);
private:
	int job_id;
	string job_description;
	int n_procs;
	int n_ticks;
};

