#include <vector>
#include <string>
#include <sstream>

struct rgb_tuple
{
	float r;
	float g;
	float b;
};

rgb_tuple rgb_from_hex(std::string hex)
{
	if (hex.size() != 6)
	{
		return {0.0f, 0.0f, 0.0f};
	}

	int raw_literals[6];

	for (int j = 0; j < 6; j++)
	{
		for (int i = 0; i < 16; i++)
		{
			if ("0123456789abcdef"[i] == tolower(hex[j]))
			{
				raw_literals[j] = i;

				break;
			}
		}
	}

	int combined_literals[3];

	for (int i = 0; i < 3; i++)
	{
		combined_literals[i] = raw_literals[i * 2] * 16 + raw_literals[i * 2 + 1];
	}

	float r = combined_literals[0] / 255.0f;
	float g = combined_literals[1] / 255.0f;
	float b = combined_literals[2] / 255.0f;

	return {r, g, b};
}

struct gradient_stop
{
	float position;

	rgb_tuple color;

	gradient_stop(float position, rgb_tuple color)
	{
		this->position = position;

		this->color = color;
	}
};
