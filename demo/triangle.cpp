// Mental triangles using Boiler.

#include "../boiler/boiler.h"

struct game: boiler
{	
	void steam() override
	{
		width = 800;
		height = 600;

		title = "Software renderer (using Boiler)";
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		for (int i = 0; i < 10; i++)
		{
			int x1 = rand() % width;
			int y1 = rand() % height;
			int x2 = rand() % width;
			int y2 = rand() % height;
			int x3 = rand() % width;
			int y3 = rand() % height;

			ftrianglergb
			(
				x1, y1,
				x2, y2,
				x3, y3,

				rgb(rand() % 256, rand() % 256, rand() % 256)
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