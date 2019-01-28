// K-means clustering? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct point
{
	float x;
	float y;

	point()
	{
		x = 0.0f;
		y = 0.0f;
	}

	point(float _x, float _y)
	{
		x = _x;
		y = _y;
	}
};

std::vector<point> k_means(std::vector<point>& dataset, int k, int j)
{
	// 'k' cluster locations.

	float cx[k];
	float cy[k];

	// Start at random positions.

	for (int i = 0; i < k; i++)
	{
		cx[i] = rand_01();
		cy[i] = rand_01();
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

				float d = dx * dx + dy * dy;

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
			float avgx = 0.0f;
			float avgy = 0.0f;

			for (int n = 0; n < buckets[m].size(); n++)
			{
				avgx += buckets[m][n]->x;
				avgy += buckets[m][n]->y;
			}

			cx[m] = avgx / float(buckets[m].size());
			cy[m] = avgy / float(buckets[m].size());
		}
	}

	// Construct a vector of clusters to return.

	std::vector<point> clusters;

	for (int i = 0; i < k; i++)
	{
		point c;

		c.x = cx[i];
		c.y = cy[i];

		clusters.push_back(c);
	}

	return clusters;
}

struct game: boiler
{	
	std::vector<point> dataset;

	std::vector<point> clusters;

	void steam() override
	{
		width = 960;
		
		height = 960;

		title = "K-means clustering (using Boiler)";

		// Initialize the random number generator.

		srand(time(NULL));

		// Initialize a dataset.

		for (int i = 0; i < 16; i++)
		{
			float rx = rand_01();
			float ry = rand_01();

			for (int j = 0; j < 64; j++)
			{
				float r1 = rand_11();
				float r2 = rand_11();

				float px = rx + cos(r1 * 2.0f * M_PI) * r2 * 0.1f;
				float py = ry + sin(r1 * 2.0f * M_PI) * r2 * 0.1f;

				dataset.push_back(point(px, py));
			}
		}

		// Do k-means clustering.

		clusters = k_means(dataset, 16, 1024);
	}

	void draw() override
	{
		black();

		// Render the dataset.

		for (int i = 0; i < dataset.size(); i++)
		{
			circlergb(dataset[i].x * width, dataset[i].y * width, 3, rgb(255, 255, 255));
		}

		// Render the clusters.

		for (int i = 0; i < clusters.size(); i++)
		{
			circlergb(clusters[i].x * width, clusters[i].y * width, 5, hsl_to_rgb(i * (360 / clusters.size()), 1.0f, 0.5f));
		}

		// Render lines connecting the dataset and the clusters.

		for (int i = 0; i < dataset.size(); i++)
		{
			float max_dist = INFINITY;

			int index = -1;

			for (int j = 0; j < clusters.size(); j++)
			{
				float dx = dataset[i].x - clusters[j].x;
				float dy = dataset[i].y - clusters[j].y;

				float d = dx * dx + dy * dy;

				if (d < max_dist)
				{
					max_dist = d;

					index = j;
				}
			}

			linergb(dataset[i].x * width, dataset[i].y * width, clusters[index].x * width, clusters[index].y * width, hsl_to_rgb(index * (360 / clusters.size()), 1.0f, 0.5f));
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