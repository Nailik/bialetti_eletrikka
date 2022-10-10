/******************************************************************
  sTune QuickPID Example
  This sketch does on-the-fly tunning and PID Digital Output control
  of a thermal heater (TIP31C). Tunning parameters are quickly
  determined and applied during the temperature ramp-up to setpoint.
  Open the serial plotter to view the graphical results.

bei 65 grad kocht das wasser schon

 ----------------------------------------------
 testTimeSec = 50
 SetPoint = 40
 outputStep = 1

  .00
 Sampl:        0.1000

 Pv Start:          0.000
 Pv Max:            50.418
 Pv Diff:           50.418

 Process Gain:      252.092
 Dead Time Sec:     1.209
 Tau Sec:           87.079

 Tau/Dead Time:     72.0 (easy to control)
 Tau/Sample Period: 870.8 (good sample rate)

 Tuning Method: NoOvershoot_PID
  Kp: 0.171
  Ki: 0.011  Ti: 14.926
  Kd: 1.654  Td: 0.104


   ----------------------------------------------
 SetPoint = 65 (overshoot in beginning big, later up to 67,5°C)
 outputStep = 10
 directIP

  Ip Pv:             34.250
 Pv Start:          0.000
 Pv Max:            162.689
 Pv Diff:           162.689

 Process Gain:      271.148
 Dead Time Sec:     1.207
 Tau Sec:           1.880

 Tau/Dead Time:     1.6 (easy to control)
 Tau/Sample Period: 56.4 (good sample rate)

 Tuning Method: NoOvershoot_PID
  Kp: 0.003
  Ki: 0.532  Ti: 0.006
  Kd: 1.658  Td: 0.002





 ________________________________________





 Testen:


 Heizen -> Druckaufbau -> mehr Energie notwendig
 sobald Druck > Kaffee Druck -> Druck fällt -> weniger Energie notwendig
 Wasser neigt sich Ende -> deutlich weniger Energie -> zufuhr Abschalten, kein Sprudeln

 eigenes soft pwm
 tau (differenz zwischen windows messen bei x energie)
 0 - 100% energie





  *****************************************************************/

#include <cstdint>
#include "sensors.h"
#include "pid.h"
#include "main.h"
#include <sTune.h>
#include <QuickPID.h>
#include <Arduino.h>

// user settings
uint32_t Pid::settleTimeSec = 10;
uint32_t Pid::testTimeSec = 10;  // runPid interval = testTimeSec / samples
const uint16_t Pid::samples = 300;
const float Pid::inputSpan = 60;
const uint32_t Pid::outputSpan = 1000;
float Pid::outputStart = 0;
float Pid::outputStep = 10;
float Pid::tempLimit = 100;
uint8_t Pid::debounce = 0;

// variables
float Pid::Input;
float Pid::Output;
float Pid::SetPoint = 65;
float Pid::Kp;
float Pid::Ki;
float Pid::Kd;

bool Pid::isActive = false;


int Pid::status = -1;

sTune Pid::tuner = sTune(&Input, &Output, tuner.NoOvershoot_PID, tuner.directIP, tuner.printDEBUG);
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
                quickPid.SetSampleTimeUs((outputSpan - 1) * 1000);
                Output = 0;
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
    } else {
        digitalWrite(PIN_RELAY, LOW);
        Sensors::relay = digitalRead(PIN_RELAY);
    }
}