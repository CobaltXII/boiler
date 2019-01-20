#ifndef __TERMINAL_EXTENSION_BOILER__
#define __TERMINAL_EXTENSION_BOILER__

// Image metrics.

const unsigned int tfi_w = 288;
const unsigned int tfi_h = 128;

// Glyph metrics.

const unsigned int tf_w = 9;
const unsigned int tf_h = 16;

unsigned char __vga__m[4] =
{
	0x00,
	0x55,
	0xAA,
	0xFF
};

// Original VGA color palette.

unsigned int __vga__
(
	unsigned char v,
	unsigned char g,
	unsigned char a
)
{
	return __vga__m[v] << 16 | __vga__m[g] << 8 | __vga__m[a];
}

unsigned int vga_rgb[16] =
{
	__vga__(0, 0, 0), // Black
	__vga__(0, 0, 2), // Dark blue
	__vga__(0, 2, 0), // Dark green
	__vga__(0, 2, 2), // Dark cyan
	__vga__(2, 0, 0), // Dark red
	__vga__(2, 0, 2), // Dark pink
	__vga__(2, 1, 0), // Dark yellow
	__vga__(2, 2, 2), // Gray
	__vga__(1, 1, 1), // Dark gray
	__vga__(1, 1, 3), // Blue
	__vga__(1, 3, 1), // Green
	__vga__(1, 3, 3), // Cyan
	__vga__(3, 1, 1), // Red
	__vga__(3, 1, 3), // Pink
	__vga__(3, 3, 1), // Yellow
	__vga__(3, 3, 3)  // White
};

};

#endif