#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#include "Arduino.h"
#include <SoftwareSerial.h>

class CommunicationManager {
public:
    CommunicationManager(int PIN_RX_XBEE, int PIN_TX_XBEE, int PIN_SLEEP_XBEE, unsigned long KEEPALIVE_THREASHOLD_MS);

    bool isKeepAliveNeeded(unsigned long nowTimestamp);
    void sendData(int ID, bool light, int distance, unsigned long nowTimestamp);
    unsigned long getMinDelayBeforAction(unsigned long nowTimestamp);

private:
    SoftwareSerial* xBee;
    int PIN_SLEEP_XBEE;
    unsigned long KEEPALIVE_THREASHOLD_MS;

    unsigned long lastSend;

    unsigned long getTimestampDiff(unsigned long start, unsigned long end);
    void WakeXBee();
    void SleepXBee();
};

#endif
