//
// Created by kilia on 14.08.2022.
//

#include <Arduino.h>
#include "sensors.h"
#include "main.h"
#include "ZACwire.h"
#include "actions.h"
#include "pid.h"

int Sensors::button = 0;
int Sensors::relay = 0;
int Sensors::led = 0;
float Sensors::temp_1 = 0;
float Sensors::temp_2 = 0;
float Sensors::temp_3 = 0;

ZACwire Sensor1(PIN_SENSOR_1, 306);
ZACwire Sensor2(PIN_SENSOR_2, 306);
ZACwire Sensor3(PIN_SENSOR_3, 306);


void Sensors::setup() {
    Serial.println("Setup PIN_SENSOR_1");
    if (Sensor1.begin()) {     //check if a sensor is connected to the pin
        Serial.println("Signal found on pin PIN_SENSOR_1");
    }
    Serial.println("Setup PIN_SENSOR_2");
    if (Sensor2.begin()) {     //check if a sensor is connected to the pin
        Serial.println("Signal found on pin PIN_SENSOR_2");
    }
    Serial.println("Setup PIN_SENSOR_3");
    if (Sensor3.begin()) {     //check if a sensor is connected to the pin
        Serial.println("Signal found on pin PIN_SENSOR_3");
    }
    Serial.println("Setup PIN_BUTTON");
    attachInterrupt(PIN_BUTTON, isr_btn, CHANGE);
     ledcAttachPin(PIN_LED, 0);
    ledcSetup(0, 4000, 8);
}

void Sensors::loop() {
    //
    button = digitalRead(PIN_BUTTON);
    ledcWrite(0, uint8_t ((Pid::getPidPercentage()/100.0)*255.0));
    temp_1 = Sensor1.getTemp();
    temp_2 = Sensor2.getTemp();
    temp_3 = Sensor3.getTemp();
}

/**
 * called when button is pressed by isr
 *
 * @param button pointer to button that was pressed
 */
void IRAM_ATTR Sensors::isr_btn() {
    int state = digitalRead(PIN_BUTTON);
    Serial.println("isr_btn " + String(state));
    //will be high after button was released
    if (!state) {
        Actions::togglePid();
    }
}