//
// Created by kilia on 14.08.2022.
//

#include <Arduino.h>
#include "actions.h"
#include "sensors.h"
#include "main.h"

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
    if(Sensors::relay == 0){
        Sensors::relay = 1;
        digitalWrite(PIN_RELAY, HIGH);
        digitalWrite(PIN_LED_1, HIGH);
    } else {
        Sensors::relay = 0;
        digitalWrite(PIN_RELAY, LOW);
        digitalWrite(PIN_LED_1, LOW);
    }

}