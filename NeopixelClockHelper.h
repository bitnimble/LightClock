#pragma once

#include "Adafruit_NeoPixel.h"
#include "Color.h"

#define secondSideLength 2
#define secondFadeLevel 0.3f
#define minuteSideLength 0
#define minuteFadeLevel 1.0f
#define hourSideLength 1	
#define hourFadeLevel 0.3f



class NeopixelClock {
	Adafruit_NeoPixel *strip;
	int pixelCount;

	//FIXME: make 60 a #define variable
	Color secondPixels[60];
	Color minutePixels[60];
	Color hourPixels[60];

	Color secondColor;
	Color minuteColor;
	Color hourColor;

	void invalidate();
	void setPixelColor(int position, Color *pixels, Color color);
	void setHand(float position, Color *pixels, Color color, int sideLength, float fadeLevel);
	void setHourHand(int hour, int minute);
public:
	NeopixelClock(Adafruit_NeoPixel *strip, int pixelCount) : strip(strip), pixelCount(pixelCount) {
	}

	void resetClock();
	void resetClock(Color color);
	void setTime(int hour, int minute, int second, uint16_t millis);
	void setSecondColor(Color c) {
		secondColor = c;
	}
	void setMinuteColor(Color c) {
		minuteColor = c;
	}
	void setHourColor(Color c) {
		hourColor = c;
	}
};