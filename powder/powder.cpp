// Falling sand? Awesome.

#include "../terminal/terminal_codepage.h"
#include "../terminal/terminal_extras.h"

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

// Random scalar.

#define rfs (rand() / (double)RAND_MAX)

// Random unit scalar.

#define rus (rand() / (double)RAND_MAX * 2.0 - 1.0)

// Random velocity.

#define rvx ((rand() / (double)RAND_MAX * 2.0 - 1.0) * 5.0)
#define rvy ((rand() / (double)RAND_MAX * 2.0 - 1.0) * 5.0)

#include "include/element.hpp"
#include "include/particle.hpp"
#include "include/simulation.hpp"

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

	unsigned int c_size = 8;

	// Selected element.

	el_t c_element = el_ston;

	// Initializer.

	void steam() override
	{
		// Seed the random number generator (although the shittyness of this random number 
		// generator is such that it may not be considered a random number generator at all).

		srand((unsigned int)time(NULL));

		width = 640;
		height = 480;

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
	}

	// Keydown handler.

	void keydown(SDL_Event e) override
	{
		if (e.key.keysym.sym == SDLK_SPACE)
		{
			c_element = (el_t)((c_element + 1) % el_last);

			if (c_element == el_null)
			{
				c_element = (el_t)((int)c_element + 1);
			}
		}
	}

	// Mousewheel handler.

	void wheelup(SDL_Event e) override
	{
		if (c_size)
		{
			c_size--;
		}
	}

	void wheeldown(SDL_Event e) override
	{
		c_size++;
	}

	#include "helper/raster.hpp"

	// Frame renderer.

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		#include "helper/mouse.hpp"

		// Update all particles in heap.

		int iteration_n = 0;

		iterate:

		iteration_n++;

		for (unsigned int i = 0; i < pc; i++)
		{
			p_t* p = pmap[i];

			float ox = p->x;
			float oy = p->y;

			switch (p->t)
			{
				case el_wvpr:
				{
					p->vx = rus * 5.0;
					p->vy = rus * 5.0;

					break;
				}

				default:
				{
					// Apply gravity.

					p->vy += 0.1;

					break;
				}
			}

			// Apply drag.

			if (p->vx > 24.0)
			{
				p->vx = 24.0;
			}

			if (p->vx < -24.0)
			{
				p->vx = -24.0;
			}

			if (p->vy > 24.0)
			{
				p->vy = 24.0;
			}

			if (p->vy < -24.0)
			{
				p->vy = -24.0;
			}

			p->vx *= 0.995;
			p->vy *= 0.995;

			// Move particle using modified version of Bresenham's line drawing algorithm. This is
			// pretty intuitive, and it works super well.

			#include "helper/collision.hpp"

			done_collisions:

			p->x = x;
			p->y = y;

			if (collision)
			{
				// Lose momentum upon collision.

				p->vx *= rfs * 0.5;
				p->vy *= rfs * 0.5;
			}

			/*

			Affect this particle (and maybe it's neighbors) based on it's material.

			To developer(s), this switch statement is the part of the code that makes the powder
			behave properly; in other words, it is the soul of the engine.

			I'm going to write 'bagels' here, so whenever I want to find this code, I just search
			for 'bagels' using CTRL+F.

			*/

			int inx = (int)(p->x);
			int iny = (int)(p->y);

			switch (p->t)
			{
				// Solids.

				case el_wice:
				case el_ston:
				{
					// Don't move.

					break;
				}

				// Powders.

				case el_sand:
				{
					// If both the bottom-left and bottom-right neighbors exist, stay where you 
					// are. If only the bottom-left neighbor exists, move to the bottom-right
					// neighbor's position. Vice-versa.

					if (iny < hmo - 1 && lmap[(iny + 1) * width + inx] != NULL)
					{
						bool bln = lmap[(iny + 1) * width + (inx - 1)] == NULL;
						bool brn = lmap[(iny + 1) * width + (inx + 1)] == NULL;

						if (bln && !brn)
						{
							p->y++;
							p->x--;
						}
						else if (!bln && brn)
						{
							p->y++;
							p->x++;
						}
						else if (bln && brn)
						{
							if (rand() % 2 == 0)
							{
								p->y++;
								p->x--;
							}
							else
							{
								p->y++;
								p->x++;
							}
						}
					}
					else if (iny < hmo - 1 && lmap[(iny + 1) * width + inx] == NULL)
					{
						p->y++;
					}

					break;
				}

				// Liquids.

				case el_watr:
				{
					if (p->t == el_watr)
					{
						/*

						if (iny > 0)
						{
							p_t* tmn = lmap[(iny - 1) * width + inx];

							if (tmn != NULL)
							{
								if (tmn->t == el_sand)
								{
									// Sand sinks, so we swap the positions of water and sand if a 
									// sand particle is above a water particle.
									
									tmn->t = el_watr;

									p->t = el_sand;
								}
							}
						}

						*/
					}

					// Move either right or left depending on obstructions; if no obstructions
					// move randomly.

					if (iny < hmo - 1 && lmap[(iny + 1) * width + inx] != NULL)
					{
						bool mln = lmap[iny * width + (inx - 1)] == NULL;
						bool mrn = lmap[iny * width + (inx + 1)] == NULL;

						if (mln && !mrn)
						{
							p->x--;
						}
						else if (!mln && mrn)
						{
							p->x++;
						}
						else if (mln && mrn)
						{
							if (rand() % 2 == 0)
							{
								p->x--;
							}
							else
							{
								p->x++;
							}
						}
					}
					else if (iny < hmo - 1 && lmap[(iny + 1) * width + inx] == NULL)
					{
						p->y++;
					}
				}

				// Something else?

				default:
				{
					break;
				}
			}

			// Constrain particles to boundaries using the rules defined by the current edge_mode.

			#include "helper/constrain.hpp"

			// Set the old position in the lookup map to NULL, and the new position in the lookup
			// map to the pointer to this particle.

			// We could optimize by not doing anything if the particle remains stationary, but we
			// won't do that just yet.

			lmap[(int)oy * width + (int)ox] = NULL;

			lmap[(int)(p->y) * width + (int)(p->x)] = p;
		}

		if (iteration_n < 2)
		{
			goto iterate;
		}

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