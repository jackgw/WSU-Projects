#ifndef SEPARATE_CHAINING_H
#define SEPARATE_CHAINING_H

/*
Code Modified from SeparateChaining.h, as provided in the assignment description:
https://users.cs.fiu.edu/~weiss/dsaa_c++4/code/
*/

#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include <chrono>
#include <iostream>
using namespace std;
using namespace std::chrono;

int nextPrime( int n );

// SeparateChaining Hash table class
//
// CONSTRUCTION: an approximate initial size or default of 101
//
// ******************PUBLIC OPERATIONS*********************
// bool insert( x )       --> Insert x
// bool remove( x )       --> Remove x
// bool contains( x )     --> Return true if x is present
// void makeEmpty( )      --> Remove all items

template <typename HashedObj>
class ChainingHT
{
  public:
    explicit ChainingHT( int size = 101 ) : currentSize{ 0 }
      { theLists.resize( 101 ); }

	~ChainingHT()
	{
		makeEmpty();
	}
    bool contains( const HashedObj & x ) const
    {
        auto & whichList = theLists[ myhash( x ) ];
        return find( begin( whichList ), end( whichList ), x ) != end( whichList );
    }

    void makeEmpty( )
    {
        for( auto & thisList : theLists )
            thisList.clear( );
    }

    bool insert( const HashedObj & x )
    {
        auto & whichList = theLists[ myhash( x ) ];
		if (begin(whichList) != end(whichList)) // not empty
		{
			CollisionsChainingHT++;
		}
        if( find( begin( whichList ), end( whichList ), x ) != end( whichList) )
            return false;
        whichList.push_back( x );

            // Rehash; see Section 5.5
        if( ++currentSize > theLists.size( ) )
            rehash( );

        return true;
    }
    
    bool insert( HashedObj && x )
    {
        auto & whichList = theLists[ myhash( x ) ];      
		if (begin(whichList) != end(whichList)) // not empty
		{
			CollisionsChainingHT++;
		}
        if( find( begin( whichList ), end( whichList ), x ) != end( whichList ) )
            return false;
        whichList.push_back( std::move( x ) );

            // Rehash; see Section 5.5
        if( ++currentSize > theLists.size( ) )
            rehash( );

        return true;
    }

    bool remove( const HashedObj & x )
    {
        auto & whichList = theLists[ myhash( x ) ];
        auto itr = find( begin( whichList ), end( whichList ), x );

        if( itr == end( whichList ) )
            return false;

        whichList.erase( itr );
        --currentSize;
        return true;
    }

	// Added function, insert all items from data array
	void InsertIntoChainingHT(const vector<HashedObj> DataArray)
	{
		for (int i = 0; i < DataArray.size(); i++)
		{
			currentTime = high_resolution_clock::now();
			insert(DataArray.at(i));
			diff = (high_resolution_clock::now() - currentTime);
			InsertionTimerChainingHT += diff.count();
		}
	}

	// Added function, search for all items in query array
	void SearchChainingHT(const vector<HashedObj> QueryArray)
	{
		for (int i = 0; i < QueryArray.size(); i++)
		{
			currentTime = high_resolution_clock::now();
			contains(QueryArray.at(i));
			diff = (high_resolution_clock::now() - currentTime);
			SearchTimerChainingHT += diff.count();
		}
	}

	void PrintData()
	{
		std::cout << "Separate Chaining:\nCollisions: " << this->CollisionsChainingHT << "\nTotal Insertion Time (ms): "
			<< this->InsertionTimerChainingHT << "\nAverage Insertion Time (ms): " << this->InsertionTimerChainingHT / 10377
			<< "\nTotal Search Time (ms): " << this->SearchTimerChainingHT << "\nAverage Search Time (ms): "
			<< this->SearchTimerChainingHT / 1500 << "\n\n";
	}

  private:
    vector<list<HashedObj>> theLists;   // The array of Lists
    int  currentSize;
	int CollisionsChainingHT = 0; // total collisions
	high_resolution_clock::time_point currentTime;
	duration<double, milli> diff;
	double InsertionTimerChainingHT = 0.0; // total time to insert
	double SearchTimerChainingHT = 0.0; // total time to search

    void rehash( )
    {
        vector<list<HashedObj>> oldLists = theLists;

            // Create new double-sized, empty table
        theLists.resize( nextPrime( 2 * theLists.size( ) ) );
        for( auto & thisList : theLists )
            thisList.clear( );

            // Copy table over
        currentSize = 0;
        for( auto & thisList : oldLists )
            for( auto & x : thisList )
                insert( std::move( x ) );
    }

    size_t myhash( const HashedObj & x ) const
    {
        static hash<HashedObj> hf;
        return hf( x ) % theLists.size( );
    }
};

#endif
