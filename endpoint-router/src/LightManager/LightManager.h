#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "Arduino.h"

class LightManager {
public:
    LightManager(int PIN);
    bool getLight();
private:
    bool isBright(int lightValueAnalog);
    int PIN_LIGHT;
};

#endif
