#include "NeopixelClockHelper.h"

void NeopixelClock::invalidate() {	
	//Merge the colors now
	for (int i = 0; i < pixelCount; i++) {
		Color c = secondPixels[i] + minutePixels[i] + hourPixels[i];
		setPixelColor(i, NULL, c);
	}

	strip->show();
}

void NeopixelClock::resetClock(Color color) {
	for (int i = 0; i < pixelCount; i++) {
		secondPixels[i] = color;
		minutePixels[i] = Colors::black;
		hourPixels[i] = Colors::black;
		setPixelColor(i, NULL, color);
	}
}

void NeopixelClock::resetClock() {
	resetClock(Colors::black);
}

void NeopixelClock::setPixelColor(int position, Color *pixels, Color color) {
	if (pixels)
		pixels[position] = color;

	uint8_t r;
	uint8_t g;
	uint8_t b;

	if (color.r > 255) r = 255;
	else if (color.r < 0) r = 0;
	else r = (uint8_t)color.r;

	if (color.g > 255) g = 255;
	else if (color.g < 0) g = 0;
	else g = (uint8_t)color.g;

	if (color.b > 255) b = 255;
	else if (color.b < 0) b = 0;
	else b = (uint8_t)color.b;

	strip->setPixelColor(position, r, g, b);
}

void NeopixelClock::setHand(float position, Color *pixels, Color color, int sideLength, float fadeLevel) {
	//First, find the adjacent floored integer position
	int center = (int)position;

	//Calculate the faded factor
	float fadeFactor = position - (float)center;

	//Create the original values
	setPixelColor(center, pixels, color);
	
	short next;
	short prev;
	Color fadeColor = Color(color);

	for (int i = 1; i < sideLength + 1; i++) {
		fadeColor = fadeColor * fadeLevel;
		next = (center + i) % pixelCount;
		//Arduino doesn't do negative modulo so we have to do it ourselves
		prev = center - i;
		if (prev < 0)
			prev += pixelCount;

		setPixelColor(next, pixels, pixels[next] + fadeColor);
		setPixelColor(prev, pixels, pixels[prev] + fadeColor);
	}

	//Now shift the color along by the amount specified by fadeFactor
	int leftMost = center - sideLength;
	if (leftMost < 0)
		leftMost = pixelCount + leftMost;

	for (int i = 2 * sideLength; i >= 0; i--) {
		int current = (leftMost + i) % pixelCount;
		int next = (current + 1) % pixelCount;
		Color colorFactored = Color(pixels[current]) * fadeFactor;

		//Increase the next pixel by the factored color
		setPixelColor(next, pixels, pixels[next] + colorFactored);
		//... and decrease our current pixel by the same amount.
		setPixelColor(current, pixels, pixels[current] - colorFactored);
	}
}

void NeopixelClock::setTime(int hour, int minute, int second, uint16_t millis) {
	setHand(((float)(hour) * 5) + ((float)(minute) / 60), hourPixels, hourColor, hourSideLength, hourFadeLevel);
	setHand((float)minute, minutePixels, minuteColor, minuteSideLength, minuteFadeLevel);
	setHand((float)second + ((float)(millis) / 1000), secondPixels, secondColor, secondSideLength, secondFadeLevel);

	invalidate();
}