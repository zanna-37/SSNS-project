#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "Arduino.h"

class DataManager {
public:
    DataManager();
    void updateData(bool light, int distance, unsigned long nowTimestamp);
    bool isTransmissionNeeded(bool light, int distance, unsigned long nowTimestamp);
private:
    int lastLight;
    int lastDistance;
    unsigned long lastChange;
};

#endif
