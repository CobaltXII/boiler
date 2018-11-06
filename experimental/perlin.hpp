/*

Perlin noise.

Perlin noise is like black magic. I don't know why it works, but I do know its concepts. However,
I won't try to explain about gradients, interpolation or ease curves here. Just be aware that this
is a fully functional Perlin noise library.

I took most (if not all) of the code from this awesome repository

	https://github.com/Reputeless/PerlinNoise

I removed most of the modern C++ garbage and made it a bit more portable. It's also named in a
more eye-friendly convention (for my eyes, at least).

*/

#include <algorithm>
#include <random>

struct perlin
{
	// Permutation.

	int p[512];

	// Ease curve or fade function.

	static double fade(double t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	// Interpolate.

	static double lerp(double t, double a, double b)
	{
		return a + t * (b - a);
	}

	// gradient function.

	static double grad(int hash, double x, double y, double z)
	{
		int h = hash & 15;

		double u;
		double v;

		if (h < 8)
		{
			u = x;
		}
		else
		{
			u = y;
		}

		if (h < 4)
		{
			v = y;
		}
		else
		{
			if (h == 12 || h == 14)
			{
				v = x;
			}
			else
			{
				v = z;
			}
		}

		double a;
		double b;

		if ((h & 1) == 0)
		{
			a = u;
		}
		else
		{
			a = -u;
		}

		if ((h & 2) == 0)
		{
			b = v;
		}
		else
		{
			b = -v;
		}

		return a + b;
	}

	// Three-dimensional noise.

	double noise(double x, double y, double z)
	{
		int X = int(std::floor(x)) & 255;
		int Y = int(std::floor(y)) & 255;
		int Z = int(std::floor(z)) & 255;

		x -= std::floor(x);
		y -= std::floor(y);
		z -= std::floor(z);

		double u = fade(x);
		double v = fade(y);
		double w = fade(z);

		int A = p[X] + Y;

		int B = p[X + 1] + Y;

		int AA = p[A] + Z;
		int BA = p[B] + Z;

		int AB = p[A + 1] + Z;
		int BB = p[B + 1] + Z;
		
		return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)), lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))), lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)), lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}

	// Two-dimensional noise.

	double noise(double x, double y)
	{
		return noise(x, y, 0.0);
	}

	// One-dimensional noise.

	double noise(double x)
	{
		return noise(x, 0.0, 0.0);
	}

	// Three-dimensional octave noise.

	double octave_noise(double x, double y, double z, int octaves)
	{
		double result = 0.0;

		double amp = 1.0;

		for (int i = 0; i < octaves; i++)
		{
			result += noise(x, y, z) * amp;

			x *= 2.0;
			y *= 2.0;
			z *= 2.0;
			
			amp *= 0.5;
		}

		return result;
	}

	// Two-dimensional octave noise.

	double octave_noise(double x, double y, int octaves)
	{
		double result = 0.0;

		double amp = 1.0;

		for (int i = 0; i < octaves; i++)
		{
			result += noise(x, y) * amp;

			x *= 2.0;
			y *= 2.0;

			amp *= 0.5;
		}

		return result;
	}

	// One-dimensional octave noise.

	double octave_noise(double x, int octaves)
	{
		double result = 0.0;

		double amp = 1.0;

		for (int i = 0; i < octaves; i++)
		{
			result += noise(x) * amp;

			x *= 2.0;

			amp *= 0.5;
		}

		return result;
	}

	// Scaled one-dimensional noise.

	double noise_0_1(double x)
	{
		return noise(x) * 0.5 + 0.5;
	}

	// Scaled two-dimensional noise.

	double noise_0_1(double x, double y)
	{
		return noise(x, y) * 0.5 + 0.5;
	}

	// Scaled three-dimensional noise.

	double noise_0_1(double x, double y, double z)
	{
		return noise(x, y, z) * 0.5 + 0.5;
	}

	// Scaled one-dimensional octave noise.

	double octave_noise_0_1(double x, int octaves)
	{
		return octave_noise(x, octaves) * 0.5 + 0.5;
	}

	// Scaled two-dimensional octave noise.

	double octave_noise_0_1(double x, double y, int octaves)
	{
		return octave_noise(x, y, octaves) * 0.5 + 0.5;
	}

	// Scaled three-dimensional octave noise.

	double octave_noise_0_1(double x, double y, double z, int octaves)
	{
		return octave_noise(x, y, z, octaves) * 0.5 + 0.5;
	}

	// Reseed permutations.

	void reseed(int seed)
	{
		for (unsigned int i = 0; i < 256; i++)
		{
			p[i] = i;
		}

		std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

		for (unsigned int i = 0; i < 256; i++)
		{
			p[256 + i] = p[i];
		}
	}

	// Constructor.

	perlin(int seed = std::default_random_engine::default_seed)
	{
		reseed(seed);
	}
};