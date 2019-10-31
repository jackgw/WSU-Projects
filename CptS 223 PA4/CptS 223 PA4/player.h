#include <iostream>
#include <map>
#include <set>
#include <iterator>

using namespace std;

class Player
{
public:
	Player();
	Player(int playerid, int x, int y);
	~Player();

	// setters and getters
	int getX();
	int getY();
	void setX(int x);
	void setY(int y);

private:
	int ID;
	int xPos;
	int yPos;
};