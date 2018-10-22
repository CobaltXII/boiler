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

	// Fill a scanline with particles. Does not place particle if a particle already exists at the
	// position.

	void pscanline(int x1, int x2, int y1, el_t c)
	{
		for (int i = x1; i <= x2; i++)
		{
			if (i >= 0 && i < width && y1 >= 0 && y1 < height)
			{
				if (lmap[y1 * width + i] == NULL)
				{
					pmap[pc] = new p_t(c, i, y1, 0.0, 0.0, pc);

					lmap[(int)(pmap[pc]->y) * width + (int)(pmap[pc]->x)] = pmap[pc];

					pc++;
				}
			}		
		}
	}

	// Remove all particles in the scanline.

	void prscanline(int x1, int x2, int y1)
	{
		for (int i = x1; i <= x2; i++)
		{
			if (i >= 0 && i < width && y1 >= 0 && y1 < height)
			{
				if (lmap[y1 * width + i] != NULL)
				{
					unsigned int _idx = lmap[y1 * width + i]->idx;

					lmap[y1 * width + i] = NULL;

					pmap[_idx] = pmap[pc - 1];

					pmap[_idx]->idx = _idx;

					pc--;
				}
			}		
		}
	}

	// Frame renderer.

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Handle mouse presses.

		if (ml_pressed)
		{
			// Fill cursor with selected material.

			int xc = mouse_x;
			int yc = mouse_y;

			int r = c_size;

			int x = 0;
			int y = 0;

			int p = 3 - (2 * r);

			if (r)
			{
				y = r;

				while (y >= x)
				{
					pscanline(xc - x, xc + x, yc - y, c_element);
					pscanline(xc - y, xc + y, yc - x, c_element);
					pscanline(xc - x, xc + x, yc + y, c_element);
					pscanline(xc - y, xc + y, yc + x, c_element);

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
		}
		else if (ml_held)
		{
			#define x1 mouse_ox
			#define y1 mouse_oy

			#define x2 mouse_x
			#define y2 mouse_y

			int x;
			int y;

			int dx = x2 - x1;
			int dy = y2 - y1;

			int dx1 = std::abs(dx);
			int dy1 = std::abs(dy);

			int px = 2 * dy1 - dx1;
			int py = 2 * dx1 - dy1;

			int xe;
			int ye;

			if (dy1 <= dx1)
			{
				if (dx >= 0)
				{
					x = x1;
					y = y1;

					xe = x2;
				}
				else
				{
					x = x2;
					y = y2;

					xe = x1;
				}

				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					{
						int xc = x;
						int yc = y;

						int r = c_size;

						int x = 0;
						int y = 0;

						int p = 3 - (2 * r);

						if (r)
						{
							y = r;

							while (y >= x)
							{
								pscanline(xc - x, xc + x, yc - y, c_element);
								pscanline(xc - y, xc + y, yc - x, c_element);
								pscanline(xc - x, xc + x, yc + y, c_element);
								pscanline(xc - y, xc + y, yc + x, c_element);

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
					}
				}

				for (int i = 0; x < xe; i++)
				{
					x = x + 1;

					if (px < 0)
					{
						px = px + 2 * dy1;
					}
					else
					{
						if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						{
							y = y + 1;
						}
						else
						{
							y = y - 1;
						}

						px = px + 2 * (dy1 - dx1);
					}

					if (x >= 0 && x < width && y >= 0 && y < height)
					{
						{
							int xc = x;
							int yc = y;

							int r = c_size;

							int x = 0;
							int y = 0;

							int p = 3 - (2 * r);

							if (r)
							{
								y = r;

								while (y >= x)
								{
									pscanline(xc - x, xc + x, yc - y, c_element);
									pscanline(xc - y, xc + y, yc - x, c_element);
									pscanline(xc - x, xc + x, yc + y, c_element);
									pscanline(xc - y, xc + y, yc + x, c_element);

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
						}
					}
				}
			}
			else
			{
				if (dy >= 0)
				{
					x = x1;
					y = y1;

					ye = y2;
				}
				else
				{
					x = x2;
					y = y2;

					ye = y1;
				}

				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					{
						int xc = x;
						int yc = y;

						int r = c_size;

						int x = 0;
						int y = 0;

						int p = 3 - (2 * r);

						if (r)
						{
							y = r;

							while (y >= x)
							{
								pscanline(xc - x, xc + x, yc - y, c_element);
								pscanline(xc - y, xc + y, yc - x, c_element);
								pscanline(xc - x, xc + x, yc + y, c_element);
								pscanline(xc - y, xc + y, yc + x, c_element);

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
					}
				}

				for (int i = 0; y < ye; i++)
				{
					y = y + 1;

					if (py <= 0)
					{
						py = py + 2 * dx1;
					}
					else
					{
						if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						{
							x = x + 1;
						}
						else
						{
							x = x - 1;
						}

						py = py + 2 * (dx1 - dy1);
					}

					if (x >= 0 && x < width && y >= 0 && y < height)
					{
						{
							int xc = x;
							int yc = y;

							int r = c_size;

							int x = 0;
							int y = 0;

							int p = 3 - (2 * r);

							if (r)
							{
								y = r;

								while (y >= x)
								{
									pscanline(xc - x, xc + x, yc - y, c_element);
									pscanline(xc - y, xc + y, yc - x, c_element);
									pscanline(xc - x, xc + x, yc + y, c_element);
									pscanline(xc - y, xc + y, yc + x, c_element);

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
						}
					}
				}
			}

			#undef x1
			#undef y1

			#undef x2
			#undef y2
		}

		if (mr_pressed)
		{
			// Remove all particles within cursor.

			int xc = mouse_x;
			int yc = mouse_y;

			int r = c_size;

			int x = 0;
			int y = 0;

			int p = 3 - (2 * r);

			if (r)
			{
				y = r;

				while (y >= x)
				{
					prscanline(xc - x, xc + x, yc - y);
					prscanline(xc - y, xc + y, yc - x);
					prscanline(xc - x, xc + x, yc + y);
					prscanline(xc - y, xc + y, yc + x);

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
		}
		else if (mr_held)
		{
			#define x1 mouse_ox
			#define y1 mouse_oy

			#define x2 mouse_x
			#define y2 mouse_y

			int x;
			int y;

			int dx = x2 - x1;
			int dy = y2 - y1;

			int dx1 = std::abs(dx);
			int dy1 = std::abs(dy);

			int px = 2 * dy1 - dx1;
			int py = 2 * dx1 - dy1;

			int xe;
			int ye;

			if (dy1 <= dx1)
			{
				if (dx >= 0)
				{
					x = x1;
					y = y1;

					xe = x2;
				}
				else
				{
					x = x2;
					y = y2;

					xe = x1;
				}

				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					{
						int xc = x;
						int yc = y;

						int r = c_size;

						int x = 0;
						int y = 0;

						int p = 3 - (2 * r);

						if (r)
						{
							y = r;

							while (y >= x)
							{
								prscanline(xc - x, xc + x, yc - y);
								prscanline(xc - y, xc + y, yc - x);
								prscanline(xc - x, xc + x, yc + y);
								prscanline(xc - y, xc + y, yc + x);

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
					}
				}

				for (int i = 0; x < xe; i++)
				{
					x = x + 1;

					if (px < 0)
					{
						px = px + 2 * dy1;
					}
					else
					{
						if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						{
							y = y + 1;
						}
						else
						{
							y = y - 1;
						}

						px = px + 2 * (dy1 - dx1);
					}

					if (x >= 0 && x < width && y >= 0 && y < height)
					{
						{
							int xc = x;
							int yc = y;

							int r = c_size;

							int x = 0;
							int y = 0;

							int p = 3 - (2 * r);

							if (r)
							{
								y = r;

								while (y >= x)
								{
									prscanline(xc - x, xc + x, yc - y);
									prscanline(xc - y, xc + y, yc - x);
									prscanline(xc - x, xc + x, yc + y);
									prscanline(xc - y, xc + y, yc + x);

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
						}
					}
				}
			}
			else
			{
				if (dy >= 0)
				{
					x = x1;
					y = y1;

					ye = y2;
				}
				else
				{
					x = x2;
					y = y2;

					ye = y1;
				}

				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					{
						int xc = x;
						int yc = y;

						int r = c_size;

						int x = 0;
						int y = 0;

						int p = 3 - (2 * r);

						if (r)
						{
							y = r;

							while (y >= x)
							{
								prscanline(xc - x, xc + x, yc - y);
								prscanline(xc - y, xc + y, yc - x);
								prscanline(xc - x, xc + x, yc + y);
								prscanline(xc - y, xc + y, yc + x);

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
					}
				}

				for (int i = 0; y < ye; i++)
				{
					y = y + 1;

					if (py <= 0)
					{
						py = py + 2 * dx1;
					}
					else
					{
						if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						{
							x = x + 1;
						}
						else
						{
							x = x - 1;
						}

						py = py + 2 * (dx1 - dy1);
					}

					if (x >= 0 && x < width && y >= 0 && y < height)
					{
						{
							int xc = x;
							int yc = y;

							int r = c_size;

							int x = 0;
							int y = 0;

							int p = 3 - (2 * r);

							if (r)
							{
								y = r;

								while (y >= x)
								{
									prscanline(xc - x, xc + x, yc - y);
									prscanline(xc - y, xc + y, yc - x);
									prscanline(xc - x, xc + x, yc + y);
									prscanline(xc - y, xc + y, yc + x);

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
						}
					}
				}
			}

			#undef x1
			#undef y1

			#undef x2
			#undef y2
		}

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
					p->vx += ((rand() % 2) * 2.0 - 1.0) * 0.1;
					p->vy += ((rand() % 2) * 2.0 - 1.0) * 0.1;
				}

				default:
				{
					break;
				}
			}

			if (p->t != el_wvpr)
			{
				// Apply gravity.

				p->vy += 0.3;
			}

			// Apply drag.

			p->vx *= 0.995;
			p->vy *= 0.995;

			// Move particle using modified version of Bresenham's line drawing algorithm. This is
			// pretty intuitive, and it works super well.

			int vx = p->vx;
			int vy = p->vy;

			int c = rgb(50, 50, 50);

			float x1 = ox;
			float y1 = oy;

			float x2 = ox + vx;
			float y2 = oy + vy;

			float x;
			float y;

			float dx = x2 - x1;
			float dy = y2 - y1;

			float dx1 = std::abs(dx);
			float dy1 = std::abs(dy);

			float px = 2.0 * dy1 - dx1;
			float py = 2.0 * dx1 - dy1;

			float xe;
			float ye;

			float lx = x1;
			float ly = y1;

			bool collision = false;

			switch (p->t)
			{
				case el_wice:
				case el_ston:
				{
					x = x1;
					y = y1;

					goto done_collisions;
				}

				default:
				{
					break;
				}
			}

			if (dy1 <= dx1)
			{
				if (dx >= 0)
				{
					x = x1;
					y = y1;

					xe = x2;
				}
				else
				{
					x = x2;
					y = y2;

					xe = x1;
				}

				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					p_t* a_ptr = lmap[(int)y * width + (int)x];

					if (a_ptr != NULL && a_ptr != p)
					{
						// It's okay, in this scenario. 'goto' isn't always evil, guys.

						x = lx;
						y = ly;

						collision = true;

						goto done_collisions;
					}

					lx = x;
					ly = y;
				}

				for (int i = 0; x < xe; i++)
				{
					x = x + 1;

					if (px < 0)
					{
						px = px + 2 * dy1;
					}
					else
					{
						if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						{
							y = y + 1;
						}
						else
						{
							y = y - 1;
						}

						px = px + 2 * (dy1 - dx1);
					}

					if (x >= 0 && x < width && y >= 0 && y < height)
					{
						p_t* a_ptr = lmap[(int)y * width + (int)x];

						if (a_ptr != NULL && a_ptr != p)
						{	
							x = lx;
							y = ly;

							collision = true;

							goto done_collisions;
						}

						lx = x;
						ly = y;
					}
				}
			}
			else
			{
				if (dy >= 0)
				{
					x = x1;
					y = y1;

					ye = y2;
				}
				else
				{
					x = x2;
					y = y2;

					ye = y1;
				}

				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					p_t* a_ptr = lmap[(int)y * width + (int)x];

					if (a_ptr != NULL && a_ptr != p)
					{	
						x = lx;
						y = ly;

						collision = true;

						goto done_collisions;
					}

					lx = x;
					ly = y;
				}

				for (int i = 0; y < ye; i++)
				{
					y = y + 1;

					if (py <= 0)
					{
						py = py + 2 * dx1;
					}
					else
					{
						if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						{
							x = x + 1;
						}
						else
						{
							x = x - 1;
						}

						py = py + 2 * (dx1 - dy1);
					}

					if (x >= 0 && x < width && y >= 0 && y < height)
					{
						p_t* a_ptr = lmap[(int)y * width + (int)x];

						if (a_ptr != NULL && a_ptr != p)
						{	
							x = lx;
							y = ly;

							collision = true;

							goto done_collisions;
						}

						lx = x;
						ly = y;
					}
				}
			}

			done_collisions:

			p->x = x;
			p->y = y;

			if (collision)
			{
				// Lose momentum upon collision.

				p->vx *= rfs * 0.5;
				p->vy *= rfs * 0.5;
			}

			// Affect this particle (and maybe it's neighbors) based on it's material.

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

					if (iny < hmo - 1 && (lmap[(iny + 1) * width + inx] != NULL))
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

					break;
				}

				// Liquids.

				case el_watr:
				{
					if (p->t == el_watr)
					{
						if (iny > 0)
						{
							p_t* tmn = lmap[(iny - 1) * width + inx];

							if (tmn != NULL && tmn->t == el_sand)
							{
								// Sand sinks, so we swap the positions of water and sand if a 
								// sand particle is above a water particle.
								
								tmn->t = el_watr;

								p->t = el_sand;
							}
						}
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
				}

				// Something else?

				default:
				{
					break;
				}
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