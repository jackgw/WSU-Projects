#include "player.h"

Player::Player()
{
	this->ID = 0;
	this->xPos = 0;
	this->yPos = 0;
}

Player::Player(int playerid, int x, int y)
{
	this->ID = playerid;
	this->xPos = x;
	this->yPos = y;
}

Player::~Player() {}

//setters and getters
int Player::getX()
{
	return this->xPos;
}

int Player::getY()
{
	return this->yPos;
}

void Player::setX(int x)
{
	this->xPos = x;
}

void Player::setY(int y)
{
	this->yPos = y;
}