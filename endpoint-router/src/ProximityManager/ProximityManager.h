#ifndef PROXIMITYMANAGER_H
#define PROXIMITYMANAGER_H

#include "Arduino.h"

class ProximityManager {
public:
    ProximityManager(int PIN_TRIG, int PIN_ECHO);
    int getProximity();
private:
    int PIN_TRIG;
    int PIN_ECHO;
};

#endif
