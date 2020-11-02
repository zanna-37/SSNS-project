#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#include "Arduino.h"
#include <SoftwareSerial.h>
#include "xbee-arduino-master/XBee.h"

class CommunicationManager {
public:
    CommunicationManager(int PIN_RX_XBEE, int PIN_TX_XBEE, int PIN_SLEEP_XBEE, unsigned long KEEPALIVE_THREASHOLD_MS, bool isRouter);

    bool isKeepAliveNeeded(unsigned long nowTimestamp);
    void sendData(int ID, bool light, int distance, unsigned long nowTimestamp);
    unsigned long getMinDelayBeforeAction(unsigned long nowTimestamp);

private:
    SoftwareSerial* xBeeSerial;
    XBee* xBee;
    XBeeAddress64 address;

    int PIN_SLEEP_XBEE;
    unsigned long KEEPALIVE_THREASHOLD_MS;
    bool isRouter;

    unsigned long lastSend;

    unsigned long getTimestampDiff(unsigned long start, unsigned long end);
    void wakeXBeeIfNeeded();
    void sleepXBeeIfNeeded();
};

#endif
