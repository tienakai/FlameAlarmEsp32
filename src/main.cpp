#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "mqtt_client.h"
#include "alarm.h"
#include <WiFi.h>

bool alarmManualRequest = false;
bool lastAlarmState = false;
const char* WIFI_SSID = "Rolll";
const char* WIFI_PASSWORD = "88888888";
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("ESP32 Fire Monitor starting...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");
  int cnt = 0;

  while (WiFi.status() != WL_CONNECTED && cnt < 30) {
    Serial.print(".");
    delay(500);
    cnt++;
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi failed");
  }

  sensors_setup();
  mqtt_setup();
  alarm_setup();
}

unsigned long lastPublish = 0;

void loop() {
  mqtt_loop();

  int mq2_adc;
  int flame_d;
  float temp_c;
  float hum;

  sensors_read(mq2_adc, flame_d, temp_c, hum);

  String smokeStatus = "normal";
  if (mq2_adc >= THRESH_MQ2_ALARM)
    smokeStatus = "alarm";
  else if (mq2_adc >= THRESH_MQ2_WARN)
    smokeStatus = "warning";

  bool flameDetected = (flame_d == LOW);

  bool shouldAlarm =
    alarmManualRequest ||
    flameDetected ||
    (smokeStatus == "alarm") ||
    (temp_c > TEMP_ALARM);

  alarm_set(shouldAlarm);

  digitalWrite(PIN_RELAY_PUMP, flameDetected ? HIGH : LOW);

  unsigned long now = millis();
  if (now - lastPublish >= PUBLISH_INTERVAL) {
    lastPublish = now;

    mqtt_publish_sensor(mq2_adc, flame_d, temp_c, hum);

    if (shouldAlarm != lastAlarmState) {
      lastAlarmState = shouldAlarm;
      mqtt_publish_alarm_state(shouldAlarm);
    }
  }

  delay(100);
}