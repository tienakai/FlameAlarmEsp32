#include "mqtt_client.h"
#include "config.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>


static WiFiClientSecure wifiClient;
static PubSubClient client(wifiClient);

static unsigned long lastReconnectAttempt = 0;

extern bool alarmManualRequest;

void handleCmdMessage(const JsonDocument& doc);

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  msg.reserve(length);
  for (uint i = 0; i < length; i++) msg += (char)payload[i];

  Serial.printf("MQTT IN [%s]: %s\n", topic, msg.c_str());

  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, msg)) {
    Serial.println("JSON parse error");
    return;
  }

  const char* type = doc["type"];
  if (!type) return;

  if (strcmp(type, "cmd") == 0) {
    handleCmdMessage(doc);
  }
}

void mqtt_setup() {
  wifiClient.setInsecure(); // dùng TLS không cần chứng chỉ
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
}

bool mqtt_connect() {
  if (client.connected()) return true;

  Serial.print("Connecting MQTT...");
  String cid = "ESP32-FIRE-";
  cid += String(random(0xffff), HEX);

  bool ok = client.connect(cid.c_str(), MQTT_USER, MQTT_PASS_MQTT);

  if (ok) {
    Serial.println("connected");
    client.subscribe(TOPIC_FIRE);
  } else {
    Serial.print("failed rc=");
    Serial.println(client.state());
  }

  return ok;
}

void mqtt_loop() {
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 2000) {
      lastReconnectAttempt = now;
      if (mqtt_connect()) lastReconnectAttempt = 0;
    }
  } else {
    client.loop();
  }
}

void mqtt_publish_sensor(int mq2_adc, int flame_d, float temp_c, float hum) {
  StaticJsonDocument<256> doc;

  doc["type"] = "sensor";
  doc["mq2_adc"] = mq2_adc;
  doc["flame"] = (flame_d == LOW);
  doc["temp"] = temp_c;
  doc["hum"] = hum;

  char buf[256];
  size_t n = serializeJson(doc, buf);
  client.publish(TOPIC_FIRE, buf, n);
}

void mqtt_publish_alarm_state(bool state) {
  StaticJsonDocument<128> doc;

  doc["type"] = "alarm";
  doc["state"] = state ? "on" : "off";

  char buf[128];
  size_t n = serializeJson(doc, buf);
  client.publish(TOPIC_FIRE, buf, n);
}

void handleCmdMessage(const JsonDocument& doc) {
  if (doc.containsKey("alarm")) {
    const char* v = doc["alarm"];
    alarmManualRequest = (strcmp(v, "on") == 0);
  }
}
