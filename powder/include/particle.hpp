#pragma once

// Particle structure.

struct p_t
{
	el_t t;

	float x;
	float y;

	float vx;
	float vy;

	unsigned int idx;

	p_t(el_t _t, int _x, int _y, unsigned int _idx)
	{
		t = _t;

		x = _x;
		y = _y;

		idx = _idx;
	}

	p_t(el_t _t, int _x, int _y, int _vx, int _vy, unsigned int _idx)
	{
		t = _t;

		x = _x;
		y = _y;

		vx = _vx;
		vy = _vy;

		idx = _idx;
	}
};