#include "CommunicationManager.h"
#include "Arduino.h"
#include <Math.h>
#include <SoftwareSerial.h>

#define DEBUG //comment: off | uncomment: on
//#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

#define TIMEOUT_KEEPALIVE_MS 10000
#define THRESHOLD_DIFF_DISTANCE 1

CommunicationManager::CommunicationManager(int PIN_RX_XBEE, int PIN_TX_XBEE, int PIN_SLEEP_XBEE) {
    CommunicationManager::PIN_SLEEP_XBEE = PIN_SLEEP_XBEE;

    xBee = new SoftwareSerial(PIN_RX_XBEE, PIN_TX_XBEE); // RX, TX
    xBee->begin(9600);

    pinMode(PIN_SLEEP_XBEE, OUTPUT);
}

bool CommunicationManager::isTransmissionNeeded(bool light, int distance) {
    bool isNeeded = false;
    if (lastLight == -1 || light != lastLight) {
#ifdef DEBUG
        Serial.print("[.] Change in light: ");
        Serial.print(lastLight);
        Serial.print("-->");
        Serial.println(light);
#endif

        lastLight = light;
        isNeeded = true;
    }

    if (lastDistance == -1 || abs(distance - lastDistance) > THRESHOLD_DIFF_DISTANCE) {
#ifdef DEBUG
        Serial.print("[.] Change in proximity: ");
        Serial.print(lastDistance);
        Serial.print("-->");
        Serial.println(distance);
#endif

        lastDistance = distance;
        isNeeded = true;
    }

    return isNeeded;
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

    if (lastSend == 0 || abs(nowTimestamp - lastSend) > TIMEOUT_KEEPALIVE_MS) {
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

void CommunicationManager::WakeXBee() {
    digitalWrite(PIN_SLEEP_XBEE, LOW);
    delay(100); //wait for the XBee to wake up
}

void CommunicationManager::SleepXBee() {
    digitalWrite(PIN_SLEEP_XBEE, HIGH);
}