#ifndef MAX30105_HANDLER_H
#define MAX30105_HANDLER_H

#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

class MAX30105Handler {
public:
    MAX30105Handler();
    void begin();
    void poll();
    bool isDone() const { return scanningDone; }
    String toJson();
private:
    MAX30105 particleSensor;
    uint32_t irBuffer[100];
    uint32_t redBuffer[100];
    int32_t bufferLength;
    int32_t spo2;
    int8_t validSPO2;
    int32_t heartRate;
    int8_t validHeartRate;
    bool scanningDone;
};

#endif // MAX30105_HANDLER_H
