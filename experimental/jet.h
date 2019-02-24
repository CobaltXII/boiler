/*

Implementation of the MATLAB Jet color palette.

*/

double ___JET_clamp(double x, double min, double max)
{
	if (x < min)
	{
		return min;
	}
	else if (x > max)
	{
		return max;
	}

	return x;
}

double ___JET_min(double x, double y)
{
	if (x < y)
	{
		return x;
	}

	return y;
}

unsigned int* ___JET_jet_colormap()
{
	unsigned int* jet = new unsigned int[256];

	for (int i = 0; i < 256; i++)
	{
		double x = i / 255.0;

		unsigned char r = ___JET_clamp(___JET_min(4.0 * x - 1.5, -4.0 * x + 4.5), 0.0, 1.0) * 255;
		unsigned char g = ___JET_clamp(___JET_min(4.0 * x - 0.5, -4.0 * x + 3.5), 0.0, 1.0) * 255;
		unsigned char b = ___JET_clamp(___JET_min(4.0 * x + 0.5, -4.0 * x + 2.5), 0.0, 1.0) * 255;

		jet[i] = r << 16 | g << 8 | b;
	}

	return jet;
}

unsigned int* jet_colormap = ___JET_jet_colormap();

template <typename T>

inline unsigned int sample_jet(T value)
{
	if (value < T(0.0f))
	{
		value = T(0.0f);
	}

	if (value > T(255.0f))
	{
		value = T(255.0f);
	}

	return jet_colormap[int(value)];
}