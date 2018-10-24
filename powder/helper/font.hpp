#pragma once

// Include me within a class that derives from 'boiler'.

void drawfont(int x, int y, std::string text, unsigned int color)
{
	for (int c = 0; c < text.size(); c++)
	{
		int fx = (text[c] % 16) * fontw;

		int fy = (text[c] / 16) * fonth;
		
		for (int _x = 0; _x < fontw; _x++)
		{
			for (int _y = 0; _y < fonth; _y++)
			{
				if (font[(fy + _y) * (fontw * 16) + (fx + _x)] == 0)
				{
					pixels[(y + _y) * width + ((c * fontw) + x + _x)] = color;
				}
			}
		}
	}
}