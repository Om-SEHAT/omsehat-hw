#include "MAX30105Handler.h"
#include <ArduinoJson.h>

MAX30105Handler::MAX30105Handler()
    : bufferLength(0), spo2(0), validSPO2(0), heartRate(0), validHeartRate(0), scanningDone(false) {}

void MAX30105Handler::begin() {
    scanningDone = false;
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("MAX30102 was not found. Please check wiring/power. ");
        while (1);
    }
    Serial.println("MAX30102 initialized.");
    byte ledBrightness = 0x1F;
    byte sampleAverage = 4;
    byte ledMode = 2;
    int sampleRate = 100;
    int pulseWidth = 411;
    int adcRange = 4096;
    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
    particleSensor.clearFIFO();
    Serial.println("Place your finger on the sensor.");
}

void MAX30105Handler::poll() {
    
    uint32_t irValue = particleSensor.getIR();
    uint32_t redValue = particleSensor.getRed();
    if (irValue > 10000) {
        if (bufferLength < 100) {
            irBuffer[bufferLength] = irValue;
            redBuffer[bufferLength] = redValue;
            bufferLength++;
        }
    }
    if (bufferLength == 100) {
        // Serial.println("Buffer full. Calculating SpO2 and Heart Rate...");
        maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
        if (validSPO2 && validHeartRate) {
            Serial.print("Calculated SpO2 = ");
            Serial.print(spo2);
            Serial.print(" %, Calculated HR = ");
            Serial.print(heartRate);
            Serial.println(" bpm");
            bufferLength = 0;
            particleSensor.clearFIFO();
            scanningDone = true;
            // if (scanningDone) {
            //     ESP.restart();
            //     return;
            // }
            return;
        } else {
            // if (!validSPO2) Serial.print("  Could not calculate SpO2 reliably.");
            // if (!validHeartRate) Serial.println(", Could not calculate HR reliably.");
            bufferLength = 0;
            particleSensor.clearFIFO();
        }
    }
}

String MAX30105Handler::toJson() {
    DynamicJsonDocument doc(128);
    if (scanningDone) {
        doc["spo2"] = static_cast<float>(spo2);
        doc["heartRate"] = static_cast<float>(heartRate);
        scanningDone = false;
    } else {
        doc["spo2"] = -1.0;
        doc["heartRate"] = -1.0;
    }
    String json;
    serializeJson(doc, json);
    return json;
}