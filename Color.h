#pragma once

#include <stdint.h>
typedef uint8_t color_t;

struct Color {
	color_t r;
	color_t g;
	color_t b;

	Color() : r(0), g(0), b(0) {
	}
	Color(color_t r, color_t g, color_t b) : r(r), g(g), b(b) {
	}
	Color(uint32_t p) {
		r = (p >> 16) & 0xff;
		g = (p >> 8) & 0xff;
		b = p & 0xff;
	}

	Color operator-(const Color c) {
		return Color(r - c.r, g - c.g, b - c.b);
		//color_t newR = (short)r - (short)c.r >= 0 ? r - c.r : 0;
		//color_t newG = (short)g - (short)c.g >= 0 ? g - c.g : 0;
		//color_t newB = (short)b - (short)c.b >= 0 ? b - c.b : 0;
		//return Color(newR, newG, newB);
	}

	Color operator+(const Color c) {
		//color_t newR = (short)r + (short)c.r < 256 ? r + c.r : 255;
		//color_t newG = (short)g + (short)c.g < 256 ? g + c.g : 255;
		//color_t newB = (short)b + (short)c.b < 256 ? b + c.b : 255;
		//return Color(newR, newG, newB);
		return Color(r + c.r, g + c.g, b + c.b);
	}

	Color operator+(const uint32_t c) {
		color_t cr = (c >> 16) & 0xff;
		color_t cg = (c >> 8) & 0xff;
		color_t cb = c & 0xff;
		return Color(r + cr, g + cg, b + cb);
	}

	void operator-=(const Color c) {
		r -= c.r;
		g -= c.g;
		b -= c.b;
	}

	void operator+=(const Color c) {
		r += c.r;
		g += c.g;
		b += c.b;
	}

	Color operator*(const float f) {
		color_t tR = r * f;
		color_t tG = g * f;
		color_t tB = b * f;
		if (tR == 0 && r != 0)
			tR = 1;
		if (tG == 0 && g != 0)
			tG = 1;
		if (tB == 0 && b != 0)
			tB = 1;

		return Color(tR, tG, tB);
	}

	Color operator/(const int f) {
		return Color(r / f, g / f, b / f);
	}
};


struct Colors {
	static const Color red;
	static const Color green;
	static const Color blue;
	static const Color black;
	static const Color white;
};
