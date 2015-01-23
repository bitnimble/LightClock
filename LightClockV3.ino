#include "Color.h"
#include "NeopixelClockHelper.h"
#include "TimeSyncer.h"
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Wire.h>

#define frametime 20

Adafruit_NeoPixel *strip;
NeopixelClock *neoclock;
TimeSyncer *syncer;

int hourPos = 0;
int minutePos = 0;
int secondPos = 0;
uint16_t millisPos = 0;

int get_free_memory() {
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

int hourTwelve(time_t t) {
	int _hour = hour(t);
	if (_hour >= 12)
		_hour -= 12;
	return _hour;
}

void syncTime() {
	time_t t = syncer->getCurrentTime();
	//setTime(t);
	millisPos = 0;
	hourPos = hourTwelve(t);
	minutePos = minute(t);
	secondPos = second(t);

	neoclock->setTime(hourPos, minutePos, secondPos, millisPos);
}

void setup()
{
	Serial.begin(9600);
	Serial.println(F("Starting..."));

	//Timezone is 10 in Brisbane
	syncer = new TimeSyncer(10, false);

	//Init LED strip
	strip = new Adafruit_NeoPixel(60);
	strip->begin();
	//strip->setBrightness(128);

	neoclock = new NeopixelClock(strip, 60);
	neoclock->setSecondColor(Colors::red);
	neoclock->setMinuteColor(Colors::green);
	neoclock->setHourColor(Colors::blue);


	//Init timer1
	noInterrupts();
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 1249; // compare match register set to 50Hz
	TCCR1B |= (1 << WGM12); // CTC mode
	TCCR1B |= (1 << CS12); // 256 prescaler
	TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt

	//Re-enable interrupts
	interrupts();

	Serial.println(get_free_memory());
	Serial.println(F("Done!"));

	syncTime();
}

ISR(TIMER1_COMPA_vect) {
	neoclock->resetClock();
	millisPos += frametime;
	if (millisPos >= 1000) {
		millisPos -= 1000;
		secondPos++;
		if (secondPos == 60) {
			secondPos = 0;
			minutePos++;
			if (minutePos == 60) {
				syncTime();
				minutePos = 0;
				hourPos++;
				if (hourPos == 12) {
					hourPos = 0;
				}
			}
		}
	}
	neoclock->setTime(hourPos, minutePos, secondPos, millisPos);
}

void printCurrentTime() {
	Serial.print(hourPos);
	Serial.print(F(" "));
	Serial.print(minutePos);
	Serial.print(F(" "));
	Serial.print(secondPos);
	Serial.print(F(" "));
	Serial.println(millisPos);
}

void loop()
{
	int first = Serial.read();
	if (first == 65) {
		printCurrentTime();
	} else if (first == 66) {
		Serial.println(get_free_memory());
	} else if (first == 67) {
		syncTime();
	}
}
