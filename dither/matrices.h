#pragma once

// Dither matrices are two-dimensional matrices of integer values. They do not need to be squares,
// and the values may be anything. They can be represented with vectors.

typedef std::vector<std::vector<int>> d_matrix;

// The classic 2 by 2 Bayer dither matrix.

d_matrix bayer_2 =
{
	{
		0, 3
	},
	{
		2, 1
	}
};

// The classic 4 by 4 Bayer dither matrix.

d_matrix bayer_4 =
{
	{
		0, 12, 13, 15
	},
	{
		8, 4, 11, 7
	},
	{
		2, 14, 1, 13
	},
	{
		10, 6, 8, 5
	}
};

// The classic 8 by 8 Bayer dither matrix.

d_matrix bayer_8 =
{
	{
		0, 48, 12, 60, 3, 51, 15, 63
	},
	{
		32, 16, 44, 28, 35, 19, 47, 31
	},
	{
		8, 56, 4, 52, 11, 59, 7, 55,
	},
	{
		40, 24, 36, 20, 43, 27, 39, 23
	},
	{
		2, 50, 14, 62, 1, 49, 13, 61
	},
	{
		34, 18, 46, 30, 33, 17, 45, 29
	},
	{
		10, 58, 6, 54, 9, 57, 5, 53
	},
	{
		42, 26, 38, 22, 41, 25, 37, 21
	}
};

// The 4 by 4 cluster dot matrix.

d_matrix cluster_4 =
{
	{
		15, 5, 6, 13
	},
	{
		4, 0, 1, 7
	},
	{
		11, 3, 2, 8
	},
	{
		15, 10, 9, 14
	}
};

// The 8 by 8 cluster dot matrix.

d_matrix cluster_8 =
{
	{
		24, 10, 12, 26, 35, 47, 49, 37
	},
	{
		8, 0, 2, 14, 45, 59, 61, 51
	},
	{
		22, 6, 4, 16, 43, 57, 63, 53
	},
	{
		30, 30, 18, 28, 33, 41, 55, 39
	},
	{
		34, 46, 48, 36, 25, 11, 13, 27
	},
	{
		44, 58, 60, 50, 9, 1, 3, 15
	},
	{
		42, 56, 62, 52, 23, 7, 5, 17
	},
	{
		32, 40, 54, 38, 31, 21, 19, 29
	}
};

// A 5 by 3 vertical line matrix.

d_matrix vertical_line =
{
	{
		9, 3, 0, 6, 12
	},
	{
		10, 4, 1, 7, 13
	},
	{
		11, 5, 2, 8, 14
	}
};

// A 3 by 5 horizontal line matrix.

d_matrix horizontal_line =
{
	{
		9, 10, 11
	},
	{
		3, 4, 5
	},
	{
		0, 1, 2
	},
	{
		6, 7, 8
	},
	{
		12, 13, 14
	}
};