#pragma once

// Handle mouse presses.

if (ml_pressed)
{
	// Fill cursor with selected material.

	int xc = mouse_x;
	int yc = mouse_y;

	int r = c_size;

	int x = 0;
	int y = 0;

	int p = 3 - (2 * r);

	if (r)
	{
		y = r;

		while (y >= x)
		{
			pscanline(xc - x, xc + x, yc - y, c_element);
			pscanline(xc - y, xc + y, yc - x, c_element);
			pscanline(xc - x, xc + x, yc + y, c_element);
			pscanline(xc - y, xc + y, yc + x, c_element);

			if (p < 0)
			{
				p += 4 * x++ + 6;
			}
			else
			{
				p += 4 * (x++ - y--) + 10;
			}
		}
	}
}
else if (ml_held)
{
	#define x1 mouse_ox
	#define y1 mouse_oy

	#define x2 mouse_x
	#define y2 mouse_y

	int x;
	int y;

	int dx = x2 - x1;
	int dy = y2 - y1;

	int dx1 = std::abs(dx);
	int dy1 = std::abs(dy);

	int px = 2 * dy1 - dx1;
	int py = 2 * dx1 - dy1;

	int xe;
	int ye;

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
			{
				int xc = x;
				int yc = y;

				int r = c_size;

				int x = 0;
				int y = 0;

				int p = 3 - (2 * r);

				if (r)
				{
					y = r;

					while (y >= x)
					{
						pscanline(xc - x, xc + x, yc - y, c_element);
						pscanline(xc - y, xc + y, yc - x, c_element);
						pscanline(xc - x, xc + x, yc + y, c_element);
						pscanline(xc - y, xc + y, yc + x, c_element);

						if (p < 0)
						{
							p += 4 * x++ + 6;
						}
						else
						{
							p += 4 * (x++ - y--) + 10;
						}
					}
				}
			}
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
				{
					int xc = x;
					int yc = y;

					int r = c_size;

					int x = 0;
					int y = 0;

					int p = 3 - (2 * r);

					if (r)
					{
						y = r;

						while (y >= x)
						{
							pscanline(xc - x, xc + x, yc - y, c_element);
							pscanline(xc - y, xc + y, yc - x, c_element);
							pscanline(xc - x, xc + x, yc + y, c_element);
							pscanline(xc - y, xc + y, yc + x, c_element);

							if (p < 0)
							{
								p += 4 * x++ + 6;
							}
							else
							{
								p += 4 * (x++ - y--) + 10;
							}
						}
					}
				}
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
			{
				int xc = x;
				int yc = y;

				int r = c_size;

				int x = 0;
				int y = 0;

				int p = 3 - (2 * r);

				if (r)
				{
					y = r;

					while (y >= x)
					{
						pscanline(xc - x, xc + x, yc - y, c_element);
						pscanline(xc - y, xc + y, yc - x, c_element);
						pscanline(xc - x, xc + x, yc + y, c_element);
						pscanline(xc - y, xc + y, yc + x, c_element);

						if (p < 0)
						{
							p += 4 * x++ + 6;
						}
						else
						{
							p += 4 * (x++ - y--) + 10;
						}
					}
				}
			}
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
				{
					int xc = x;
					int yc = y;

					int r = c_size;

					int x = 0;
					int y = 0;

					int p = 3 - (2 * r);

					if (r)
					{
						y = r;

						while (y >= x)
						{
							pscanline(xc - x, xc + x, yc - y, c_element);
							pscanline(xc - y, xc + y, yc - x, c_element);
							pscanline(xc - x, xc + x, yc + y, c_element);
							pscanline(xc - y, xc + y, yc + x, c_element);

							if (p < 0)
							{
								p += 4 * x++ + 6;
							}
							else
							{
								p += 4 * (x++ - y--) + 10;
							}
						}
					}
				}
			}
		}
	}

	#undef x1
	#undef y1

	#undef x2
	#undef y2
}

if (mr_pressed)
{
	// Remove all particles within cursor.

	int xc = mouse_x;
	int yc = mouse_y;

	int r = c_size;

	int x = 0;
	int y = 0;

	int p = 3 - (2 * r);

	if (r)
	{
		y = r;

		while (y >= x)
		{
			prscanline(xc - x, xc + x, yc - y);
			prscanline(xc - y, xc + y, yc - x);
			prscanline(xc - x, xc + x, yc + y);
			prscanline(xc - y, xc + y, yc + x);

			if (p < 0)
			{
				p += 4 * x++ + 6;
			}
			else
			{
				p += 4 * (x++ - y--) + 10;
			}
		}
	}
}
else if (mr_held)
{
	#define x1 mouse_ox
	#define y1 mouse_oy

	#define x2 mouse_x
	#define y2 mouse_y

	int x;
	int y;

	int dx = x2 - x1;
	int dy = y2 - y1;

	int dx1 = std::abs(dx);
	int dy1 = std::abs(dy);

	int px = 2 * dy1 - dx1;
	int py = 2 * dx1 - dy1;

	int xe;
	int ye;

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
			{
				int xc = x;
				int yc = y;

				int r = c_size;

				int x = 0;
				int y = 0;

				int p = 3 - (2 * r);

				if (r)
				{
					y = r;

					while (y >= x)
					{
						prscanline(xc - x, xc + x, yc - y);
						prscanline(xc - y, xc + y, yc - x);
						prscanline(xc - x, xc + x, yc + y);
						prscanline(xc - y, xc + y, yc + x);

						if (p < 0)
						{
							p += 4 * x++ + 6;
						}
						else
						{
							p += 4 * (x++ - y--) + 10;
						}
					}
				}
			}
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
				{
					int xc = x;
					int yc = y;

					int r = c_size;

					int x = 0;
					int y = 0;

					int p = 3 - (2 * r);

					if (r)
					{
						y = r;

						while (y >= x)
						{
							prscanline(xc - x, xc + x, yc - y);
							prscanline(xc - y, xc + y, yc - x);
							prscanline(xc - x, xc + x, yc + y);
							prscanline(xc - y, xc + y, yc + x);

							if (p < 0)
							{
								p += 4 * x++ + 6;
							}
							else
							{
								p += 4 * (x++ - y--) + 10;
							}
						}
					}
				}
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
			{
				int xc = x;
				int yc = y;

				int r = c_size;

				int x = 0;
				int y = 0;

				int p = 3 - (2 * r);

				if (r)
				{
					y = r;

					while (y >= x)
					{
						prscanline(xc - x, xc + x, yc - y);
						prscanline(xc - y, xc + y, yc - x);
						prscanline(xc - x, xc + x, yc + y);
						prscanline(xc - y, xc + y, yc + x);

						if (p < 0)
						{
							p += 4 * x++ + 6;
						}
						else
						{
							p += 4 * (x++ - y--) + 10;
						}
					}
				}
			}
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
				{
					int xc = x;
					int yc = y;

					int r = c_size;

					int x = 0;
					int y = 0;

					int p = 3 - (2 * r);

					if (r)
					{
						y = r;

						while (y >= x)
						{
							prscanline(xc - x, xc + x, yc - y);
							prscanline(xc - y, xc + y, yc - x);
							prscanline(xc - x, xc + x, yc + y);
							prscanline(xc - y, xc + y, yc + x);

							if (p < 0)
							{
								p += 4 * x++ + 6;
							}
							else
							{
								p += 4 * (x++ - y--) + 10;
							}
						}
					}
				}
			}
		}
	}

	#undef x1
	#undef y1

	#undef x2
	#undef y2
}