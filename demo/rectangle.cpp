// Mental rectangles using Boiler.

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
			int x1 = randx();
			int y1 = randy();

			int w = rand() % (width * 2) - width;
			int h = rand() % (height * 2) - height;

			Uint32 c = rgb
			(
				rand() % 256,
				rand() % 256,
				rand() % 256
			);

			if (rand() % 2 == 0)
			{
				frectrgb(x1, y1, w, h, c);
			}
			else
			{
				rectrgb(x1, y1, w, h, c);
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