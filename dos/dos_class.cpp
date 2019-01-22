// Class based DOS GUI? Awesome.

#include "../terminal/terminal_codepage.h"
#include "../terminal/terminal_extras.h"

#include "../boiler/boiler.h"

#include "dos_gui.h"

#include <vector>
#include <utility>
#include <iostream>

#define STATE_NORMAL 0

#define STATE_DIALOG 1
struct game: boiler
{	
	dos_gui GUI = dos_gui(80, 25, this);

	void steam() override
	{
		GUI.main_menu =
		{
			"Test 1", "Test 2", "Test 3", "Test 4", "Test 5"
		};

		GUI.main_menu_contents =
		{
			{"Test 1 Subitem 1", "Test 1 Subitem 2", "Test 1 Subitem 3", "", "Test 1 Subitem 4", "Test 1 Subitem 5"},
			{"Test 2 Subitem 1", "Test 2 Subitem 2", "Test 2 Subitem 3", "", "Test 2 Subitem 4", "Test 2 Subitem 5"},
			{"Test 3 Subitem 1", "Test 3 Subitem 2", "Test 3 Subitem 3", "", "Test 3 Subitem 4", "Test 3 Subitem 5"},
			{"Test 4 Subitem 1", "Test 4 Subitem 2", "Test 4 Subitem 3", "", "Test 4 Subitem 4", "Test 4 Subitem 5"},
			{"Test 5 Subitem 1", "Test 5 Subitem 2", "Test 5 Subitem 3", "", "Test 5 Subitem 4", "Test 5 Subitem 5"}
		};

		GUI.common_foreground = vga_dark_blue;

		GUI.common_background = vga_gray;

		GUI.finalize();

		width = GUI.x_res;

		height = GUI.y_res;

		title = "Class based DOS GUI (using Boiler)";
	}

	void draw() override
	{
		clear(vga_rgb[vga_dark_blue]);

		GUI.render();
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