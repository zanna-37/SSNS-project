#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#include "Arduino.h"
#include <SoftwareSerial.h>

class CommunicationManager {
	public:
		CommunicationManager(int PIN_RX_XBEE, int PIN_TX_XBEE, int PIN_SLEEP_XBEE);
		
		bool isTransmissionNeeded(bool light, int distance);
		bool isKeepAliveNeeded(unsigned long nowTimestamp);
		void sendData(int ID, bool light, int distance, unsigned long nowTimestamp);
		
	private:
		SoftwareSerial *xBee;
		int PIN_SLEEP_XBEE;
		
		int lastLight = -1;
		int lastDistance = -1;
		unsigned long lastSend = 0;
		
		void WakeXBee();
		void SleepXBee();
};

#endif
