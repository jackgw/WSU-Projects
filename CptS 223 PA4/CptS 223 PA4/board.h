#include "player.h"

using namespace std;

class Board
{
public:
	Board();
	Board(int m);
	~Board();

	bool insert(int playerID, int x, int y);
	bool remove(int playerID);
	bool find(int playerID);
	bool isOccupied(pair<int, int> coordinates);
	bool moveTo(int playerID, int x2, int y2);
	void printByID();

private:
	int M;
	int N;
	map <int, Player> playerMap; // map of player IDs and associated Players
	map <int, Player>::iterator playerItr;
	map <pair<int, int>, int> coordinateMap; // occupied coordinates
	map <pair<int, int>, int>::iterator coordinateItr;
};