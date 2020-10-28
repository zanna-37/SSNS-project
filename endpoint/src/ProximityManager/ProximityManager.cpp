#include "Arduino.h"
#include "ProximityManager.h"

#define DEBUG //comment: off | uncomment: on
//#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

ProximityManager::ProximityManager(int PIN_TRIG, int PIN_ECHO) {
    ProximityManager::PIN_TRIG = PIN_TRIG;
    ProximityManager::PIN_ECHO = PIN_ECHO;

    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
}

int ProximityManager::getProximity() {
    float duration, distance;
    //Reset
    digitalWrite(PIN_TRIG, LOW);
    delayMicroseconds(2);

    //Sense
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);

    //Calculations
    duration = pulseIn(PIN_ECHO, HIGH);
    distance = (duration / 2) * 0.0344;

#ifdef DEBUG
#ifdef VERBOSE
    Serial.print("[.] Distance (cm): ");
    Serial.println(distance);
#endif
#endif

    return round(distance);
}