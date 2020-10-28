#include "src/Sleep_n0m1_custom/Sleep_n0m1_custom.h"
#include "src/LightManager/LightManager.h"
#include "src/ProximityManager/ProximityManager.h"
#include "src/CommunicationManager/CommunicationManager.h"

#define DEBUG //comment: off | uncomment: on
#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

#define PIN_RX_XBEE 2
#define PIN_TX_XBEE 3
#define PIN_SLEEP_XBEE 9
#define PIN_LIGHT A5
#define PIN_TRIG 10
#define PIN_ECHO 11

#define SLEEP_MS 4000
#define ID_ENDPOINT 8

Sleep sleep;
LightManager lightManager(PIN_LIGHT);
ProximityManager proximityManager(PIN_TRIG, PIN_ECHO);
CommunicationManager commMngr(PIN_RX_XBEE, PIN_TX_XBEE, PIN_SLEEP_XBEE);

void setup() {
  Serial.begin(9600);
    
  #ifdef DEBUG
    Serial.println();
    Serial.println("##############");
    Serial.println("#  STARTING  #");
    Serial.println("##############");
    Serial.println();
    Serial.print  ("TYPE           : ENDPOINT "); Serial.println(ID_ENDPOINT);
    Serial.println("PIN_RX_XBEE    : " + String(PIN_RX_XBEE));
    Serial.println("PIN_TX_XBEE    : " + String(PIN_TX_XBEE));
    Serial.println("PIN_SLEEP_XBEE : " + String(PIN_SLEEP_XBEE));
    Serial.println("PIN_LIGHT      : " + String(PIN_LIGHT));
    Serial.println("PIN_TRIG       : " + String(PIN_TRIG));
    Serial.println("PIN_ECHO       : " + String(PIN_ECHO));
    Serial.println("--------------");
  #endif
}

void loop() {
  bool light = lightManager.getLight();
  int distance = proximityManager.getProximity();
  unsigned long nowTimestamp = getElapsedRealTime();

  if( commMngr.isTransmissionNeeded(light, distance) ) {
    commMngr.sendData(ID_ENDPOINT, light, distance, nowTimestamp);
  } else if( commMngr.isKeepAliveNeeded(nowTimestamp) ) {
    commMngr.sendData(ID_ENDPOINT, light, distance, nowTimestamp);
  }

  #ifdef DEBUG
    Serial.println();
  #endif

  Serial.flush();
  ArduinoSleep(SLEEP_MS);
}

unsigned long getElapsedRealTime() {
  return sleep.WDTMillis();
}

void ArduinoSleep(int mills) {
  sleep.pwrDownMode();
  sleep.sleepDelay(mills);
}



  
