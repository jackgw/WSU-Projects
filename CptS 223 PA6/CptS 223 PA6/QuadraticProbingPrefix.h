#ifndef QUADRATIC_PROBING_PREFIX_H
#define QUADRATIC_PROBING_PREFIX_H

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
class QuadraticProbingPrefixHT
{
  public:
    explicit QuadraticProbingPrefixHT( int size = 101 ) : array( nextPrime( size ) )
      { makeEmpty( ); }

	~QuadraticProbingPrefixHT()
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

	// Added function, insert from vector
	void InsertIntoQuadraticProbingHT(const vector<HashedObj> DataArray)
	{
		for (int i = 0; i < DataArray.size(); i++)
		{
			currentTime = high_resolution_clock::now();
			insert(DataArray.at(i));
			diff = (high_resolution_clock::now() - currentTime);
			InsertionTimerQuadraticProbingHT += diff.count();
		}
	}

	// Added function, search from vector
	void SearchQuadraticProbingHT(const vector<HashedObj> QueryArray)
	{
		for (int i = 0; i < QueryArray.size(); i++)
		{
			currentTime = high_resolution_clock::now();
			contains(QueryArray.at(i));
			diff = (high_resolution_clock::now() - currentTime);
			SearchTimerQuadraticProbingHT += diff.count();
		}
	}

	// Added function, print collisions and timing
	void PrintData()
	{
		std::cout << "Quadratic Probing (Prefix Hash):\nCollisions: " << this->CollisionsQuadraticProbingHT << "\nTotal Insertion Time (ms): "
			<< this->InsertionTimerQuadraticProbingHT << "\nAverage Insertion Time (ms): " << this->InsertionTimerQuadraticProbingHT / 10377
			<< "\nTotal Search Time (ms): " << this->SearchTimerQuadraticProbingHT << "\nAverage Search Time (ms): "
			<< this->SearchTimerQuadraticProbingHT / 1500 << "\n\n";
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
	int CollisionsQuadraticProbingHT = 0;
	high_resolution_clock::time_point currentTime;
	duration<double, milli> diff;
	double InsertionTimerQuadraticProbingHT = 0.0;
	double SearchTimerQuadraticProbingHT = 0.0;

    bool isActive( int currentPos ) const
      { return array[ currentPos ].info == ACTIVE; }

    int findPos(const HashedObj & x )
    {
        int offset = 1;
        int currentPos = PrefixHash( x );

        while( array[ currentPos ].info != EMPTY &&
               array[ currentPos ].element != x )
        {
			CollisionsQuadraticProbingHT++; // Add one to total collisions
            currentPos += offset;  // Compute ith probe
            offset += 2;
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

	// Function taken from assignment description
	size_t PrefixHash(const HashedObj & key) const
	{
		unsigned int hashVal = 0;
		for (char ch : key)
		{
			hashVal = 37 * hashVal + ch;
		}
		return hashVal % array.size();
	}
};

#endif
