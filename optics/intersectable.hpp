#pragma once

// All intersectable types.

enum intersectable_type
{
	intersectable_reflective_segment,

	intersectable_refractive_segment,

	intersectable_strobe_filter_segment,

	intersectable_subtractive_filter_segment
};

// An intersectable type (base class).

struct intersectable
{
	intersectable_type type;
};