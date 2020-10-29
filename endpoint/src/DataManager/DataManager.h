#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "Arduino.h"

class DataManager {
public:
    DataManager(unsigned long SHORT_SLEEP, unsigned long DEFER_UNSTABLE_CHANGES_FOR_MS, unsigned long LONG_SLEEP);
    void resetIntemediateData(bool light, int distance, unsigned long nowTimestamp);
    bool addIntermediateAndCheckTransmissionNeeded(bool light, int distance, unsigned long nowTimestamp);
    unsigned long getMinDelayBeforeAction(unsigned long nowTimestamp);
private:
    unsigned long SHORT_SLEEP;
    unsigned long DEFER_UNSTABLE_CHANGES_FOR_MS;
    unsigned long LONG_SLEEP;

    int lastLight;
    int lastDistance;
    unsigned long firstChange;
    bool wasTransmissionDeferred();
    void deferTransmission(unsigned long nowTimestamp);
    void resetDefer();
};

#endif
