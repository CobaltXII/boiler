// Terminal font generator, do not use.

#define BOIL_USE_STB_IMAGE

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <fstream>

struct game: boiler
{	
	// Load the image.

	int __437__w;
	int __437__h;

	Uint32* __437__ = loadimg("437.png", __437__w, __437__h);

	void steam() override
	{
		width = 801;
		height = 608;

		title = "Font decoding (using Boiler)";

		// Generate a boolean dump of the font data. We could use xxd and then stb_image,

		std::stringstream __437__o;

		__437__o << "unsigned char __437__[" << __437__w * __437__h << "] =\n{\n";

		for (int y = 0; y < __437__h; y++)
		{
			__437__o << "\t";

			for (int x = 0; x < __437__w - 1; x++)
			{
				if (__437__[(y * __437__w) + x] == rgb(0, 0, 0))
				{
					__437__o << "0, ";
				}
				else
				{
					__437__o << "1, ";
				}
			}

			if (__437__[(y * __437__w) + __437__w - 1] == rgb(0, 0, 0))
			{
				__437__o << "0,\n";
			}
			else
			{
				__437__o << "1,\n";
			}
		}

		__437__o << "};\n";

		// Save to a file.

		std::ofstream __of = std::ofstream("terminal_codepage.h");

		__of << __437__o.str();

		__of.close();

		// Done, don't run demos.

		nuke();
	}

	void draw() override
	{
		clear(rgb(255, 255, 255));

		/*

		// Test glyph integrity.

		for (int x = 0; x < __437__w; x++)
		{
			for (int y = 0; y < __437__h; y++)
			{
				Uint32 __437__p = __437__[(y * __437__w) + x];

				Uint32 __OUT__;

				if (__437__p == rgb(0, 0, 0))
				{
					if ((x % 18 >= 9) ^ (y % 32 >= 16))
					{
						__OUT__ = rgb(127, 127, 127);
					}
					else
					{
						__OUT__ = rgb(96, 96, 96);
					}
				}
				else
				{
					__OUT__ = rgb(0, 0, 0);
				}

				pixels[(y * width) + x] = __OUT__;
			}
		}

		*/

		// Test rendering scheme.

		for (int xg = 0; xg < width / 9; xg++)
		{
			for (int yg = 0; yg < height / 16; yg++)
			{
				Uint8 _glyph = (iteration / 5) % 256;

				for (int x = 0; x < 9; x++)
				{
					for (int y = 0; y < 16; y++)
					{
						Uint32 __437__p = __437__[((_glyph / 32) * 16 + y) * __437__w + (_glyph % 32) * 9 + x];

						Uint32 __OUT__;

						if (__437__p == rgb(0, 0, 0))
						{
							if ((xg % 2 == 0) ^ (yg % 2 == 0))
							{
								__OUT__ = rgb(127, 127, 127);
							}
							else
							{
								__OUT__ = rgb(96, 96, 96);
							}
						}
						else
						{
							__OUT__ = rgb(255, 255, 255);
						}

						pixels[(yg * 16 + y) * width + (xg * 9 + x)] = __OUT__; 
					}
				}
			}
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