// The matrix? Awesome.

#include "../terminal/terminal_codepage.h"
#include "../terminal/terminal_extras.h"

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

// Green palette.

unsigned char pal_1[3] =
{
	0xA, 0x2, 0x8
};

// Blue palette.

unsigned char pal_2[3] =
{
	0x9, 0x1, 0x8
};

// Cyan palette.

unsigned char pal_3[3] =
{
	0xB, 0x3, 0x8
};

struct matrix_t
{
	// The position of the matrix entity.

	double px;
	double py;

	// The velocity of the matrix entity.

	double vy;
	double vx;

	// The character code of the matrix entity.

	double ascii;
};

struct game: boiler
{	
	std::vector<matrix_t> _Ents;

	unsigned char* pal_x = pal_3;

	void steam() override
	{
		srand((unsigned int)time(NULL));

		width = 801;
		height = 608;

		title = "The matrix (using Boiler)";

		f_No_Debug = SDL_TRUE;

		// Initialize the matrix.

		for (int i = 0; i < 400; i++)
		{
			matrix_t _ent;

			_ent.px = rand() % (width / tf_w);
			_ent.py = rand() % (height / tf_h);

			_ent.vy = ((double)((rand() + (RAND_MAX / 10)) % RAND_MAX) / (double)RAND_MAX);

			// Initialize the ASCII code.

			_ent.ascii = rand() % 256;

			_Ents.push_back(_ent);
		}
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		for (int i = 0; i < _Ents.size(); i++)
		{
			matrix_t& _ent = _Ents[i];

			_ent.px += _ent.vx;
			_ent.py += _ent.vy;

			_ent.ascii += _ent.vy;

			int rx = ((int)_ent.px % (width / tf_w)) * tf_w;
			int ry = ((int)_ent.py % (height / tf_h)) * tf_h;

			fontvga(std::string(1, (unsigned char)_ent.ascii), rx, ry, pal_x[0], 0x0, align_top_left);

			ry = ((int)(_ent.py - 1) % (height / tf_h)) * tf_h;

			fontvga(std::string(1, (unsigned char)_ent.ascii), rx, ry, pal_x[1], 0x0, align_top_left);

			ry = ((int)(_ent.py - 2) % (height / tf_h)) * tf_h;

			fontvga(std::string(1, (unsigned char)_ent.ascii), rx, ry, pal_x[2], 0x0, align_top_left);
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