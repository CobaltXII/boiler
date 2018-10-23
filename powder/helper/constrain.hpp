#pragma once

if (edge_mode == em_void)
{
	if (p->x < 0 || p->x > wmo || p->y < 0 || p->y > hmo)
	{
		unsigned int _idx = lmap[(int)(oy) * width + (int)(ox)]->idx;

		lmap[(int)(oy) * width + (int)(ox)] = NULL;

		pmap[_idx] = pmap[pc - 1];

		pmap[_idx]->idx = _idx;

		pc--;

		// Always continue after removal.

		continue;
	}
}
else if (edge_mode == em_loop)
{
	// This is a bit broken.

	if (p->x < 0)
	{
		p->x = wmo;
	}
	else if (p->x > wmo)
	{
		p->x = 0;
	}

	if (p->y < 0)
	{
		p->y = hmo;
	}
	else if (p->y > hmo)
	{
		p->y = 0;
	}
}
else
{
	// This is a bit broken.

	if (p->x < 0)
	{
		p->x = 0;

		p->vx = -(p->vx) * rfs * 0.5;
	}
	else if (p->x > wmo)
	{
		p->x = wmo;

		p->vx = -(p->vx) * rfs * 0.5;
	}

	if (p->y < 0)
	{
		p->y = 0;

		p->vy = -(p->vy) * rfs * 0.5;
	}
	else if (p->y > hmo)
	{
		p->y = hmo;

		p->vy = -(p->vy) * rfs * 0.5;
	}
}