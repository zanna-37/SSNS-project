#include "src/Sleep_n0m1_custom/Sleep_n0m1_custom.h"
#include "src/LightManager/LightManager.h"
#include "src/ProximityManager/ProximityManager.h"
#include "src/CommunicationManager/CommunicationManager.h"
#include "src/DataManager/DataManager.h"

#define DEBUG //comment: off | uncomment: on
#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

#define PIN_RX_XBEE 2
#define PIN_TX_XBEE 3
#define PIN_SENSORS_POWER 12
#define PIN_SLEEP_XBEE 9
#define PIN_LIGHT A5
#define PIN_TRIG 10
#define PIN_ECHO 11

#define ID_ENDPOINT 8 // CHANGE FOR EVERY DEVICE
#define SHORT_SLEEP 2000 //PRODUCTION: 10000
#define DEFER_UNSTABLE_CHANGES_FOR_MS 10000 //PRODUCTION: 30000
#define LONG_SLEEP 15000 //PRODUCTION: 60000
#define KEEPALIVE_THREASHOLD_MS 60000 //PRODUCTION: 1800000 (30min)

static_assert(SHORT_SLEEP < DEFER_UNSTABLE_CHANGES_FOR_MS, "Assert SHORT_SLEEP < DEFER_UNSTABLE_CHANGES_FOR_MS failed");
static_assert(DEFER_UNSTABLE_CHANGES_FOR_MS < LONG_SLEEP, "Assert DEFER_UNSTABLE_CHANGES_FOR_MS < LONG_SLEEP failed");
static_assert(LONG_SLEEP < KEEPALIVE_THREASHOLD_MS, "Assert LONG_SLEEP < KEEPALIVE_THREASHOLD_MS failed");

Sleep sleep;
LightManager lightMngr(PIN_LIGHT);
ProximityManager proximityMngr(PIN_TRIG, PIN_ECHO);
DataManager dataMngr(SHORT_SLEEP, DEFER_UNSTABLE_CHANGES_FOR_MS, LONG_SLEEP);
CommunicationManager commMngr(PIN_RX_XBEE, PIN_TX_XBEE, PIN_SLEEP_XBEE, KEEPALIVE_THREASHOLD_MS);


void setup() {
    Serial.begin(9600);

#ifdef DEBUG
    Serial.println();
    Serial.println("##############");
    Serial.println("#  STARTING  #");
    Serial.println("##############");
    Serial.println();
    Serial.print("TYPE              : ENDPOINT "); Serial.println(ID_ENDPOINT);
    Serial.println("PIN_RX_XBEE       : " + String(PIN_RX_XBEE));
    Serial.println("PIN_TX_XBEE       : " + String(PIN_TX_XBEE));
    Serial.println("PIN_SENSORS_POWER : " + String(PIN_SENSORS_POWER));
    Serial.println("PIN_SLEEP_XBEE    : " + String(PIN_SLEEP_XBEE));
    Serial.println("PIN_LIGHT         : " + String(PIN_LIGHT));
    Serial.println("PIN_TRIG          : " + String(PIN_TRIG));
    Serial.println("PIN_ECHO          : " + String(PIN_ECHO));
    Serial.println("--------------");
#endif

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_SENSORS_POWER, OUTPUT);

    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    wakeSensors();
    bool light = lightMngr.getLight();
    int distance = proximityMngr.getProximity();
    unsigned long nowTimestamp = getElapsedRealTime();
    sleepSensors();

    bool sendData = false;

    if (dataMngr.addIntermediateAndCheckTransmissionNeeded(light, distance, nowTimestamp)) {
        sendData = true;
    }
    if (commMngr.isKeepAliveNeeded(nowTimestamp)) {
        sendData = true;
    }

    if (sendData) {
        dataMngr.resetIntemediateData(light, distance, nowTimestamp);
        commMngr.sendData(ID_ENDPOINT, light, distance, nowTimestamp);
    }

    nowTimestamp = getElapsedRealTime();
    unsigned long sleep_comm = commMngr.getMinDelayBeforAction(nowTimestamp);
    unsigned long sleep_data = dataMngr.getMinDelayBeforAction(nowTimestamp);
    unsigned long sleep_time = min(sleep_comm, sleep_data);

#ifdef DEBUG
#ifdef VERBOSE
    Serial.print("[.] sleep_comm: ");
    Serial.println(sleep_comm);
    Serial.print("[.] sleep_data: ");
    Serial.println(sleep_data);
#endif
#endif

#ifdef DEBUG
    Serial.print("[.] Zzz (");
    Serial.println(sleep_time);
    Serial.println(" ms)");
#endif

    Serial.flush();
    sleepArduino(sleep_time);
}

unsigned long getElapsedRealTime() {
    return sleep.WDTMillis();
}

void wakeSensors(){
    digitalWrite(PIN_SENSORS_POWER, HIGH);
    delay(100);
}
void sleepSensors(){
    digitalWrite(PIN_SENSORS_POWER, LOW);
}

void sleepArduino(int mills) {
    digitalWrite(LED_BUILTIN, LOW);

    sleep.pwrDownMode();
    sleep.sleepDelay(mills);

    digitalWrite(LED_BUILTIN, HIGH);
}




