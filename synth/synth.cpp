// Sound machines? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

double _Frequency = 3200.0;

inline double ftoa(double _Frequency)
{
	return _Frequency * 2.0 * M_PI;
}

inline double fosc(double _Time, double _Hertz, int _Type)
{
	double _Freq = ftoa(_Hertz) * _Time;

	if (_Type == 0)
	{
		return sin(_Freq);
	}
	else if (_Type == 1)
	{
		return sin(_Freq) > 0.0 ? 1.0 : -1.0;
	}
	else if (_Type == 2)
	{
		return asin(sin(_Freq)) * (2.0 / M_PI);
	}
	else if (_Type == 3)
	{
		return (2.0 / M_PI) * (_Hertz * M_PI * fmod(_Time, 1.0 / _Hertz) - (M_PI / 2.0));
	}

	return 0.0;
}

double sound_callback(double _Time)
{
	return fosc(_Time, _Frequency, 441);
}

struct game: boiler
{	
	void steam() override
	{
		width = 800;
		height = 600;

		title = "Sound machine (using Boiler)";

		_Audio_Samples = 44100;
		_Audio_Buffers = 2048;

		_Audio_Callback = sound_callback;
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	game demo;

	if (demo.make(BOIL_EX_INIT_AUDIO) != 0)
	{
		std::cout << "Could not initialize Boiler." << std::endl;

		return 1;
	}

	demo.engine();
	demo.sweep();

	return 0;
}