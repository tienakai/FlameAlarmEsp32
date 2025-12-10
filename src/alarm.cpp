#include <Arduino.h>
#include "alarm.h"
#include "config.h"

void alarm_setup() {
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELAY_PUMP, OUTPUT);

  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_RELAY_PUMP, LOW);
}

void alarm_set(bool on) {
  digitalWrite(PIN_BUZZER, on ? HIGH : LOW);
  digitalWrite(PIN_LED, on ? HIGH : LOW);

  
  //digitalWrite(PIN_RELAY_PUMP, on ? HIGH : LOW);
}
