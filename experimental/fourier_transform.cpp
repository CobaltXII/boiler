// Fourier transform? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <sstream>
#include <iostream>

struct fourier_t
{
	float re;
	float im;

	float freq;

	float amp;

	float phase;
};

// Discrete Fourier transform.

std::vector<fourier_t> discrete_fourier_transform(std::vector<float> x)
{
	float N = x.size();

	std::vector<fourier_t> X(N);

	for (float k = 0.0f; k < N; k++)
	{
		float re = 0.0f;
		float im = 0.0f;

		for (float n = 0.0f; n < N; n++)
		{
			float phi = (M_PI * 2.0f * k * n) / N;

			re = re + x[int(n)] * cos(phi);
			im = im - x[int(n)] * sin(phi);
		}

		re = re / N;
		im = im / N;

		X[int(k)] = {re, im, k, sqrt(re * re + im * im), atan2(im, re)};
	}

	return X;
}

struct game: boiler
{	
	float cx = 0.0f;
	float cy = 0.0f;

	float vx = 0.0f;

	float cr;

	std::vector<float> line;

	std::vector<float> line2x;
	std::vector<float> line2y;

	std::vector<fourier_t> fourier;

	void steam() override
	{
		width = 1400;
		
		height = 400;

		title = "Fourier transform (using Boiler)";

		cx = 364.0f;
		cy = 200.0f;

		cr = 96.0f;

		std::vector<float> input;

		for (int i = 0; i < 256; i++)
		{
			input.push_back(((i % 128) / 64.0f - 1.0f) * 96.0f);
		}

		fourier = discrete_fourier_transform(input);
	}

	void draw() override
	{
		black();

		// Do velocity.

		const Uint8* keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_A])
		{
			vx -= 1.0f;
		}
		else if (keys[SDL_SCANCODE_D])
		{
			vx += 1.0f;
		}

		cx += vx *= 0.9f;

		// Get the time.

		float t = iteration * (2.0f * M_PI) / float(fourier.size());

		// Draw a line at the origin.

		linergb(cx, 0, cx, height, rgb(127, 127, 127));

		// Draw the fractal spirograph.

		float x = cx;
		float y = cy;

		for (int i = 0; i < fourier.size(); i++)
		{
			float px = x;
			float py = y;

    		float radius = fourier[i].amp;

    		x += radius * cos(fourier[i].freq * t + fourier[i].phase + M_PI / 2.0f);
    		y += radius * sin(fourier[i].freq * t + fourier[i].phase + M_PI / 2.0f);

    		circlergb(px, py, radius, rgb(127, 127, 127));

    		linergb(px, py, x, y, rgb(255, 255, 255));
		}

		line.push_back(y);

		// Add the position of the 'head' to line2x and line2y. Do this many 
		// times to create a smooth line.

		line2x.push_back(x - cx);
		line2y.push_back(y - cy);

		// Draw a line connecting the 'head' of the fractal spirograph to the
		// start of line, making it look like the 'head' is 'writing'.

		linergb(x, y, cx + cr + 256, y, rgb(255, 255, 255));

		// Draw line (the Y coordinate of the 'head' over time).

		for (int i = 1; i < line.size(); i++)
		{
			linergb(cx + cr + 256 + i, line[line.size() - i], cx + cr + 256 + i + 1, line[line.size() - i - 1], rgb(255, 255, 255));
		}

		if (line.size() > fourier.size() * 16)
		{
			line.erase(line.begin());
		}

		// Draw line2x and line2y (the path of the 'head' over time).

		for (int i = 0; i < line2x.size() - 1; i++)
		{
			linergb(line2x[i] + cx, line2y[i] + cy, line2x[i + 1] + cx, line2y[i + 1] + cy, rgb(255, 0, 0));
		}

		if (line2x.size() > fourier.size() * 16)
		{
			line2x.erase(line2x.begin());
			line2y.erase(line2y.begin());
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