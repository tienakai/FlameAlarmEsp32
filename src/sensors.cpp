#include "sensors.h"
#include "config.h"
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DHTTYPE DHT11
DHT dht(PIN_DHT_PIN, DHTTYPE);

OneWire oneWire(PIN_DS18B20);
DallasTemperature ds18b20(&oneWire);

void sensors_setup() {
  dht.begin();
  ds18b20.begin();
  analogReadResolution(12);
  pinMode(PIN_FLAME_DIG, INPUT);
}

void sensors_read(int &mq2_adc, int &flame_d, float &temp_c, float &hum) {
  mq2_adc = analogRead(PIN_MQ2_ADC);
  flame_d = digitalRead(PIN_FLAME_DIG);

  float h = dht.readHumidity();
  if (isnan(h)) h = -1.0f;

  ds18b20.requestTemperatures();
  float t = ds18b20.getTempCByIndex(0);
  if (t == DEVICE_DISCONNECTED_C) t = -127.0f;

  temp_c = t;
  hum = h;
}
