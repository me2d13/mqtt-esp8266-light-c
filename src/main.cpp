#include <Arduino.h>
#include <Ticker.h>
#include "config.h"
#include "ota.h"
#include "mqtt.h"

// mosquitto_sub -t '/devices/rgbled/log'

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

void blick();
void release();

Ticker heartBeatTimer(mqttHeartBeat, HEART_BEAT_S*1000);

void setupAndWaitForWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  WiFi.setAutoReconnect(true);
}

void messageHandler(int red_value, int green_value, int blue_value) {
  analogWrite(PIN_R, red_value);
  analogWrite(PIN_G, green_value);
  analogWrite(PIN_B, blue_value);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // initialize onboard LED as output
  pinMode(PIN_R, OUTPUT);  
  pinMode(PIN_G, OUTPUT);  
  pinMode(PIN_B, OUTPUT);  
  Serial.begin(115200);
  Serial.println("Booting");
  setupAndWaitForWifi();
  setupOTA();
  setupMqtt();
  connectMqtt();
  setMessageHandler(&messageHandler);
  heartBeatTimer.start();
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  ArduinoOTA.handle();
  mqttLoop();
  heartBeatTimer.update();
}