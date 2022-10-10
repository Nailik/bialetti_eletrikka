#include <Arduino.h>
#include "webserver.h"
#include "sensors.h"
#include "actions.h"
#include "pid.h"
#include "WebSerialLite.h"

//https://github.com/Dlloydev/QuickPID
//https://github.com/Dlloydev/sTune
void setup() {
    Serial.begin(115200);
    Serial.println("startup");
    // put your setup code here, to run once:
    Sensors::setup();
    Serial.println("Sensors");
    Actions::setup();
    Serial.println("Actions");
    WebServer::setup();
    Serial.println("WebServer ");
    Pid::setup();
}

void loop() {
    // put your main code here, to run repeatedly:
    Sensors::loop();
   // Pid::loop();
    //custom loop for pid
    WebServer::loop();

    if (Serial.available()) {      // If anything comes in Serial (USB),
        WebSerial.println(Serial.readString());
    }

    delay(100);
}