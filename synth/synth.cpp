// Sound machines? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

double _Frequency = 441.0;

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

inline double fosc(double _Time, double _Hertz, double _Vibrato_Amp, double _Vibrato_Hertz, osc _Type)
{
	double _Freq = ftoa(_Hertz) * _Time + _Vibrato_Amp * _Hertz * sin(ftoa(_Vibrato_Hertz) * _Time);

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

struct note
{
	// The frequency of the note.

	double n_Frequency;

	// The amplitude of the note.

	double n_Amplitude;

	// The start time of the note.

	double n_Started;

	// A boolean to indicate whether the note is still producing audible sound.

	bool n_Active = true;

	// Default constructor.

	note(double _n_Frequency, double _n_Amplitude, double _n_Started)
	{
		n_Frequency = _n_Frequency;
		n_Amplitude = _n_Amplitude;
		n_Started = _n_Started;
	}

	// Generate the sound wave.

	inline double sound(double _Time)
	{
		if (_Time > n_Started + 1.0)
		{
			n_Active = false;

			return 0.0;
		}
		else
		{
			return fosc(_Time - n_Started, n_Frequency, 0.3, 5.0, osc_sine) * (1.0 - (_Time - n_Started) / 1.0);
		}
	}
};

std::vector<note> _Notes;

double sound_callback(double _Time)
{
	double _Sum = 0.0;

	for (int i = 0; i < _Notes.size(); i++)
	{
		_Sum += _Notes[i].sound(_Time);
	}

	return _Sum;
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

		_Notes.push_back(note(_Frequency, 1.0, 0.0));
	}

	void keydown(SDL_Event e) override
	{
		// Get the current time in seconds. This can be done by dividing the current sample number
		// by the samples per second measurement.

		double _Time = (double)__BOIL_AUDIO_SAMPLE__ / (double)_Audio_Samples;

		// Keys will add notes.

		for (int i = 0; i < 9; i++)
		{
			if (e.key.keysym.sym == i + 49)
			{
				/*

				for (int i = 0; i < _Notes.size(); i++)
				{
					if 
					(
						_Notes[i].n_Frequency > (i + 1) * 100.0 - 0.1 &&
						_Notes[i].n_Frequency < (i + 1) * 100.0 + 0.1
					)
					{
						_Notes[i].n_Started = _Time;

						return;
					}
				}

				*/

				_Notes.push_back(note((i + 1) * 1000.0, 1.0, _Time));

				return;
			}
		}
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Remove inaudible notes.

		auto _Iter = _Notes.begin();

		while (_Iter != _Notes.end())
		{
			if (_Iter->n_Active == false)
			{
				_Iter = _Notes.erase(_Iter);
			}
			else
			{
				_Iter++;
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