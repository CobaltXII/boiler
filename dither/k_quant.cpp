#define BOIL_USE_STB_IMAGE

#include "../boiler/boiler.h"

#include <random>
#include <vector>
#include <utility>
#include <iostream>

std::string path_to_img;

// Initialize a good random number generator.

std::default_random_engine rng_source;

// An image is a one-dimensional array of unsigned integers. However, a one-dimensional array of
// unsigned integers can be any number of other things as well. For clarity, we use a typedef.

typedef Uint32* image_rgb;

// Similar to the above statement, a grayscale image can be represented by a one-dimensional array
// of unsigned chars. An array of unsigned chars is ambiguous, so we use a typedef.

typedef Uint8* image_gs;

// A three-dimensional point.

struct point
{
	float x;
	float y;
	float z;

	point()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	point(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};

// Do k-means clustering on a dataset of points. j is the number of iterations.

std::vector<point> k_means(std::vector<point>& dataset, int k, int j)
{
	// 'k' cluster locations.

	float cx[k];
	float cy[k];
	float cz[k];

	// Start at random positions.

	for (int i = 0; i < k; i++)
	{
		cx[i] = rand_01();
		cy[i] = rand_01();
		cz[i] = rand_01();
	}

	// Iterate 'j' times.

	for (int i = 0; i < j; i++)
	{
		// 'k' cluster buckets.

		std::vector<point*> buckets[k];

		// Assign each point in the dataset to a bucket depending on which 
		// cluster the point is closest to.

		for (int m = 0; m < dataset.size(); m++)
		{
			float max_dist = INFINITY;

			int index = -1;

			for (int n = 0; n < k; n++)
			{
				float dx = dataset[m].x - cx[n];
				float dy = dataset[m].y - cy[n];
				float dz = dataset[m].z - cz[n];

				float d = dx * dx + dy * dy + dz * dz;

				if (d < max_dist)
				{
					max_dist = d;

					index = n;
				}
			}

			buckets[index].push_back(&(dataset[m]));
		}

		// Move each cluster to the average position of it's bucket's 
		// contents.

		for (int m = 0; m < k; m++)
		{
			if (buckets[m].size() == 0)
			{
				cx[m] = rand_01();
				cy[m] = rand_01();
				cz[m] = rand_01();

				j++;

				continue;
			}

			float avgx = 0.0f;
			float avgy = 0.0f;
			float avgz = 0.0f;

			for (int n = 0; n < buckets[m].size(); n++)
			{
				avgx += buckets[m][n]->x;
				avgy += buckets[m][n]->y;
				avgz += buckets[m][n]->z;
			}

			cx[m] = avgx / float(buckets[m].size());
			cy[m] = avgy / float(buckets[m].size());
			cz[m] = avgz / float(buckets[m].size());
		}
	}

	// Construct a vector of clusters to return.

	std::vector<point> clusters;

	for (int i = 0; i < k; i++)
	{
		point c;

		c.x = cx[i];
		c.y = cy[i];
		c.z = cz[i];

		clusters.push_back(c);
	}

	return clusters;
}

// Quantize an image using a color palette.

image_rgb quant(image_rgb img, int w, int h, std::vector<point> pal)
{
	image_rgb o_rgb = (image_rgb)malloc(sizeof(Uint32) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			// Find the closest color in RGB space.

			float max_dist = INFINITY;

			int index = -1;

			float nr = getr(img[y * w + x]) / 255.0f;
			float ng = getg(img[y * w + x]) / 255.0f;
			float nb = getb(img[y * w + x]) / 255.0f;

			for (int p = 0; p < pal.size(); p++)
			{
				float dx = nr - pal[p].x;
				float dy = ng - pal[p].y;
				float dz = nb - pal[p].z;

				float d = dx * dx + dy * dy + dz * dz;

				if (d < max_dist)
				{
					max_dist = d;

					index = p;
				}
			}
			
			// Set the output color to the closest color.

			int pr = pal[index].x * 255;
			int pg = pal[index].y * 255;
			int pb = pal[index].z * 255;

			o_rgb[y * w + x] = rgb(pr, pg, pb);
		}
	}

	return o_rgb;
}

// Lena Söderberg is commonly used as a test image for computer graphics. Given her importance, it
// would be rude to not declare her as a global variable.

int lena_w;
int lena_h;

image_rgb lena_rgb = NULL;

// Total annihilation!

image_rgb lena_gs_rgb = NULL;

image_gs lena_gs = NULL;

// The Boiler structure used to render Lena.

struct game: boiler
{	
	int lena_m_w;
	int lena_m_h;

	image_rgb lena_m;

	void steam() override
	{
		lena_rgb = loadimg
		(
			path_to_img,

			lena_w, 
			lena_h
		);

		if (!lena_rgb)
		{
			nuke("Could not load image.");
		}

		width = 800;
		height = 600;

		title = "K-means quantization (using Boiler)";

		// Generate the palette.

		std::vector<point> dataset(lena_w * lena_h);

		for (int x = 0; x < lena_w; x++)
		for (int y = 0; y < lena_h; y++)
		{
			int r = getr(lena_rgb[y * lena_w + x]);
			int g = getg(lena_rgb[y * lena_w + x]);
			int b = getb(lena_rgb[y * lena_w + x]);

			float nr = r / 255.0f;
			float ng = g / 255.0f;
			float nb = b / 255.0f;

			point data;

			data.x = nr;
			data.y = ng;
			data.z = nb;

			dataset[y * lena_w + x] = data;
		}

		std::vector<point> palette = k_means(dataset, 8, 256);

		// Quantize the image.

		lena_m = quant(lena_rgb, lena_w, lena_h, palette);

		lena_m_w = lena_w;
		lena_m_h = lena_h;
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Draw Lena really, really fast.

		int ox = h_width - (lena_m_w / 2);
		int oy = h_height - (lena_m_h / 2);

		for (int y = 0; y < lena_m_h; y++)
		{
			memcpy(&(pixels[(oy + y) * width + ox]), &(lena_m[y * lena_m_w]), sizeof(Uint32) * lena_m_w);
		}
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	if (argc != 1 && argc != 2)
	{
		nuke("Usage: ./dither [path-to-img]");
	}

	if (argc == 2)
	{
		path_to_img = std::string(argv[1]);
	}
	else
	{
		path_to_img = "lena_color.bmp";
	}

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