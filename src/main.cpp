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

// Giữ trạng thái flame trong 1500 ms
unsigned long flameHoldUntil = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("ESP32 Fire Monitor starting...");

  // WiFi
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

  // MQ2 smoke status
  String smokeStatus = "normal";
  if (mq2_adc >= THRESH_MQ2_ALARM)
    smokeStatus = "alarm";
  else if (mq2_adc >= THRESH_MQ2_WARN)
    smokeStatus = "warning";

  // --- Flame detection giữ trạng thái ---
  bool rawFlame = (flame_d == LOW);  // LOW = phát hiện lửa

  if (rawFlame) {
    flameHoldUntil = millis() + 1500; // giữ 1.5 giây
  }

  bool flameDetected = (millis() < flameHoldUntil);
  // --------------------------------------

  // Alarm logic
  bool shouldAlarm =
    alarmManualRequest ||
    flameDetected ||
    (smokeStatus == "alarm") ||
    (temp_c > TEMP_ALARM);

  alarm_set(shouldAlarm);

  // Pump ON chỉ khi có lửa
  digitalWrite(PIN_RELAY_PUMP, flameDetected ? LOW : HIGH);

  // Publish MQTT
  unsigned long now = millis();
  if (now - lastPublish >= PUBLISH_INTERVAL) {
    lastPublish = now;

    mqtt_publish_sensor(mq2_adc, flameDetected ? 0 : 1, temp_c, hum);

    if (shouldAlarm != lastAlarmState) {
      lastAlarmState = shouldAlarm;
      mqtt_publish_alarm_state(shouldAlarm);
    }
  }

  delay(100);
}
