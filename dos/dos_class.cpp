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

int menu_callback(void* userdata, std::string label, int tab, int index);

struct game: boiler
{	
	dos_gui GUI = dos_gui(80, 25, this, menu_callback, this);

	int state = STATE_NORMAL;

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

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_ESCAPE)
		{
			GUI.locked_menus = false;
			
			state = STATE_NORMAL;
		}
	}

	void draw() override
	{
		clear(vga_rgb[vga_dark_blue]);

		GUI.render();

		if (state == STATE_DIALOG)
		{
			GUI.draw_dialog("This is a title", "This is a caption", "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.");
		}

		GUI.display();
	}
};

int menu_callback(void* userdata, std::string label, int tab, int index)
{
	((game*)userdata)->state = STATE_DIALOG;

	return DOS_MIR_HIDE_LOCK;
}

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