#include "CommunicationManager.h"
#include "Arduino.h"
#include <Math.h>
#include <String.h>
#include <SoftwareSerial.h>
#include "xBee-arduino-master/XBee.h"

#define DEBUG //comment: off | uncomment: on
//#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

CommunicationManager::CommunicationManager(int PIN_RX_XBEE, int PIN_TX_XBEE, int PIN_SLEEP_XBEE, unsigned long KEEPALIVE_THREASHOLD_MS, bool isRouter) {
    CommunicationManager::PIN_SLEEP_XBEE = PIN_SLEEP_XBEE;
    CommunicationManager::KEEPALIVE_THREASHOLD_MS = KEEPALIVE_THREASHOLD_MS;
    CommunicationManager::isRouter = isRouter;
    lastSend = 0;

    address = XBeeAddress64(0x0, 0x0);

    xBeeSerial = new SoftwareSerial(PIN_RX_XBEE, PIN_TX_XBEE); // RX, TX
    xBeeSerial->begin(9600);

    xBee = new XBee;
    xBee->setSerial(*xBeeSerial);

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
    Serial.print(getTimestampDiff(nowTimestamp, lastSend));
    Serial.println(")");
    #endif
    #endif

    if (lastSend == 0 || getTimestampDiff(nowTimestamp, lastSend) > KEEPALIVE_THREASHOLD_MS) {
        #ifdef DEBUG
        Serial.print("[.] KeepAlive needed ");
        Serial.print(" (diff ");
        Serial.print(getTimestampDiff(nowTimestamp, lastSend));
        Serial.println(")");
        #endif

        isNeeded = true;
    }

    return isNeeded;
}

void CommunicationManager::sendData(int ID, bool light, int distance, unsigned long nowTimestamp) {
    wakeXBeeIfNeeded();
    #ifdef DEBUG
    Serial.println("[.] Sending data...");
    #endif

    String payload_str = "";
    payload_str += "{";
    payload_str += ID;
    payload_str += ",";
    payload_str += light;
    payload_str += ",";
    payload_str += distance;
    payload_str += "}";

    uint8_t* payload;
    int payload_length = payload_str.length();
    payload = new uint8_t(payload_length);
    for (int i = 0; i < payload_length; i++) {
        payload[i] = (uint8_t)payload_str[i];
    }

    #ifdef DEBUG
    #ifdef VERBOSE
    for (int i = 0; i < payload_length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    #endif
    #endif

    ZBTxRequest req = ZBTxRequest(address, payload, payload_length);
    ZBTxStatusResponse txStatus = ZBTxStatusResponse();

    xBee->send(req);
    if (xBee->readPacket(500)) {
        // got a response!

        // should be a znet tx status
        if (xBee->getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
            xBee->getResponse().getZBTxStatusResponse(txStatus);

            // get the delivery status, the fifth byte

            if (txStatus.getDeliveryStatus() == SUCCESS) {
                #ifdef DEBUG
                Serial.println("[+] Transmission succeed");
                #endif
            } else {
                // the remote XBee did not receive our packet. is it powered on?
                #ifdef DEBUG
                Serial.print("[!] Transmission failed: 0x");
                Serial.println(txStatus.getDeliveryStatus(), HEX);
                #endif
            }
        } else {
            #ifdef DEBUG
            Serial.print("[!] Wrong API Id: 0x");
            Serial.println(xBee->getResponse().getApiId());
            #endif
        }
    } else if (xBee->getResponse().isError()) {
        #ifdef DEBUG
        Serial.print("[-] Transmission error: ");
        Serial.println(xBee->getResponse().getErrorCode());
        #endif
    } else {
        // local XBee did not provide a timely TX Status Response -- should not happen
        #ifdef DEBUG
        Serial.println("[-] Transmission error");
        Serial.println(" |-> did you enable AP: API 2 with escaping?");
        Serial.println(" |-> otherwise likely hardware related");
        #endif
    }

    /*xBee->print("{");
    xBee->print(ID);
    xBee->print(",");
    xBee->print(light);
    xBee->print(",");
    xBee->print(distance);
    xBee->println("}");

    xBee->flush();*/
    sleepXBeeIfNeeded();

    lastSend = nowTimestamp;
}

unsigned long CommunicationManager::getMinDelayBeforeAction(unsigned long nowTimestamp) {
    return max(
        0,
        (long)KEEPALIVE_THREASHOLD_MS - (long)getTimestampDiff(nowTimestamp, lastSend) //cast to long to avoid underflow
    );
}

void CommunicationManager::wakeXBeeIfNeeded() {
    if (isRouter) { return; }
    digitalWrite(PIN_SLEEP_XBEE, LOW);
    delay(200); //wait for the XBee to wake up
}

void CommunicationManager::sleepXBeeIfNeeded() {
    if (isRouter) { return; }
    digitalWrite(PIN_SLEEP_XBEE, HIGH);
}

unsigned long CommunicationManager::getTimestampDiff(unsigned long start, unsigned long end) {
    return start - end;
}