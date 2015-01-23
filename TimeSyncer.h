#pragma once

#include "Time.h"
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

class TimeSyncer {
	EthernetUDP Udp;
	IPAddress timeServer;
	short timeZone;

	void initNetworking();

	void sendNTPpacket(IPAddress &address);
	time_t getCurrentTimeNtp();
	time_t getCurrentTimeRTC();
public:
	TimeSyncer(int timeZone, bool syncWithInternet);
	time_t getCurrentTime();
};