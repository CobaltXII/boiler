// Quadtrees? Awesome.

#define BOIL_USE_STB_IMAGE

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

int image_w;
int image_h;

int qtw = 512;
int qth = 512;

Uint32* image;

std::string path_to_img;

// It's a region quadtree.

struct quadtree
{
	quadtree* tl;
	quadtree* tr;
	quadtree* bl;
	quadtree* br;

	double ar;
	double ag;
	double ab;

	double er;
	double eg;
	double eb;

	double x;
	double y;

	double w;
	double h;

	// Create an empty quadtree.

	quadtree(double _x, double _y, double _w, double _h)
	{
		x = _x;
		y = _y;

		w = _w;
		h = _h;

		tl = nullptr;
		tr = nullptr;
		bl = nullptr;
		br = nullptr;

		// Calculate average color.

		ar = 0.0;
		ag = 0.0;
		ab = 0.0;

		double s = double(_w * _h);

		for (int __x = _x; __x < _x + _w; __x++)
		{
			for (int __y = _y; __y < _y + _h; __y++)
			{
				ar += double(getr(image[__y * image_w + __x])) / s;
				ag += double(getg(image[__y * image_w + __x])) / s;
				ab += double(getb(image[__y * image_w + __x])) / s;
			}	
		}

		// Calculate error.

		er = 0.0;
		eg = 0.0;
		eb = 0.0;

		for (int __x = _x; __x < _x + _w; __x++)
		{
			for (int __y = _y; __y < _y + _h; __y++)
			{
				er += abs(double(getr(image[__y * image_w + __x])) - ar) / s;
				eg += abs(double(getg(image[__y * image_w + __x])) - ag) / s;
				eb += abs(double(getb(image[__y * image_w + __x])) - ab) / s;
			}	
		}

		#ifdef QTI_DEBUG

		std::cout << "ar: " << ar << std::endl;
		std::cout << "ag: " << ag << std::endl;
		std::cout << "ab: " << ab << std::endl;


		std::cout << "er: " << er << std::endl;
		std::cout << "eg: " << eg << std::endl;
		std::cout << "eb: " << eb << std::endl;

		#endif
	}

	// Subdivide me into four quadrants.

	void subdivide()
	{
		if (tl == nullptr)
		{
			double hw = w / 2.0;
			double hh = h / 2.0;

			tl = new quadtree(x, y, hw, hh);

			tr = new quadtree(x + hw, y, hw, hh);

			bl = new quadtree(x, y + hh, hw, hh);

			br = new quadtree(x + hw, y + hh, hw, hh);
		}
	}

	// Deconstructor.

	~quadtree()
	{
		if (tl != nullptr)
		{
			delete tl;
			delete tr;
			delete bl;
			delete br;
		}
	}
};

struct game: boiler
{	
	int offx;
	int offy;

	int segments = 0;

	quadtree* parent;

	bool automatic = true;

	bool outlines = false;

	// Initialize Boiler.

	void steam() override
	{
		image = loadimg(path_to_img, image_w, image_h);

		if (image == NULL)
		{
			nuke("Could not load image.");
		}

		if (image_w != image_h)
		{
			std::cout << "You will see skewing; your image is not square." << std::endl;
		}

		width = 800;
		height = 600;

		f_No_Debug = SDL_TRUE;

		title = "Quadtrees (using Boiler)";

		offx = width / 2 - qtw / 2;
		offy = height / 2 - qth / 2;

		parent = new quadtree(0, 0, qtw, qth);
	}

	// Draw a quadtree.

	void draw_tree(quadtree* qt, int x, int y)
	{
		segments++;

		if (qt->tl != nullptr)
		{
			draw_tree(qt->tl, x, y);
			draw_tree(qt->tr, x, y);
			draw_tree(qt->bl, x, y);
			draw_tree(qt->br, x, y);
		}
		else
		{
			double nx = double(qt->x) / double(qtw);
			double ny = double(qt->y) / double(qth);

			int ix = nx * image_w;
			int iy = ny * image_h;

			frectrgb(qt->x + x, qt->y + y, qt->w, qt->h, rgb(qt->ar, qt->ag, qt->ab));

			if (outlines)
			{
				rectrgb(qt->x + x, qt->y + y, qt->w, qt->h, rgb(0, 0, 0));
			}
		}
	}

	// Find the quadtree with the highest error.

	quadtree* h_err;

	void highest_error(quadtree* root, int depth = 0)
	{
		if (depth == 0)
		{
			h_err = nullptr;
		}

		if (root->tl == nullptr)
		{
			if (h_err == nullptr)
			{
				h_err = root;
			}
			else
			{
				double err = (root->er + root->eg + root->eb) * pow(root->w * root->h, 0.25);

				if ((h_err->er + h_err->eg + h_err->eb) * pow(h_err->w * h_err->h, 0.25) < err)
				{
					h_err = root;
				}
			}
		}
		else
		{
			highest_error(root->tl, depth + 1);
			highest_error(root->tr, depth + 1);
			highest_error(root->bl, depth + 1);
			highest_error(root->br, depth + 1);
		}
	}

	// Render a frame.

	void draw() override
	{
		segments = 0;

		black();

		draw_tree(parent, offx, offy);

		if (automatic)
		{
			for (int i = 0; i < 1; i++)
			{
				highest_error(parent);

				if (h_err != nullptr)
				{
					frectrgb(h_err->x + offx, h_err->y + offy, h_err->w, h_err->h, rgb(255, 255, 255));

					h_err->subdivide();
				}
			}	
		}

		if (iteration % 1 == 0)
		{
			std::cout << "Segments: " << segments << '\r' << std::flush;
		}
	}

	// Handle keypresses.

	void keydown(SDL_Event e) override
	{
		if (e.key.keysym.sym == SDLK_c)
		{
			// Clear.

			parent = new quadtree(0, 0, qtw, qth);
		}
		else if (e.key.keysym.sym == SDLK_t)
		{
			// Toggle automatic mode.

			automatic = !automatic;
		}
		else if (e.key.keysym.sym == SDLK_s)
		{
			// Step.

			highest_error(parent);

			if (h_err != nullptr)
			{
				h_err->subdivide();
			}
		}
		else if (e.key.keysym.sym == SDLK_r)
		{
			// Toggle outlines.

			outlines = !outlines;
		}
	}
};

