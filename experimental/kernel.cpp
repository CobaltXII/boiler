// Image convolution using kernels? Awesome.

#define BOIL_USE_STB_IMAGE

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

std::string path_to_img;

struct kernel
{
	int kx;
	int ky;

	float* k;
};

kernel box_blur(int x)
{
	kernel kern = {x, x, (float*)malloc(x * x * sizeof(float))};

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		kern.k[j * x + i] = 1.0f / float(x * x);
	}

	return kern;
}

kernel edge_detect_1()
{
	int x = 3;

	kernel kern = {x, x, (float*)malloc(x * x * sizeof(float))};

	float k[3][3] =
	{
		{0.0f + 1.0f, 0.0f + 0.0f, 0.0f - 1.0f},
		{0.0f + 0.0f, 0.0f + 0.0f, 0.0f + 0.0f},
		{0.0f - 1.0f, 0.0f + 0.0f, 0.0f + 1.0f}
	};

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		kern.k[j * x + i] = k[j][i];
	}

	return kern;
}

kernel edge_detect_2()
{
	int x = 3;

	kernel kern = {x, x, (float*)malloc(x * x * sizeof(float))};

	float k[3][3] =
	{
		{0.0f + 0.0f, 0.0f + 1.0f, 0.0f + 0.0f},
		{0.0f + 1.0f, 0.0f - 4.0f, 0.0f + 1.0f},
		{0.0f + 0.0f, 0.0f + 1.0f, 0.0f + 0.0f}
	};

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		kern.k[j * x + i] = k[j][i];
	}

	return kern;
}

kernel edge_detect_3()
{
	int x = 3;

	kernel kern = {x, x, (float*)malloc(x * x * sizeof(float))};

	float k[3][3] =
	{
		{0.0f - 1.0f, 0.0f - 1.0f, 0.0f - 1.0f},
		{0.0f - 1.0f, 0.0f + 8.0f, 0.0f - 1.0f},
		{0.0f - 1.0f, 0.0f - 1.0f, 0.0f - 1.0f}
	};

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		kern.k[j * x + i] = k[j][i];
	}

	return kern;
}

kernel sharpen()
{
	int x = 3;

	kernel kern = {x, x, (float*)malloc(x * x * sizeof(float))};

	float k[3][3] =
	{
		{0.0f + 0.0f, 0.0f - 1.0f, 0.0f + 0.0f},
		{0.0f - 1.0f, 0.0f + 5.0f, 0.0f - 1.0f},
		{0.0f + 0.0f, 0.0f - 1.0f, 0.0f + 0.0f}
	};

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		kern.k[j * x + i] = k[j][i];
	}

	return kern;
}

// I wrote this one by translating the math.

float guassian_2d(float x, float y, float stdev)
{
	return 1.0f / (2.0f * M_PI * (stdev * stdev)) * pow(M_E, -(x * x + y * y) / (2.0f * (stdev * stdev)));
}

// However, most programmers seem to use this one.

float popular_guassian_2d(float x, float y, float stdev)
{
	return exp(-(x * x + y * y) / (2.0f * stdev * stdev)) / (2.0f * M_PI * stdev * stdev);
}

kernel guassian(int x, float stdev)
{
	kernel kern = {x, x, (float*)malloc(x * x * sizeof(float))};

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		float kx = i - (x - 1) / 2;
		float ky = j - (x - 1) / 2;

		kern.k[j * x + i] = popular_guassian_2d(kx, ky, stdev);
	}

	// Normalize.

	float sum = 0.0f;

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		sum += kern.k[j * x + i];
	}

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		kern.k[j * x + i] /= sum;	
	}

	return kern;
}

struct game: boiler
{	
	int image_w;
	int image_h;

	unsigned int* image;

	void steam() override
	{
		width = 600;
		
		height = 600;

		title = "Image convolution (using Boiler)";

		// Load the image.

		image = loadimg(path_to_img, image_w, image_h);

		if (!image)
		{
			exit(EXIT_FAILURE);
		}

		// Generate the kernel.

		kernel filter = guassian(20, sqrtf(10.0f));

		// Do the filtering.

		unsigned int* new_image = (unsigned int*)malloc(image_w * image_h * sizeof(unsigned int));

		for (int y = 0; y < image_h; y++)
		for (int x = 0; x < image_w; x++)
		{
			// Ready an accumulator.

			float accumulator_r = 0.0f;
			float accumulator_b = 0.0f;
			float accumulator_g = 0.0f;

			// Iterate through the filter.

			for (int j = (filter.ky - 1) / -2; j <= (filter.ky - 1) / 2; j++)
			for (int i = (filter.kx - 1) / -2; i <= (filter.kx - 1) / 2; i++)
			{
				{
					// Find the coordinates of the pixel to sample. This is
					// using the wrap method, as described by Wikipedia
					//
					//     The image is conceptually wrapped (or tiled) and
					//    values are taken from the opposite edge or corner.

					// int px = ((x + i) + image_w) % image_w;
					// int py = ((y + j) + image_h) % image_h;
				}

				{
					// Find the coordinates of the pixel to sample. This is
					// just setting out-of-bounds pixels to black.

					// int px = x + i;
					// int py = y + j;
				}

				// Find the coordinates of the pixel to sample. This is using
				// the extend method, as described by Wikipedia
				//
				//     The nearest border pixels are conceptually extended as
				//     far as necessary to provide values for the convolution.
				//     Corner pixels are extended in 90° wedges. Other edge
				//     pixels are extended in lines.

				int px = std::max(0, std::min(x + i, image_w - 1));
				int py = std::max(0, std::min(y + j, image_h - 1));

				if (px >= 0 && py >= 0 && px < image_w && py < image_h)
				{
					// Find the coordinates of the filter coefficient.

					int ex = i + (filter.kx - 1) / 2;
					int ey = j + (filter.ky - 1) / 2;

					// Fetch the coefficient.

					float coefficient = filter.k[ey * filter.kx + ex];

					// Multiply the sampled pixel by the coefficient.

					float pixel_r = mgetr(image[py * image_w + px]) * coefficient;
					float pixel_g = mgetg(image[py * image_w + px]) * coefficient;
					float pixel_b = mgetb(image[py * image_w + px]) * coefficient;

					// Add the previously calculated value to the accumulator.

					accumulator_r += pixel_r;
					accumulator_g += pixel_g;
					accumulator_b += pixel_b;
				}
			}

			// Write the accumulator to the new image.

			unsigned char r = mclamprgb(accumulator_r);
			unsigned char g = mclamprgb(accumulator_g);
			unsigned char b = mclamprgb(accumulator_b);

			new_image[y * image_w + x] = mrgb(r, g, b);
		}

		image = new_image;
	}

	void draw() override
	{
		black();

		// I forgot how to use blitrgb, so I stole this from CobaltXII/boiler/dither/dither.cpp.

		int ox = h_width - (image_w / 2);

		int oy = h_height - (image_h / 2);

		for (int y = 0; y < image_h; y++)
		{
			memcpy(&(pixels[(oy + y) * width + ox]), &(image[y * image_w]), sizeof(Uint32) * image_w);
		}
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	if (argc != 2 && argc != 1)
	{
		std::cout << "Usage: " << argv[0] << " <path-to-img>" << std::endl;

		return EXIT_FAILURE;
	}

	path_to_img = "test/macaw.png";

	if (argc == 2)
	{
		path_to_img = std::string(argv[1]);
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