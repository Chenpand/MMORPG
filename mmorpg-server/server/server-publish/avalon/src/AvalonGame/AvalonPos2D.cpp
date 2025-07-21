#include "AvalonPos2D.h"
#include "cmath"

namespace Avalon
{
	Int32 Pos2D::GetDirTo(const Pos2D& pos) const
	{
		const static Real32 PI_DIVIDE_EIGHT_TAN = std::tan(AVALON_PI / 8);
		const static Real32 PI_THREE_DIVIDE_ENGHT_TAN = std::tan(AVALON_PI * 3 / 8); 

		Real32 abs_tan = std::numeric_limits<Real32>::max();
		if(x != pos.x)
		{
			abs_tan = std::abs((Real32(pos.y) - y) / (Real32(pos.x) - x));
		}

		if(abs_tan >= PI_THREE_DIVIDE_ENGHT_TAN)
		{
			if(pos.y <= y) return DIR_UP;
			else return DIR_DOWN;
		}
		else if(abs_tan > PI_DIVIDE_EIGHT_TAN && abs_tan < PI_THREE_DIVIDE_ENGHT_TAN)
		{
			if(pos.x > x && pos.y < y) return DIR_RIGHTUP;
			else if(pos.x < x && pos.y < y) return DIR_LEFTUP;
			else if(pos.x < x && pos.y > y) return DIR_LEFTDOWN;
			else return DIR_RIGHTDOWN;
		}
		else
		{
			if(pos.x >= x) return DIR_RIGHT;
			else return DIR_LEFT;
		}
	}

	bool Pos2D::CheckIfInDir(const Pos2D& pos, Int32 dir) const
	{
		if(pos.x == x && pos.y == y) return true;

		switch(dir)
		{
		case DIR_UP:
			return pos.x == x && pos.y < y;
		case DIR_DOWN:
			return pos.x == x && pos.y > y;
		case DIR_LEFT:
			return pos.x < x && pos.y == y;
		case DIR_RIGHT:
			return pos.x > x && pos.y == y;
		case DIR_LEFTUP:
			return (pos.x + y  == pos.y + x) && pos.x < x;
		case DIR_LEFTDOWN:
			return (pos.x + pos.y == x + y) && pos.x < x;
		case DIR_RIGHTUP:
			return (pos.x + pos.y == x + y) && pos.x > x;
		case DIR_RIGHTDOWN:
			return (pos.x + y == pos.y + x) && pos.x > x;
		}

		return false;
	}

	bool Pos2D::GetNextPosInDir(Int32 dir, Pos2D& pos) const
	{
		pos.x = x;
		pos.y = y;

		switch(dir)
		{
		case DIR_UP:
			if(pos.y == 0) return false;
			pos.y = pos.y - 1;
			return true;
		case DIR_DOWN:
			pos.y = pos.y + 1;
			return true;
		case DIR_LEFT:
			if(pos.x == 0) return false;
			pos.x = pos.x - 1;
			return true;
		case DIR_RIGHT:
			pos.x = pos.x + 1;
			return true;
		case DIR_LEFTUP:
			if(pos.x == 0 || pos.y == 0) return false;
			pos.x = pos.x - 1;
			pos.y = pos.y - 1;
			return true;
		case DIR_LEFTDOWN:
			if(pos.x == 0) return false;
			pos.x = pos.x - 1;
			pos.y = pos.y + 1;
			return true;
		case DIR_RIGHTUP:
			if(pos.y == 0) return false;
			pos.x = pos.x + 1;
			pos.y = pos.y - 1;
			return true;
		case DIR_RIGHTDOWN:
			pos.x = pos.x + 1;
			pos.y = pos.y + 1;
			return true;
		}
		return false;
	}

	bool Pos2D::CheckIfInSector(const Pos2D& pos, Int32 dir) const
	{
		if(pos.x == x && pos.y == y) return true;

		Int32 distx = std::abs(Int32(pos.x) - x);
		Int32 disty = std::abs(Int32(pos.y) - y);

		switch(dir)
		{
		case DIR_UP:
			return disty >= distx && pos.y < y;
		case DIR_DOWN:
			return disty >= distx && pos.y > y;
		case DIR_LEFT:
			return disty <= distx && pos.x < x;
		case DIR_RIGHT:
			return disty <= distx && pos.x > x;
		case DIR_LEFTUP:
			return pos.x <= x && pos.y <= y;
		case DIR_LEFTDOWN:
			return pos.x <= x && pos.y >= y;
		case DIR_RIGHTUP:
			return pos.x >= x && pos.y <= y;
		case DIR_RIGHTDOWN:
			return pos.x >= x && pos.y >= y;
		}

		return false;
	}
}
