/*

Gradient

Press E to export a gradient. You can then construct a gradient object using
the exported code. The function for this is gradient_from_string.

A good idea is to use coolors.co to generate a cool palette. Then copy the hex
part of the URL, like "b9d6f2-061a40-0353a4-006daa-003559". Then run this
application like this:

	./gradient.out -H "b9d6f2-061a40-0353a4-006daa-003559"

*/

#include "../boiler/boiler.h"

#include "gradient.h"

#include <vector>
#include <utility>
#include <iostream>

struct game: boiler
{	
	gradient my_gradient;

	gradient_stop* my_stop = nullptr;

	gradient_stop* my_old_stop = nullptr;

	int my_color = 0;

	void steam() override
	{
		width = 576;
		
		height = 384;

		title = "Gradient (using Boiler)";

		f_No_Debug = SDL_TRUE;
	}

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_ESCAPE)
		{
			running = SDL_FALSE;
		}
		else if (key == SDLK_e)
		{
			std::cout << my_gradient.encode() << std::endl;
		}
	}

	void draw() override
	{
		black();

		// Render the gradient.

		for (int i = 0; i <= 512; i++)
		{
			int x = i + 32;

			rgb_tuple color = my_gradient.sample(i / 512.0f);

			unsigned char r = color.r * 255;
			unsigned char g = color.g * 255;
			unsigned char b = color.b * 255;

			unsigned int color_int = rgb(r, g, b);

			for (int j = 0; j < 64; j++)
			{
				int y = j + 32;

				plotp(x, y, color_int);
			}
		}

		// Render the gradient stops.

		for (int i = 0; i < my_gradient.stops.size(); i++)
		{
			unsigned char r = my_gradient.stops[i].color.r * 255;
			unsigned char g = my_gradient.stops[i].color.g * 255;
			unsigned char b = my_gradient.stops[i].color.b * 255;

			int x = my_gradient.stops[i].position * 512 + 32;

			int y = 112;

			linergb(x, 96, x, y, rgb(r, g, b));

			fcirclergb(x, y, 3, rgb(r, g, b));

			circlergb(x, y, 4, 0xFFFFFF);

			if (!my_stop && mouse_l)
			{
				int dx = mouse_x - x;
				int dy = mouse_y - y;

				if (dx * dx + dy * dy < 4 * 4)
				{
					my_stop = &(my_gradient.stops[i]);
				}
			}
		}

		// Render the color of the last selected gradient stop (if any).

		if (my_old_stop)
		{	
			for (int i = 0; i <= 512; i++)
			{
				int x = i + 32;

				rgb_tuple color =
				{
					i / 512.0f,
					
					my_old_stop->color.g,
					my_old_stop->color.b
				};

				unsigned char r = color.r * 255;
				unsigned char g = color.g * 255;
				unsigned char b = color.b * 255;

				unsigned int color_int = rgb(r, g, b);

				for (int j = 0; j < 48; j++)
				{
					int y = j + 128;

					plotp(x, y, color_int);
				}
			}

			for (int i = 0; i <= 512; i++)
			{
				int x = i + 32;

				rgb_tuple color =
				{
					my_old_stop->color.r,

					i / 512.0f,

					my_old_stop->color.b
				};

				unsigned char r = color.r * 255;
				unsigned char g = color.g * 255;
				unsigned char b = color.b * 255;

				unsigned int color_int = rgb(r, g, b);

				for (int j = 0; j < 48; j++)
				{
					int y = j + 208;

					plotp(x, y, color_int);
				}
			}

			for (int i = 0; i <= 512; i++)
			{
				int x = i + 32;

				rgb_tuple color =
				{	
					my_old_stop->color.r,
					my_old_stop->color.g,

					i / 512.0f
				};

				unsigned char r = color.r * 255;
				unsigned char g = color.g * 255;
				unsigned char b = color.b * 255;

				unsigned int color_int = rgb(r, g, b);

				for (int j = 0; j < 48; j++)
				{
					int y = j + 288;

					plotp(x, y, color_int);
				}
			}
		}

		// Render the handles of each color component of the last selected
		// gradient stop (if any).

		if (my_old_stop)
		{
			rgb_tuple color =
			{
				my_old_stop->color.r,
				my_old_stop->color.g,
				my_old_stop->color.b
			};

			unsigned char r = color.r * 255;
			unsigned char g = color.g * 255;
			unsigned char b = color.b * 255;

			unsigned int color_int = rgb(r, g, b);

			int xr = my_old_stop->color.r * 512 + 32;
			int xg = my_old_stop->color.g * 512 + 32;
			int xb = my_old_stop->color.b * 512 + 32;

			linergb(xr, 176, xr, 192, color_int);
			linergb(xg, 256, xg, 272, color_int);
			linergb(xb, 336, xb, 352, color_int);

			fcirclergb(xr, 192, 3, color_int);
			fcirclergb(xg, 272, 3, color_int);
			fcirclergb(xb, 352, 3, color_int);

			circlergb(xr, 192, 4, 0xFFFFFF);
			circlergb(xg, 272, 4, 0xFFFFFF);
			circlergb(xb, 352, 4, 0xFFFFFF);

			// Selection.

			int dx_r = mouse_x - xr;
			int dx_g = mouse_x - xg;
			int dx_b = mouse_x - xb;

			int dy_r = mouse_y - 192;
			int dy_g = mouse_y - 272;
			int dy_b = mouse_y - 352;

			if (!my_color)
			{
				if (dx_r * dx_r + dy_r * dy_r < 4 * 4)
				{
					my_color = 1;
				}

				if (dx_g * dx_g + dy_g * dy_g < 4 * 4)
				{
					my_color = 2;
				}

				if (dx_b * dx_b + dy_b * dy_b < 4 * 4)
				{
					my_color = 3;
				}
			}

			if (mouse_l && my_color)
			{
				if (my_color == 1)
				{
					my_old_stop->color.r = std::max(0.0f, std::min(1.0f, (mouse_x - 32.0f) / 512.0f));
				}
				else if (my_color == 2)
				{
					my_old_stop->color.g = std::max(0.0f, std::min(1.0f, (mouse_x - 32.0f) / 512.0f));
				}
				else if (my_color == 3)
				{
					my_old_stop->color.b = std::max(0.0f, std::min(1.0f, (mouse_x - 32.0f) / 512.0f));
				}
			}
			else if (!mouse_l)
			{
				my_color = 0;
			}
		}

		// Move the selected gradient stop.

		if (my_stop)
		{
			my_old_stop = my_stop;
		}

		if (my_stop && mouse_l)
		{
			my_stop->position = std::max(0.0f, std::min(1.0f, (mouse_x - 32.0f) / 512.0f));
		}
		else if (!mouse_l)
		{
			my_stop = nullptr;
		}
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	if (argc != 1 && argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " [-E [encoded-gradient] | -H [hex-codes]]" << std::endl;

		exit(EXIT_FAILURE);
	}

	game demo;

	if (argc == 3)
	{
		if (std::string(argv[1]) == "-E")
		{
			demo.my_gradient = gradient_from_string(std::string(argv[2]));
		}
		else if (std::string(argv[1]) == "-H")
		{
			std::string hexes = std::string(argv[2]);

			for (int i = 0; i < (hexes.size() + 1) / 7; i++)
			{
				std::string hex = hexes.substr(i * 7, 6);

				demo.my_gradient.stops.push_back(gradient_stop(i * (1.0f / float((hexes.size() + 1) / 7 - 1)), rgb_from_hex(hex)));
			}
		}
		else
		{
			std::cout << "Usage: " << argv[0] << " [-E [encoded-gradient] | -H [hex-codes]]" << std::endl;

			exit(EXIT_FAILURE);
		}
	}
	else
	{
		std::string hexes = "083d77-ebebd3-f4d35e-ee964b-f95738";

		for (int i = 0; i < (hexes.size() + 1) / 7; i++)
		{
			std::string hex = hexes.substr(i * 7, 6);

			demo.my_gradient.stops.push_back(gradient_stop(i * (1.0f / float((hexes.size() + 1) / 7 - 1)), rgb_from_hex(hex)));
		}
	}

	if (demo.make() != 0)
	{
		std::cout << "Could not initialize Boiler." << std::endl;

		return 1;
	}

	demo.engine();
	demo.sweep();

	return 0;
}