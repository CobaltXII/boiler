// Emptyness? Awesome.

#include "../boiler/boiler.h"

struct game: boiler
{	
	void steam() override
	{
		width = 800;
		height = 600;

		title = "Emptyness (using Boiler)";
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));
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