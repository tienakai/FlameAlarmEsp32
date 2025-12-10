#ifndef CONFIG_H
#define CONFIG_H

// --- WiFi ---
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// --- MQTT ---
#define MQTT_HOST        "4158db786b55496d9d8e6a655d408e16.s1.eu.hivemq.cloud"
#define MQTT_PORT        8883
#define MQTT_USER        "tienbn"
#define MQTT_PASS_MQTT   "Tien123bn"

// --- Topic ---
#define TOPIC_FIRE       "flame-alarm-sensor"

// --- Pins ---
#define PIN_MQ2_ADC      32      // ADC1 → OK khi dùng WiFi
#define PIN_FLAME_DIG    34      // Input-only
#define PIN_DHT_PIN       4
#define PIN_DS18B20       5
#define PIN_BUZZER       25      // Output OK
#define PIN_LED          33
#define PIN_RELAY_PUMP   12

// --- Thresholds ---
#define BASELINE_MQ2       200
#define THRESH_MQ2_WARN   1000
#define THRESH_MQ2_ALARM  3000

#define TEMP_WARN        45.0f
#define TEMP_ALARM       60.0f

// --- Publish interval ---
#define PUBLISH_INTERVAL  2000

#endif // CONFIG_H