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
