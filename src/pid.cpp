/******************************************************************
  sTune QuickPID Example
  This sketch does on-the-fly tunning and PID Digital Output control
  of a thermal heater (TIP31C). Tunning parameters are quickly
  determined and applied during the temperature ramp-up to setpoint.
  Open the serial plotter to view the graphical results.
  *****************************************************************/

#include <cstdint>
#include "sensors.h"
#include "pid.h"
#include "main.h"
#include "WebSerialLite.h"
#include <sTune.h>
#include <QuickPID.h>
#include <Arduino.h>

// user settings
uint32_t Pid::settleTimeSec = 10;
uint32_t Pid::testTimeSec = 5;  // runPid interval = testTimeSec / samples
const uint16_t Pid::samples = 200;
const float Pid::inputSpan = 200;
const uint32_t Pid::outputSpan = 10;
float Pid::outputStart = 0;
float Pid::outputStep = 1;
float Pid::tempLimit = 100;
uint8_t Pid::debounce = 0;

// variables
float Pid::Input;
float Pid::Output;
float Pid::SetPoint = 90;
float Pid::Kp;
float Pid::Ki;
float Pid::Kd;

bool Pid::isActive = false;


int Pid::status = -1;

sTune Pid::tuner = sTune(&Input, &Output, tuner.ZN_PID, tuner.directIP, tuner.printOFF);
QuickPID Pid::quickPid(&Input, &Output, &SetPoint);

void Pid::setup() {
    tuner.Configure(inputSpan, outputSpan, outputStart, outputStep, testTimeSec, settleTimeSec, samples);
    tuner.SetEmergencyStop(tempLimit);
}

void Pid::loop() {
    if (isActive) {
        tuner.softPwm(PIN_RELAY, Input, Output, SetPoint, outputSpan, debounce);
        Sensors::relay = digitalRead(PIN_RELAY);
        status = tuner.Run();

        switch (status) {
            case tuner.sample: // active once per sample during test
                Input = Sensors::temp_1; // get degC (using 3.3v AREF)
                break;
            case tuner.tunings: // active just once when sTune is done
                tuner.GetAutoTunings(&Kp, &Ki, &Kd); // sketch variables updated by sTune
                quickPid.SetOutputLimits(0, outputSpan);
                quickPid.SetSampleTimeUs(1000);
                quickPid.SetMode(QuickPID::Control::automatic); // the PID is turned on
                quickPid.SetProportionalMode(QuickPID::pMode::pOnMeas);
                quickPid.SetAntiWindupMode(QuickPID::iAwMode::iAwClamp);
                quickPid.SetTunings(Kp, Ki, Kd); // update PID with the new tunings
                break;
            case tuner.runPid: // active once per sample after tunings
                Input = Sensors::temp_1; // get degC (using 3.3v AREF)
                quickPid.Compute();
                break;
            default:
                break;
        }
    }
}