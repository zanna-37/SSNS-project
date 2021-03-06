#include "Arduino.h"
#include "DataManager.h"

#define DEBUG //comment: off | uncomment: on
//#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

#define TOLERANCE_DIFF_DISTANCE 2

DataManager::DataManager(unsigned long SHORT_SLEEP, unsigned long DEFER_UNSTABLE_CHANGES_FOR_MS, unsigned long LONG_SLEEP) {
    DataManager::SHORT_SLEEP = SHORT_SLEEP;
    DataManager::DEFER_UNSTABLE_CHANGES_FOR_MS = DEFER_UNSTABLE_CHANGES_FOR_MS;
    DataManager::LONG_SLEEP = LONG_SLEEP;
    lastLight = -1;
    lastDistance = -1;
    resetDefer();
}

void DataManager::resetIntemediateData(bool light, int distance, unsigned long nowTimestamp) {
    lastLight = light;
    lastDistance = distance;
    resetDefer();
}

bool DataManager::addIntermediateAndCheckTransmissionNeeded(bool light, int distance, unsigned long nowTimestamp) {
    bool hasChanged = false;

    if (lastLight == -1 || light != lastLight) {
        #ifdef DEBUG
        Serial.print("[.] Change in light: ");
        Serial.print(lastLight);
        Serial.print("-->");
        Serial.println(light);
        #endif

        lastLight = light;
        hasChanged = true;
    }

    if (lastDistance == -1 || abs(distance - lastDistance) > TOLERANCE_DIFF_DISTANCE) {
        #ifdef DEBUG
        Serial.print("[.] Change in proximity: ");
        Serial.print(lastDistance);
        Serial.print("-->");
        Serial.println(distance);
        #endif

        lastDistance = distance;
        hasChanged = true;
    }

    if (wasTransmissionDeferred()) {
        if (abs(nowTimestamp - firstChange) > DEFER_UNSTABLE_CHANGES_FOR_MS) {
            //transmit if deferred-data timed out
            #ifdef DEBUG
            Serial.println("[.] Defer timeout");
            #endif
            return true;
        } else {
            //transmit if the deferred data become stable
            #ifdef DEBUG
            if (hasChanged) {
                Serial.println("[.] Unstable change");
            } else {
                Serial.println("[.] Stable change");
            }
            #endif
            return !hasChanged;
        }
    } else {
        if (hasChanged) {
            #ifdef DEBUG
            Serial.println("[.] Defer transmission");
            #endif
            deferTransmission(nowTimestamp);
        }
        return false;
    }
}

unsigned long DataManager::getMinDelayBeforeAction(unsigned long nowTimestamp) {
    if (wasTransmissionDeferred()) {
        return SHORT_SLEEP;
    } else {
        return LONG_SLEEP;
    }
}

bool DataManager::wasTransmissionDeferred() {
    return firstChange != 0;
}
void DataManager::deferTransmission(unsigned long nowTimestamp) {
    firstChange = nowTimestamp;
}
void DataManager::resetDefer() {
    firstChange = 0;
}