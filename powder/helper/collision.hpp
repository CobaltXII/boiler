#pragma once

int vx = p->vx;
int vy = p->vy;

int c = rgb(50, 50, 50);

float x1 = ox;
float y1 = oy;

float x2 = ox + vx;
float y2 = oy + vy;

float x;
float y;

float dx = x2 - x1;
float dy = y2 - y1;

float dx1 = std::abs(dx);
float dy1 = std::abs(dy);

float px = 2.0 * dy1 - dx1;
float py = 2.0 * dx1 - dy1;

float xe;
float ye;

float lx = x1;
float ly = y1;

bool collision = false;

switch (p->t)
{
	case el_wice:
	case el_ston:
	{
		x = x1;
		y = y1;

		goto done_collisions;
	}

	default:
	{
		break;
	}
}

if (dy1 <= dx1)
{
	if (dx >= 0)
	{
		x = x1;
		y = y1;

		xe = x2;
	}
	else
	{
		x = x2;
		y = y2;

		xe = x1;
	}

	if (x >= 0 && x < width && y >= 0 && y < height)
	{
		p_t* a_ptr = lmap[(int)y * width + (int)x];

		if (a_ptr != NULL && a_ptr != p)
		{
			// It's okay, in this scenario. 'goto' isn't always evil, guys.

			x = lx;
			y = ly;

			collision = true;

			goto done_collisions;
		}

		lx = x;
		ly = y;
	}

	for (int i = 0; x < xe; i++)
	{
		x = x + 1;

		if (px < 0)
		{
			px = px + 2 * dy1;
		}
		else
		{
			if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
			{
				y = y + 1;
			}
			else
			{
				y = y - 1;
			}

			px = px + 2 * (dy1 - dx1);
		}

		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			p_t* a_ptr = lmap[(int)y * width + (int)x];

			if (a_ptr != NULL && a_ptr != p)
			{	
				x = lx;
				y = ly;

				collision = true;

				goto done_collisions;
			}

			lx = x;
			ly = y;
		}
	}
}
else
{
	if (dy >= 0)
	{
		x = x1;
		y = y1;

		ye = y2;
	}
	else
	{
		x = x2;
		y = y2;

		ye = y1;
	}

	if (x >= 0 && x < width && y >= 0 && y < height)
	{
		p_t* a_ptr = lmap[(int)y * width + (int)x];

		if (a_ptr != NULL && a_ptr != p)
		{	
			x = lx;
			y = ly;

			collision = true;

			goto done_collisions;
		}

		lx = x;
		ly = y;
	}

	for (int i = 0; y < ye; i++)
	{
		y = y + 1;

		if (py <= 0)
		{
			py = py + 2 * dx1;
		}
		else
		{
			if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
			{
				x = x + 1;
			}
			else
			{
				x = x - 1;
			}

			py = py + 2 * (dx1 - dy1);
		}

		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			p_t* a_ptr = lmap[(int)y * width + (int)x];

			if (a_ptr != NULL && a_ptr != p)
			{	
				x = lx;
				y = ly;

				collision = true;

				goto done_collisions;
			}

			lx = x;
			ly = y;
		}
	}
}