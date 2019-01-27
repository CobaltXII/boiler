// Fourier transform? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <sstream>
#include <iostream>

#include "the_coding_train.h"

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
	float cx1 = 0.0f;
	float cy1 = 0.0f;

	float cx2 = 0.0f;
	float cy2 = 0.0f;

	std::vector<float> linex;
	std::vector<float> liney;

	std::vector<fourier_t> fourier1;
	std::vector<fourier_t> fourier2;

	void steam() override
	{
		width = 800;
		
		height = 800;

		title = "Fourier transform (using Boiler)";

		cx1 = width / 2.0f + 64.0f;

		cy1 = 160.0f;

		cx2 = 160.0f;

		cy2 = height / 2.0f + 64.0f;

		std::vector<float> input1;
		std::vector<float> input2;

		for (int i = 0; i < the_coding_train.size(); i++)
		{
			input1.push_back(the_coding_train[i].x);
			input2.push_back(the_coding_train[i].y);
		}

		fourier1 = discrete_fourier_transform(input1);
		fourier2 = discrete_fourier_transform(input2);

		struct amp_sort_t
		{
			bool operator () 
			(
				fourier_t f1,
				fourier_t f2
			)
			{
				return f1.amp > f2.amp;
			}
		};

		amp_sort_t amp_sort;

		std::sort(fourier1.begin(), fourier1.end(), amp_sort);
		std::sort(fourier2.begin(), fourier2.end(), amp_sort);
	}

	void draw() override
	{
		black();

		// Get the time.

		float t = iteration * (2.0f * M_PI) / float(fourier1.size());

		// Draw a line through both fractal spirographs.

		linergb(cx2, 0, cx2, height, rgb(127, 127, 127));

		linergb(0, cy1, width, cy1, rgb(127, 127, 127));

		// Draw the 1st fractal spirograph.

		float x1 = cx1;
		float y1 = cy1;

		for (int i = 0; i < fourier1.size(); i++)
		{
			float px = x1;
			float py = y1;

    		float radius = fourier1[i].amp;

    		x1 += radius * cos(fourier1[i].freq * t + fourier1[i].phase);
    		y1 += radius * sin(fourier1[i].freq * t + fourier1[i].phase);

    		circlergb(px, py, radius, rgb(127, 127, 127));

    		linergb(px, py, x1, y1, rgb(255, 255, 255));
		}

		// Draw the 2nd fractal spirograph.

		float x2 = cx2;
		float y2 = cy2;

		for (int i = 0; i < fourier2.size(); i++)
		{
			float px = x2;
			float py = y2;

    		float radius = fourier2[i].amp;

    		x2 += radius * cos(fourier2[i].freq * t + fourier2[i].phase + M_PI / 2.0f);
    		y2 += radius * sin(fourier2[i].freq * t + fourier2[i].phase + M_PI / 2.0f);

    		circlergb(px, py, radius, rgb(127, 127, 127));

    		linergb(px, py, x2, y2, rgb(255, 255, 255));
		}

		// Draw a line connecting the 'heads' of both fractal spirographs to
		// the start of the path, making it look like the 'heads' are 
		// 'writing'.

		linergb(x1, y1, x1, y2, rgb(255, 255, 255));
		linergb(x2, y2, x1, y2, rgb(255, 255, 255));

		// Draw linex and liney.

		linex.push_back(x1);
		liney.push_back(y2);

		for (int i = 0; i < linex.size() - 1; i++)
		{
			linergb(linex[i], liney[i], linex[i + 1], liney[i + 1], rgb(255, 255, 255));
		}

		if (linex.size() > fourier1.size() * 16)
		{
			linex.erase(linex.begin());
			liney.erase(liney.begin());
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