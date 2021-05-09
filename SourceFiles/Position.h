#pragma once
#include <string>
class Position
{
public:
	Position(int x, int y, int z);
	int x;
	int y;
	int z;

	std::string convertToString(int index);
};

