/*

Minecraft flyby (ported to Boiler)

Original was written by Markus Persson.

This page was really helpful

	https://the8bitpimp.wordpress.com/2014/07/30/minecraft-4k-c-port/

*/

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

int x_width = 800;
int x_height = 600;

// Some kind of hash modification of rand().

inline int random(int max)
{
	return (rand() ^ (rand() << 16)) % max;
}

// Fixed point byte/byte multiply.

inline int fbb_mul(int a, int b)
{
	return (a * b) >> 8;
}

// Fixed point 8-bit color/color multiply.

inline int rgb_mul(int a, int b)
{
	int _r = (((a >> 0x10) & 0xFF) * b) >> 8;
	int _g = (((a >> 0x08) & 0xFF) * b) >> 8;
	int _b = (((a >> 0x00) & 0xFF) * b) >> 8;

	return (_r << 16) | (_g << 8) | _b;
}

// Texture map.

int t_map[16 * 16 * 16 * 3];

// Voxel map.

int v_map[64 * 64 * 64];

// Generate textures, this is basically black magic.
//
//  0: Air
//  1: Grass
//  2: Dirt
//  3: Dirt
//  4: Stone
//  5: Brick
//  6: Dirt
//  7: Wood
//  8: Leaf
//  9: Dirt
// 10: Dirt
// 11: Dirt
// 12: Dirt
// 13: Dirt
// 14: Dirt
// 15: Dirt

void make_textures()
{
	for (int j = 0; j < 16; j++)
	{
		int k = 255 - random(96);

		for (int m = 0; m < 16 * 3; m++)
		{
			for (int n = 0; n < 16; n++)
			{
				int i_1 = 0x966C4a;

				int i_2 = 0;
				int i_3 = 0;

				if (j == 4)
				{
					i_1 = 0x7F7F7F;
				}

				if (j != 4 || (random(3) == 0))
				{
					k = 255 - random(96);
				}

				if (j == 1)
				{
					if (m < (((n * n * 3 + n * 81) >> 2) & 0x3) + 18)
					{
						i_1 = 0x6AAA40;
					}
					else if (m < (((n * n * 3 + n * 81) >> 2) & 0x3) + 19)
					{
						k = k * 2 / 3;
					}
				}

				if (j == 7)
				{
					i_1 = 0x675231;

					if ((n > 0) && (n < 15) && (((m > 0) && (m < 15)) || ((m > 32) && (m < 47))))
					{
						i_1 = 0xBC9862;

						i_2 = n - 7;

						i_3 = (m & 0xF) - 7;

						if (i_2 < 0)
						{
							i_2 = 1 - i_2;
						}

						if (i_3 < 0)
						{
							i_3 = 1 - i_3;
						}

						if (i_3 > i_2)
						{
							i_2 = i_3;
						}

						k = 196 - random(32) + i_2 % 3 * 32;
					}
					else if (random(2) == 0)
					{
						k = k * (150 - (n & 0x1) * 100) / 100;
					}
				}

				if (j == 5)
				{
					i_1 = 0xB53A15;

					if (((n + m / 4 * 4) % 8 == 0) || (m % 4 == 0))
					{
						i_1 = 0xBCAFA5;
					}
				}

				i_2 = k;

				if (m >= 32)
				{
					i_2 /= 2;
				}

				if (j == 8)
				{
					i_1 = 5298487;

					if (random(2) == 0)
					{
						i_1 = 0;

						i_2 = 255;
					}
				}

				i_3 = ((((i_1 >> 16) & 0xFF) * i_2 / 255) << 16) | ((((i_1 >> 8) & 0xFF) * i_2 / 255) << 8) | ((i_1 & 0xFF) * i_2 / 255);

				t_map[n + m * 16 + j * 256 * 3] = i_3;
			}
		}
	}
}

// Generate the voxel map, also black magic.

void make_voxels()
{
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			for (int z = 0; z < 64; z++)
			{
				int i = (z << 12) | (y << 6) | x;

				float yd = (y - 32.5) * 0.4;
				float zd = (z - 32.5) * 0.4;

				v_map[i] = random(16);

				float th = random(256) / 256.0;

				if (th > sqrtf(sqrtf(yd * yd + zd * zd)) - 0.8)
				{
					v_map[i] = 0;
				}
			}
		}
	}
}

struct game: boiler
{	
	// Plot a pixel.

	inline void plot(int x, int y, unsigned int c)
	{
		pixels[y * x_width + x] = c;
	}

	// Initialize Boiler.

	void steam() override
	{
		width = x_width;
		height = x_height;

		title = "Minecraft (using Boiler)";
	}

	// Render frame.

	void draw() override
	{
		// This calculates a modified version of the current time.

		float now = (float)(SDL_GetTicks() % 10000) / 10000.0;

		// I think this generates the ray direction or rotation values.

		float x_rot = sinf(now * M_PI * 2.0) * 0.4 + M_PI / 2.0;

		float y_rot = cosf(now * M_PI * 2.0) * 0.4;

		float y_cos = cosf(y_rot);
		float y_sin = sinf(y_rot);

		float x_cos = cosf(x_rot);
		float x_sin = sinf(x_rot);

		float ox = 32.5 + now * 64.0;

		float oy = 32.5;
		float oz = 32.5;

		for (int x = 0; x < x_width; x++)
		{
			// Get the X axis delta?

			float ___xd = ((float)x - (float)x_width / 2.0) / (float)x_height;

			for (int y = 0; y < x_height; y++)
			{
				// Get the Y axis delta?

				float __yd = ((float)y - (float)x_height / 2.0) / (float)x_height;

				float __zd = 1;

				float ___zd = __zd * y_cos + __yd * y_sin;

				float _yd = __yd * y_cos - __zd * y_sin;

				float _xd = ___xd * x_cos + ___zd * x_sin;

				float _zd = ___zd * x_cos - ___xd * x_sin;

				// Initialize some variables unrelated to the delta?

				int col = 0;

				int br = 255;

				float ddist = 0.0;

				float closest = 32.0;

				// For each axis?

				for (int d = 0; d < 3; d++)
				{
					float dim_length = _xd;

					if (d == 1)
					{
						dim_length = _yd;
					}

					if (d == 2)
					{
						dim_length = _zd;
					}

					float ll = 1.0;

					if (dim_length < 0.0)
					{
						ll /= -dim_length;
					}
					else
					{
						ll /= dim_length;
					}

					float xd = (_xd) * ll;
					float yd = (_yd) * ll;
					float zd = (_zd) * ll;

					float initial = ox - floor(ox);

					if (d == 1)
					{
						initial = oy - floor(oy);
					}

					if (d == 2)
					{
						initial = oz - floor(oz);
					}

					if (dim_length > 0.0)
					{
						initial = 1.0 - initial;
					}

					float dist = ll * initial;

					float xp = ox + xd * initial;
					float yp = oy + yd * initial;
					float zp = oz + zd * initial;

					if (dim_length < 0.0)
					{
						if (d == 0)
						{
							xp--;
						}

						if (d == 1)
						{
							yp--;
						}

						if (d == 2)
						{
							zp--;
						}
					}

					// Basic raytracing loop.

					while (dist < closest)
					{
						int tex = v_map[(((int)zp & 63) << 12) | (((int)yp & 63) << 6) | ((int)xp & 63)];

						if (tex > 0)
						{
							// Get U, V coordinates of intersection point.

							int u = ((int)((xp + zp) * 16.0)) & 15;

							int v = ((int)(yp * 16.0) & 15) + 16;

							if (d == 1)
							{
								u = ((int)(xp * 16.0)) & 15;

								v = (((int)(zp * 16.0)) & 15);

								if (yd < 0)
								{
									v += 32;
								}
							}

							// Fetch the color of the intersection point.

							int cc = t_map[u + v * 16 + tex * 256 * 3];

							if (cc > 0)
							{
								col = cc;

								ddist = 255 - ((dist / 32 * 255));

								br = 255 * (255 - ((d + 2) % 3) * 50) / 255;

								// End the ray.

								closest = dist;
							}
						}

						// Increment the ray position.

						xp += xd;
						yp += yd;
						zp += zd;

						dist += ll;
					}
				}

				plot(x, y, rgb_mul(col, fbb_mul(br, ddist)));
			}
		}
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	make_textures();

	make_voxels();

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