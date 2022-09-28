//
// Created by kilia on 14.08.2022.
//

#include <Arduino.h>
#include "actions.h"
#include "sensors.h"
#include "main.h"
#include "pid.h"

void Actions::setup() {
    Serial.println("toggle setup1");
    pinMode(PIN_BUTTON, INPUT);
    pinMode(PIN_RELAY, OUTPUT);
    pinMode(PIN_LED_1, OUTPUT);
   //pinMode(PIN_LED_2, OUTPUT);
    digitalWrite(PIN_RELAY, LOW);
    digitalWrite(PIN_LED_1, LOW);
    digitalWrite(PIN_LED_2, LOW);
    Serial.println("toggle setup2");
}

void Actions::toggleRelay() {
    Pid::isActive = !Pid::isActive;
}

void Actions::toggleLed(int id) {
    Serial.println("toggleLed " + String(id));
    int val = HIGH;

    if (id == 1) {
        if (digitalRead(PIN_LED_1)) {
            val = LOW;
        }
        digitalWrite(PIN_LED_1, val);
        Sensors::led_1 = val;
    } else if (id == 2) {
        if (digitalRead(PIN_LED_2)) {
            val = LOW;
        }
        digitalWrite(PIN_LED_2, val);
        Sensors::led_2 = val;
    }
}

