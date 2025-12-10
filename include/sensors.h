#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

void sensors_setup();
void sensors_read(int &mq2_adc, int &flame_d, float &temp_c, float &hum);
#endif // SENSORS_H