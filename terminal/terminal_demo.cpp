// Code Page 437? Awesome.

#include "terminal_codepage.h"
#include "terminal_extras.h"

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct game: boiler
{	
	std::string _Ostr;

	void steam() override
	{
		width = 801;
		height = 608;

		title = "Code Page 437 (using Boiler)";

		_Ostr += "Nearly ten years had passed since the Dursleys had woken up to find their nephew on the front step, but Privet Drive had hardly changed at all. The sun rose on the same tidy front gardens and lit up the brass number four on the Dursleys' front door; it crept into their living room, which was almost exactly the same as it had been on the night when Mr. Dursley had seen that fateful news report about the owls. Only the photographs on the mantelpiece really showed how much time had passed. Ten years ago, there had been lots of pictures of what looked like a large pink beach ball wearing different-colored bonnets - but Dudley Dursley was no longer a baby, and now the photographs showed a large blond boy riding his first bicycle, on a carousel at the fair, playing a computer game with his father, being hugged and kissed by his mother. The room held no sign at all that another boy lived in the house, too.";

		// Haha, this is what we do to add lines. :P.

		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');
		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');

		_Ostr += "Yet Harry Potter was still there, asleep at the moment, but not for long. His Aunt Petunia was awake and it was her shrill voice that made the first noise of the day.";

		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');
		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');

		_Ostr += "Up! Get up! Now!";

		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');
		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');

		_Ostr += "Harry woke with a start. His aunt rapped on the door again.";

		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');
		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');

		_Ostr += "\"Up!\" she screeched. Harry heard her walking toward the kitchen and then the sound of the frying pan being put on the stove. He rolled onto his back and tried to remember the dream he had been having. It had been a good one. There had been a flying motorcycle in it. He had a funny feeling he'd had the same dream before.";
	
		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');
		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');

		_Ostr += "His aunt was back outside the door.";

		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');
		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');

		_Ostr += "\"Are you up yet?\" she demanded.";

		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');
		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');

		_Ostr += "\"Nearly,\" said Harry.";

		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');
		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');

		_Ostr += "\"Well, get a move on, I want you to look after the bacon. And don't you dare let it burn, I want everything perfect on Duddy's birthday.\"";

		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');
		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');

		_Ostr += "Harry groaned.";

		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');
		_Ostr += std::string((width / tf_w) - (_Ostr.size() % (width / tf_w)), ' ');

		_Ostr += "Excerpt from `Harry Potter and the Sorcerers Stone`. Written by J.K. (Joanne) Rowling.";
	}

	void keydown(SDL_Event e) override
	{
		if (e.key.keysym.sym >= 0x61 && e.key.keysym.sym <= 0x7A)
		{
			if ((SDL_GetModState() & KMOD_CAPS) == KMOD_CAPS)
			{
				_Ostr.push_back((unsigned char)(e.key.keysym.sym - 32));
			}
			else
			{
				_Ostr.push_back((unsigned char)(e.key.keysym.sym));
			}
		}
		else if (e.key.keysym.sym >= 0x30 && e.key.keysym.sym <= 0x39)
		{
			_Ostr.push_back((unsigned char)(e.key.keysym.sym));
		}
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		std::string _Buf = _Ostr;

		std::vector<std::string> _Bufl;

		while (_Buf.size() > width / tf_w)
		{
			_Bufl.push_back(_Buf.substr(0, width / tf_w));

			_Buf.erase(0, width / tf_w);
		}

		_Bufl.push_back(_Buf);

		for (int i = 0; i < _Bufl.size() && i < height / tf_h; i++)
		{
			fontrgb(_Bufl[i], 0, i * tf_h, rgb(192, 192, 192), rgb(0, 0, 0), align_top_left);
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