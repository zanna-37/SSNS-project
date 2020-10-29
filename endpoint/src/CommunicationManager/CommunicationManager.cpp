#include "CommunicationManager.h"
#include "Arduino.h"
#include <Math.h>
#include <SoftwareSerial.h>

#define DEBUG //comment: off | uncomment: on
//#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

CommunicationManager::CommunicationManager(int PIN_RX_XBEE, int PIN_TX_XBEE, int PIN_SLEEP_XBEE, unsigned long KEEPALIVE_THREASHOLD_MS) {
    CommunicationManager::PIN_SLEEP_XBEE = PIN_SLEEP_XBEE;
    CommunicationManager::KEEPALIVE_THREASHOLD_MS = KEEPALIVE_THREASHOLD_MS;
    lastSend = 0;

    xBee = new SoftwareSerial(PIN_RX_XBEE, PIN_TX_XBEE); // RX, TX
    xBee->begin(9600);

    pinMode(PIN_SLEEP_XBEE, OUTPUT);
}

bool CommunicationManager::isKeepAliveNeeded(unsigned long nowTimestamp) {
    bool isNeeded = false;

#ifdef DEBUG
#ifdef VERBOSE
    Serial.print("[.] Timestamp: ");
    Serial.print(lastSend);
    Serial.print("-->");
    Serial.print(nowTimestamp);
    Serial.print(" (diff ");
    Serial.print(abs(nowTimestamp - lastSend));
    Serial.println(")");
#endif
#endif

    if (lastSend == 0 || getTimestampDiff(nowTimestamp, lastSend) > KEEPALIVE_THREASHOLD_MS) {
#ifdef DEBUG
        Serial.print("[.] KeepAlive needed ");
        Serial.print(" (diff ");
        Serial.print(abs(nowTimestamp - lastSend));
        Serial.println(")");
#endif

        isNeeded = true;
    }

    return isNeeded;
}

void CommunicationManager::sendData(int ID, bool light, int distance, unsigned long nowTimestamp) {
    WakeXBee();
#ifdef DEBUG
    Serial.println("[.] Sending data...");
#endif

    xBee->print("{");
    xBee->print(ID);
    xBee->print(",");
    xBee->print(light);
    xBee->print(",");
    xBee->print(distance);
    xBee->println("}");

    xBee->flush();
    SleepXBee();

    lastSend = nowTimestamp;
}

unsigned long CommunicationManager::getMinDelayBeforAction(unsigned long nowTimestamp) {
    return max(
        0,
        (long)KEEPALIVE_THREASHOLD_MS - (long)getTimestampDiff(nowTimestamp, lastSend)
    );
}

void CommunicationManager::WakeXBee() {
    digitalWrite(PIN_SLEEP_XBEE, LOW);
    delay(100); //wait for the XBee to wake up
}

void CommunicationManager::SleepXBee() {
    digitalWrite(PIN_SLEEP_XBEE, HIGH);
}

unsigned long CommunicationManager::getTimestampDiff(unsigned long start, unsigned long end) {
    return start - end;
}