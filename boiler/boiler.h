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

#include <SDL2/SDL.h>

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

// Convert degrees to radians.

inline double degrad(double x)
{
	return 2 * M_PI * (x / 360);
}

// Clamp a value to the range 0 to 255.

inline double clamprgb(double x)
{
	return std::max(0.0, std::min(255.0, x));
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

	std::string title;

	SDL_bool running = SDL_FALSE;

	SDL_bool mouse_l = SDL_FALSE;
	SDL_bool mouse_r = SDL_FALSE;
	SDL_bool mouse_m = SDL_FALSE;

	Uint32 mouse_x;
	Uint32 mouse_y;

	Uint32 iteration;

	// Flags.

	SDL_bool f_Clear_Renderer = true;

	// This function will initialize the width, height, and title of the boiler. You must overload
	// it if you would like to modify it's values.

	virtual void steam()
	{
		width = 640;
		height = 480;
	}

	// This function will create the window, renderer, and texture required to use the boiler.
	// This function will not fail, but will return a non-zero value if an error occurs.

	virtual Uint32 make()
	{
		steam();

		iteration = 0;

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			return 1;
		}

		window = SDL_CreateWindow
		(
			title.c_str(),

			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,

			width,
			height,

			SDL_WINDOW_SHOWN
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

			if (f_Clear_Renderer)
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

			if (iteration % 60 == 0)
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

	return m_bmp;
}