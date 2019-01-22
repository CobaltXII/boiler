#include <string>
#include <vector>

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

// Menu item callback type.

typedef bool (*DOS_GUI_MENU_CALLBACK)(void*, std::string, int, int);
struct dos_gui
{
	boiler* parent;

	// Common colors.

	unsigned char common_foreground = vga_black;

	unsigned char common_background = vga_gray;

	// Check if the mouse pointer is within a character.

	inline bool within_character(unsigned int x, unsigned int y)
	{
		return
		(
			parent->mouse_x >= x * tf_w && parent->mouse_x < (x + 1) * tf_w &&
			parent->mouse_y >= y * tf_h && parent->mouse_y < (y + 1) * tf_h
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

	std::vector<std::string> main_menu;

	// The contents of each main menu. These contents will be displayed under
	// their corresponding tab when the mouse pointer hovers over it.

	std::vector<std::vector<std::string>> main_menu_contents;

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

	int chx_res;
	int chy_res;

	// Resolution/dimensions in pixels.

	int x_res;
	int y_res;

	// Constructor.

	dos_gui(int _chx_res, int _chy_res, boiler* _parent)
	{
		chx_res = _chx_res;
		chy_res = _chy_res;

		x_res = chx_res * tf_w;
		y_res = chy_res * tf_h;

		parent = _parent;
	}

	// Finalize and initialize.

	void finalize()
	{
		// Allocate a block of memory for 'map1' and 'map2'.

		map1 = (unsigned char*)malloc(chx_res * chy_res * sizeof(unsigned char));
		map2 = (unsigned char*)malloc(chx_res * chy_res * sizeof(unsigned char));

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

	// Render to parent.

	void render()
	{
		// The following macro 'ENABLE' will set the boolean 'target' to true
		// if 'value' is true, otherwise 'target' is set to itself.

		#define ENABLE(__TARGET, __VALUE) __TARGET = __VALUE ? __VALUE : __TARGET;

		// The following macro 'ASSIGN' will set the variable 'still_selected'
		// to true if within_character(__X, __Y) is true. The following macro 
		// will also call assign(__X, __Y, __ASCII, __FOREGROUND, 
		// __BACKGROUND).

		#define ASSIGN(__X, __Y, __ASCII, __FOREGROUND, __BACKGROUND) assign(__X, __Y, __ASCII, __FOREGROUND, __BACKGROUND); ENABLE(selected_menu_tab_still_selected, within_character(__X, __Y));

		// Clear buffers.

		for (int k = 0; k < chx_res * chy_res; k++)
		{
			map1[k] = 255;
			map2[k] = 255;
		}

		// Draw the main menu strip.

		for (int i = 0; i < chx_res; i++)
		{
			assign(i, 0, 0, common_foreground, common_background);
		}

		// If no menu tab is currently selected (i.e. selected_menu_tab is -1)
		// then find the selected menu tab (if any).

		bool selected_menu_tab_still_selected = false;

		if (selected_menu_tab == -1)
		{
			// No menu tab is currently selected.

			selected_menu_tab_x_offset = 2;

			for (int n = 0; n < main_menu.size(); n++)
			{
				int x_offset_at_start = selected_menu_tab_x_offset;

				std::string main_menu_tab = main_menu[n];

				for (int c = 0; c < main_menu_tab.size(); c++)
				{
					if (within_character(selected_menu_tab_x_offset, 0))
					{
						// The menu tab at index 'n' is currently selected, so
						// write it to 'selected_menu_tab'.

						selected_menu_tab = n;

						selected_menu_tab_x_offset = x_offset_at_start;

						goto done_selected_menu_tab;
					}

					selected_menu_tab_x_offset++;
				}

				selected_menu_tab_x_offset += 2;
			}
		}

		done_selected_menu_tab:

		// Draw the menu tabs. Menu tabs start at horizontal offset 2 and 
		// vertical offset 0.

		int menu_tab_index = 2;

		for (int n = 0; n < main_menu.size(); n++)
		{
			std::string main_menu_tab = main_menu[n];

			if (selected_menu_tab == n)
			{
				// Draw the left bumper.

				ASSIGN(menu_tab_index - 1, 0, 0, common_background, common_foreground);

				// Draw the right bumper.

				ASSIGN(menu_tab_index + main_menu_tab.size(), 0, 0, common_background, common_foreground);
			}

			// Draw the menu tab's text.

			for (int c = 0; c < main_menu_tab.size(); c++)
			{
				if (selected_menu_tab == n)
				{
					// Draw gray text with a black background.

					ASSIGN(menu_tab_index, 0, main_menu_tab[c], common_background, common_foreground);
				}
				else
				{
					// Draw black text with a gray background.

					assign(menu_tab_index, 0, main_menu_tab[c], common_foreground, common_background);
				}

				menu_tab_index++;
			}

			menu_tab_index += 2;
		}

		// Draw the selected menu dropdown (if any).

		if (selected_menu_tab != -1)
		{
			// Vertical lines.

			for (int i = 0; i < main_menu_contents[selected_menu_tab].size(); i++)
			{
				ASSIGN(selected_menu_tab_x_offset - 1, i + 2, 179, common_foreground, common_background);

				ASSIGN(selected_menu_tab_x_offset + main_menu_widths[selected_menu_tab], i + 2, 179, common_foreground, common_background);
			}

			// Horizontal lines.

			for (int i = 0; i < main_menu_widths[selected_menu_tab]; i++)
			{
				ASSIGN(selected_menu_tab_x_offset + i, 1, 196, common_foreground, common_background);

				ASSIGN(selected_menu_tab_x_offset + i, main_menu_contents[selected_menu_tab].size() + 2, 196, common_foreground, common_background);
			}

			// Top-left corner.

			ASSIGN(selected_menu_tab_x_offset - 1, 1, 218, common_foreground, common_background);

			// Top-right corner.

			ASSIGN(selected_menu_tab_x_offset + main_menu_widths[selected_menu_tab], 1, 191, common_foreground, common_background);

			// Bottom-left corner.

			ASSIGN(selected_menu_tab_x_offset - 1, main_menu_contents[selected_menu_tab].size() + 2, 192, common_foreground, common_background);

			// Bottom-right corner.

			ASSIGN(selected_menu_tab_x_offset + main_menu_widths[selected_menu_tab], main_menu_contents[selected_menu_tab].size() + 2, 217, common_foreground, common_background);

			// Fill the menu tab dropdown.

			for (int i = 0; i < main_menu_contents[selected_menu_tab].size(); i++)
			{
				bool is_hover = false;

				for (int j = 0; j < main_menu_widths[selected_menu_tab]; j++)
				{
					ENABLE(is_hover, within_character(selected_menu_tab_x_offset + j, i + 2));
				}

				for (int j = 0; j < main_menu_widths[selected_menu_tab]; j++)
				{
					if (is_hover)
					{
						ASSIGN(selected_menu_tab_x_offset + j, i + 2, 0, common_background, common_foreground);
					}
					else
					{
						ASSIGN(selected_menu_tab_x_offset + j, i + 2, 0, common_foreground, common_background);
					}
				}
			}

			// Write the text inside the menu tab dropdown.

			for (int i = 0; i < main_menu_contents[selected_menu_tab].size(); i++)
			{
				std::string menu_item = main_menu_contents[selected_menu_tab][i];

				if (menu_item.size() == 0)
				{
					// Divider.

					assign(selected_menu_tab_x_offset - 1, i + 2, 195, common_foreground, common_background);

					assign(selected_menu_tab_x_offset + main_menu_widths[selected_menu_tab], i + 2, 180, common_foreground, common_background);

					for (int j = 0; j < main_menu_widths[selected_menu_tab]; j++)
					{
						assign(selected_menu_tab_x_offset + j, 2 + i, 196, common_foreground, common_background);
					}
				}
				else
				{
					// Menu item.

					bool is_hover = false;

					for (int j = 0; j < main_menu_widths[selected_menu_tab]; j++)
					{
						ENABLE(is_hover, within_character(selected_menu_tab_x_offset + j, i + 2));
					}

					if (is_hover)
					{
						for (int j = 0; j < menu_item.size(); j++)
						{
							assign(selected_menu_tab_x_offset + j, 2 + i, menu_item[j], common_background, common_foreground);
						}
					}
					else
					{
						for (int j = 0; j < menu_item.size(); j++)
						{
							assign(selected_menu_tab_x_offset + j, 2 + i, menu_item[j], common_foreground, common_background);
						}
					}
				}
			}
		}

		// Reset the 'selected_menu_tab' if the mouse moved out of the hitbox.

		if (!selected_menu_tab_still_selected)
		{
			selected_menu_tab = -1;
		}

		// Render 'map1' and 'map2' to the screen as rasterized pixel 
		// graphics.

		for (int i = 0; i < chx_res; i++)
		{
			for (int j = 0; j < chy_res; j++)
			{
				unsigned char ascii = map1[j * chx_res + i];

				if (ascii == 255 & map2[j * chx_res + i] == 255)
				{
					continue;		
				}

				for (int x = 0; x < tf_w; x++)
				for (int y = 0; y < tf_h; y++)
				{
					if (fetch_glyph(x, y, ascii))
					{
						parent->plotp(i * tf_w + x, j * tf_h + y, vga_rgb[map2[j * chx_res + i] >> 4]);
					}
					else
					{
						parent->plotp(i * tf_w + x, j * tf_h + y, vga_rgb[map2[j * chx_res + i] & 0xF]);
					}
				}
			}
		}

		// Undefine common macros.

		#undef ENABLE

		#undef ASSIGN
	}
};