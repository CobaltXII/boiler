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

struct gradient
{
	std::vector<gradient_stop> stops;

	inline rgb_tuple lerp(rgb_tuple a, rgb_tuple b, float t)
	{
		return
		{
			a.r + (b.r - a.r) * t,
			a.g + (b.g - a.g) * t,
			a.b + (b.b - a.b) * t
		};
	}

	inline rgb_tuple sample(float t)
	{
		if (stops.size() == 0)
		{
			return {0.0f, 0.0f, 0.0f};
		}
		else if (stops.size() == 1)
		{
			return stops[0].color;
		}
		else
		{
			gradient_stop g1 = gradient_stop(0.0f - INFINITY, {0.0f, 0.0f, 0.0f});
			gradient_stop g2 = gradient_stop(0.0f + INFINITY, {0.0f, 0.0f, 0.0f});

			for (int i = 0; i < stops.size(); i++)
			{
				if (stops[i].position == t)
				{
					return stops[i].color;
				}

				if (stops[i].position > g1.position && stops[i].position < t)
				{
					g1 = stops[i];
				}

				if (stops[i].position < g2.position && stops[i].position > t)
				{
					g2 = stops[i];
				}
			}

			return lerp(g1.color, g2.color, (t - g1.position) * (1.0f / (g2.position - g1.position)));
		}
	}

	inline unsigned int sample_uint(float t)
	{
		rgb_tuple color = sample(t);

		unsigned char r = color.r * 255;
		unsigned char g = color.g * 255;
		unsigned char b = color.b * 255;

		return r << 16 | g << 8 | b;
	}

	std::string encode()
	{
		std::stringstream builder;

		for (int i = 0; i < stops.size(); i++)
		{
			builder << stops[i].position;

			builder << " " << stops[i].color.r;
			builder << " " << stops[i].color.g;
			builder << " " << stops[i].color.b;

			if (i != stops.size() - 1)
			{
				builder << " ";
			}
		}

		return builder.str();
	}
};
