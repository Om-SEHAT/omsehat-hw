#include <Arduino.h>
#include <Wire.h>
#include "MLX90614Handler.h"
#include "MAX30105Handler.h"
#include "MQTTHandler.h"
#include <ArduinoJson.h>

MLX90614Handler mlxHandler;
MAX30105Handler max30105Handler;
MQTTHandler mqttHandler;
bool max30105Active = false;
TaskHandle_t mqttTaskHandle = nullptr;

void mqttTask(void* pvParameters) {
    for (;;) {
        mqttHandler.loop();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void mqttControlCallback(const String& cmd) {
    DynamicJsonDocument doc(64);
    DeserializationError error = deserializeJson(doc, cmd);
    if (error) {
        Serial.println("Failed to parse JSON command");
        return;
    }
    String req = doc["request_sensor"] | "";
    if (req == "mlx") {
        float mlx = mlxHandler.update();
        DynamicJsonDocument respDoc(64);
        respDoc["temp"] = mlx;
        String json;
        serializeJson(respDoc, json);
        mqttHandler.publishMessage(json.c_str(), "omsehat/omsapa/mlx_data");
    } else if (req == "max") {
        if (!max30105Active || max30105Handler.isDone()) {
            max30105Handler.begin();
            max30105Active = true;
        }
    }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Ready. Type 'start mlx' or 'start max'");
  mlxHandler.begin();
  mqttHandler.begin();
  xTaskCreate(
    mqttTask,           // Task function
    "MQTTTask",        // Name
    4096,               // Stack size
    nullptr,            // Parameter
    1,                  // Priority
    &mqttTaskHandle     // Task handle
  );
}

void loop() {
    // No serial control, only MQTT
    if (max30105Active && !max30105Handler.isDone()) {
        max30105Handler.poll();
        // After polling, if a result is ready, publish to MQTT
        if (max30105Handler.isDone()) {
            String json = max30105Handler.toJson();
            mqttHandler.publishMessage(json.c_str(), "omsehat/omsapa/max_data");
            delay(2000); // Delay to avoid flooding the MQTT broker
            ESP.restart();
            return;
        }
    }
    delay(20);
}