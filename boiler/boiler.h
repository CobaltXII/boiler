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

#include <SDL2/SDL.h>

Uint32 BOIL_EX_INIT_NOSUB = 1;
Uint32 BOIL_EX_INIT_AUDIO = 2;

#include <string>
#include <iostream>

Uint32 expected = 1000.0 / 60.0;

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

#define mrgb(r, g, b) (Uint32)((r) << 16 | (g) << 8 | (b))

#define margb(a, r, g, b) (Uint32)((a) << 24 | (r) << 16 | (g) << 8 | (b))

// Convert degrees to radians.

inline double degrad(double x)
{
	return 2.0 * M_PI * (x / 360.0);
}

// Macro derivative.

#define mdegrad(x) (double)(2 * M_PI * ((x) / 360.0))

// Clamp a value to the range 0 to 255.

inline double clamprgb(double x)
{
	return std::max(0.0, std::min(255.0, x));
}

// Macro derivative.

#define mclamprgb(x) (double)(std::max(0.0, std::min(255.0, (x))))

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

#define mgeta(argb) (Uint8)((argb) >> 24)

#define mgetr(argb) (Uint8)((argb) >> 16)

#define mgetg(argb) (Uint8)((argb) >> 8)

#define mgetb(argb) (Uint8)((argb))

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

	Uint32 width;
	Uint32 height;

	Uint32 h_width;
	Uint32 h_height;

	std::string title;

	SDL_bool running = SDL_FALSE;

	SDL_bool mouse_l = SDL_FALSE;
	SDL_bool mouse_r = SDL_FALSE;
	SDL_bool mouse_m = SDL_FALSE;

	Uint32 mouse_x;
	Uint32 mouse_y;

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

	// This function will enter the main loop. This function will not return until the program
	// exits in one way or another.

	virtual void engine()
	{
		while (running == SDL_TRUE)
		{
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

	void fontrgb(std::string str, unsigned int xn, unsigned int yn, Uint32 fcol, Uint32 bcol, __font_align align)
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

	void fontvga(std::string str, unsigned int xn, unsigned int yn, Uint8 fcol, Uint8 bcol, __font_align align)
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

// Load image function. Supports Windows BMP Image (.bmp) image files. This function will return
// a pointer to an array of pixel data, which consists of w * h elements. The integers w and h
// will contain the dimensions of the image (or zero) when the function returns. This function
// supports 24- and 32- bit bitmaps. Returns NULL on error.

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
					m_bmp[y * s_bitmap->w + x] = p[2] << 16 | p[1] << 8 | p[0];
				}
				else
				{
					m_bmp[y * s_bitmap->w + x] = p[2] | p[1] << 8 | p[0] << 16;
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

	// Annihilate the stbi stuff.

	stbi_image_free(data);

	return m_bmp;
}

#endif