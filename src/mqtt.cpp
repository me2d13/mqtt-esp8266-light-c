#include <ESP8266WiFi.h>
#include "mqtt.h"
#include <WifiClient.h>
#include "config.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient net;
PubSubClient client(net);
StaticJsonDocument<200> doc;
messageHandlerType messageHandler = NULL;

char buffer[100];

int state_r = 0;
int state_g = 0;
int state_b = 0;
int state_br = 255;
int state_on = 0;

void connectMqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("rgbled-pracovna")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      sprintf(buffer, "RGB pracovna connected to MQTT with IP  %s", WiFi.localIP().toString().c_str());
      mqttLog(buffer);
      // ... and resubscribe
      client.subscribe(MQTT_SUB_TOPIC);
    } else {
      Serial.print("Mqtt connect failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqttHeartBeat() {
    mqttLog("Pracovna RGB alive");
}

void logReal(int red_value, int green_value, int blue_value) {
  if (client.connected()) {
    sprintf(buffer, "Pracovna state  R%d, G%d, B%d", red_value, green_value, blue_value);
    client.publish(MQTT_LOG_TOPIC,buffer);
  }
}

void logState() {
  if (client.connected()) {
    sprintf(buffer, "Pracovna state  R%d, G%d, B%d, br %d, on %d", state_r, state_g, state_b, state_br, state_on);
    client.publish(MQTT_LOG_TOPIC,buffer);
  }
}

void mqttLog(char *message) {
  if (client.connected()) {
    client.publish(MQTT_LOG_TOPIC,message);
  }
}

void setMessageHandler(messageHandlerType handler) {
    messageHandler = handler;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  if (messageHandler == NULL) {
    Serial.println("WARNING: mqtt message handler not set");
    return;
  }
  const char *state = doc["state"];
  Serial.print("State is ");
  Serial.println(state);
  int brightness = doc["brightness"] | state_br;
  state_br = brightness;
  if (strcmp(state, "OFF") == 0) {
    state_on = 0;
    logState();
    logReal(0, 0, 0);
    messageHandler(0, 0, 0);
  } else if (strcmp(state, "ON") == 0) {
    state_on = 1;
    int red = doc["color"]["r"] | -1;
    int green = doc["color"]["g"] | -1;
    int blue = doc["color"]["b"] | -1;
    if (red >= 0 && green >= 0 && blue >= 0) {
      state_r = red;
      state_g = green;
      state_b = blue;
    }
    int value_r = state_r * state_br / 255;
    int value_g = state_g * state_br / 255;
    int value_b = state_b * state_br / 255;
    logState();
    logReal(value_r, value_g, value_b);
    messageHandler(value_r, value_g, value_b);
  }
}

void setupMqtt() {
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void mqttLoop() {
  if (!client.connected()) {
    connectMqtt();
  }
  client.loop();
}