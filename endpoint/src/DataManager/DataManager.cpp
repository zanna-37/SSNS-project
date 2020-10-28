#include "Arduino.h"
#include "DataManager.h"

#define DEBUG //comment: off | uncomment: on
//#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

#define THRESHOLD_DIFF_DISTANCE 2
#define DEFER_UNSTABLE_CHANGES_FOR_MS 5000

DataManager::DataManager() {
    lastLight = -1;
    lastDistance = -1;
    lastChange = 0;
}

void DataManager::updateData(bool light, int distance, unsigned long nowTimestamp) {
    lastLight = light;
    lastDistance = distance;
    lastChange = nowTimestamp;
}

bool DataManager::isTransmissionNeeded(bool light, int distance, unsigned long nowTimestamp) {
    bool hasChanged = false;

    if (lastLight == -1 || light != lastLight) {
#ifdef DEBUG
        Serial.print("[.] Change in light: ");
        Serial.print(lastLight);
        Serial.print("-->");
        Serial.println(light);
#endif

        hasChanged = true;
    }

    if (lastDistance == -1 || abs(distance - lastDistance) > THRESHOLD_DIFF_DISTANCE) {
#ifdef DEBUG
        Serial.print("[.] Change in proximity: ");
        Serial.print(lastDistance);
        Serial.print("-->");
        Serial.println(distance);
#endif

        hasChanged = true;
    }

    return hasChanged;
}
