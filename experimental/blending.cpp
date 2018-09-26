// Additive blending?

#include "../boiler/boiler.h"

struct game: boiler
{	
	// Render scanline while applying additive blending.

	void ab_scanlinergb(int x1, int x2, int y1, unsigned int c)
	{
		for (int i = x1; i <= x2; i++)
		{
			if (i >= 0 && i < width && y1 >= 0 && y1 < height)
			{
				Uint32 oc = pixels[y1 * width + i];

				Uint8 _or = oc >> 16;
				Uint8 _og = oc >> 8;
				Uint8 _ob = oc >> 0;

				Uint8 _nr = c >> 16;
				Uint8 _ng = c >> 8;
				Uint8 _nb = c >> 0;

				pixels[y1 * width + i] = rgb
				(
					clamprgb(_or + _nr),
					clamprgb(_og + _ng),
					clamprgb(_ob + _nb)
				);
			}		
		}
	}

	// Render circle with additive blending.

	void ab_fcirclergb(int xc, int yc, int r, unsigned int c)
	{
		int x = 0;
		int y = 0;

		int p = 3 - (2 * r);

		if (!r)
		{
			return;
		}

		y = r;

		while (y >= x)
		{
			ab_scanlinergb(xc - x, xc + x, yc - y, c);
			ab_scanlinergb(xc - y, xc + y, yc - x, c);
			ab_scanlinergb(xc - x, xc + x, yc + y, c);
			ab_scanlinergb(xc - y, xc + y, yc + x, c);

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

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Additive blending (using Boiler)";
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		for (int n = 0; n < 3; n++)
		{
			ab_fcirclergb
			(
				(((sin(iteration / 20.0) * 76.0) + 20.0) * cos(2.0 * M_PI * n / 3.0 + iteration / 100.0)) + width / 2,
				(((sin(iteration / 20.0) * 76.0) + 20.0) * sin(2.0 * M_PI * n / 3.0 + iteration / 100.0)) + height / 2,

				std::abs(sin(iteration / 30.0) * 118.0),

				n == 2 ? rgb(255, 0, 0) :
				n == 1 ? rgb(0, 255, 0) :
						 rgb(0, 0, 255)
			);
		}
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