// boiler.h
//
// This header file is a boilerplate header for making graphical projects using the SDL library. 
// I created this header to help me begin SDL projects quickly and efficiently, so that I don't
// need to write boilerplate code every time.
//
// This header is not meant for creating games in the traditional way of loading textures and
// drawing them directly to the screen. This header requires that you place each pixel yourself,
// in exchange for an extremely low render time.
//
// Finally, this header requires an object to be used for each project. This means that you will
// need to overload functions and create child classes, but that is very simple. A sample project
// is included with this header to show you how to use it. Thank you!
//
// This code is licensed under the GNU GPLv3 license. All rights reserved.

// Known bugs
//
// - Black screen on macOS Mojave
//
//		- Will be fixed with the release of SDL 2.0.9, for now just move the window

#ifdef BOIL_USE_STB_IMAGE

// You need to set this flag if you want to be able to use stb_image dependant functions such as
// loadimg. Not setting the flag greatly increases compile time.

#define STB_IMAGE_IMPLEMENTATION

// stb_image.h is a great, lightweight image loading API. Much better than linking large libraries
// such as SDL_image, or using difficult, massive APIs like libpng. Thanks to Sean T. Barrett and
// other contributors.

#include "stb/stb_image.hpp"

#endif

#ifndef BOIL_PRE_INCLUDE

#include <SDL2/SDL.h>

#endif

// On Windows, it is reported that one must define WinMain() instead of main(). This breaks
// compatibility on many sub-projects. For this reason, #define main WinMain. If you don't want
// that to happen, #define BOIL_NO_ENTRY_POINT_CONVERSIONS.

#ifndef BOIL_NO_ENTRY_POINT_CONVERSIONS

#ifdef _WIN32

#define main WinMain

#endif

#endif

Uint32 BOIL_EX_INIT_NOSUB = 1;
Uint32 BOIL_EX_INIT_AUDIO = 2;

#include <string>
#include <iostream>

Uint32 expected = 1000.0 / 60.0;

// Common includes.

#include <cmath>
#include <climits>

#include <memory>

#include <time.h>

// This helper function will generate a Uint32 value which represents the value of the provided
// color. The format of the Uint32 is SDL_PIXELFORMAT_ARGB8888.

inline Uint32 rgb(Uint8 r, Uint32 g, Uint32 b)
{
	return r << 16 | g << 8 | b;
}

inline Uint32 argb(Uint8 a, Uint8 r, Uint32 g, Uint32 b)
{
	return a << 24 | r << 16 | g << 8 | b;
}

// Macro derivatives.

#define mrgb(r, g, b) (Uint32)((Uint8)(r) << 16 | (Uint8)(g) << 8 | (Uint8)(b))

#define margb(a, r, g, b) (Uint32)((Uint8)(a) << 24 | (Uint8)(r) << 16 | (Uint8)(g) << 8 | (Uint8)(b))

// This function is internally used for converting for converting HSL to RGB.

inline double hue_to_rgb(double v1, double v2, double vh)
{
	if (vh < 0.0)
	{
		vh += 1.0;
	}

	if (vh > 1.0)
	{
		vh -= 1.0;
	}

	if ((6.0 * vh) < 1.0)
	{
		return (v1 + (v2 - v1) * 6.0 * vh);
	}

	if ((2.0 * vh) < 1.0)
	{
		return v2;
	}

	if ((3.0 * vh) < 2.0)
	{
		return (v1 + (v2 - v1) * ((2.0 / 3.0) - vh) * 6);
	}

	return v1;
}

// This function will convert HSL to a compiled RGB color.

inline unsigned int hsl_to_rgb(int h, double s, double l)
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;

	if (s == 0)
	{
		r = (unsigned char)(l * 255);
		g = (unsigned char)(l * 255);
		b = (unsigned char)(l * 255);
	}
	else
	{
		double v1;
		double v2;

		double hue = h / 360.0;

		if (l < 0.5)
		{
			v2 = l * (1.0 + s);
		}
		else
		{
			v2 = (l + s) - (l * s);
		}

		v1 = 2.0 * l - v2;

		r = (unsigned char)(255 * hue_to_rgb(v1, v2, hue + (1.0 / 3.0)));

		g = (unsigned char)(255 * hue_to_rgb(v1, v2, hue));

		b = (unsigned char)(255 * hue_to_rgb(v1, v2, hue - (1.0 / 3.0)));
	}

	return rgb(r, g, b);
}

// Convert degrees to radians.

inline double degrad(double x)
{
	return 2.0 * M_PI * (x / 360.0);
}

// Macro derivative.

#define mdegrad(x) (double)(2 * M_PI * ((double)(x) / 360.0))

// Clamp a value to the range 0 to 255.

inline double clamprgb(double x)
{
	return std::max(0.0, std::min(255.0, x));
}

// Macro derivative.

#define mclamprgb(x) (double)(std::max(0.0, std::min(255.0, (double)(x))))

// Get the A, R, G and B components of a Uint32.

inline Uint8 geta(Uint32 argb)
{
	return argb >> 24;
}

inline Uint8 getr(Uint32 argb)
{
	return argb >> 16;
}

inline Uint8 getg(Uint32 argb)
{
	return argb >> 8;
}

inline Uint8 getb(Uint32 argb)
{
	return argb;
}

// Macro derivatives.

#define mgeta(argb) (Uint8)((Uint32)(argb) >> 24)

#define mgetr(argb) (Uint8)((Uint32)(argb) >> 16)

#define mgetg(argb) (Uint8)((Uint32)(argb) >> 8)

#define mgetb(argb) (Uint8)((Uint32)(argb))

// Don't overload! Used as a 'middleman' between _Audio_Callback and SDL.

double(*__BOIL_USER_CALLBACK__)(double);

int __BOIL_HALF_SHORT__ = pow(2, 16 - 1) - 1;

int __BOIL_AUDIO_SAMPLE__ = 0;

void __BOIL_AUDIO_CALLBACK__(void* _User, Uint8* _Buffer, int _Bytes)
{
	Sint16* _New_Buffer = (Sint16*)_Buffer;

	int _Length = _Bytes / 2;

	int& _Sample_Number = (*(int*)_User);

	for (int i = 0; i < _Length; i++, _Sample_Number++)
	{
		double _Time = (double)_Sample_Number / 44100;

		double _User_Data = __BOIL_USER_CALLBACK__(_Time);

		if (_User_Data > 1.0)
		{
			_User_Data = 1.0;
		}
		else if (_User_Data < -1.0)
		{
			_User_Data = -1.0;
		}

		_New_Buffer[i] = (Sint16)(_User_Data * __BOIL_HALF_SHORT__);
	}
}

// This structure contains all the code needed to initialize, use, and destroy an instance of a
// boiler.

struct boiler
{
	SDL_Window* window;
	SDL_Texture* texture;
	SDL_Renderer* renderer;

	Uint32* pixels;

	Sint32 width;
	Sint32 height;

	Sint32 h_width;
	Sint32 h_height;

	std::string title;

	SDL_bool running = SDL_FALSE;

	SDL_bool mouse_ol = SDL_FALSE;
	SDL_bool mouse_or = SDL_FALSE;
	SDL_bool mouse_om = SDL_FALSE;

	SDL_bool mouse_l = SDL_FALSE;
	SDL_bool mouse_r = SDL_FALSE;
	SDL_bool mouse_m = SDL_FALSE;

	Sint32 mouse_x = 0;
	Sint32 mouse_y = 0;

	Uint32 mouse_ox = 0;
	Uint32 mouse_oy = 0;

	Uint32 iteration;

	// Audio.

	Uint32 _Audio_Samples;
	Uint32 _Audio_Buffers;

	// You need to override this if you want use the audio functionality.

	double (*_Audio_Callback)(double);

	// Flags.

	SDL_bool f_Clear_Renderer = SDL_TRUE;

	SDL_bool f_No_Debug = SDL_FALSE;

	// This function will initialize the width, height, and title of the boiler. You must overload
	// it if you would like to modify it's values.

	virtual void steam()
	{
		width = 640;
		height = 480;

		title = "Boiler demonstration";

		// Below are good parameters.

		_Audio_Samples = 44100;
		_Audio_Buffers = 2048;
	}

	// This function will create the window, renderer, and texture required to use the boiler.
	// This function will not fail, but will return a non-zero value if an error occurs.

	virtual Uint32 make(Uint32 _Extra = BOIL_EX_INIT_NOSUB)
	{
		steam();

		h_width = width / 2;
		h_height = height / 2;

		iteration = 0;

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			return 1;
		}

		if (_Extra == BOIL_EX_INIT_AUDIO)
		{
			SDL_AudioSpec _Request;

			// The 'frequency' refers to the sample rate, or the amount of samples to be taken per
			// second. This value is initialized by the user, but a good amount is 44100.

			_Request.freq = _Audio_Samples;

			// The 'format' member refers to the format of the audio written to the output buffer.
			// In other words, it is the digital accuracy (in bits) of the audio.

			_Request.format = AUDIO_S16SYS;

			// Boiler current only supports mono audio, perhaps stereo will be implemented in a
			// later patch.

			_Request.channels = 1;

			// The buffer size of the audio stream. Audio output is always a bit delayed, so 
			// having a appropriately sized buffer helps to compensate.

			_Request.samples = 2048;

			// SDL will invoke a function to fill up the buffer. That function is provided by 
			// Boiler, and is a bit unwieldy, so Boiler calls a simple function from within that 
			// callback. 

			_Request.callback = __BOIL_AUDIO_CALLBACK__;

			// Assign the sound callback to the global sound callback variable. This callback is
			// used by the __BOIL_AUDIO_CALLBACK__ function.

			__BOIL_USER_CALLBACK__ = _Audio_Callback;

			// Track the time in samples, so that time can be accurately calculated.

			_Request.userdata = &__BOIL_AUDIO_SAMPLE__;

			// Register the audio specification.

			SDL_AudioSpec _Response;

			if (SDL_OpenAudio(&_Request, &_Response) != 0)
			{
				return 6;
			}

			if (_Request.format != _Response.format)
			{
				return 7;
			}

			SDL_PauseAudio(0);
		}

		window = SDL_CreateWindow
		(
			title.c_str(),

			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,

			width,
			height,

			SDL_WINDOW_SHOWN |

			// This fixes the blurred screen bug in macOS Mojave. For other operating systems, it
			// won't have any effect.

			SDL_WINDOW_ALLOW_HIGHDPI
		);

		if (window == NULL)
		{
			return 2;
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

		if (renderer == NULL)
		{
			return 3;
		}

		texture = SDL_CreateTexture
		(
			renderer, 

			SDL_PIXELFORMAT_ARGB8888, 
			SDL_TEXTUREACCESS_STREAMING,

			width,
			height
		);

		if (texture == NULL)
		{
			return 4;
		}

		pixels = (Uint32*)malloc(width * height * sizeof(Uint32));

		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		if (pixels == NULL)
		{
			return 5;
		}

		running = SDL_TRUE;

		return 0;
	}

	// This function will render graphics to the pixel buffer. You should overload it, because it
	// just renders a random gradient by default. It is called very often, so try to optimize.

	virtual void draw()
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				pixels[y * width + x] = rgb((x + iteration) % 256, (y + iteration) % 256, 128);
			}
		}
	}

	// These functions are called whenever a key is pressed or released. The entire event is given
	// to you, so you can dissect it as you wish.

	virtual void keydown(SDL_Event e)
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_ESCAPE)
		{
			running = SDL_FALSE;
		}
	}

	virtual void keyup(SDL_Event e)
	{
		return;
	}

	// These functions are called whenever the mouse wheel is triggered. The entire event is given
	// to you, so you can dissect it as you wish.

	virtual void wheelup(SDL_Event e)
	{
		return;
	}

	virtual void wheeldown(SDL_Event e)
	{
		return;
	}

	virtual void wheelleft(SDL_Event e)
	{
		return;
	}

	virtual void wheelright(SDL_Event e)
	{
		return;
	}

	// This function will enter the main loop. This function will not return until the program
	// exits in one way or another.

	virtual void engine()
	{
		while (running == SDL_TRUE)
		{
			// The previous frame's mouse state has now past.

			mouse_ol = mouse_l;
			mouse_or = mouse_r;
			mouse_om = mouse_m;

			mouse_ox = mouse_x;
			mouse_oy = mouse_y;

			// Process any pending events. If this is not done, the program will hang and 
			// eventually become unresponsive.

			SDL_Event e;

			while (SDL_PollEvent(&e) == SDL_TRUE)
			{
				if (e.type == SDL_QUIT)
				{
					running = SDL_FALSE;
				}
				else if (e.type == SDL_KEYDOWN)
				{
					keydown(e);
				}
				else if (e.type == SDL_KEYUP)
				{
					keyup(e);
				}
				else if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						mouse_l = SDL_TRUE;
					}
					else if (e.button.button == SDL_BUTTON_RIGHT)
					{
						mouse_r = SDL_TRUE;
					}
					else if (e.button.button == SDL_BUTTON_MIDDLE)
					{
						mouse_m = SDL_TRUE;
					}
				}
				else if (e.type == SDL_MOUSEBUTTONUP)
				{
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						mouse_l = SDL_FALSE;
					}
					else if (e.button.button == SDL_BUTTON_RIGHT)
					{
						mouse_r = SDL_FALSE;
					}
					else if (e.button.button == SDL_BUTTON_MIDDLE)
					{
						mouse_m = SDL_FALSE;
					}
				}
				else if (e.type == SDL_MOUSEMOTION)
				{
					mouse_x = e.motion.x;
					mouse_y = e.motion.y;
				}
				else if (e.type == SDL_MOUSEWHEEL)
				{
					if (e.wheel.y > 0)
					{
						wheelup(e);
					}
					else if (e.wheel.y < 0)
					{
						wheeldown(e);
					}

					if (e.wheel.x > 0)
					{
						wheelright(e);
					}
					else if (e.wheel.x < 0)
					{
						wheelleft(e);
					}
				}
			}

			// Track the current time so that we can use it to cap the framerate after the
			// rendering has finished.

			Uint32 start = SDL_GetTicks();

			// Call the draw function to render the current frame.

			draw();

			// Dump the chunk of memory into the texture, so that the renderer can easily 
			// interface with it.

			SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(Uint32));

			// The texture information is now stored in the texture, so we can transfer this 
			// texture to the screen by using the renderer.

			if (f_Clear_Renderer == SDL_TRUE)
			{
				SDL_RenderClear(renderer);
			}

			SDL_RenderCopy(renderer, texture, NULL, NULL);

			SDL_RenderPresent(renderer);

			// Cap the framerate to 60 frames per second (60 fps). Optionally debug the time 
			// taken to render the frame if the iteration is a multiple of 60.

			Uint32 elapsed = SDL_GetTicks() - start;

			if (elapsed < expected)
			{
				SDL_Delay(std::max(0, (int)(expected - elapsed)));
			}

			if (!f_No_Debug && iteration % 60 == 0)
			{
				std::cout << "Frame " << iteration << " rendered in " << elapsed << " ms." << std::endl;
			}

			iteration++;
		}
	}

	// This function will clean up all resources which were allocated upon calling make. You will
	// need to clean up any other variables yourself.

	virtual void sweep()
	{
		free((void*)pixels);

		SDL_CloseAudio();

		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);

		SDL_Quit();
	}

	// These macros check whether any mouse button is held, was just released, or was just 
	// pressed.

	#define ml_held (mouse_ol && mouse_l)
	#define mr_held (mouse_or && mouse_r)
	#define mm_held (mouse_om && mouse_m)

	#define ml_released (mouse_ol && !mouse_l)
	#define mr_released (mouse_or && !mouse_r)
	#define mm_released (mouse_om && !mouse_m)

	#define ml_pressed (!mouse_ol && mouse_l)
	#define mr_pressed (!mouse_or && mouse_r)
	#define mm_pressed (!mouse_om && mouse_m)

	// Draws a line. Does not blend, replaces colors directly. This function was taken from the 
	// One Lone Coder (javidx9)'s olcConsoleGameEngine.h file. I cleaned it up a bit.

	void linergb(int x1, int y1, int x2, int y2, unsigned int c)
	{
		int x;
		int y;

		int dx = x2 - x1;
		int dy = y2 - y1;

		int dx1 = std::abs(dx);
		int dy1 = std::abs(dy);

		int px = 2 * dy1 - dx1;
		int py = 2 * dx1 - dy1;

		int xe;
		int ye;

		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;

				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;

				xe = x1;
			}

			if (x >= 0 && x < width && y >= 0 && y < height)
			{
				pixels[y * width + x] = c;
			}

			for (int i = 0; x < xe; i++)
			{
				x = x + 1;

				if (px < 0)
				{
					px = px + 2 * dy1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						y = y + 1;
					}
					else
					{
						y = y - 1;
					}

					px = px + 2 * (dy1 - dx1);
				}

				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					pixels[y * width + x] = c;
				}
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;

				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;

				ye = y1;
			}

			if (x >= 0 && x < width && y >= 0 && y < height)
			{
				pixels[y * width + x] = c;
			}

			for (int i = 0; y < ye; i++)
			{
				y = y + 1;

				if (py <= 0)
				{
					py = py + 2 * dx1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						x = x + 1;
					}
					else
					{
						x = x - 1;
					}

					py = py + 2 * (dx1 - dy1);
				}

				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					pixels[y * width + x] = c;
				}
			}
		}
	}

	// Same as above, but makes dashed lines. dash is the half period of the dashed line. cond is 
	// the value that pixels_placed % (dash * 2) must be >= to place a pixel. Change pixels_placed 
	// to make the dash move.

	void dashedlinergb(int x1, int y1, int x2, int y2, int dash, int cond, int pixels_placed, unsigned int c)
	{
		int x;
		int y;

		int dx = x2 - x1;
		int dy = y2 - y1;

		int dx1 = std::abs(dx);
		int dy1 = std::abs(dy);

		int px = 2 * dy1 - dx1;
		int py = 2 * dx1 - dy1;

		int xe;
		int ye;

		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				pixels_placed = INT_MAX / 2 - pixels_placed;

				x = x1;
				y = y1;

				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;

				xe = x1;
			}

			if (x >= 0 && x < width && y >= 0 && y < height)
			{
				if (pixels_placed % (dash * 2) >= cond)
				{
					pixels[y * width + x] = c;
				}

				pixels_placed++;
			}

			for (int i = 0; x < xe; i++)
			{
				x = x + 1;

				if (px < 0)
				{
					px = px + 2 * dy1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						y = y + 1;
					}
					else
					{
						y = y - 1;
					}

					px = px + 2 * (dy1 - dx1);
				}

				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					if (pixels_placed % (dash * 2) >= cond)
					{
						pixels[y * width + x] = c;
					}

					pixels_placed++;
				}
			}
		}
		else
		{
			if (dy >= 0)
			{
				pixels_placed = INT_MAX / 2 - pixels_placed;

				x = x1;
				y = y1;

				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;

				ye = y1;
			}

			if (x >= 0 && x < width && y >= 0 && y < height)
			{
				if (pixels_placed % (dash * 2) >= cond)
				{
					pixels[y * width + x] = c;
				}

				pixels_placed++;
			}

			for (int i = 0; y < ye; i++)
			{
				y = y + 1;

				if (py <= 0)
				{
					py = py + 2 * dx1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						x = x + 1;
					}
					else
					{
						x = x - 1;
					}

					py = py + 2 * (dx1 - dy1);
				}

				if (x >= 0 && x < width && y >= 0 && y < height)
				{
					if (pixels_placed % (dash * 2) >= cond)
					{
						pixels[y * width + x] = c;
					}

					pixels_placed++;
				}
			}
		}
	}

	// Draws a triangle. Does not blend, replaces colors directly.

	void trianglergb(int x1, int y1, int x2, int y2, int x3, int y3, unsigned int c)
	{
		linergb(x1, y1, x2, y2, c);
		linergb(x2, y2, x3, y3, c);
		linergb(x3, y3, x1, y1, c);
	}

	// Fills a triangle. Does not blend, replaces colors directly. This function was taken from 
	// the One Lone Coder (javidx9)'s olcConsoleGameEngine.h file. I cleaned it up a bit.

	void scanlinergb(int x1, int x2, int y1, unsigned int c)
	{
		for (int i = x1; i <= x2; i++)
		{
			if (i >= 0 && i < width && y1 >= 0 && y1 < height)
			{
				pixels[y1 * width + i] = c;
			}		
		}
	}

	void ftrianglergb(int x1, int y1, int x2, int y2, int x3, int y3, unsigned int c)
	{	
		int t1x; 
		int t2x;

		int y;

		int minx;
		int maxx;

		int t1xp; 
		int t2xp;

		bool changed1 = false;
		bool changed2 = false;

		int signx1;
		int signx2;

		int dx1;
		int dy1;

		int dx2;
		int dy2;

		int e1;
		int e2;
		
		if (y1 > y2) 
		{ 
			std::swap(y1, y2); 
			std::swap(x1, x2); 
		}
		
		if (y1 > y3) 
		{ 
			std::swap(y1, y3); 
			std::swap(x1, x3); 
		}
		
		if (y2 > y3)
		{ 
			std::swap(y2, y3); 
			std::swap(x2, x3); 
		}

		t1x = x1; 
		t2x = x1;

		y = y1;

		dx1 = (int)(x2 - x1);   
		dy1 = (int)(y2 - y1);

		if (dx1 < 0)
		{ 
			dx1 = -dx1; 

			signx1 = -1; 
		}
		else 
		{
			signx1 = 1;
		}		

		dx2 = (int)(x3 - x1);
		dy2 = (int)(y3 - y1);

		if (dx2 < 0) 
		{ 
			dx2 = -dx2; 

			signx2 = -1; 
		}
		else
		{
			signx2 = 1;
		}		

		if (dy1 > dx1) 
		{
			std::swap(dx1, dy1);

			changed1 = true;
		}
		if (dy2 > dx2) 
		{
			std::swap(dy2, dx2);

			changed2 = true;
		}

		e1 = (int)(dx1 >> 1);
		e2 = (int)(dx2 >> 1);
		
		if (y1 == y2)
		{
			goto next;
		}	

		for (int i = 0; i < dx1;) 
		{
			t1xp = 0; 
			t2xp = 0;

			if (t1x < t2x)
			{
			 	minx = t1x; 
			 	maxx = t2x; 
			}
			else
			{ 
				minx = t2x; 
				maxx = t1x;
			}

			while (i < dx1) 
			{
				i++;
				
				e1 += dy1;
				
				while (e1 >= dx1) 
				{
					e1 -= dx1;
					
					if (changed1) 
					{
						t1xp = signx1;
					}
					else
					{	
						goto next1;
					}          
				}

				if (changed1)
				{
					break;
				}
				else 
				{
					t1x += signx1;
				}
			}
			
			next1:

			while (1) 
			{
				e2 += dy2;

				while (e2 >= dx2) 
				{
					e2 -= dx2;

					if (changed2)
					{
						t2xp = signx2;
					}
					else
					{
						goto next2;
					}
				}

				if (changed2)
				{     
					break;
				}
				else
				{
					t2x += signx2;
				}
			}

			next2:
			
			if (minx > t1x)
			{
				minx = t1x;
			}

			if (minx > t2x) 
			{
				minx = t2x;
			}

			if (maxx < t1x) 
			{
				maxx = t1x;
			} 

			if (maxx < t2x)
			{ 
				maxx = t2x;
			}

			scanlinergb(minx, maxx, y, c);
										 
			if (!changed1) 
			{
				t1x += signx1;
			}

			t1x += t1xp;

			if (!changed2)
			{
				t2x += signx2;
			}
			
			t2x += t2xp;
			
			y += 1;
			
			if (y == y2)
			{
				break;
			}
		}

		next:

		dx1 = (int)(x3 - x2); 
		dy1 = (int)(y3 - y2);

		if (dx1 < 0)
		{ 
			dx1 = -dx1; 

			signx1 = -1; 
		}
		else
		{
			signx1 = 1;
		}
		
		t1x = x2;

		if (dy1 > dx1) 
		{
			std::swap(dy1, dx1);

			changed1 = true;
		}
		else 
		{
			changed1 = false;
		}

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) 
		{
			t1xp = 0; 
			t2xp = 0;

			if (t1x < t2x)
			{ 
				minx = t1x; 
				maxx = t2x; 
			}
			else 
			{ 
				minx = t2x; 
				maxx = t1x; 
			}
			
			while (i < dx1) 
			{
				e1 += dy1;
			
				while (e1 >= dx1) 
				{
					e1 -= dx1;
					
					if (changed1) 
					{ 
						t1xp = signx1; 

						break; 
					}
					else
					{          
						goto next3;
					}
				}

				if (changed1) 
				{
					break;
				}
				else
				{ 	  
					t1x += signx1;
				};

				if (i < dx1) 
				{
					i++;
				}
			}

			next3:
			
			while (t2x != x3) 
			{
				e2 += dy2;

				while (e2 >= dx2) 
				{
					e2 -= dx2;

					if (changed2) 
					{
						t2xp = signx2;
					}
					else
					{
						goto next4;
					}
				}

				if (changed2)
				{
					break;
				}
				else
				{
					t2x += signx2;
				}
			}

			next4:

			if (minx > t1x) 
			{
				minx = t1x;
			} 

			if (minx > t2x) 
			{
				minx = t2x;
			}

			if (maxx < t1x) 
			{
				maxx = t1x;
			} 

			if (maxx < t2x) 
			{
				maxx = t2x;
			}

			scanlinergb(minx, maxx, y, c); 

			t1x += t1xp;
			t2x += t2xp;  										
			
			if (!changed1) 
			{
				t1x += signx1;
			}

			if (!changed2) 
			{
				t2x += signx2;
			}		
			
			y += 1;
			
			if (y > y3) 
			{
				return;
			}
		}
	}

	// Draws a circle. Does not blend, replaces colors directly. This function was taken from the
	// One Lone Coder (javidx9)'s olcConsoleGameEngine.h file. I cleaned it up a bit.

	inline void putp(int y, int x, unsigned int c)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			pixels[y * width + x] = c;
		}
	}

	void circlergb(int xc, int yc, int r, unsigned int c)
	{
		r = std::abs(r);

		int x = 0;
		int y = 0;

		int p = 3 - (2 * r);

		if (!r)
		{
			return;
		}

		y = r;

		while (y >= x)
		{
			putp(yc - y, xc - x, c);
			putp(yc - x, xc - y, c);
			putp(yc - x, xc + y, c);
			putp(yc - y, xc + x, c);
			putp(yc + y, xc - x, c);
			putp(yc + x, xc - y, c);
			putp(yc + x, xc + y, c);
			putp(yc + y, xc + x, c);

			if (p < 0)
			{
				p += 4 * x++ + 6;
			}
			else
			{
				p += 4 * (x++ - y--) + 10;
			}
		}
	}

	// Fills a circle. Does not blend, replaces colors directly. This function was taken from the
	// One Lone Coder (javidx9)'s olcConsoleGameEngine.h file. I cleaned it up a bit.

	void fcirclergb(int xc, int yc, int r, unsigned int c)
	{
		r = std::abs(r);

		int x = 0;
		int y = 0;

		int p = 3 - (2 * r);

		if (!r)
		{
			return;
		}

		y = r;

		while (y >= x)
		{
			scanlinergb(xc - x, xc + x, yc - y, c);
			scanlinergb(xc - y, xc + y, yc - x, c);
			scanlinergb(xc - x, xc + x, yc + y, c);
			scanlinergb(xc - y, xc + y, yc + x, c);

			if (p < 0)
			{
				p += 4 * x++ + 6;
			}
			else
			{
				p += 4 * (x++ - y--) + 10;
			}
		}
	}

	// Bresenham's ellipse drawing algorithm. This code was taken from https://sites.google.com/si
	// te/ruslancray/lab/projects/bresenhamscircleellipsedrawingalgorithm/bresenham-s-circle-ellip
	// se-drawing-algorithm. I modified it.
	
	void ellipsergb(int yc, int xc, int w, int h, unsigned int c)
	{
		int a2 = w * w;
		int b2 = h * h;

		int fa2 = 4 * a2;
		int fb2 = 4 * b2;

		int x;
		int y;

		int sigma;

		// First half.

		for (x = 0, y = h, sigma = 2 * b2 + a2 * (1 - 2 * h); b2 * x <= a2 * y; x++)
		{
			putp(xc + x, yc + y, c);
			putp(xc - x, yc + y, c);
			putp(xc + x, yc - y, c);
			putp(xc - x, yc - y, c);

			if (sigma >= 0)
			{
				sigma += fa2 * (1 - y);

				y--;
			}

			sigma += b2 * ((4 * x) + 6);
		}

		// Second half.

		for (x = w, y = 0, sigma = 2 * a2 + b2 * (1 - 2 * w); a2 * y <= b2 * x; y++)
		{
			putp(xc + x, yc + y, c);
			putp(xc - x, yc + y, c);
			putp(xc + x, yc - y, c);
			putp(xc - x, yc - y, c);

			if (sigma >= 0)
			{
				sigma += fb2 * (1 - x);

				x--;
			}

			sigma += a2 * ((4 * y) + 6);
		}
	}

	// Draws a rectangle using lines.

	void rectrgb(int x, int y, int w, int h, unsigned int c)
	{
		linergb(x, y, x + w, y, c);

		linergb(x + w, y, x + w, y + h, c);
		
		linergb(x + w, y + h, x, y + h, c);

		linergb(x, y + h, x, y, c);
	}

	// Fills a rectangle using scanlines.

	void frectrgb(int x, int y, int w, int h, unsigned int c)
	{
		w--;

		for (int y_ = y; y_ < y + h; y_++)
		{
			scanlinergb(x, x + w, y_, c);
		}
	}

	// Clears the screen to any color (fast).

	void clear(unsigned int c)
	{
		for (int i = 0; i < width * height; i++)
		{
			pixels[i] = c;
		}
	}

	// Clears the screen to black (extremely fast).

	void black()
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));
	}

	// Random X coordinate.

	int randx()
	{
		return rand() % width;
	}

	// Random Y coordinate.

	int randy()
	{
		return rand() % height;
	}

	// Macro derivatives.

	#define mrandx (rand() % width)
	#define mrandy (rand() % height)

	// Draws an image to the screen. Clips the image beforehand. Source and destination 
	// coordinates accepted. Does not honour transparency, but is extremely fast.

	void blitrgb(Uint32* img, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh)
	{
		dw--;
		dh--;

		// Trivial rejection.

		if (dx >= (int)width || dy >= (int)height)
		{
			return;
		}
		else if (dx + dw < 0 || dy + dh < 0)
		{
			return;
		}

		if (sx >= sw || sy >= sh || sx < 0 || sy < 0)
		{
			return;
		}
		else if (sx + dw > sw || sy + dh > sh)
		{
			return;
		}

		// If any component of the destination coordinate is less than zero, then we must only 
		// draw the part of the scanline of the image that lies on the right side of the left
		// boundary. We must also scale the texture coordinates to match the offset.

		if (dx < 0)
		{
			dw += dx;
			sx -= dx;

			dx = 0;
		}

		if (dy < 0)
		{
			dh += dy;
			sy -= dy;

			dy = 0;
		}

		// The same thing as above is done for components greater than the dimensions of the 
		// destination plane. This is simpler, because we do not need to offset texture 
		// coordinates as the texture coordinates are relative to the top-left origin.

		if (dx + dw >= (int)width)
		{
			dw -= (dx + dw) - (int)width;
		}

		if (dy + dh >= (int)height)
		{
			dh -= (dy + dh) - (int)height;
		}

		dw++;

		for (int y = 0; y < dh + 1; y++)
		{
			memcpy(&(pixels[(y + dy) * width + dx]), &(img[(y + sy) * sw + sx]), sizeof(Uint32) * dw);
		}
	}

	// The following functions are part of the terminal extension for Boiler. They make it simpler
	// to render raster fonts to the screen. The font used is Code Page 437 (by IBM).

	#ifdef __TERMINAL_CODEPAGE_BOILER__
	
	#ifdef __TERMINAL_EXTENSION_BOILER__

	enum __font_align
	{
		align_top_left,
		align_top_right,

		align_bottom_left,
		align_bottom_right,

		align_center
	};

	// Draw a font to the screen. If the given background color is equal to the value returned by
	// argb(255, 0, 0, 0), then no background is rendered.

	const int __fnb = argb(255, 0, 0, 0);

	void fontrgb(std::string str, unsigned int xn, unsigned int yn, Uint32 fcol, Uint32 bcol, __font_align align = align_top_left)
	{
		bool nobg = (bcol == __fnb);

		// Top left alignment only, for now.

		for (int i = 0; i < str.size(); i++)
		{
			Uint8 _G = str[i];

			unsigned int xf = (_G % (tfi_w / tf_w)) * tf_w;
			unsigned int yf = (_G / (tfi_w / tf_w)) * tf_h;

			for (int x = 0; x < tf_w; x++)
			{
				for (int y = 0; y < tf_h; y++)
				{
					if (__437__[(yf + y) * tfi_w + (xf + x)] == 0)
					{
						// Background pixel.

						if (!nobg)
						{
							pixels[(yn + y) * width + (xn + x) + (i * tf_w)] = bcol;
						}
					}
					else
					{
						// Foreground pixel.

						pixels[(yn + y) * width + (xn + x) + (i * tf_w)] = fcol;
					}
				}
			}
		}
	}

	// Draw a font to the screen (using the VGA color palette).

	void fontvga(std::string str, unsigned int xn, unsigned int yn, Uint8 fcol, Uint8 bcol, __font_align align = align_top_right)
	{
		// Top left alignment only, for now.

		for (int i = 0; i < str.size(); i++)
		{
			Uint8 _G = str[i];

			unsigned int xf = (_G % (tfi_w / tf_w)) * tf_w;
			unsigned int yf = (_G / (tfi_w / tf_w)) * tf_h;

			for (int x = 0; x < tf_w; x++)
			{
				for (int y = 0; y < tf_h; y++)
				{
					if (__437__[(yf + y) * tfi_w + (xf + x)] == 0)
					{
						// Background pixel.

						pixels[(yn + y) * width + (xn + x) + (i * tf_w)] = vga_rgb[bcol];
					}
					else
					{
						// Foreground pixel.

						pixels[(yn + y) * width + (xn + x) + (i * tf_w)] = vga_rgb[fcol];
					}
				}
			}
		}
	}

	#endif

	#endif

	// Set the pixel at x, y to c if x, y is in bounds of the screen.

	inline void plotp(int x, int y, unsigned int c)
	{
		if (x >= 0 && y >= 0 && x < width && y < height)
		{
			pixels[y * width + x] = c;
		}
	}

	// Point.

	struct DASHED_CIRCLE__point
	{
		double x;
		double y;

		DASHED_CIRCLE__point(double _x, double _y)
		{
			x = _x;
			y = _y;
		}
	};

	// Convert from polar to Cartesian coordinates. X is rho and Y is theta.

	inline DASHED_CIRCLE__point DASHED_CIRCLE__polar_to_cartesian(DASHED_CIRCLE__point polar)
	{
		return DASHED_CIRCLE__point
		(
			polar.x * cos(polar.y),
			polar.x * sin(polar.y)
		);
	}

	// Dashed circles. Refer to dashedlinergb to see the significance of parameters.

	void dashedcirclergb(int cx, int cy, double radius, int dash, int cond, int pixels_placed, unsigned int color)
	{
		double circ = 2.0 * M_PI * radius;

		double inc = 360.0 / circ;

		double deg = 0.0;

		for (int i = 0; i < circ; i++)
		{
			DASHED_CIRCLE__point polar = DASHED_CIRCLE__point(radius, degrad(deg));

			DASHED_CIRCLE__point cartesian = DASHED_CIRCLE__polar_to_cartesian(polar);

			if (pixels_placed % (dash * 2) >= cond)
			{
				plotp(cartesian.x + cx, cartesian.y + cy, color);
			}

			pixels_placed++;

			deg += inc;
		}
	}
};

// Pointless nuke, big explosion without radiation cleansing.

void nuke()
{
	exit(1);
}

// Nuke, big explosion without radiation cleansing.

void nuke(std::string death_note)
{
	std::cerr << death_note << std::endl;

	exit(1);
}

// Load image function. Supports Windows Bitmap Image (.bmp) image files. This function will 
// return a pointer to an array of pixel data, which consists of w * h elements. The integers 
// w and h will contain the dimensions of the image (or zero) when the function returns. This
// function supports 24- and 32- bit bitmaps. Returns NULL on error.

Uint32* loadbmp(std::string path, int &w, int &h)
{
	SDL_Surface* s_bitmap = SDL_LoadBMP(path.c_str());

	if (s_bitmap == NULL)
	{
		std::cerr << "Could not load image." << std::endl;

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
		std::cerr << "Only 32-bit and 24-bit bitmaps are supported." << std::endl;

		w = 0;
		h = 0;

		return NULL;
	}

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

	// Annihilate the surface.

	SDL_FreeSurface(s_bitmap);

	return m_bmp;
}

// Save an image to a bitmap (.bmp) file. Explodes if you give it garbage.

void savebmp(std::string file, Uint32* pixels, int w, int h)
{
	SDL_Surface* _surf = SDL_CreateRGBSurfaceFrom(pixels, w, h, 8 * 4, w * 4, 0, 0, 0, 0);

	SDL_SaveBMP(_surf, file.c_str());

	SDL_FreeSurface(_surf);
} 

#ifdef BOIL_USE_STB_IMAGE

// A much better image loading function. This may be redundant in a few ways, such as converting 
// into a surface and then back, but I haven't tested that yet. Code is based off of the sample
// provided by https://wiki.libsdl.org/SDL_CreateRGBSurfaceWithFormatFrom.

Uint32* loadimg(std::string path, int &w, int &h, int stb_f = STBI_rgb)
{
	int width;
	int height;

	int orig_format;

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &orig_format, stb_f);

	if (data == NULL)
	{
		std::cerr << "Error on stbi_load; could not load image." << std::endl;

		w = 0;
		h = 0;

		return NULL;
	}

	int depth;
	int pitch;

	Uint32 pixel_format;

	if (stb_f == STBI_rgb)
	{
		depth = 24;

		pitch = 3 * width;

		pixel_format = SDL_PIXELFORMAT_RGB24;
	}
	else if (stb_f == STBI_rgb_alpha)
	{
		depth = 32;

		pitch = 4 * width;

		pixel_format = SDL_PIXELFORMAT_RGBA32;
	}
	else
	{
		std::cerr << "Supported formats include STBI_rgb and STBI_rgb_alpha." << std::endl;

		w = 0;
		h = 0;

		return NULL;
	}

	SDL_Surface* s_bitmap = SDL_CreateRGBSurfaceWithFormatFrom
	(
		(void*)data,

		width,
		height,

		depth,
		pitch,

		pixel_format
	);

	if (s_bitmap == NULL)
	{
		std::cerr << "Error on SDL_CreateRGBSurfaceWithFormatFrom; could not load image." << std::endl;

		w = 0;
		h = 0;

		stbi_image_free(data);

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
		std::cerr << "Only 32-bit and 24-bit images are supported." << std::endl;

		w = 0;
		h = 0;

		return NULL;
	}

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
					if (stb_f == STBI_rgb_alpha)
					{
						m_bmp[y * s_bitmap->w + x] = p[3] << 24 | p[2] << 16 | p[1] << 8 | p[0];
					}
					else
					{
						m_bmp[y * s_bitmap->w + x] = p[2] << 16 | p[1] << 8 | p[0];
					}
				}
				else
				{
					if (stb_f == STBI_rgb_alpha)
					{
						// Probably broken.

						m_bmp[y * s_bitmap->w + x] = p[3] << 24 | p[0] << 16 | p[1] << 8 | p[2];
					}
					else
					{
						m_bmp[y * s_bitmap->w + x] = p[0] << 16 | p[1] << 8 | p[2];
					}
				}
			}
			else
			{
				if (stb_f == STBI_rgb_alpha)
				{
					Uint8 _oa;
					Uint8 _or;
					Uint8 _og;
					Uint8 _ob;

					SDL_GetRGBA(*(Uint32*)p, s_bitmap->format, &_or, &_og, &_ob, &_oa);		

					m_bmp[y * s_bitmap->w + x] = argb(_oa, _or, _og, _ob);
				}
				else
				{
					m_bmp[y * s_bitmap->w + x] = *(Uint32*)p;
				}
			}
		}
	}

	// Annihilate the surface.

	SDL_FreeSurface(s_bitmap);

	// Annihilate the stbi stuff.

	stbi_image_free(data);

	return m_bmp;
}

#endif

// Random value between 0.0 and 1.0.

inline double rand_01()
{
	return double(rand()) / double(RAND_MAX);
}

// Random value between -1.0 and 1.0.

inline double rand_11()
{
	return double(rand()) / double(RAND_MAX) * 2.0 - 1.0;
}

// Simple OpenCL "starterpack".

#ifdef BOIL_USE_OPENCL

// Stringification macro.

#define __stringify(...) #__VA_ARGS__

// Include OpenCL.

#ifdef __APPLE__

#define CL_SILENCE_DEPRECATION

#include <OpenCL/OpenCL.h>

#else

#include <CL/cl.h>

#endif

// Convert an error code to a string.
//
// https://stackoverflow.com/questions/24326432/convenient-way-to-show-opencl-error-codes

const char* cl_error_string(cl_int error)
{
	switch (error)
	{
		case 0: return "CL_SUCCESS";

		case -1: return "CL_DEVICE_NOT_FOUND";

		case -2: return "CL_DEVICE_NOT_AVAILABLE";

		case -3: return "CL_COMPILER_NOT_AVAILABLE";

		case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";

		case -5: return "CL_OUT_OF_RESOURCES";

		case -6: return "CL_OUT_OF_HOST_MEMORY";

		case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";

		case -8: return "CL_MEM_COPY_OVERLAP";

		case -9: return "CL_IMAGE_FORMAT_MISMATCH";

		case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";

		case -11: return "CL_BUILD_PROGRAM_FAILURE";

		case -12: return "CL_MAP_FAILURE";

		case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";

		case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";

		case -15: return "CL_COMPILE_PROGRAM_FAILURE";

		case -16: return "CL_LINKER_NOT_AVAILABLE";

		case -17: return "CL_LINK_PROGRAM_FAILURE";

		case -18: return "CL_DEVICE_PARTITION_FAILED";

		case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

		case -30: return "CL_INVALID_VALUE";

		case -31: return "CL_INVALID_DEVICE_TYPE";

		case -32: return "CL_INVALID_PLATFORM";

		case -33: return "CL_INVALID_DEVICE";

		case -34: return "CL_INVALID_CONTEXT";

		case -35: return "CL_INVALID_QUEUE_PROPERTIES";

		case -36: return "CL_INVALID_COMMAND_QUEUE";

		case -37: return "CL_INVALID_HOST_PTR";

		case -38: return "CL_INVALID_MEM_OBJECT";

		case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";

		case -40: return "CL_INVALID_IMAGE_SIZE";

		case -41: return "CL_INVALID_SAMPLER";

		case -42: return "CL_INVALID_BINARY";

		case -43: return "CL_INVALID_BUILD_OPTIONS";

		case -44: return "CL_INVALID_PROGRAM";

		case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";

		case -46: return "CL_INVALID_KERNEL_NAME";

		case -47: return "CL_INVALID_KERNEL_DEFINITION";

		case -48: return "CL_INVALID_KERNEL";

		case -49: return "CL_INVALID_ARG_INDEX";

		case -50: return "CL_INVALID_ARG_VALUE";

		case -51: return "CL_INVALID_ARG_SIZE";

		case -52: return "CL_INVALID_KERNEL_ARGS";

		case -53: return "CL_INVALID_WORK_DIMENSION";

		case -54: return "CL_INVALID_WORK_GROUP_SIZE";

		case -55: return "CL_INVALID_WORK_ITEM_SIZE";

		case -56: return "CL_INVALID_GLOBAL_OFFSET";

		case -57: return "CL_INVALID_EVENT_WAIT_LIST";

		case -58: return "CL_INVALID_EVENT";

		case -59: return "CL_INVALID_OPERATION";

		case -60: return "CL_INVALID_GL_OBJECT";

		case -61: return "CL_INVALID_BUFFER_SIZE";

		case -62: return "CL_INVALID_MIP_LEVEL";

		case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";

		case -64: return "CL_INVALID_PROPERTY";

		case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";

		case -66: return "CL_INVALID_COMPILER_OPTIONS";

		case -67: return "CL_INVALID_LINKER_OPTIONS";

		case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

		case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";

		case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";

		case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";

		case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";

		case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";

		case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";

		default: return "Unknown OpenCL error";
	}
}

// Write a message to std::cout.

void __boiler_cl_say(std::string message)
{
	std::cout << message << std::endl;
}

// The basics of all OpenCL programs.

struct boiler_opencl
{
	cl_device_id device_id;

	cl_context context;

	cl_command_queue command_queue;

	// Load a program.

	cl_program load_program(const char* kernel_source, size_t kernel_source_size)
	{
		// Create a variable to hold return codes.

		cl_int r_code;

		// Create an OpenCL program from the kernel source.

		cl_program program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, (const size_t*)&kernel_source_size, &r_code);

		// Make sure the OpenCL program was created successfully.

		if (r_code != CL_SUCCESS)
		{
			__boiler_cl_say("Could not create an OpenCL program.");

			exit(EXIT_FAILURE);
		}

		// Build the OpenCL program.

		r_code = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

		// Make sure the OpenCL program was built successfully.

		if (r_code != CL_SUCCESS)
		{
			__boiler_cl_say("Could not build an OpenCL program.");

			// https://stackoverflow.com/questions/18973371/build-opencl-kernel-failure

			size_t len;

			clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);

			char* log = new char[len];

			clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, len, log, NULL);

			std::cout << log << std::endl;

			exit(EXIT_FAILURE);
		}

		return program;
	}

	// Load a kernel.

	cl_kernel load_kernel(cl_program program, const char* kernel_name)
	{
		// Create a variable to hold return codes.

		cl_int r_code;

		// Create the OpenCL kernel from the function within the OpenCL
		// program.

		cl_kernel kernel = clCreateKernel(program, kernel_name, &r_code);

		// Make sure the OpenCL kernel was created successfully.

		if (r_code != CL_SUCCESS)
		{
			__boiler_cl_say("Could not create an OpenCL kernel.");

			exit(EXIT_FAILURE);
		}

		return kernel;
	}

	// Clean up.

	void sweep()
	{
		// Free the command queue.

		clReleaseCommandQueue(command_queue);

		// Free the context.

		clReleaseContext(context);
	}
};

// Load the basics of all OpenCL programs.

boiler_opencl load_opencl()
{
	// Create a variable to hold return codes.

	cl_int r_code;

	// Create identifier objects to hold information about the available
	// platforms and available devices.

	cl_platform_id platform_id = NULL;

	cl_device_id device_id = NULL;

	// Create unsigned integer objects to hold the amount of available
	// platforms and available devices.

	cl_uint num_platforms;

	cl_uint num_devices;

	// Get the first available platform and store the amount of available
	// platforms.

	clGetPlatformIDs(1, &platform_id, &num_platforms);

	// Get the first available device on the first available platform. Store
	// the amount of available devices. This device will be referred to as the
	// 'default device'.

	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &num_devices);

	// Create an OpenCL context on the default device.

	cl_context context = clCreateContext(0, 1, &device_id, NULL, NULL, &r_code);

	// Make sure the OpenCL context was created successfully.

	if (r_code != CL_SUCCESS)
	{
		__boiler_cl_say("Could not create an OpenCL context.");

		exit(EXIT_FAILURE);
	}

	// Create an OpenCL command queue.

	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &r_code);

	// Make sure the OpenCL command queue was created successfully.

	if (r_code != CL_SUCCESS)
	{
		__boiler_cl_say("Could not create an OpenCL command queue.");

		exit(EXIT_FAILURE);
	}

	return {device_id, context, command_queue};
};

#endif