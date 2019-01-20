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
};