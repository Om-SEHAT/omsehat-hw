#include "MQTTHandler.h"
#include <PubSubClient.h>

// Forward declaration to resolve undefined identifier
void mqttControlCallback(const String& cmd);

WiFiClient MQTTHandler::espClient;
PubSubClient MQTTHandler::mqtt(espClient);
char MQTTHandler::g_szDeviceId[30];
Ticker MQTTHandler::timerPublish;

extern MQTTHandler mqttHandler;

void mqttCallback(char* topic, byte* payload, unsigned int len) {
    String msg;
    for (unsigned int i = 0; i < len; ++i) {
        msg += (char)payload[i];
    }
    msg.trim();
    mqttControlCallback(msg);
}

MQTTHandler::MQTTHandler() {}

void MQTTHandler::begin() {
    Serial.begin(115200);
    wifiConnect();
    mqttConnect();
    timerPublish.attach_ms(3000, onPublishMessage);
}

void MQTTHandler::loop() {
    mqtt.loop();
    reconnectMqtt();
}

void MQTTHandler::publishMessage(const char* msg, const char* topic) {
    mqtt.publish(topic, msg);
}

bool MQTTHandler::isConnected() {
    return mqtt.connected();
}

void MQTTHandler::onPublishMessage() {
    // Do nothing (empty implementation)
}

bool MQTTHandler::mqttConnect() {
    sprintf(g_szDeviceId, "esp32_%08X",(uint32_t)ESP.getEfuseMac());
    mqtt.setServer(MQTT_BROKER, 1883);
    mqtt.setCallback(mqttCallback);
    Serial.printf("Connecting to %s clientId: %s\n", MQTT_BROKER, g_szDeviceId);
    bool fMqttConnected = false;
    for (int i=0; i<3 && !fMqttConnected; i++) {
        Serial.print("Connecting to mqtt broker...");
        fMqttConnected = mqtt.connect(g_szDeviceId);
        if (fMqttConnected == false) {
            Serial.print(" fail, rc=");
            Serial.println(mqtt.state());
            delay(1000);
        }
    }
    if (fMqttConnected) {
        Serial.println(" success");
        // mqtt.subscribe(MQTT_TOPIC_SUBSCRIBE);
        mqtt.subscribe("omsehat/omsapa/request");
        Serial.printf("Subcribe topic: %s\n", "omsehat/omsapa/request");
        // onPublishMessage();
    }
    return mqtt.connected();
}

void MQTTHandler::wifiConnect() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    Serial.print("System connected with IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("RSSI: %d\n", WiFi.RSSI());
}

void MQTTHandler::reconnectMqtt() {
    if (!mqtt.connected()) {
        Serial.println("Reconnecting to MQTT broker...");
        if (mqttConnect()) {
            Serial.println("Reconnected to MQTT broker.");
        } else {
            Serial.println("Failed to reconnect to MQTT broker.");
        }
    }
}
