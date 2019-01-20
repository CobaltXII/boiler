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
};