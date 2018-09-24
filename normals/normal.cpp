// Normal mapping?

#include "../boiler/boiler.h"

#include <vector>
#include <iostream>

// A point light.

struct light
{
	// The origin of the light.

	double x;
	double y;
	double z;

	// The three intensities of the light, corresponding to the R, G and B color components of the
	// resulting color. Can be any value, no need to clamp or normalize.

	double ir;
	double ig;
	double ib;
};

// A normal.

struct normal
{
	double x;
	double y;
	double z;
};

// Convert a normal map into a normal.

normal* normalmap(Uint32* pixels, int w, int h)
{
	if (w == 0 || h == 0)
	{
		return NULL;
	}

	// Allocate a buffer large enough to contain the output normals. This buffer should contain
	// w * h elements.

	normal* n_out = (normal*)malloc(w * h * sizeof(normal));

	if (n_out == NULL)
	{
		return NULL;
	}

	// Loop through all the pixels in the image.

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			normal n_elt;

			// Fetch the color components of the image.

			Uint32 c = pixels[y * w + x];

			Uint8 r = c >> 16;
			Uint8 g = c >> 8;
			Uint8 b = c >> 0;

			// Scale them into normal space.

			n_elt.x = ((r / 255.0) - 0.5) * 2;
			n_elt.y = ((g / 255.0) - 0.5) * 2;
			n_elt.z = ((b / 255.0) - 0.5) * 2;

			// Assign the new normal to the normal array.

			n_out[y * w + x] = n_elt;
		}
	}

	// Return the normal map.

	return n_out;
}

struct game: boiler
{	
	int m_img_w = 0;
	int m_img_h = 0;

	int off_x = 0;
	int off_y = 0;

	Uint32* m_texture = NULL;

	// The normal map.

	normal* m_normal = NULL;

	// The lights in the scene.

	std::vector<light> lights;

	// Initialize the program, using the Boiler framework.

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Normal mapping (using Boiler)";

		// Load the normal map from a file, and convert it into a normal map consisting of
		// precalculated normals.

		int w = 0;
		int h = 0;

		Uint32* m_img_map = loadbmp("maps/normal_map_3.bmp", w, h);

		if (w == 0 || h == 0)
		{
			if (m_img_map != NULL)
			{
				free(m_img_map);
			}

			nuke();
		}

		normal* m_normal_map = normalmap
		(
			m_img_map,

			w, h
		);

		if (m_normal_map == NULL)
		{
			nuke();
		}

		m_img_w = w;
		m_img_h = h;

		m_texture = m_img_map;
		m_normal = m_normal_map;

		// Add two lights to the scene. These lights will be extremely bright white, and will be
		// positioned at the default position.

		for (int i = 0; i < 2; i++)
		{
			light l_elt;

			l_elt.ir = 10000.0;
			l_elt.ig = 10000.0;
			l_elt.ib = 10000.0;

			l_elt.z = 8.0;

			lights.push_back(l_elt);
		}

		// Get the offset needed to center the render, so that it doesn't need to be recalculated
		// every frame. Small optimization.

		off_x = (width / 2) - (m_img_w / 2);
		off_y = (height / 2) - (m_img_h / 2);
	}

	// Draw a frame, using the Boiler framework.

	void draw() override
	{
		// Clear the screen to black.

		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Spin the lights around.

		lights[0].x = sin(iteration / 30.0) * m_img_w / 2.5 + (m_img_w / 2);
		lights[0].y = cos(iteration / 30.0) * m_img_h / 2.5 + (m_img_h / 2);

		lights[1].x = m_img_w - lights[0].x;
		lights[1].y = m_img_h - lights[0].y;

		// Alter the lights' positions if the mouse is down.

		if (mouse_l)
		{
			lights[0].z -= 0.5;
			lights[1].z -= 0.5;
		}
		else if (mouse_r)
		{
			lights[0].z += 0.5;
			lights[1].z += 0.5;
		}

		// Calculate lighting for each pixel in the normal map, and plot the correct color to the
		// output pixel array.

		for (int x = 0; x < m_img_w; x++)
		{
			for (int y = 0; y < m_img_h; y++)
			{
				// Get the color of the pixel from the texture.

				Uint32 pc = m_texture[y * m_img_w + x];

				Uint8 pr = pc >> 16;
				Uint8 pg = pc >> 8;
				Uint8 pb = pc >> 0;

				double prn = pr / 255.0;
				double pgn = pg / 255.0;
				double pbn = pb / 255.0;

				// Get the normal of the current pixel.

				normal pn = m_normal[y * m_img_w + x];

				double nx = pn.x;
				double ny = pn.y;
				double nz = pn.z;

				// Create empty intensity values for each color component.

				double ir = 0.0;
				double ig = 0.0;
				double ib = 0.0;

				// Do calculations for each light in the scene.

				for (int i = 0; i < lights.size(); i++)
				{
					light& l_i = lights[i];

					// Find the direction to the light.

					double dx = (l_i.x - x);
					double dy = (l_i.y - y);

					double dz = (l_i.z - 0.0);

					double dq = sqrt
					(
						dx * dx +
						dy * dy +
						dz * dz
					);

					dx = dx / dq;
					dy = dy / dq;
					dz = dz / dq;

					// Compare the light normal to the pixel normal using the dot product of the
					// two unit vectors.

					double d =
					(
						dx * nx +
						dy * ny +
						dz * nz
					);

					// Calculate the intensities for each color component using the distance to
					// the light and the intensities associated with the light.

					double lu =
					(
						(x - l_i.x) * (x - l_i.x) +
						(y - l_i.y) * (y - l_i.y) +

						(l_i.z * l_i.z)
					);

					d = d / lu;

					ir += d * l_i.ir;
					ig += d * l_i.ig;
					ib += d * l_i.ib;
				}

				// Multiply the color intensities by the color components of the texture of the
				// current pixel; this gives the illusion of reflection.

				ir = ir * prn;
				ig = ig * pgn;
				ib = ib * pbn;

				// We've got color intensities, now we need to scale them into color space. Make
				// sure to clamp them so that the colors don't glitch.

				Uint8 cr = std::max(0.0, std::min(255.0, ir * 255.0));
				Uint8 cg = std::max(0.0, std::min(255.0, ig * 255.0));
				Uint8 cb = std::max(0.0, std::min(255.0, ib * 255.0));

				pixels[(y + off_y) * width + (x + off_x)] = rgb(cr, cg, cb);
			}
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

	free(demo.m_texture);
	free(demo.m_normal);

	return 0;
}