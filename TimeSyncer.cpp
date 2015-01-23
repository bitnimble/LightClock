#include "TimeSyncer.h"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <Wire.h>
#include "DS1307RTC.h"

//54-53-ED-38-FB-AE
byte mac[] = { 0x54, 0x53, 0xED, 0x38, 0xFB, 0xAE };

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

void TimeSyncer::initNetworking() {
	//Init ethernet and set time from time server
}

TimeSyncer::TimeSyncer(int timeZone, bool syncWithInternet) : timeZone(timeZone) {
	if (syncWithInternet) {
		Serial.println(F("Starting ethernet..."));
		int status = Ethernet.begin(mac);
		if (status != 0) {
			Serial.print(F("IP Address: "));
			Serial.println(Ethernet.localIP());

			Serial.println(F("Starting UDP..."));
			Udp.begin(8888);
			Serial.println(F("UDP started."));
			//timeServer = IPAddress(64, 4, 10, 33);
			timeServer = IPAddress(192, 189, 54, 33);

			time_t currentTime = getCurrentTimeNtp();
			if (currentTime != 0) {
				RTC.set(currentTime);
				setTime(currentTime);
			}

			if (timeStatus() != timeSet)
				Serial.println(F("Unable to sync with the RTC"));
			else
				Serial.println(F("RTC has synced"));

		} else {
			//Error, could not connect
			Serial.println(F("Ethernet connection failed, not syncing with internet!"));
		}
	}
}

void TimeSyncer::sendNTPpacket(IPAddress &address) {
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 227;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;
	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:                 
	Udp.beginPacket(address, 123); //NTP requests are to port 123
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket();
}

time_t TimeSyncer::getCurrentTimeNtp() {
	while (Udp.parsePacket() > 0); // discard any previously received packets
	Serial.println(F("Transmit NTP Request"));
	sendNTPpacket(timeServer);
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1000) {
		int size = Udp.parsePacket();
		if (size >= NTP_PACKET_SIZE) {
			Serial.println(F("Receive NTP Response"));
			Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
			unsigned long secsSince1900;
			// convert four bytes starting at location 40 to a long integer
			secsSince1900 = (unsigned long)packetBuffer[40] << 24;
			secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
			secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
			secsSince1900 |= (unsigned long)packetBuffer[43];
			return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
		}
	}
	Serial.println(F("No NTP Response :-("));
	return 0; // return 0 if unable to get the time
}

time_t TimeSyncer::getCurrentTimeRTC() {
	//return now();
}

time_t TimeSyncer::getCurrentTime() {
	return RTC.get();
}

