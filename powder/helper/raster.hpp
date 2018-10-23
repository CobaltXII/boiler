#pragma once

// Fill a scanline with particles. Does not place particle if a particle already exists at the
// position.

void pscanline(int x1, int x2, int y1, el_t c)
{
	for (int i = x1; i <= x2; i++)
	{
		if (i >= 0 && i < width && y1 >= 0 && y1 < height)
		{
			if (lmap[y1 * width + i] == NULL)
			{
				pmap[pc] = new p_t(c, i, y1, 0.0, 0.0, pc);

				lmap[(int)(pmap[pc]->y) * width + (int)(pmap[pc]->x)] = pmap[pc];

				pc++;
			}
		}		
	}
}

// Remove all particles in the scanline.

void prscanline(int x1, int x2, int y1)
{
	for (int i = x1; i <= x2; i++)
	{
		if (i >= 0 && i < width && y1 >= 0 && y1 < height)
		{
			if (lmap[y1 * width + i] != NULL)
			{
				unsigned int _idx = lmap[y1 * width + i]->idx;

				lmap[y1 * width + i] = NULL;

				pmap[_idx] = pmap[pc - 1];

				pmap[_idx]->idx = _idx;

				pc--;
			}
		}		
	}
}