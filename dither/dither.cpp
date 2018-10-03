// Dithering? Awesome.

#include "../boiler/boiler.h"

#include <random>
#include <vector>
#include <utility>
#include <iostream>

// Initialize a good random number generator.

std::default_random_engine rng_source;

// An image is a one-dimensional array of unsigned integers. However, a one-dimensional array of
// unsigned integers can be any number of other things as well. For clarity, we use a typedef.

typedef Uint32* image_rgb;

// Similar to the above statement, a grayscale image can be represented by a one-dimensional array
// of unsigned chars. An array of unsigned chars is ambiguous, so we use a typedef.

typedef Uint8* image_gs;

// Import some common dithering matrices.

#include "matrices.h"

// This function will convert a value of type image_rgb to a value of type image_gs. The color to
// grayscale conversion is done by averaging the individual components of each pixel.

image_gs to_grayscale(image_rgb img, int w, int h)
{
	image_gs o_gs = (image_gs)malloc(sizeof(Uint8) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			Uint32 r = getr(img[y * w + x]);
			Uint32 g = getg(img[y * w + x]);
			Uint32 b = getb(img[y * w + x]);

			// Average the components.

			Uint8 q = (r + g + b) / 3;

			// Apply the color to the output image.

			o_gs[y * w + x] = q;
		}
	}

	return o_gs;
}

// This function will convert a value of type image_gs to a value of type image_rgb. The grayscale
// to color conversion is done by creating a RGB tuple with three identical values.

image_rgb to_rgb(image_gs img, int w, int h)
{
	image_rgb o_rgb = (image_rgb)malloc(sizeof(Uint32) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			o_rgb[y * w + x] = rgb
			(
				img[y * w + x], 
				img[y * w + x], 
				img[y * w + x]
			);
		}
	}

	return o_rgb;
}

// The following functions will convert a grayscale image into a color image which uses the 
// grayscale value as either the R, G, or B component of the color image.

image_rgb to_r(image_gs img, int w, int h)
{
	image_rgb o_rgb = (image_rgb)malloc(sizeof(Uint32) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			o_rgb[y * w + x] = rgb(img[y * w + x], 0, 0);
		}
	}

	return o_rgb;
}

image_rgb to_g(image_gs img, int w, int h)
{
	image_rgb o_rgb = (image_rgb)malloc(sizeof(Uint32) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			o_rgb[y * w + x] = rgb(0, img[y * w + x], 0);
		}
	}

	return o_rgb;
}

image_rgb to_b(image_gs img, int w, int h)
{
	image_rgb o_rgb = (image_rgb)malloc(sizeof(Uint32) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			o_rgb[y * w + x] = rgb(0, 0, img[y * w + x]);
		}
	}

	return o_rgb;
}

// The following functions will convert a color image into a grayscale image, in which the value
// of each pixel is equal to the R, G or B component of the corresponding pixel of the color 
// image.

image_gs splice_r(image_rgb img, int w, int h)
{
	image_gs o_gs = (image_gs)malloc(sizeof(Uint8) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			o_gs[y * w + x] = getr(img[y * w + x]);
		}
	}

	return o_gs;
}

image_gs splice_g(image_rgb img, int w, int h)
{
	image_gs o_gs = (image_gs)malloc(sizeof(Uint8) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			o_gs[y * w + x] = getg(img[y * w + x]);
		}
	}

	return o_gs;
}

image_gs splice_b(image_rgb img, int w, int h)
{
	image_gs o_gs = (image_gs)malloc(sizeof(Uint8) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			o_gs[y * w + x] = getb(img[y * w + x]);
		}
	}

	return o_gs;
}

// This function will create a color image using three grayscale images as input. The grayscale
// images are used to obtain the color components of each pixel.

image_rgb combine_rgb(image_gs r, image_gs g, image_gs b, int w, int h)
{
	image_rgb o_rgb = (image_rgb)malloc(sizeof(Uint32) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			o_rgb[y * w + x] = rgb
			(
				r[y * w + x],
				g[y * w + x],
				b[y * w + x]
			);
		}
	}

	return o_rgb;
}

// This function will take a grayscale image as input. It will compare the value of each pixel to
// pure black and pure white, and set the output pixel to the closest color.

image_gs threshold_bw(image_gs img, int w, int h)
{
	image_gs o_gs = (image_gs)malloc(sizeof(Uint8) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			if (img[y * w + x] < 0.5 * 255.0)
			{
				o_gs[y * w + x] = 0;
			}
			else
			{
				o_gs[y * w + x] = 255;
			}
		}
	}

	return o_gs;
}

// This function will dither a grayscale image using a random threshold value. Pixels below the
// threshold will be black, pixels above will be white.

image_gs random_dither_bw(image_gs img, int w, int h)
{
	image_gs o_gs = (image_gs)malloc(sizeof(Uint8) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			if (img[y * w + x] < rand() % 256)
			{
				o_gs[y * w + x] = 0;
			}
			else
			{
				o_gs[y * w + x] = 255;
			}
		}
	}

	return o_gs;
}

// This function is the same as the above, except that it uses a Gaussian distribution to 
// calculate threshold values.

image_gs gaussian_dither_bw(image_gs img, int w, int h, double mean = 0.5, double stddev = 0.15)
{
	// Create a Gaussian distribution device.

	std::normal_distribution<double> n_device = std::normal_distribution<double>(mean, stddev);

	image_gs o_gs = (image_gs)malloc(sizeof(Uint8) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			if (img[y * w + x] < n_device(rng_source) * 255)
			{
				o_gs[y * w + x] = 0;
			}
			else
			{
				o_gs[y * w + x] = 255;
			}
		}
	}

	return o_gs;
}

// This function will dither a grayscale image using a dithering matrix. Pixels below the
// calculated threshold will be black, pixels above will be white.

image_gs ordered_dither_bw(image_gs img, int w, int h, d_matrix matrix)
{
	image_gs o_gs = (image_gs)malloc(sizeof(Uint8) * w * h);

	int m_w = matrix[0].size();

	int m_h = matrix.size();

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			int m = matrix[y % m_h][x % m_w];

			if (img[y * w + x] < ((1.0 + m) / (1.0 + m_w * m_h)) * 255)
			{
				o_gs[y * w + x] = 0;
			}
			else
			{
				o_gs[y * w + x] = 255;
			}
		}
	}

	return o_gs;
}

// This function will gamma correct a grayscale image using the given gamma value.

image_gs gamma_gs(image_gs img, int w, int h, double gamma)
{
	image_gs o_gs = (image_gs)malloc(sizeof(Uint8) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			o_gs[y * w + x] = std::pow(img[y * w + x] / 255.0, gamma) * 255;
		}
	}

	return o_gs;
}

// This function will gamma correct a color image using the given gamma value.

image_rgb gamma_rgb(image_rgb img, int w, int h, double gamma)
{
	image_rgb o_gs = (image_rgb)malloc(sizeof(Uint32) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			o_gs[y * w + x] = rgb
			(
				std::pow(getr(img[y * w + x]) / 255.0, gamma) * 255,
				std::pow(getg(img[y * w + x]) / 255.0, gamma) * 255,
				std::pow(getb(img[y * w + x]) / 255.0, gamma) * 255
			);
		}
	}

	return o_gs;
}

// Lena Söderberg is commonly used as a test image for computer graphics. Given her importance, it
// would be rude to not declare her as a global variable.

int lena_w;
int lena_h;

image_rgb lena_rgb = loadbmp
(
	"lena_color.bmp", 

	lena_w, 
	lena_h
);

image_rgb lena_gs_rgb = loadbmp
(
	"lena_grayscale.bmp", 

	lena_w, 
	lena_h
);

image_gs lena_gs = to_grayscale
(
	lena_gs_rgb,

	lena_w,
	lena_h
);

// The Boiler structure used to render Lena.

struct game: boiler
{	
	int lena_m_w;
	int lena_m_h;

	image_rgb lena_m;

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Dithering (using Boiler)";

		// Do something to Lena.

		lena_rgb = gamma_rgb(lena_rgb, lena_w, lena_h, 2.2);

		d_matrix m = bayer_8;

		image_gs r_channel = ordered_dither_bw(splice_r(lena_rgb, lena_w, lena_h), lena_w, lena_h, m);
		image_gs g_channel = ordered_dither_bw(splice_g(lena_rgb, lena_w, lena_h), lena_w, lena_h, m);
		image_gs b_channel = ordered_dither_bw(splice_b(lena_rgb, lena_w, lena_h), lena_w, lena_h, m);

		lena_m = combine_rgb
		(
			r_channel,
			g_channel,
			b_channel,

			lena_w,
			lena_h
		);

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