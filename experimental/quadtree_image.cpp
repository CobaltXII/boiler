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
