#pragma once

// Element types.

enum el_t: unsigned short
{
	el_null,

	el_wice,
	el_watr,
	el_wvpr,

	el_ston,
	el_sand,

	el_last
};

// Element names.

std::string enmap[el_last] =
{
	"NULL",

	"WICE",
	"WATR",
	"WVPR",

	"STON",
	"SAND"
};

// Element descriptions.

std::string edmap[el_last] =
{
	"An absence of matter",

	"Ice, derived from water",
	"Water, a pure liquid",
	"Water vapour, derived from water",

	"Stone, a hard solid",
	"Sand, a powdery substance"
};

// Color map.

unsigned int cmap[el_last] =
{
	0x00000000,

	0x00A0C0FF,
	0x002030D0,
	0x00A0A0FF,

	0x00A0A0A0,
	0x00FFD090
};