#ifndef MQTT_H
#define MQTT_H

typedef void (*messageHandlerType)(int, int, int);


void mqttHeartBeat();
void setupMqtt();
void connectMqtt();
void mqttLoop();
void mqttLog(char *message);
void setMessageHandler(messageHandlerType handler);
void logState(int red_value, int green_value, int blue_value);

#endif