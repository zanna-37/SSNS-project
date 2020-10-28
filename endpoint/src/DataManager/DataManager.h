#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "Arduino.h"

class DataManager {
public:
    DataManager();
    void resetIntemediateData(bool light, int distance, unsigned long nowTimestamp);
    bool addIntermediateAndCheckTransmissionNeeded(bool light, int distance, unsigned long nowTimestamp);
private:
    int lastLight;
    int lastDistance;
    unsigned long firstChange;
    bool wasTransmissionDeferred();
    void deferTransmission(unsigned long nowTimestamp);
    void resetDefer();
};

#endif
