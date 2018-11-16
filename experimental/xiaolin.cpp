// Xiaolin Wu's algorithms? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct game: boiler
{	
	// This needs to be optimized a LOT.

	inline void plot_aa(int x, int y, unsigned int c, unsigned char a)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			float w1 = (255 - a) / 255.0f;

			float w2 = 1.0f - w1;

			float fa = w1;

			float r = mgetr(c) * fa;
			float g = mgetg(c) * fa;
			float b = mgetb(c) * fa;

			unsigned int bc = pixels[y * width + x];

			float br = mgetr(bc) * w2;
			float bg = mgetg(bc) * w2;
			float bb = mgetb(bc) * w2;

			pixels[y * width + x] = mrgb(r + br, g + bg, b + bb);
		}
	}

	void xiaolin_line(int x0, int y0, int x1, int y1, unsigned int c)
	{
		int dx = abs(x1 - x0);
		int dy = abs(y1 - y0);

		int sx = x0 < x1 ? 1 : -1;
		int sy = y0 < y1 ? 1 : -1; 

		int err = dx - dy;

		int e2; 
		int x2;

		int ed = dx + dy == 0 ? 1 : sqrt((float)dx * dx + (float)dy * dy);

		for (;;)
		{
			plot_aa(x0, y0, c, 255 * abs(err - dx + dy) / ed);

			e2 = err; 

			x2 = x0;

			if (2 * e2 >= -dx) 
			{
				if (x0 == x1) 
				{
					break;
				}

				if (e2 + dy < ed) 
				{
					plot_aa(x0, y0 + sy, c, 255 * (e2 + dy) / ed);
				}

				err -= dy; 

				x0 += sx; 
			} 

			if (2 * e2 <= dy) 
			{
				if (y0 == y1)
				{
					break;
				}

				if (dx - e2 < ed)
				{
					plot_aa(x2 + sx, y0, c, 255 * (dx - e2) / ed);
				}

				err += dx; 

				y0 += sy; 
			}
		}
	}

	void xiaolin_thick_line(int x0, int y0, int x1, int y1, float wd, unsigned int c)
	{ 
		int dx = abs(x1 - x0);

		int sx = x0 < x1 ? 1 : -1;

		int dy = abs(y1 - y0);

		int sy = y0 < y1 ? 1 : -1;

		int err = dx - dy;

		int e2;

		int x2;
		int y2;

		float ed = dx + dy == 0 ? 1 : sqrt((float)dx * dx + (float)dy * dy);

		for (wd = (wd + 1) / 2;;) 
		{
			plot_aa(x0, y0, c, fmax(0, 255 * (abs(err -dx + dy) / ed - wd + 1)));

			e2 = err; 

			x2 = x0;
			
			if (2 * e2 >= -dx) 
			{
				for (e2 += dy, y2 = y0; e2 < ed * wd && (y1 != y2 || dx > dy); e2 += dx)
				
				plot_aa(x0, y2 += sy, c, fmax(0, 255 * (abs(e2) / ed - wd + 1)));
				
				if (x0 == x1)
				{
					break;
				}
				
				e2 = err; 

				err -= dy; 

				x0 += sx; 
			} 

			if (2 * e2 <= dy) 
			{
				for (e2 = dx - e2; e2 < ed * wd && (x1 != x2 || dx < dy); e2 += dy)
				
				plot_aa(x2 += sx, y0, c, fmax(0, 255 * (abs(e2) / ed - wd + 1)));
				
				if (y0 == y1)
				{
					break;
				}
				
				err += dx; 

				y0 += sy; 
			}
		}
	}

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Xiaolin Wu's algorithms (using Boiler)";
	}

	void draw() override
	{
		if (iteration % 60 == 0)
		{
			black();
		}

		xiaolin_line(mrandx, mrandy, mrandx, mrandy, rgb(255, 255, 255));
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	game demo;

	if (demo.make() != 0)
	{
		std::cout << "Could not initialize Boiler." << std::endl;

		return 1;
	}

	demo.engine();
	demo.sweep();

	return 0;
}