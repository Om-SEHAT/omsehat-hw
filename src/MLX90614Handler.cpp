#include "MLX90614Handler.h"

MLX90614Handler::MLX90614Handler() : mlx() {}

void MLX90614Handler::begin() {
    if (!mlx.begin()) {
        Serial.println("Error connecting to MLX sensor. Check wiring.");
        while (1);
    }
    Serial.println("MLX90614 initialized");
}

float MLX90614Handler::update() {
    float totalTemp = 0.0;
    const int sampleCount = 5;

    delay(2000); // Allow sensor to stabilize before taking readings

    for (int i = 0; i < sampleCount; i++) {
        totalTemp += mlx.readObjectTempC();
        delay(500); // Small delay between samples
    }

    float averageTemp = totalTemp / sampleCount;
    Serial.println(averageTemp);
    return averageTemp;
}
