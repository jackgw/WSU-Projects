#include "board.h"

/*
Constructor:
	sets initial variables
Complexity: constant, O(1)
*/
Board::Board()
{
	this->N = 0;
	this->M = 0;
	this->playerItr = this->playerMap.begin();
	this->coordinateItr = this->coordinateMap.begin();
}

/*
Input Constructor:
	sets initial values based on an input of N
Complexity: constant, O(1)
*/
Board::Board(int m)
{
	this->M = m;
	this->N = 0;
	this->playerItr = this->playerMap.begin();
	this->coordinateItr = this->coordinateMap.begin();
}

/*
Destructor:
	Erases every node contained in both the player and coordinate maps
Complexity: O(N)
*/
Board::~Board()
{
	while (!this->playerMap.empty()) // N
	{
		this->playerItr = this->playerMap.begin();
		this->playerMap.erase(this->playerItr);
	}
	while (!this->coordinateMap.empty()) // N
	{
		this->coordinateItr = this->coordinateMap.begin();
		this->coordinateMap.erase(this->coordinateItr);
	}
}

/*
Player Insert
	inserts a player into the player and coordinate maps. Prints error messages if player already present or coordinate occupied.
Complexity: O(log(N))
*/
bool Board::insert(int playerID, int x, int y)
{
	bool result = false;
	if (!find(playerID)) // log(N)
	{
		if (!isOccupied(pair<int, int>(x, y))) // log(N)
		{
			Player tempPlayer(playerID, x, y);
			this->playerMap.insert(pair<int, Player>(playerID, tempPlayer)); // log(N)
			this->coordinateMap.insert(pair<pair<int, int>, int>(pair<int, int>(x, y), playerID)); // log(N)
			this->N++;
			result = true;
		}
		else
		{
			cout << "Error: coordinate occupied" << endl;
		}
	}
	else
	{
		cout << "Error: ID already present" << endl;
	}
	return result;
}

/*
Player Remove:
	removes a player from both maps based on a ID key. prints error if ID not found.
Complexity: O(log(N))
*/
bool Board::remove(int playerID)
{
	bool found = false;
	this->playerItr = this->playerMap.find(playerID); // log(N)
	if (this->playerItr != this->playerMap.end())
	{
		this->coordinateMap.erase(pair<int, int>(this->playerItr->second.getX(), this->playerItr->second.getY())); // remove coordinate from occupied list (log(N))
		this->playerMap.erase(this->playerItr);
		this->N--;
		found = true;
	}
	else
	{
		cout << "Error: ID not found" << endl;
	}
	return found;
}

/*
Find:
	returns boolean variable reflecting if the playerID is already present in the player map.
Complexity: O(log(N))
*/
bool Board::find(int playerID)
{
	bool found = false;
	this->playerItr =  this->playerMap.find(playerID); // log(N)
	if (this->playerItr != this->playerMap.end())
	{
		found = true;
	}
	return found;
}

/*
isOccupied:
	returns boolean variable reflecting if the coordinate already contains a player.
Complexity: O(log(N))
*/
bool Board::isOccupied(pair<int, int> coordinates)
{
	bool found = false;
	this->coordinateItr = this->coordinateMap.find(coordinates); // log(N)
	if (this->coordinateItr != this->coordinateMap.end())
	{
		found = true;
	}
	return found;
}

/*
Move Player:
	Moves a player to another coordinate. Performs checks to make sure that the ID exists, move is not out of bounds, and move does not violate
	directional constraints, printing an error messages if any of these rules are broken. If a player already exists at the move coordinate, the
	function first deletes the player at that coordinate, then makes the move, printing the ID of the deleted player.
Complexity: O(log(N))
*/
bool Board::moveTo(int playerID, int x2, int y2)
{
	bool results = false;
	this->playerItr = this->playerMap.find(playerID); // log(N)
	if (this->playerItr != this->playerMap.end()) // ID exists
	{
		if ((x2 >= 0 && this->M >= x2) && (y2 >= 0 && this->M >= y2)) // move not out of bounds
		{
			if (((this->playerItr->second.getX() == x2) || (this->playerItr->second.getY() == y2))  // vertical or horizontal
				|| (((x2 - y2) == (this->playerItr->second.getX() - this->playerItr->second.getY()))
					|| ((x2 + y2) == (this->playerItr->second.getX() + this->playerItr->second.getY())))) // diagonal
			{
				results = true;
				if (isOccupied(pair<int, int>(x2, y2))) // log(N)
				{
					// use coordinateMap to search for ID based on coordinate key
					this->coordinateItr = this->coordinateMap.find(pair<int, int>(x2, y2)); // log(N)
					cout << "Player " << this->coordinateItr->second << " removed" << endl;
					this->playerMap.erase(this->coordinateItr->second); // remove destination ID from player map (log(N))
					this->coordinateMap.erase(pair<int, int>(x2, y2)); // remove destination coordinates from coordinate map (log(N))
					this->N--;
				}
				this->coordinateMap.erase(pair<int, int>(this->playerItr->second.getX(), this->playerItr->second.getY())); // remove prev coordinates from coordinate map
				this->playerItr->second.setX(x2);
				this->playerItr->second.setY(y2);
				this->coordinateMap.insert(pair<pair<int, int>, int>(pair<int, int>(x2, y2), playerID)); // add destination coordinates to coordinate map
			}
			else
			{
				cout << "Error: invalid direction" << endl;
			}
		}
		else
		{
			cout << "Error: out of bounds" << endl;
		}
	}
	else
	{
		cout << "Error: nonexistant ID" << endl;
	}
	return results;
}

/*
Print Players by ID:
	Traverses through the player map, printing all the players and their coordinates in ascending order.
Complexity: O(N)
*/
void Board::printByID()
{
	cout << endl;
	for (this->playerItr = this->playerMap.begin(); this->playerItr != this->playerMap.end(); playerItr++)
	{
		cout << "ID: " << this->playerItr->first << ", Coordinates: (" 
			<< this->playerItr->second.getX() << "," << this->playerItr->second.getY() << ")" << endl;
	}
	cout << endl;
}