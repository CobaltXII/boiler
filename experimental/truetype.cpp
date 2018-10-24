// Testing stb_truetype? Awesome.

#define STB_TRUETYPE_IMPLEMENTATION

#include "../boiler/stb/stb_truetype.hpp"

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

// Excerpt from The Lord of the Rings.

const char paragraph[] = "Bilbo was very rich and very peculiar, and had been the wonder of the Shire for sixty years, ever since his remarkable disappearance and unexpected return. The riches he had brought back from his travels had now become a local legend, and it was popularly believed, whatever the old folk might say, that the Hill at Bag End was full of tunnels stuffed with treasure. And if that was not enough for fame, there was also his prolonged vigour to marvel at. Time wore on, but it seemed to have little effect on Mr. Baggins. At ninety he was much the same as at fifty. At ninety-nine they began to call him well-preserved; but unchanged would have been nearer the mark. There were some that shook their heads and thought this was too much of a good thing; it seemed unfair that anyone should possess (apparently) perpetual youth as well as (reputedly) inexhaustible wealth.";

struct game: boiler
{	
	stbtt_fontinfo font_info;

	unsigned char* font_bitmap = NULL;

	char* ttf_buffer = NULL;

	double font_scale;

	int font_ascent;
	int font_descent;

	const int font_size_px = 12;

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Testing stb_truetype (using Boiler)";

		f_No_Debug = SDL_TRUE;

		// This opens the font.

		FILE* font_f = fopen("minecraftia.ttf", "rb");

		if (font_f == NULL)
		{
			nuke("Couldn't open font!");
		}

		// This loads the font into a buffer.

		fseek(font_f, 0, SEEK_END);

		int font_f_size = ftell(font_f);

		rewind(font_f);

		ttf_buffer = new char[font_f_size];

		if (ttf_buffer == NULL)
		{
			nuke("Couldn't load font!");
		}

		fread(ttf_buffer, 1, font_f_size, font_f);

		// This intializes the stb_truetype font.

		stbtt_InitFont(&font_info, (unsigned char*)ttf_buffer, stbtt_GetFontOffsetForIndex((unsigned char*)ttf_buffer, 0));
	
		font_scale = stbtt_ScaleForPixelHeight(&font_info, font_size_px - 1);

		stbtt_GetFontVMetrics(&font_info, &font_ascent, &font_descent, 0);
	}

	void drawfont(int x, int y, const char* text, unsigned int c)
	{
		// This prepares for rendering the font.

   		int font_index;
   		int font_advance;

   		int f_x0;
   		int f_y0;

   		int f_w;
   		int f_h;

   		int f_xpos;
   		int f_ypos;

   		f_xpos = x;

   		f_ypos = y + font_scale * font_ascent;

   		// This renders the font.

   		for (font_index = 0; text[font_index]; font_index++)
   		{
   			font_bitmap = stbtt_GetCodepointBitmap(&font_info, 0, stbtt_ScaleForPixelHeight(&font_info, font_size_px - 1), text[font_index], &f_w, &f_h, 0, 0);
   		
   			stbtt_GetCodepointHMetrics(&font_info, text[font_index], &font_advance, 0);

   			stbtt_GetCodepointBitmapBox(&font_info, text[font_index], font_scale, font_scale, &f_x0, &f_y0, 0, 0);
   		
   			int i;
   			int j;

   			int x = f_xpos;
   			int y = f_ypos;

   			for (j = 0; j < f_h; j++)
   			{
   				for (i = 0; i < f_w; i++)
   				{
   					unsigned char px = font_bitmap[j * f_w + i];

   					// No antialiasing!

   					if (px)
   					{
   						pixels[(y + f_y0) * width + (x + f_x0)] = c;
   					}

   					x++;
   				}

   				x = f_xpos;

   				y++;
   			}

   			stbtt_FreeBitmap(font_bitmap, 0);

   			if (!text[font_index + 1])
   			{
   				break;
   			}

   			f_xpos += font_scale * font_advance;
		}
	}

	void draw() override
	{
		std::vector<std::string> lines;

		std::string string;

		for (int i = 0; i < sizeof(paragraph); i++)
		{
			if (i % 100 != 99)
			{
				string += paragraph[i];
			}
			else
			{
				lines.push_back(string);

				string.clear();

				string.push_back(paragraph[i]);
			}
		}

		lines.push_back(string);

		for (int i = 0; i < lines.size(); i++)
		{
			drawfont(25, 25 + (i * 14), lines[i].c_str(), rgb(0x66, 0x66, 0x66));
			drawfont(24, 24 + (i * 14), lines[i].c_str(), rgb(0xFF, 0xFF, 0xFF));
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

	delete[] demo.ttf_buffer;

	return 0;
}