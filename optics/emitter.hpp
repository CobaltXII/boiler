#pragma once

// An emitter.

const real emitter_length = 48.0f;

const real emitter_width = 16.0f;

const real emitter_normal = 16.0f;

struct emitter
{
	// The position of the emitter.

	point p;

	// The normal of the emitter.

	point n;

	// The normal anchor of the emitter.

	point a;

	// The state of the emitter.

	bool state;

	// The color of the emitter.

	unsigned char cr;
	unsigned char cg;
	unsigned char cb;

	emitter
	(
		point _p, 

		point _n, 

		unsigned char _cr, 
		unsigned char _cg, 
		unsigned char _cb, 

		bool _state = true
	)
	{
		p = _p;

		n = _n;

		cr = _cr;
		cg = _cg;
		cb = _cb;

		real bmx = p.x - n.x * emitter_length / 2.0f;
		real bmy = p.y - n.y * emitter_length / 2.0f;

		a = point(bmx - n.x * emitter_normal, bmy - n.y * emitter_normal);

		state = _state;
	}
};