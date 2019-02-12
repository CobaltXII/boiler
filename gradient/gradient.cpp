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