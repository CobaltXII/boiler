#pragma once

// A line.

struct line
{
	point p1;
	point p2;

	line(point _p1, point _p2)
	{
		p1 = _p1;
		p2 = _p2;
	}
};
