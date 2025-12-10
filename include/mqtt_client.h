#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>

void mqtt_setup();
void mqtt_loop();
void mqtt_publish_sensor(int mq2_adc, int flame_d, float temp_c, float hum);
void mqtt_publish_alarm_state(bool state);
#endif // MQTT_CLIENT_H