#ifndef LINEAR_PROBING_H
#define LINEAR_PROBING_H

/*
Code Modified from QuadraticProbing.h, as provided in the assignment description:
https://users.cs.fiu.edu/~weiss/dsaa_c++4/code/
*/

#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <chrono>
#include <iostream>
using namespace std;
using namespace std::chrono;

int nextPrime( int n );

// QuadraticProbing Hash table class
//
// CONSTRUCTION: an approximate initial size or default of 101
//
// ******************PUBLIC OPERATIONS*********************
// bool insert( x )       --> Insert x
// bool remove( x )       --> Remove x
// bool contains( x )     --> Return true if x is present
// void makeEmpty( )      --> Remove all items
// int hashCode( string str ) --> Global method to hash strings

template <typename HashedObj>
class LinearProbingHT
{
  public:
    explicit LinearProbingHT( int size = 101 ) : array( nextPrime( size ) )
      { makeEmpty( ); }

	~LinearProbingHT()
	{
		makeEmpty();
	}

    bool contains( const HashedObj & x )
    {
        return isActive( findPos( x ) );
    }

    void makeEmpty( )
    {
        currentSize = 0;
        for( auto & entry : array )
            entry.info = EMPTY;
    }

    bool insert( const HashedObj & x )
    {
            // Insert x as active
        int currentPos = findPos( x );
        if( isActive( currentPos ) )
            return false;

        if( array[ currentPos ].info != DELETED )
            ++currentSize;

        array[ currentPos ].element = x;
        array[ currentPos ].info = ACTIVE;

            // Rehash; see Section 5.5
        if( currentSize > array.size( ) / 2 )
            rehash( );

        return true;
    }
    
    bool insert( HashedObj && x )
    {
            // Insert x as active
        int currentPos = findPos( x );
        if( isActive( currentPos ) )
            return false;

        if( array[ currentPos ].info != DELETED )
            ++currentSize;

        array[ currentPos ] = std::move( x );
        array[ currentPos ].info = ACTIVE;

            // Rehash; see Section 5.5
        if( currentSize > array.size( ) / 2 )
            rehash( );

        return true;
    }

    bool remove( const HashedObj & x )
    {
        int currentPos = findPos( x );
        if( !isActive( currentPos ) )
            return false;

        array[ currentPos ].info = DELETED;
        return true;
    }

	void InsertIntoLinearProbingHT(const vector<HashedObj> DataArray)
	{
		for (int i = 0; i < DataArray.size(); i++)
		{
			currentTime = high_resolution_clock::now();
			insert(DataArray.at(i));
			diff = (high_resolution_clock::now() - currentTime);
			InsertionTimerLinearProbingHT += diff.count();
		}
	}

	void SearchLinearProbingHT(const vector<HashedObj> QueryArray)
	{
		for (int i = 0; i < QueryArray.size(); i++)
		{
			currentTime = high_resolution_clock::now();
			contains(QueryArray.at(i));
			diff = (high_resolution_clock::now() - currentTime);
			SearchTimerLinearProbingHT += diff.count();
		}
	}

	void PrintData()
	{
		std::cout << "Linear Probing:\nCollisions: " << this->CollisionsLinearProbingHT << "\nTotal Insertion Time (ms): "
			<< this->InsertionTimerLinearProbingHT << "\nAverage Insertion Time (ms): " << this->InsertionTimerLinearProbingHT / 10377
			<< "\nTotal Search Time (ms): " << this->SearchTimerLinearProbingHT << "\nAverage Search Time (ms): "
			<< this->SearchTimerLinearProbingHT / 1500 << "\n\n";
	}

    enum EntryType { ACTIVE, EMPTY, DELETED };

  private:
    struct HashEntry
    {
        HashedObj element;
        EntryType info;

        HashEntry( const HashedObj & e = HashedObj{ }, EntryType i = EMPTY )
          : element{ e }, info{ i } { }
        
        HashEntry( HashedObj && e, EntryType i = EMPTY )
          : element{ std::move( e ) }, info{ i } { }
    };
    
    vector<HashEntry> array;
    int currentSize;
	int CollisionsLinearProbingHT = 0;
	high_resolution_clock::time_point currentTime;
	duration<double, milli> diff;
	double InsertionTimerLinearProbingHT = 0.0;
	double SearchTimerLinearProbingHT = 0.0;

    bool isActive( int currentPos ) const
      { return array[ currentPos ].info == ACTIVE; }

    int findPos( const HashedObj & x )
    {
        int offset = 1;
        int currentPos = myhash( x );

        while( array[ currentPos ].info != EMPTY &&
               array[ currentPos ].element != x )
        {
			CollisionsLinearProbingHT++; // increment total collisions
            currentPos += offset;  // Compute ith probe
            if( currentPos >= array.size( ) )
                currentPos -= array.size( );
        }

        return currentPos;
    }

    void rehash( )
    {
        vector<HashEntry> oldArray = array;

            // Create new double-sized, empty table
        array.resize( nextPrime( 2 * oldArray.size( ) ) );
        for( auto & entry : array )
            entry.info = EMPTY;

            // Copy table over
        currentSize = 0;
        for( auto & entry : oldArray )
            if( entry.info == ACTIVE )
                insert( std::move( entry.element ) );
    }

    size_t myhash( const HashedObj & x ) const
    {
        static hash<HashedObj> hf;
        return hf( x ) % array.size( );
    }
};

#endif
