#include <AsyncPrinter.h>
#include <DebugPrintMacros.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncTCPbuffer.h>
#include <SyncClient.h>
#include <async_config.h>
#include <tcp_axtls.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include <AsyncMqttClient.hpp>
#include <Ticker.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define MQTT_HOST IPAddress()
#define MQTT_PORT 
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

#define MQTT_TEMP "topic"

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event){
  Serial.println("Connected to Wifi");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wifi");
  mqttReconnectTimer.detach();
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT…");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  //mqttClient.onSubscribe(onMqttSubscribe);
  //mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  mqttClient.setCredentials(MQTT_USERNAME, MQTT_PASSWORD);
  connectToWifi();
}

void loop() {
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_TEMP, 1, true, "heat_detected");
}






























