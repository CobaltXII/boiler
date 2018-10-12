#ifndef __TERMINAL_EXTENSION_BOILER__
#define __TERMINAL_EXTENSION_BOILER__

// Glyph metrics.

unsigned int tf_w = 9;
unsigned int tf_h = 16;

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
	__vga__(0, 0, 0),
	__vga__(0, 0, 2),
	__vga__(0, 2, 0),
	__vga__(0, 2, 2),
	__vga__(2, 0, 0),
	__vga__(2, 0, 2),
	__vga__(2, 1, 0),
	__vga__(2, 2, 2),
	__vga__(1, 1, 1),
	__vga__(1, 1, 3),
	__vga__(1, 3, 1),
	__vga__(1, 3, 3),
	__vga__(3, 1, 1),
	__vga__(3, 1, 3),
	__vga__(3, 3, 1),
	__vga__(3, 3, 3)
};

#endif