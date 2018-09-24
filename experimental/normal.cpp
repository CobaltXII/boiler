// Normal mapping?

#include "../boiler/boiler.h"

// Load image function.

Uint32* loadbmprgb(std::string path, int &w, int &h)
{
	SDL_Surface* s_bitmap = SDL_LoadBMP(path.c_str());

	if (s_bitmap == NULL)
	{
		std::cout << "Could not load image." << std::endl;

		w = 0;
		h = 0;

		return NULL;
	}

	w = s_bitmap->w;
	h = s_bitmap->h;

	int bpp = s_bitmap->format->BytesPerPixel;

	if 
	(
		bpp != 4 &&
		bpp != 3
	)
	{
		std::cout << "Only 32-bit and 24-bit bitmaps are supported." << std::endl;

		w = 0;
		h = 0;

		return NULL;
	}

	std::cout << bpp << " bytes per pixel" << std::endl;

	// Allocate space for the texture.

	Uint32* m_bmp = (Uint32*)malloc(s_bitmap->w * s_bitmap->h * sizeof(Uint32));

	// Loop through each pixel in the surface and apply it to the correct memory offset of the raw
	// texture (m_bmp).

	for (int x = 0; x < s_bitmap->w; x++)
	{
		for (int y = 0; y < s_bitmap->h; y++)
		{
			Uint8* p = (Uint8*)s_bitmap->pixels + y * s_bitmap->pitch + x * bpp;

			if (bpp == 3)
			{
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				{
					m_bmp[y * s_bitmap->w + x] = p[0] << 16 | p[1] << 8 | p[2];
				}
				else
				{
					m_bmp[y * s_bitmap->w + x] = p[0] | p[1] << 8 | p[2] << 16;
				}
			}
			else
			{
				m_bmp[y * s_bitmap->w + x] = *(Uint32*)p;
			}
		}
	}

	return m_bmp;
}

struct game: boiler
{	
	double lx = +0.0;
	double ly = +0.0;
	double lz = +2.0;

	double lir = 10000.0;
	double lig = 100.0;
	double lib = 1.0;

	double li = 10000.0;

	int m_img_w = 0;
	int m_img_h = 0;

	Uint32* m_img = loadbmprgb("normal_map_3.bmp", m_img_w, m_img_h);

	void steam() override
	{
		width = 640;
		height = 400;

		title = "Normal mapping (using Boiler)";
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		const Uint8* state = SDL_GetKeyboardState(NULL);

		// lx = mouse_x;
		// ly = mouse_y;

		lx = sin(iteration / 30.0) * m_img_w / 2.5 + (m_img_w / 2);
		ly = cos(iteration / 30.0) * m_img_h / 2.5 + (m_img_h / 2);

		lir = 10000.0;
		lig = 10000.0;
		lib = 10000.0;

		if (mouse_l)
		{
			lz -= 0.5;
		}
		else if (mouse_r)
		{
			lz += 0.5;
		}

		for (int x = 0; x < m_img_w; x++)
		{
			for (int y = 0; y < m_img_h; y++)
			{
				Uint32 c = m_img[y * m_img_w + (x)];
				
				// Get the color values from the color.

				Uint8 r = c >> 16;
				Uint8 g = c >> 8;
				Uint8 b = c >> 0;

				// Turn the color values into a vector.

				double nx = ((r / 255.0) - 0.5) * 2;
				double ny = ((g / 255.0) - 0.5) * 2;
				double nz = ((b / 255.0) - 0.5) * 2;

				// Find the direction to the light.

				double dx = (lx - x);
				double dy = (ly - y);

				double dz = (lz - 0.0);

				double dq = sqrt
				(
					dx * dx +
					dy * dy +
					dz * dz
				);

				dx = dx / dq;
				dy = dy / dq;
				dz = dz / dq;

				double lx1 = m_img_w - lx;
				double ly1 = m_img_h - ly;

				double lz1 = lz;

				double dx1 = (lx1 - x);
				double dy1 = (ly1 - y);

				double dz1 = (lz1 - 0.0);

				double dq1 = sqrt
				(
					dx1 * dx1 +
					dy1 * dy1 +
					dz1 * dz1
				);

				dx1 = dx1 / dq1;
				dy1 = dy1 / dq1;
				dz1 = dz1 / dq1;

				// Compare to normal.

				double d =
				(
					dx * nx +
					dy * ny +
					dz * nz
				);

				double d1 =
				(
					dx1 * nx +
					dy1 * ny +
					dz1 * nz
				);

				// Modulate using distance and intensity.

				double lu =
				(
					(x - lx) * (x - lx) +
					(y - ly) * (y - ly) +

					(lz * lz)
				);

				d = d / lu;

				double lu1 =
				(
					(x - lx1) * (x - lx1) +
					(y - ly1) * (y - ly1) +

					(lz1 * lz1)
				);

				d1 = d1 / lu1;

				// For each color.

				double ir = d * lir;
				double ig = d * lig;
				double ib = d * lib;

				double ir1 = d1 * lib;
				double ig1 = d1 * lig;
				double ib1 = d1 * lir;

				ir += ir1;
				ig += ig1;
				ib += ib1;

				Uint32 t = m_img[y * m_img_w + (x)];

				Uint8 r1 = t >> 16;
				Uint8 g1 = t >> 8;
				Uint8 b1 = t >> 0;

				double aar = r1 / 255.0;
				double aag = g1 / 255.0;
				double aab = b1 / 255.0;

				if (true)
				{
					ir = aar * ir;
					ig = aag * ig;
					ib = aab * ib;
				}

				// Place as intensity.

				Uint8 cr = std::max(0.0, std::min(255.0, ir * 255.0));
				Uint8 cg = std::max(0.0, std::min(255.0, ig * 255.0));
				Uint8 cb = std::max(0.0, std::min(255.0, ib * 255.0));

				pixels[(y + (height / 2) - (m_img_h / 2)) * width + (x + (width / 2) - (m_img_w / 2))] = rgb
				(
					cr,
					cg,
					cb
				);
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

	free(demo.m_img);

	return 0;
}