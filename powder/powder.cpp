// Falling sand? Awesome.

#include "../terminal/terminal_codepage.h"
#include "../terminal/terminal_extras.h"

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>


struct game: boiler
{	
	// Lookup array.

	p_t** lmap;

	// Particle heap.

	p_t** pmap;

	// Particle heap length.

	unsigned int pc = 0;

	// Mutated dimensions.

	unsigned int wmo;
	unsigned int hmo;

	unsigned int hwd;
	unsigned int hhd;

	// Cursor size.

	unsigned int c_size = 16;

	// Selected element.

	el_t c_element = el_ston;

	// Initializer.

	void steam() override
	{
		// Seed the random number generator (although the shittyness of this random number 
		// generator is such that it may not be considered a random number generator at all).

		srand((unsigned int)time(NULL));

		width = 800;
		height = 600;

		// Initialize mutated dimensions.

		wmo = width - 1;
		hmo = height - 1;

		hwd = width / 2;
		hhd = height / 2;

		title = "Falling sand (using Boiler)";

		// Allocate lookup array.

		lmap = (p_t**)malloc(width * height * sizeof(p_t*));

		// Allocate particle heap.

		pmap = (p_t**)malloc(width * height * sizeof(p_t*));

		if (lmap == NULL)
		{
			free(pmap);

			nuke("Could not allocate lookup array.");
		}

		if (pmap == NULL)
		{
			free(lmap);

			nuke("Could not allocate particle heap.");
		}

		memset(lmap, 0, width * height * sizeof(p_t*));
		memset(pmap, 0, width * height * sizeof(p_t*));

		for (int i = 0; i < 0; i++)
		{
			// If you follow this code, this is the standard particle addition template. First, it
			// sets the last element of the particle heap to a new particle. Second, it sets the
			// lookup table entry at the coordinates of the new particle to the address of the new
			// particle. Finally, it increments the particle counter.

			int nx = mrandx;
			int ny = mrandy;

			if (lmap[ny * width + nx] == NULL)
			{
				pmap[pc] = new p_t(el_watr, nx, ny, rvx, rvy, pc);

				lmap[(int)(pmap[pc]->y) * width + (int)(pmap[pc]->x)] = pmap[pc];

				pc++;
			}
		}
	}

	// Frame renderer.

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Render all particles in heap.

		for (unsigned int i = 0; i < pc; i++)
		{
			p_t* p = pmap[i];

			pixels[(int)(p->y) * width + (int)(p->x)] = cmap[p->t];
		}

		// Render cursor.

		circlergb(mouse_x, mouse_y, c_size, rgb(255, 255, 255));

		// Debug information.

		fontrgb(edmap[c_element], 0, 0, rgb(127, 127, 127), __fnb);

		// Log particle count.

		if (iteration % 60 == 0)
		{
			std::cout << "Processing " << pc << " particles." << std::endl;
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

	// Free lookup array and particle heap.

	free((void*)demo.lmap);
	free((void*)demo.pmap);

	return 0;
}