/*

Seperating axis theorem, thanks to javidx9

	https://www.youtube.com/watch?v=7Ik2vowGcU0

*/

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct point
{
	float x;
	float y;

	point
	(
		float x,
		float y
	)
	{
		this->x = x;
		this->y = y;
	}

	point()
	{
		x = 0.0f;
		y = 0.0f;
	}
};
