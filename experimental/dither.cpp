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

// Grayscale palettes are simply a list of 8-bit colors. We could represent this as a pointer or
// array, but using STL vectors is a neater and simpler solution.

typedef std::vector<Uint8> palette_gs;

// Dither matrices are two-dimensional matrices of integer values. They do not need to be squares,
// and the values may be anything. They can be represented with vectors.

typedef std::vector<std::vector<int>> d_matrix;

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

// The classic 2 by 2 Bayer dither matrix.

d_matrix bayer_2 =
{
	{
		0, 3
	},
	{
		2, 1
	}
};

// The classic 4 by 4 Bayer dither matrix.

d_matrix bayer_4 =
{
	{
		0, 12, 13, 15
	},
	{
		8, 4, 11, 7
	},
	{
		2, 14, 1, 13
	},
	{
		10, 6, 8, 5
	}
};

// The classic 8 by 8 Bayer dither matrix.

d_matrix bayer_8 =
{
	{
		0, 48, 12, 60, 3, 51, 15, 63
	},
	{
		32, 16, 44, 28, 35, 19, 47, 31
	},
	{
		8, 56, 4, 52, 11, 59, 7, 55,
	},
	{
		40, 24, 36, 20, 43, 27, 39, 23
	},
	{
		2, 50, 14, 62, 1, 49, 13, 61
	},
	{
		34, 18, 46, 30, 33, 17, 45, 29
	},
	{
		10, 58, 6, 54, 9, 57, 5, 53
	},
	{
		42, 26, 38, 22, 41, 25, 37, 21
	}
};

// The 4 by 4 cluster dot matrix.

d_matrix cluster_4 =
{
	{
		15, 5, 6, 13
	},
	{
		4, 0, 1, 7
	},
	{
		11, 3, 2, 8
	},
	{
		15, 10, 9, 14
	}
};

// The 8 by 8 cluster dot matrix.

d_matrix cluster_8 =
{
	{
		24, 10, 12, 26, 35, 47, 49, 37
	},
	{
		8, 0, 2, 14, 45, 59, 61, 51
	},
	{
		22, 6, 4, 16, 43, 57, 63, 53
	},
	{
		30, 30, 18, 28, 33, 41, 55, 39
	},
	{
		34, 46, 48, 36, 25, 11, 13, 27
	},
	{
		44, 58, 60, 50, 9, 1, 3, 15
	},
	{
		42, 56, 62, 52, 23, 7, 5, 17
	},
	{
		32, 40, 54, 38, 31, 21, 19, 29
	}
};

// A 5 by 3 vertical line matrix.

d_matrix vertical_line =
{
	{
		9, 3, 0, 6, 12
	},
	{
		10, 4, 1, 7, 13
	},
	{
		11, 5, 2, 8, 14
	}
};

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

image_gs threshold_gs(image_gs img, int w, int h, palette_gs pal)
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

// This function will generate a grayscale palette of n values. The values will be evenly spaced
// between black and white. Passing a value of 256 will result in a 8-bit palette. Similarly, 
// passing a value of 16 will result in a 4-bit palette.

palette_gs gs_palette(int n)
{
	palette_gs gs_pal;

	for (int i = 0; i < n; i++)
	{
		gs_pal.push_back(255.0 * ((double)i / ((double)n - 1)));
	}

	return gs_pal;
}

// This function will dither a grayscale image using a random threshold value. Pixels below the
// threshold will be black, pixels above will be white.

image_gs random_dither(image_gs img, int w, int h)
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

		lena_m = to_rgb(ordered_dither_bw(to_grayscale(lena_rgb, lena_w, lena_h), lena_w, lena_h, vertical_line), lena_w, lena_h);

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