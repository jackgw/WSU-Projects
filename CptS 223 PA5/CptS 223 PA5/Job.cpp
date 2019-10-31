#include "Job.h"

Job::Job()
{
	this->job_description = "\0";
	this->job_id = 0;
	this->n_procs = 0;
	this->n_ticks = 0;
}

Job::Job(int id, string description, int procs, int ticks)
{
	this->job_description = description;
	this->job_id = id;
	this->n_procs = procs;
	this->n_ticks = ticks;
}

Job::~Job() {}

int Job::getTicks() const
{
	return this->n_ticks;
}

int Job::getProcs() const
{
	return this->n_procs;
}

int Job::getID() const
{
	return this->job_id;
}

string Job::getDescription() const
{
	return this->job_description;
}

// overloaded operators to ensure proper insertion.

bool operator>(const Job &lhs, const Job &rhs)
{
	if (lhs.n_ticks < rhs.n_ticks)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool operator<(const Job &lhs, const Job &rhs)
{
	if (lhs.n_ticks > rhs.n_ticks)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool operator==(const Job &lhs, const Job &rhs)
{
	if (lhs.n_ticks == rhs.n_ticks)
	{
		return true;
	}
	else
	{
		return false;
	}
}