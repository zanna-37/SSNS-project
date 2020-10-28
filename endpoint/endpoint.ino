#include "src/Sleep_n0m1_custom/Sleep_n0m1_custom.h"
#include "src/LightManager/LightManager.h"
#include "src/ProximityManager/ProximityManager.h"
#include "src/CommunicationManager/CommunicationManager.h"
#include "src/DataManager/DataManager.h"

#define DEBUG //comment: off | uncomment: on
#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

#define PIN_RX_XBEE 2
#define PIN_TX_XBEE 3
#define PIN_SLEEP_XBEE 9
#define PIN_LIGHT A5
#define PIN_TRIG 10
#define PIN_ECHO 11

#define SLEEP_MS 4000
#define KEEPALIVE_THREASHOLD_MS 10000
#define ID_ENDPOINT 8 // CHANGE FOR EVERY DEVICE

Sleep sleep;
LightManager lightMngr(PIN_LIGHT);
ProximityManager proximityMngr(PIN_TRIG, PIN_ECHO);
CommunicationManager commMngr(PIN_RX_XBEE, PIN_TX_XBEE, PIN_SLEEP_XBEE, KEEPALIVE_THREASHOLD_MS);
DataManager dataMngr;

void setup() {
    Serial.begin(9600);

#ifdef DEBUG
    Serial.println();
    Serial.println("##############");
    Serial.println("#  STARTING  #");
    Serial.println("##############");
    Serial.println();
    Serial.print("TYPE           : ENDPOINT "); Serial.println(ID_ENDPOINT);
    Serial.println("PIN_RX_XBEE    : " + String(PIN_RX_XBEE));
    Serial.println("PIN_TX_XBEE    : " + String(PIN_TX_XBEE));
    Serial.println("PIN_SLEEP_XBEE : " + String(PIN_SLEEP_XBEE));
    Serial.println("PIN_LIGHT      : " + String(PIN_LIGHT));
    Serial.println("PIN_TRIG       : " + String(PIN_TRIG));
    Serial.println("PIN_ECHO       : " + String(PIN_ECHO));
    Serial.println("--------------");
#endif

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    bool light = lightMngr.getLight();
    int distance = proximityMngr.getProximity();
    unsigned long nowTimestamp = getElapsedRealTime();

    bool sendData = false;

    if (dataMngr.isTransmissionNeeded(light, distance, nowTimestamp)) {
        sendData = true;
    }
    if (commMngr.isKeepAliveNeeded(nowTimestamp)) {
        sendData = true;
    }

    if (sendData) {
        dataMngr.updateData(light, distance, nowTimestamp);
        commMngr.sendData(ID_ENDPOINT, light, distance, nowTimestamp);
    }

#ifdef DEBUG
    Serial.println("[.] Zzz");
#endif

    Serial.flush();
    ArduinoSleep(SLEEP_MS);
}

unsigned long getElapsedRealTime() {
    return sleep.WDTMillis();
}

void ArduinoSleep(int mills) {
    digitalWrite(LED_BUILTIN, LOW);

    sleep.pwrDownMode();
    sleep.sleepDelay(mills);

    digitalWrite(LED_BUILTIN, HIGH);
}




