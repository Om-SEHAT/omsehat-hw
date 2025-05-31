#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

#define WIFI_SSID "Riv"
#define WIFI_PASSWORD "halo12345"
#define MQTT_BROKER  "broker.emqx.io"
#define MQTT_TOPIC_PUBLISH   "triana/data"
#define MQTT_TOPIC_SUBSCRIBE "triana/data"

class MQTTHandler {
public:
    MQTTHandler();
    void begin();
    void loop();
    void publishMessage(const char* msg, const char* topic);
    bool isConnected();
    friend void mqttCallback(char* topic, byte* payload, unsigned int len); // Allow callback to access private members if needed
private:
    void wifiConnect();
    bool mqttConnect();
    void reconnectMqtt();
    static void onPublishMessage();
    static WiFiClient espClient;
    static PubSubClient mqtt;
    static char g_szDeviceId[30];
    static Ticker timerPublish;
};

void mqttCallback(char* topic, byte* payload, unsigned int len);

#endif // MQTT_HANDLER_H
