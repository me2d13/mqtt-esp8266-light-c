#ifndef CONFIG_H
#define CONFIG_H

#define MQTT_SUB_TOPIC "/devices/rgbled/set"
#define MQTT_PUB_TOPIC "/devices/rgbled"
#define MQTT_LOG_TOPIC "/devices/rgbled/log"
#define WIFI_SSID "mysid"
#define WIFI_PASSWORD "mypwd"
#define MQTT_SERVER "192.168.1.1"
#define HEART_BEAT_S 60*60

// Wemos D1
#define PIN_R 5 
// Wemos D2
#define PIN_G 4
// Wemos D3
#define PIN_B 0


#endif
