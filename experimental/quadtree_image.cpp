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
