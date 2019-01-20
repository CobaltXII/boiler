// DOS gui? Awesome.

#include "../terminal/terminal_codepage.h"
#include "../terminal/terminal_extras.h"

#include "../boiler/boiler.h"

#include <string>
#include <vector>
#include <utility>
#include <iostream>

// Fetch the pixel at the horizontal offset 'x' and the vertical offset 'y' 
// from the top-left corner of the glyph 'ascii'.

inline unsigned char fetch_glyph(unsigned int x, unsigned int y, unsigned char ascii)
{
	return __437__[(ascii / (tfi_w / tf_w) * tf_h + y) * tfi_w + (ascii % (tfi_w / tf_w) * tf_w) + x];
}

// Join the colors 'foreground' and 'background' (4-bit unsigned integers) in
// to one 8-bit unsigned integer.

inline unsigned char join_colors(unsigned char foreground, unsigned char background)
{
	return foreground << 4 | background;
}

// An instance of the Boiler framework.

struct game: boiler
{	
	// Check if the mouse pointer is within a character.

	inline bool within_character(unsigned int x, unsigned int y)
	{
		return
		(
			mouse_x >= x * tf_w && mouse_x < (x + 1) * tf_w &&
			mouse_y >= y * tf_h && mouse_y < (y + 1) * tf_h
		);
	}

	// 'map1' is an array that holds the ASCII character code of each
	// character on the screen. 'map2' is an array that holds the color 
	// information of each character on the screen.

	unsigned char* map1;
	unsigned char* map2;

	// Assign the character code 'ascii' and the colors 'foreground' and
	// 'background' to 'map1' and 'map2' at the horizontal offset 'x' and the
	// vertical offset 'y'.

	inline void assign
	(
		unsigned int x, 
		unsigned int y, 

		unsigned char ascii, 

		unsigned char foreground, 
		unsigned char background
	)
	{
		// Assign 'ascii' to 'map1'.

		map1[y * chx_res + x] = ascii;

		// Assign 'foreground' and 'background' to 'map2'.

		map2[y * chx_res + x] = join_colors(foreground, background);
	}

	// The main menu tabs. These tabs will be displayed on a menu bar at the
	// top of the screen.

	std::vector<std::string> main_menu =
	{
		"Test 1", "Test 2", "Test 3", "Test 4", "Test 5"
	};

	// The contents of each main menu. These contents will be displayed under
	// their corresponding tab when the mouse pointer hovers over it.

	std::vector<std::vector<std::string>> main_menu_contents =
	{
		{"Test 1 Subitem 1", "Test 1 Subitem 2", "Test 1 Subitem 3", "", "Test 1 Subitem 4", "Test 1 Subitem 5"},
		{"Test 2 Subitem 1", "Test 2 Subitem 2", "Test 2 Subitem 3", "", "Test 2 Subitem 4", "Test 2 Subitem 5"},
		{"Test 3 Subitem 1", "Test 3 Subitem 2", "Test 3 Subitem 3", "", "Test 3 Subitem 4", "Test 3 Subitem 5"},
		{"Test 4 Subitem 1", "Test 4 Subitem 2", "Test 4 Subitem 3", "", "Test 4 Subitem 4", "Test 4 Subitem 5"},
		{"Test 5 Subitem 1", "Test 5 Subitem 2", "Test 5 Subitem 3", "", "Test 5 Subitem 4", "Test 5 Subitem 5"}
	};

	// Width of each menu tab's dropdown. This will be calculated at runtime 
	// for ease of access.

	std::vector<unsigned int> main_menu_widths;

	// The currently selected menu tab's index is stored in 
	// 'selected_menu_tab'. 'selected_menu_tab' is -1 when no menu tab is 
	// currently selected.

	int selected_menu_tab = -1;

	// The horizontal offset (in characters) of the left edge of the currently
	// selected menu tab is stored in selected_menu_tab_x_offset.

	int selected_menu_tab_x_offset = 2;

	// Resolution/dimensions in characters.

	int chx_res = 80;
	int chy_res = 25;

	// Resolution/dimensions in pixels.

	int x_res = chx_res * tf_w;
	int y_res = chy_res * tf_h;

	// Initialize Boiler.

	void steam() override
	{
		// Pass the precalculated dimensions (in pixels) to the Boiler 
		// framework, to use as the window's dimensions.

		width = x_res;

		height = y_res;

		// Allocate a block of memory for 'map1' and 'map2'.

		map1 = (unsigned char*)malloc(chx_res * chy_res * sizeof(unsigned char));
		map2 = (unsigned char*)malloc(chx_res * chy_res * sizeof(unsigned char));

		// Set the window's title to "DOS GUI (using Boiler)".

		title = "DOS GUI (using Boiler)";

		// Calculate the width of each menu tab's dropdown and store it in
		// 'main_menu_widths'.

		for (int i = 0; i < main_menu_contents.size(); i++)
		{
			int max_width = 16;

			for (int j = 0; j < main_menu_contents[i].size(); j++)
			{
				if (main_menu_contents[i][j].size() > max_width)
				{
					max_width = main_menu_contents[i][j].size();
				}
			}

			main_menu_widths.push_back(max_width);
		}
	}

	// Draw a frame using Boiler.

	void draw() override
	{
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