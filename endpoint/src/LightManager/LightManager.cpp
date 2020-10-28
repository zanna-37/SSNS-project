#include "Arduino.h"
#include "LightManager.h"

#define DEBUG //comment: off | uncomment: on
//#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

#define THRESHOLD_LIGHT 500 //0-1024

LightManager::LightManager(int PIN) {
    PIN_LIGHT = PIN;
    pinMode(PIN_LIGHT, INPUT);
}

bool LightManager::getLight() {
    int lightValueAnalog = analogRead(PIN_LIGHT); //HIGHER: darker | LOWER: brighter

#ifdef DEBUG
#ifdef VERBOSE
    Serial.print("[.] Light: ");
    if (isBright(lightValueAnalog)) {
        Serial.print("BRIGHT (");
    } else {
        Serial.print("DARK (");
    }
    Serial.print(lightValueAnalog);
    Serial.println(")");
#endif
#endif

    return isBright(lightValueAnalog);
}

bool LightManager::isBright(int lightValueAnalog) {
    return lightValueAnalog < THRESHOLD_LIGHT;
}
