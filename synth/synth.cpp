// Sound machines? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

double _Frequency = 882.0;

inline double ftoa(double _Frequency)
{
	return _Frequency * 2.0 * M_PI;
}

enum osc
{
	osc_sine,
	osc_square,
	osc_triangle,
	osc_sawtooth,
	osc_noise
};

inline double fosc(double _Time, double _Hertz, osc _Type)
{
	double _Freq = ftoa(_Hertz) * _Time;

	if (_Type == osc_sine)
	{
		return sin(_Freq);
	}
	else if (_Type == osc_square)
	{
		return sin(_Freq) > 0.0 ? 1.0 : -1.0;
	}
	else if (_Type == osc_triangle)
	{
		return asin(sin(_Freq)) * (2.0 / M_PI);
	}
	else if (_Type == osc_sawtooth)
	{
		return (2.0 / M_PI) * (_Hertz * M_PI * fmod(_Time, 1.0 / _Hertz) - (M_PI / 2.0));
	}
	else if (_Type == osc_noise)
	{
		return ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
	}

	return 0.0;
}

double sound_callback(double _Time)
{
	return fosc(_Time, _Frequency, osc_sawtooth);
}

struct game: boiler
{	
	void steam() override
	{
		width = 800;
		height = 600;

		title = "Sound machine (using Boiler)";

		f_No_Debug = SDL_TRUE;

		_Audio_Samples = 44100;
		_Audio_Buffers = 2048;

		_Audio_Callback = sound_callback;
	}

	void draw() override
	{
		const Uint8* _Key = SDL_GetKeyboardState(NULL);

		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Get the current time in seconds. This can be done by dividing the current sample number
		// by the samples per second measurement.

		double _Time = (double)__BOIL_AUDIO_SAMPLE__ / (double)_Audio_Samples;

		// Keys will modify the frequency of the wave.

		for (int i = 0; i < 9; i++)
		{
			if (_Key[SDL_GetScancodeFromKey(i + 49)] == true)
			{
				_Frequency = (i + 1) * 100.0;
			}
		}
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