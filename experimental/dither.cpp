// Dithering? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

// An image is a one-dimensional array of unsigned integers. However, a one-dimensional array of
// unsigned integers can be any number of other things as well. For clarity, we use a typedef.

typedef Uint32* image_rgb;

// Similar to the above statement, a grayscale image can be represented by a one-dimensional array
// of unsigned chars. An array of unsigned chars is ambiguous, so we use a typedef.

typedef Uint8* image_gs;

// Lena Söderberg is commonly used as a test image for computer graphics. Given her importance, it
// would be rude to not declare her as a global variable.

int lena_w;
int lena_h;

image_rgb lena_rgb = loadbmp
(
	"lena.bmp", 

	lena_w, 
	lena_h
);

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

// This function will take a grayscale image as input. It will compare the value of each pixel to
// a static set of values, and choose the closest match. This value will be assigned to the pixel.

image_gs threshold_gs(image_gs img, int w, int h, std::vector<Uint8> pal)
{
	image_gs o_gs = (image_gs)malloc(sizeof(Uint8) * w * h);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			// Pick the closest matching value.

			Uint8 best_match = pal[0];

			// The current closest distance will be set to a value that is not representable by a
			// Uint8. This forces a color to be chosen by the for loop.

			Uint32 closest_distance = 256 * 256;

			for (int i = 0; i < pal.size(); i++)
			{
				Uint32 color_distance = 
				(
					(img[y * w + x] - pal[i]) *
					(img[y * w + x] - pal[i])
				);

				if (color_distance < closest_distance)
				{
					best_match = pal[i];

					closest_distance = color_distance;
				}
			} 

			o_gs[y * w + x] = best_match;
		}
	}

	return o_gs;
}

// The Boiler structured used to render Lena.

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

		lena_m = to_rgb(to_grayscale(lena_rgb, lena_w, lena_h), lena_w, lena_h);

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